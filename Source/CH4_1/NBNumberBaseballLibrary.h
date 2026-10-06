#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NBGameTypes.h"
#include "NBNumberBaseballLibrary.generated.h"

// World, RPC, 플레이어 상태에 접근하지 않는 순수 규칙 계산이다.
UCLASS()
class CH4_1_API UNBNumberBaseballLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NB|Rules")
	static bool ValidateInput(const FString& Input, FString& OutErrorMessage);

	// 잘못된 정답/입력은 false를 반환한다. 실패를 OUT으로 취급하지 않는다.
	UFUNCTION(BlueprintCallable, Category = "NB|Rules")
	static bool EvaluateGuess(const FString& Answer, const FString& Input, FNBGuessResult& OutResult);
};
