#include "NBGameMode.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBGameState.h"
#include "NBPlayerController.h"
#include "NBPlayerState.h"
#include "NBNumberBaseballLibrary.h"

ANBGameMode::ANBGameMode()
{
	// 엔진이 플레이어와 월드를 초기화할 때 사용할 기본 클래스다.
	PlayerControllerClass = ANBPlayerController::StaticClass();
	PlayerStateClass = ANBPlayerState::StaticClass();
	GameStateClass = ANBGameState::StaticClass();
}

void ANBGameMode::BeginPlay()
{
	Super::BeginPlay();
	TryStartRound();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}

void ANBGameMode::PreLogin(const FString& Options, const FString& Address,
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (ErrorMessage.IsEmpty() && ChatPlayerNames.Num() >= RequiredPlayers)
	{
		ErrorMessage = TEXT("Number Baseball server is full (2 players).");
		NB_LOG("Rejected connection: server is full.");
	}
}

APlayerController* ANBGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole,
	const FString& Portal, const FString& Options,
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// PreLogin 이후 시간이 지났거나 여러 접속이 통과했을 수 있어 생성 직전 재검사한다.
	if (ChatPlayerNames.Num() >= RequiredPlayers)
	{
		ErrorMessage = TEXT("Number Baseball server is full (2 players).");
		NB_LOG("Rejected login: server is full.");
		return nullptr;
	}
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void ANBGameMode::PostLogin(APlayerController* NewPlayer)
{
	NB_RETURN_IF_FALSE(IsValid(NewPlayer), "PostLogin received an invalid PlayerController.");

	Super::PostLogin(NewPlayer);

	ANBPlayerController* NBPlayer = Cast<ANBPlayerController>(NewPlayer);
	NB_RETURN_IF_FALSE(IsValid(NBPlayer), "Expected NBPlayerController.");
	ANBPlayerState* NBState = NewPlayer->GetPlayerState<ANBPlayerState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "Expected NBPlayerState after PostLogin.");

	// 이탈한 플레이어의 빈 이름을 재사용한다.
	TArray<FString> UsedNames;
	ChatPlayerNames.GenerateValueArray(UsedNames);
	int32 PlayerNumber = 1;
	FString AssignedName;
	do
	{
		AssignedName = FString::Printf(TEXT("Player %d"), PlayerNumber++);
	} while (UsedNames.Contains(AssignedName));
	ChatPlayerNames.Add(NBPlayer, AssignedName);
	NBState->SetPlayerName(AssignedName);

	TryStartRound();
	NB_LOG("World=%s PlayerController=%s Class=%s NetMode=%d LocalController=%d",
		*GetNameSafe(GetWorld()), *GetNameSafe(NewPlayer),
		*NewPlayer->GetClass()->GetName(),
		static_cast<int32>(GetNetMode()), NewPlayer->IsLocalController());
}

void ANBGameMode::Logout(AController* Exiting)
{
	bool bParticipantLeft = false;
	if (ANBPlayerController* Player = Cast<ANBPlayerController>(Exiting))
	{
		bParticipantLeft = ChatPlayerNames.Remove(Player) > 0;
	}
	Super::Logout(Exiting);
	if (bParticipantLeft)
	{
		GetWorldTimerManager().ClearTimer(ResetTimerHandle);
		SecretAnswer.Reset();
		if (ANBGameState* NBState = GetGameState<ANBGameState>())
		{
			FNBRoundState WaitingState;
			WaitingState.RoundId = NBState->GetRoundState().RoundId;
			NBState->SetRoundState(WaitingState);
		}
	}
}

void ANBGameMode::TryStartRound()
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can start a round.");
	// 첫 로컬 PostLogin은 BeginPlay보다 먼저 올 수 있다. 두 생명주기에서 확인한다.
	if (!HasActorBegunPlay() || ChatPlayerNames.Num() != RequiredPlayers)
	{
		return;
	}
	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	if (NBState->GetRoundState().Phase != ENBRoundPhase::Waiting)
	{
		return;
	}
	StartRound();
}

void ANBGameMode::StartRound()
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can start a round.");
	NB_RETURN_IF_FALSE(ChatPlayerNames.Num() == RequiredPlayers, "A round requires two players.");
	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	for (const auto& Entry : ChatPlayerNames)
	{
		NB_RETURN_IF_FALSE(IsValid(Entry.Key.Get()) && IsValid(Entry.Key->GetPlayerState<ANBPlayerState>()),
			"A participant has no NBPlayerState.");
	}
	for (const auto& Entry : ChatPlayerNames)
	{
		Entry.Key->GetPlayerState<ANBPlayerState>()->ResetAttempts();
	}
	GenerateRandomNumbers();
	FNBRoundState NewState;
	NewState.RoundId = NBState->GetRoundState().RoundId + 1;
	NewState.Phase = ENBRoundPhase::Playing;
	NBState->SetRoundState(NewState);
}

bool ANBGameMode::HaveAllPlayersExhaustedAttempts() const
{
	if (ChatPlayerNames.Num() != RequiredPlayers)
	{
		return false;
	}
	for (const auto& Entry : ChatPlayerNames)
	{
		const ANBPlayerState* State = IsValid(Entry.Key.Get())
			? Entry.Key->GetPlayerState<ANBPlayerState>() : nullptr;
		if (!IsValid(State) || State->HasRemainingAttempts())
		{
			return false;
		}
	}
	return true;
}

void ANBGameMode::FinishRound(ENBRoundResult Result, ANBPlayerController* Winner)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can finish a round.");
	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	if (NBState->GetRoundState().Phase != ENBRoundPhase::Playing)
	{
		return;
	}
	FNBRoundState FinishedState = NBState->GetRoundState();
	FinishedState.Phase = ENBRoundPhase::ShowingResult;
	FinishedState.Result = Result;
	if (Result == ENBRoundResult::Win)
	{
		const ANBPlayerState* WinnerState = IsValid(Winner) ? Winner->GetPlayerState<ANBPlayerState>() : nullptr;
		NB_RETURN_IF_FALSE(IsValid(WinnerState), "Winner has no NBPlayerState.");
		FinishedState.WinnerName = WinnerState->GetPlayerName();
		FinishedState.WinnerPlayerId = WinnerState->GetPlayerId();
	}
	NBState->SetRoundState(FinishedState);
	// 표시 시간은 서버가 결정한다. 예약 당시 라운드 ID도 함께 보관한다.
	FTimerDelegate ResetDelegate;
	ResetDelegate.BindUObject(this, &ANBGameMode::ResetGame, FinishedState.RoundId);
	GetWorldTimerManager().SetTimer(ResetTimerHandle, ResetDelegate, 3.0f, false);
}

void ANBGameMode::ResetGame(int32 ExpectedRoundId)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can reset the game.");
	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	const FNBRoundState& Round = NBState->GetRoundState();
	if (Round.RoundId != ExpectedRoundId || Round.Phase != ENBRoundPhase::ShowingResult)
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	NB_LOG("Resetting round=%d Players=%d", ExpectedRoundId, ChatPlayerNames.Num());
	if (ChatPlayerNames.Num() == RequiredPlayers)
	{
		// ShowingResult의 입력 차단을 유지한 채 초기화하고 새 Playing을 공개한다.
		StartRound();
	}
	else
	{
		SecretAnswer.Reset();
		FNBRoundState WaitingState;
		WaitingState.RoundId = ExpectedRoundId;
		NBState->SetRoundState(WaitingState);
	}
}

void ANBGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ANBGameMode::ProcessChatMessage(ANBPlayerController* Sender, const FString& Message)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Chat must be processed by the server.");
	NB_RETURN_IF_FALSE(IsValid(Sender), "Chat sender is invalid.");
	const FString* SenderName = ChatPlayerNames.Find(Sender);
	NB_RETURN_IF_FALSE(SenderName, "Chat sender has not completed PostLogin.");

	// 숫자 입력의 검증과 별개다. 일반 채팅은 200자까지 허용한다.
	if (Message.Len() > 200)
	{
		Sender->ClientShowInputError(TEXT("채팅은 200자 이하로 입력해주세요."));
		return;
	}
	const FString TrimmedMessage = Message.TrimStartAndEnd();
	if (TrimmedMessage.IsEmpty())
	{
		Sender->ClientShowInputError(TEXT("채팅 내용을 입력해주세요."));
		return;
	}
	for (const TCHAR Character : Message)
	{
		if (FChar::IsControl(Character))
		{
			Sender->ClientShowInputError(TEXT("줄바꿈이나 제어 문자 없이 입력해주세요."));
			return;
		}
	}

	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	const FString DisplayMessage = FString::Printf(TEXT("%s: %s"), **SenderName, *TrimmedMessage);
	NB_LOG("Accepted chat: Sender=%s Length=%d", **SenderName, TrimmedMessage.Len());
	NBState->MulticastReceiveMessage(DisplayMessage);
}

void ANBGameMode::GenerateRandomNumbers()
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can generate the answer.");
	TArray<int32> Candidates = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	SecretAnswer.Reset();
	for (int32 Index = 0; Index < 3; ++Index)
	{
		const int32 SelectedIndex = FMath::RandRange(0, Candidates.Num() - 1);
		SecretAnswer.AppendChar(static_cast<TCHAR>(TEXT('0') + Candidates[SelectedIndex]));
		// 뽑은 숫자를 후보에서 제거하므로 중복 재추첨 루프가 필요 없다.
		Candidates.RemoveAtSwap(SelectedIndex);
	}

#if !UE_BUILD_SHIPPING
	// NB_LOG의 선택적 화면 출력에도 비밀 정답을 노출하지 않는다.
	UE_LOG(LogNB, Log, TEXT("[GenerateRandomNumbers] Server answer=%s"), *SecretAnswer);
#endif
}

bool ANBGameMode::CheckAnswer(const FString& Input, FNBGuessResult& OutResult) const
{
	NB_RETURN_VALUE_IF_FALSE(HasAuthority(), false, "Only the server can check the answer.");
	return UNBNumberBaseballLibrary::EvaluateGuess(SecretAnswer, Input, OutResult);
}

void ANBGameMode::ProcessGuess(ANBPlayerController* Sender, const FString& Input, int32 SubmittedRoundId)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Guesses must be processed by the server.");
	NB_RETURN_IF_FALSE(IsValid(Sender), "Guess sender is invalid.");
	const FString* SenderName = ChatPlayerNames.Find(Sender);
	NB_RETURN_IF_FALSE(SenderName, "Guess sender has not completed PostLogin.");
	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	const FNBRoundState& Round = NBState->GetRoundState();
	if (SubmittedRoundId != Round.RoundId || Round.Phase != ENBRoundPhase::Playing)
	{
		NB_LOG("Rejected guess: SubmittedRound=%d CurrentRound=%d Phase=%d",
			SubmittedRoundId, Round.RoundId, static_cast<int32>(Round.Phase));
		Sender->ClientShowInputError(TEXT("현재 진행 중인 라운드에서만 제출할 수 있습니다."));
		return;
	}
	ANBPlayerState* SenderState = Sender->GetPlayerState<ANBPlayerState>();
	NB_RETURN_IF_FALSE(IsValid(SenderState), "Guess sender has no NBPlayerState.");
	if (!SenderState->HasRemainingAttempts())
	{
		NB_LOG("Rejected guess: Sender=%s Attempts=%d/%d", **SenderName,
			SenderState->GetUsedAttempts(), SenderState->GetMaxAttempts());
		Sender->ClientShowInputError(TEXT("기회를 모두 사용했습니다. 채팅은 계속할 수 있습니다."));
		return;
	}

	FString ErrorMessage;
	if (!UNBNumberBaseballLibrary::ValidateInput(Input, ErrorMessage))
	{
		Sender->ClientShowInputError(ErrorMessage);
		return;
	}

	FNBGuessResult Result;
	if (!CheckAnswer(Input, Result))
	{
		NB_ERROR("The server answer is not ready or is invalid.");
		Sender->ClientShowInputError(TEXT("게임 준비 중입니다. 잠시 후 다시 입력해주세요."));
		return;
	}

	// 검증/판정 및 결과 전달 준비가 끝난 유효한 제출만 한 번 소비한다.
	if (!SenderState->TryConsumeAttempt())
	{
		Sender->ClientShowInputError(TEXT("기회를 모두 사용했습니다."));
		return;
	}
	// 이 메시지와 PlayerState 복제의 도착 순서를 가정하지 않도록 횟수를 함께 담는다.
	const FString DisplayMessage = FString::Printf(TEXT("[R%d][숫자] %s: %s → %s [%d/%d]"),
		Round.RoundId, **SenderName, *Input, *Result.ToDisplayString(),
		SenderState->GetUsedAttempts(), SenderState->GetMaxAttempts());
	NBState->MulticastReceiveMessage(DisplayMessage);
	// 마지막 기회에 맞혀도 승리다. 무승부보다 먼저 검사한다.
	if (Result.Strikes == 3)
	{
		FinishRound(ENBRoundResult::Win, Sender);
	}
	else if (HaveAllPlayersExhaustedAttempts())
	{
		FinishRound(ENBRoundResult::Draw);
	}
}
