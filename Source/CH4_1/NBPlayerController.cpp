#include "NBPlayerController.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBGameMode.h"
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

void ANBPlayerController::ClientShowInputError_Implementation(const FString& Message)
{
	ReceiveChatMessage(FString::Printf(TEXT("[안내] %s"), *Message));
}
