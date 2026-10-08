#include "UI/NBChatWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "NBPlayerController.h"
#include "NBUtility.h"

void UNBChatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	NB_RETURN_IF_FALSE(ChatInput && SendButton && ChatScrollBox && ChatHistoryText,
		"Check WBP_NBChat BindWidget names and types.");
	// 초기화 시 한 번만 등록하여 재표시 시 이벤트 중복 연결을 방지한다.
	SendButton->OnClicked.AddDynamic(this, &UNBChatWidget::HandleSendClicked);
	ChatInput->OnTextCommitted.AddDynamic(this, &UNBChatWidget::HandleTextCommitted);
	ChatInput->SetClearKeyboardFocusOnCommit(false);
	ChatHistoryText->SetText(FText::GetEmpty());

	NB_RETURN_IF_FALSE(GuessInput && GuessButton, "Add GuessInput and GuessButton to WBP_NBChat.");
	GuessButton->OnClicked.AddDynamic(this, &UNBChatWidget::HandleGuessClicked);
	GuessInput->OnTextCommitted.AddDynamic(this, &UNBChatWidget::HandleGuessCommitted);
	GuessInput->SetClearKeyboardFocusOnCommit(false);
	// PlayerController가 자기 PlayerState를 연결하기 전에는 숫자 제출을 막는다.
	UpdateAttempts(0, 0);
}

void UNBChatWidget::HandleSendClicked()
{
	SubmitChat();
}

void UNBChatWidget::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// 버튼 클릭으로 포커스가 옮겨갈 때는 보내지 않고 Enter만 처리한다.
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SubmitChat();
	}
}

void UNBChatWidget::SubmitChat()
{
	ANBPlayerController* Player = GetOwningPlayer<ANBPlayerController>();
	NB_RETURN_IF_FALSE(IsValid(Player) && ChatInput, "Chat input or owning PC is missing.");
	const FString Message = ChatInput->GetText().ToString();
	// 전송할 문자열을 복사한 뒤 입력창을 비운다.
	ChatInput->SetText(FText::GetEmpty());
	Player->SendChat(Message);
	FocusChatInput();
}

void UNBChatWidget::FocusChatInput()
{
	if (ChatInput && GetOwningPlayer())
	{
		ChatInput->SetUserFocus(GetOwningPlayer());
	}
}

void UNBChatWidget::AppendMessage(const FString& Message)
{
	NB_RETURN_IF_FALSE(ChatHistoryText && ChatScrollBox, "Chat history widgets are missing.");
	// 로컬 표시를 최근 100건으로 제한한다. 과거 채팅은 복제하지 않는다.
	if (MessageHistory.Num() >= 100)
	{
		MessageHistory.RemoveAt(0);
	}
	MessageHistory.Add(Message);
	ChatHistoryText->SetText(FText::FromString(FString::Join(MessageHistory, TEXT("\n"))));
	ChatScrollBox->ScrollToEnd();
}

void UNBChatWidget::HandleGuessClicked()
{
	SubmitGuess();
}

void UNBChatWidget::HandleGuessCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SubmitGuess();
	}
}

void UNBChatWidget::SubmitGuess()
{
	ANBPlayerController* Player = GetOwningPlayer<ANBPlayerController>();
	NB_RETURN_IF_FALSE(IsValid(Player) && GuessInput, "Guess input or owning PC is missing.");
	const FString Input = GuessInput->GetText().ToString();
	GuessInput->SetText(FText::GetEmpty());
	// 문자열을 그대로 전달한다. 최종 검증/판정은 서버에서 수행한다.
	Player->SubmitGuess(Input);
	if (GuessInput->GetIsEnabled())
	{
		GuessInput->SetUserFocus(Player);
	}
	else
	{
		FocusChatInput();
	}
}

void UNBChatWidget::UpdateAttempts(int32 UsedAttempts, int32 MaxAttempts)
{
	NB_RETURN_IF_FALSE(AttemptsText && GuessInput && GuessButton, "Check AttemptsText and guess widgets.");
	const bool bStateReady = MaxAttempts > 0;
	bHasAttempts = bStateReady && UsedAttempts < MaxAttempts;
	AttemptsText->SetText(bStateReady
		? FText::FromString(FString::Printf(TEXT("내 시도 횟수 [%d/%d]"), UsedAttempts, MaxAttempts))
		: FText::FromString(TEXT("플레이어 정보 준비 중")));
	RefreshGuessEnabled();
}

void UNBChatWidget::SetRoundPlaying(bool bPlaying)
{
	bRoundPlaying = bPlaying;
	RefreshGuessEnabled();
}

void UNBChatWidget::ClearGuessInput()
{
	NB_RETURN_IF_FALSE(GuessInput, "Guess input is missing.");
	GuessInput->SetText(FText::GetEmpty());
}

void UNBChatWidget::RefreshGuessEnabled()
{
	NB_RETURN_IF_FALSE(GuessInput && GuessButton, "Guess widgets are missing.");
	// GS와 PS 복제 도착 순서와 관계없이 두 조건을 모두 만족해야 한다.
	const bool bCanSubmit = bRoundPlaying && bHasAttempts;
	const bool bWasEnabled = GuessInput->GetIsEnabled();
	GuessInput->SetIsEnabled(bCanSubmit);
	GuessButton->SetIsEnabled(bCanSubmit);
	if (bWasEnabled && !bCanSubmit)
	{
		FocusChatInput();
	}
}
