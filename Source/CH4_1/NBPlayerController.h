#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"


// 소유 플레이어의 입력과 서버 요청, 로컬 UI 연결을 담당할 클래스다.
UCLASS()
class CH4_1_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
