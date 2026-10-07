#include "UI/NBResultWidget.h"

#include "Components/TextBlock.h"
#include "NBUtility.h"

void UNBResultWidget::UpdateRoundState(const FNBRoundState& RoundState)
{
	// 미준비/대기/진행 상태에서는 이전 결과를 남기지 않는다.
	SetVisibility(ESlateVisibility::Collapsed);
	NB_RETURN_IF_FALSE(ResultText, "Add ResultText (TextBlock) to WBP_NBResult.");
	ResultText->SetText(FText::GetEmpty());
	if (RoundState.Phase != ENBRoundPhase::ShowingResult)
	{
		return;
	}

	FString Message;
	switch (RoundState.Result)
	{
	case ENBRoundResult::Win:
		Message = FString::Printf(TEXT("%s 승리!"), *RoundState.WinnerName);
		break;
	case ENBRoundResult::Draw:
		Message = TEXT("무승부!");
		break;
	default:
		return;
	}
	ResultText->SetText(FText::FromString(Message));
	// 공지가 위에 떠 있어도 하위 채팅창의 클릭과 입력을 가로채지 않는다.
	SetVisibility(ESlateVisibility::HitTestInvisible);
}
