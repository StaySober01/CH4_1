#pragma once

#include "CoreMinimal.h"
#include "NBGameTypes.generated.h"

// 순수 판정 결과다. 게임의 승패나 플레이어 상태는 포함하지 않는다.
USTRUCT(BlueprintType)
struct CH4_1_API FNBGuessResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "NB|Rules")
	int32 Strikes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "NB|Rules")
	int32 Balls = 0;

	FString ToDisplayString() const
	{
		return Strikes == 0 && Balls == 0
			? TEXT("OUT")
			: FString::Printf(TEXT("%dS%dB"), Strikes, Balls);
	}
};
