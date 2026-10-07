#include "NBPlayerController.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBGameMode.h"
#include "NBGameState.h"
#include "NBPlayerState.h"
#include "UI/NBChatWidget.h"
#include "UI/NBResultWidget.h"

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 서버 권한과 로컬 플레이어 여부는 서로 다른 개념이다.
	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d LocalController=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()),
		HasAuthority(), IsLocalController());

	if (!IsLocalController())
	{
		return;
	}

	// 로컬 PC당 한 번 생성해 상태에 따라 표시/숨김만 바꾼다.
	if (ResultWidgetClass)
	{
		ResultWidget = CreateWidget<UNBResultWidget>(this, ResultWidgetClass);
		if (IsValid(ResultWidget))
		{
			ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
			ResultWidget->AddToPlayerScreen(10);
		}
		else
		{
			NB_ERROR("Failed to create ResultWidget.");
		}
	}
	else
	{
		NB_ERROR("Set ResultWidgetClass on BP_NBPlayerController.");
	}
	RefreshRoundUI();

	NB_RETURN_IF_FALSE(ChatWidgetClass, "Set ChatWidgetClass on BP_NBPlayerController.");
	ChatWidget = CreateWidget<UNBChatWidget>(this, ChatWidgetClass);
	NB_RETURN_IF_FALSE(IsValid(ChatWidget), "Failed to create ChatWidget.");
	ChatWidget->AddToPlayerScreen();
	BindAttemptState();

	// 채팅과 숫자 입력에 키보드/마우스를 사용한다.
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	ChatWidget->FocusChatInput();

	for (const FString& Message : PendingMessages)
	{
		ChatWidget->AppendMessage(Message);
	}
	PendingMessages.Reset();
	// 상태 알림이 UI 생성보다 먼저 왔어도 현재 스냅샷에서 복원한다.
	RefreshRoundUI();
}

void ANBPlayerController::SendChat(const FString& Message)
{
	if (IsLocalController())
	{
		// 발신자 이름은 보내지 않는다. 서버가 이 PC를 기준으로 식별한다.
		ServerSendChat(Message);
	}
}

void ANBPlayerController::ServerSendChat_Implementation(const FString& Message)
{
	ANBGameMode* GameMode = GetWorld()->GetAuthGameMode<ANBGameMode>();
	NB_RETURN_IF_FALSE(IsValid(GameMode), "NBGameMode is missing on the server.");
	GameMode->ProcessChatMessage(this, Message);
}

void ANBPlayerController::ReceiveChatMessage(const FString& Message)
{
	if (!IsLocalController())
	{
		return;
	}

	if (IsValid(ChatWidget))
	{
		ChatWidget->AppendMessage(Message);
	}
	else
	{
		if (PendingMessages.Num() >= 100)
		{
			PendingMessages.RemoveAt(0);
		}
		PendingMessages.Add(Message);
	}
}

void ANBPlayerController::SubmitGuess(const FString& Input)
{
	if (IsLocalController())
	{
		const ANBGameState* NBState = GetWorld()->GetGameState<ANBGameState>();
		if (IsValid(NBState))
		{
			ServerSubmitGuess(Input, NBState->GetRoundState().RoundId);
		}
	}
}

void ANBPlayerController::ServerSubmitGuess_Implementation(const FString& Input, int32 SubmittedRoundId)
{
	ANBGameMode* GameMode = GetWorld()->GetAuthGameMode<ANBGameMode>();
	NB_RETURN_IF_FALSE(IsValid(GameMode), "NBGameMode is missing on the server.");
	GameMode->ProcessGuess(this, Input, SubmittedRoundId);
}

void ANBPlayerController::RefreshRoundUI()
{
	if (!IsLocalController())
	{
		return;
	}
	const ANBGameState* NBState = GetWorld()->GetGameState<ANBGameState>();
	if (IsValid(ResultWidget))
	{
		// 채팅 문구 중복 검사와 별개로 현재 상태를 항상 반영한다.
		ResultWidget->UpdateRoundState(IsValid(NBState) ? NBState->GetRoundState() : FNBRoundState());
	}
	if (!IsValid(ChatWidget))
	{
		return;
	}
	if (!IsValid(NBState))
	{
		ChatWidget->SetRoundPlaying(false);
		return;
	}
	const FNBRoundState& Round = NBState->GetRoundState();
	ChatWidget->SetRoundPlaying(Round.Phase == ENBRoundPhase::Playing);
	// 초기 조회와 BeginPlay/OnRep 알림이 겹쳐도 같은 상태 문구를 반복하지 않는다.
	if (LastDisplayedRoundId == Round.RoundId && LastDisplayedPhase == Round.Phase)
	{
		return;
	}
	LastDisplayedRoundId = Round.RoundId;
	LastDisplayedPhase = Round.Phase;
	FString StatusMessage;
	switch (Round.Phase)
	{
	case ENBRoundPhase::Waiting:
		StatusMessage = TEXT("두 플레이어의 접속을 기다립니다. 기존 라운드는 진행하지 않습니다.");
		break;
	case ENBRoundPhase::Playing:
		StatusMessage = TEXT("라운드 시작! 각자 3번 안에 정답을 맞혀주세요.");
		break;
	case ENBRoundPhase::ShowingResult:
		StatusMessage = Round.Result == ENBRoundResult::Win
			? FString::Printf(TEXT("%s 승리!"), *Round.WinnerName) : TEXT("무승부!");
		break;
	}
	ReceiveChatMessage(FString::Printf(TEXT("[R%d][라운드] %s"), Round.RoundId, *StatusMessage));
}

void ANBPlayerController::ClientShowInputError_Implementation(const FString& Message)
{
	ReceiveChatMessage(FString::Printf(TEXT("[안내] %s"), *Message));
}

void ANBPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// PlayerState가 UI보다 늦게 도착해도 복제 콜백에서 연결한다.
	BindAttemptState();
}

void ANBPlayerController::BindAttemptState()
{
	if (!IsLocalController())
	{
		return;
	}

	ANBPlayerState* CurrentState = GetPlayerState<ANBPlayerState>();
	if (BoundPlayerState.Get() != CurrentState)
	{
		if (BoundPlayerState.IsValid())
		{
			BoundPlayerState->OnAttemptsChanged.RemoveAll(this);
		}
		BoundPlayerState = CurrentState;
		if (IsValid(CurrentState))
		{
			CurrentState->OnAttemptsChanged.AddUObject(this, &ANBPlayerController::UpdateAttemptUI);
		}
	}
	// 초기값의 OnRep가 생략되거나 UI 생성 전에 알림이 왔어도 현재 값을 읽는다.
	UpdateAttemptUI();
}

void ANBPlayerController::UpdateAttemptUI()
{
	if (!IsValid(ChatWidget))
	{
		return;
	}
	if (BoundPlayerState.IsValid())
	{
		ChatWidget->UpdateAttempts(BoundPlayerState->GetUsedAttempts(), BoundPlayerState->GetMaxAttempts());
	}
	else
	{
		ChatWidget->UpdateAttempts(0, 0);
	}
}

void ANBPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BoundPlayerState.IsValid())
	{
		BoundPlayerState->OnAttemptsChanged.RemoveAll(this);
	}
	BoundPlayerState.Reset();
	if (IsValid(ResultWidget))
	{
		ResultWidget->RemoveFromParent();
		ResultWidget = nullptr;
	}
	if (IsValid(ChatWidget))
	{
		ChatWidget->RemoveFromParent();
		ChatWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}
