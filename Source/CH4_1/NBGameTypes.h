#pragma once

#include "CoreMinimal.h"
#include "NBGameTypes.generated.h"

UENUM(BlueprintType)
enum class ENBRoundPhase : uint8
{
	Waiting,
	Playing,
	ShowingResult
};

UENUM(BlueprintType)
enum class ENBRoundResult : uint8
{
	None,
	Win,
	Draw
};

// 공개 상태만 묶는다. 승자 Actor의 수명과 무관하게 표시할 수 있는 값이다.
USTRUCT(BlueprintType)
struct CH4_1_API FNBRoundState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "NB|Round")
	int32 RoundId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "NB|Round")
	ENBRoundPhase Phase = ENBRoundPhase::Waiting;

	UPROPERTY(BlueprintReadOnly, Category = "NB|Round")
	ENBRoundResult Result = ENBRoundResult::None;

	UPROPERTY(BlueprintReadOnly, Category = "NB|Round")
	FString WinnerName;

	UPROPERTY(BlueprintReadOnly, Category = "NB|Round")
	int32 WinnerPlayerId = INDEX_NONE;
};

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
