#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "NBChatWidget.generated.h"

class UButton;
class UEditableTextBox;
class UScrollBox;
class UTextBlock;

UCLASS()
class CH4_1_API UNBChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AppendMessage(const FString& Message);
	void FocusChatInput();
	void UpdateAttempts(int32 UsedAttempts, int32 MaxAttempts);

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleSendClicked();

	UFUNCTION()
	void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void SubmitChat();

	UFUNCTION()
	void HandleGuessClicked();

	UFUNCTION()
	void HandleGuessCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void SubmitGuess();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> GuessInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GuessButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttemptsText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SendButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatHistoryText;

	TArray<FString> MessageHistory;
};
