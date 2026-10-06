#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameMode.generated.h"

class ANBPlayerController;

// 서버에서 게임 규칙과 진행을 담당한다.
UCLASS()
class CH4_1_API ANBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANBGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void ProcessChatMessage(ANBPlayerController* Sender, const FString& Message);

protected:
	virtual void BeginPlay() override;

private:
	// 채팅 발신자 이름은 서버가 부여한 값만 사용한다.
	UPROPERTY(Transient)
	TMap<TObjectPtr<ANBPlayerController>, FString> ChatPlayerNames;
};
