#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameMode.generated.h"


// 서버에서 게임 규칙과 진행을 담당한다.
UCLASS()
class CH4_1_API ANBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANBGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;


protected:
	virtual void BeginPlay() override;

};
