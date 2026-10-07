#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FNBOnAttemptsChanged);

// 개인별 횟수의 원본은 서버가 관리하고, 클라이언트는 복제본을 조회한다.
UCLASS()
class CH4_1_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "NB|Attempts")
	int32 GetUsedAttempts() const { return UsedAttempts; }

	UFUNCTION(BlueprintPure, Category = "NB|Attempts")
	int32 GetMaxAttempts() const { return MaxAttempts; }

	UFUNCTION(BlueprintPure, Category = "NB|Attempts")
	int32 GetRemainingAttempts() const { return MaxAttempts - UsedAttempts; }

	UFUNCTION(BlueprintPure, Category = "NB|Attempts")
	bool HasRemainingAttempts() const { return UsedAttempts < MaxAttempts; }

	// 서버 전용 변경 함수. 유효성 판정을 마친 GameMode가 호출한다.
	bool TryConsumeAttempt();
	void ResetAttempts();

	FNBOnAttemptsChanged OnAttemptsChanged;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Attempts)
	int32 UsedAttempts = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Attempts)
	int32 MaxAttempts = 3;

	UFUNCTION()
	void OnRep_Attempts();
};
