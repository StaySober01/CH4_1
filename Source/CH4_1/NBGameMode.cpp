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
	GenerateRandomNumbers();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}

void ANBGameMode::PostLogin(APlayerController* NewPlayer)
{
	NB_RETURN_IF_FALSE(IsValid(NewPlayer), "PostLogin received an invalid PlayerController.");

	Super::PostLogin(NewPlayer);

	ANBPlayerController* NBPlayer = Cast<ANBPlayerController>(NewPlayer);
	NB_RETURN_IF_FALSE(IsValid(NBPlayer), "Expected NBPlayerController.");
	ANBPlayerState* NBState = NewPlayer->GetPlayerState<ANBPlayerState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "Expected NBPlayerState after PostLogin.");

	// 빈 이름을 재사용한다. 정원 2명 제한은 M5에서 적용한다.
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

	// 서버에서 접속을 확인한다. 인원 제한과 시작 판정은 M5에서 추가한다.
	NB_LOG("World=%s PlayerController=%s Class=%s NetMode=%d LocalController=%d",
		*GetNameSafe(GetWorld()), *GetNameSafe(NewPlayer),
		*NewPlayer->GetClass()->GetName(),
		static_cast<int32>(GetNetMode()), NewPlayer->IsLocalController());
}

void ANBGameMode::Logout(AController* Exiting)
{
	if (ANBPlayerController* Player = Cast<ANBPlayerController>(Exiting))
	{
		ChatPlayerNames.Remove(Player);
	}
	Super::Logout(Exiting);
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

void ANBGameMode::ProcessGuess(ANBPlayerController* Sender, const FString& Input)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Guesses must be processed by the server.");
	NB_RETURN_IF_FALSE(IsValid(Sender), "Guess sender is invalid.");
	const FString* SenderName = ChatPlayerNames.Find(Sender);
	NB_RETURN_IF_FALSE(SenderName, "Guess sender has not completed PostLogin.");

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

	ANBGameState* NBState = GetGameState<ANBGameState>();
	NB_RETURN_IF_FALSE(IsValid(NBState), "NBGameState is missing.");
	const FString DisplayMessage = FString::Printf(TEXT("[숫자] %s: %s → %s"),
		**SenderName, *Input, *Result.ToDisplayString());
	NBState->MulticastReceiveMessage(DisplayMessage);
	// M3는 판정까지만 수행한다. 횟수 소비와 3S 승리는 M4/M5에서 추가한다.
}
