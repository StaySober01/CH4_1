#include "NBPlayerController.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBGameMode.h"
#include "NBPlayerState.h"
#include "UI/NBChatWidget.h"

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

	NB_RETURN_IF_FALSE(ChatWidgetClass, "Set ChatWidgetClass on BP_NBPlayerController.");
	ChatWidget = CreateWidget<UNBChatWidget>(this, ChatWidgetClass);
	NB_RETURN_IF_FALSE(IsValid(ChatWidget), "Failed to create ChatWidget.");
	ChatWidget->AddToPlayerScreen();
	BindAttemptState();

	// 채팅 전용 단계이므로 키보드/마우스를 UI로 전달한다.
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	ChatWidget->FocusChatInput();

	for (const FString& Message : PendingMessages)
	{
		ChatWidget->AppendMessage(Message);
	}
	PendingMessages.Reset();
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
		ServerSubmitGuess(Input);
	}
}

void ANBPlayerController::ServerSubmitGuess_Implementation(const FString& Input)
{
	ANBGameMode* GameMode = GetWorld()->GetAuthGameMode<ANBGameMode>();
	NB_RETURN_IF_FALSE(IsValid(GameMode), "NBGameMode is missing on the server.");
	GameMode->ProcessGuess(this, Input);
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
	if (IsValid(ChatWidget))
	{
		ChatWidget->RemoveFromParent();
		ChatWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}
