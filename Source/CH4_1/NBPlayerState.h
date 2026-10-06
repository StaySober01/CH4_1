#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

// 개인별 게임 상태를 보관한다. 시도 횟수는 M4에서 추가한다.
UCLASS()
class CH4_1_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
