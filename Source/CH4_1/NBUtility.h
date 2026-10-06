#pragma once

#include "CoreMinimal.h"
#include "CH4_1.h"

// 한 프로세스의 여러 PIE 창은 화면 디버그 메시지를 공유할 수 있다.
// 기본은 Output Log만 사용한다. 필요할 때 프로젝트 전체 설정으로 활성화한다.
#ifndef NB_ENABLE_SCREEN_LOG
#define NB_ENABLE_SCREEN_LOG 0
#endif

#if NB_ENABLE_SCREEN_LOG && !UE_BUILD_SHIPPING
#include "Engine/Engine.h"
#endif

namespace NBUtility
{
	inline void PrintToScreen(const FString& Message, const FColor& Color)
	{
#if NB_ENABLE_SCREEN_LOG && !UE_BUILD_SHIPPING
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, Color, Message);
		}
#else
		(void)Message;
		(void)Color;
#endif
	}
}

// Format에는 TEXT()를 씌우지 않은 문자열 리터럴을 전달한다.
// 메시지를 한 번만 포맷하여 로그와 선택적 화면 출력에 같이 사용한다.
#define NB_PRIVATE_LOG(Verbosity, Color, Format, ...) \
	do \
	{ \
		const FString NBUtilityMessage = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
		const FString NBUtilityContextMessage = FString::Printf(TEXT("[%hs:%d] %s"), __FUNCTION__, __LINE__, *NBUtilityMessage); \
		UE_LOG(LogNB, Verbosity, TEXT("%s"), *NBUtilityContextMessage); \
		NBUtility::PrintToScreen(NBUtilityContextMessage, Color); \
	} while (false)

#define NB_LOG(Format, ...) NB_PRIVATE_LOG(Log, FColor::White, Format, ##__VA_ARGS__)
#define NB_WARNING(Format, ...) NB_PRIVATE_LOG(Warning, FColor::Yellow, Format, ##__VA_ARGS__)
#define NB_ERROR(Format, ...) NB_PRIVATE_LOG(Error, FColor::Red, Format, ##__VA_ARGS__)

// C++ 예외나 check가 아니다. 조건 실패를 기록하고 현재 함수에서 반환한다.
// 화면 로그 설정과 관계없이 조건 검사는 실행된다. Condition은 한 번만 평가한다.
#define NB_RETURN_IF_FALSE(Condition, Format, ...) \
	do \
	{ \
		if (!(Condition)) \
		{ \
			NB_ERROR(Format, ##__VA_ARGS__); \
			return; \
		} \
	} while (false)

#define NB_RETURN_VALUE_IF_FALSE(Condition, ReturnValue, Format, ...) \
	do \
	{ \
		if (!(Condition)) \
		{ \
			NB_ERROR(Format, ##__VA_ARGS__); \
			return (ReturnValue); \
		} \
	} while (false)
