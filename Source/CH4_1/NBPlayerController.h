#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBChatWidget;

// 소유 플레이어의 입력과 서버 요청, 로컬 UI 연결을 담당할 클래스다.
UCLASS()
class CH4_1_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SendChat(const FString& Message);
	void SubmitGuess(const FString& Input);
	void ReceiveChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientShowInputError(const FString& Message);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(Server, Reliable)
	void ServerSendChat(const FString& Message);

	UFUNCTION(Server, Reliable)
	void ServerSubmitGuess(const FString& Input);

	UPROPERTY(EditDefaultsOnly, Category = "NB|UI")
	TSubclassOf<UNBChatWidget> ChatWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UNBChatWidget> ChatWidget;

	// UI 생성 전 도착한 메시지만 임시 보관한다. 서버 채팅 이력이 아니다.
	TArray<FString> PendingMessages;
};
