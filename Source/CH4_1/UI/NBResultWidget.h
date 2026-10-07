#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NBGameTypes.h"
#include "NBResultWidget.generated.h"

class UTextBlock;

// 서버가 확정한 공개 결과를 표시한다. 승패 판정이나 타이머는 갖지 않는다.
UCLASS()
class CH4_1_API UNBResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateRoundState(const FNBRoundState& RoundState);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
};
