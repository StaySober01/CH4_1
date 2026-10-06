#include "NBNumberBaseballLibrary.h"

bool UNBNumberBaseballLibrary::ValidateInput(const FString& Input, FString& OutErrorMessage)
{
	OutErrorMessage.Reset();
	if (Input.Len() != 3)
	{
		OutErrorMessage = TEXT("3자리 숫자를 입력해주세요.");
		return false;
	}

	for (int32 Index = 0; Index < Input.Len(); ++Index)
	{
		// Unicode 숫자 전체가 아니라 ASCII 1~9만 허용한다. 공백도 보정하지 않는다.
		if (Input[Index] < TEXT('1') || Input[Index] > TEXT('9'))
		{
			OutErrorMessage = TEXT("1~9 사이의 숫자만 입력해주세요.");
			return false;
		}
		for (int32 Previous = 0; Previous < Index; ++Previous)
		{
			if (Input[Index] == Input[Previous])
			{
				OutErrorMessage = TEXT("중복되지 않은 숫자를 입력해주세요.");
				return false;
			}
		}
	}
	return true;
}

bool UNBNumberBaseballLibrary::EvaluateGuess(const FString& Answer, const FString& Input, FNBGuessResult& OutResult)
{
	// 실패하거나 이전 결과를 재사용해도 값이 누적되지 않게 초기화한다.
	OutResult = FNBGuessResult{};
	FString ErrorMessage;
	if (!ValidateInput(Answer, ErrorMessage) || !ValidateInput(Input, ErrorMessage))
	{
		return false;
	}

	for (int32 InputIndex = 0; InputIndex < 3; ++InputIndex)
	{
		for (int32 AnswerIndex = 0; AnswerIndex < 3; ++AnswerIndex)
		{
			if (Input[InputIndex] == Answer[AnswerIndex])
			{
				if (InputIndex == AnswerIndex)
				{
					++OutResult.Strikes;
				}
				else
				{
					++OutResult.Balls;
				}
				break;
			}
		}
	}
	return true;
}
