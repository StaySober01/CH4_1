#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameState.generated.h"

// 모든 플레이어에게 공유할 공개 게임 상태를 보관할 클래스다.
UCLASS()
class CH4_1_API ANBGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReceiveMessage(const FString& Message);

protected:
	virtual void BeginPlay() override;
};
