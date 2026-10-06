# NBUtility 사용법

JUtility.h의 함수명 로그와 조기 반환 방식을 참고하여 현재 프로젝트에 맞춘 헤더다. `Source/CH4_1/NBUtility.h`를 include한다. 게임 규칙용 UBlueprintFunctionLibrary와는 별개인 C++ 헤더이며 RPC/게임 상태를 가지지 않는다.

```cpp
#include "NBUtility.h"

NB_LOG("Ready");
NB_LOG("Count=%d", Count);
NB_WARNING("Unexpected state: %s", *StateName);
NB_ERROR("Required object is missing.");

// void 함수에서: 실패하면 로그를 남기고 return;
NB_RETURN_IF_FALSE(IsValid(Target), "Target is invalid.");

// 반환값이 있는 함수에서: 함수 반환형에 맞는 값을 지정한다.
NB_RETURN_VALUE_IF_FALSE(IsValid(Target), false, "Target is invalid.");
// 포인터 반환 함수라면 nullptr, 정수 반환 함수라면 필요에 따라 -1 등을 지정한다.
```

- Format은 `"Count=%d"`처럼 TEXT() 없는 문자열 리터럴이다. FString을 출력하려면 `NB_LOG("%s", *Message)`를 사용한다.
- Output Log 카테고리는 LogNB이며 `[호출함수:줄번호] 메시지`로 출력한다. 로그 매크로 내부에서 로그 레코드는 한 번만 생성한다.
- `NB_ENABLE_SCREEN_LOG` 기본값은 0이다. 프로젝트 전체 컴파일 정의에서 1로 설정하면 개발 빌드에서 화면에도 흰색/노란색/빨간색으로 10초간 표시한다. 번역 단위마다 다르게 정의하지 않는다. Shipping에서는 화면 출력을 끈다.
- 화면 출력 설정은 Output Log를 끄지 않는다. 다만 Shipping 로그의 실제 포함 여부는 Unreal 빌드 로깅 정책을 따른다.
- 한 프로세스 PIE의 화면 메시지는 특정 플레이어 전용 UI가 아니다. 네트워크 검증은 World/NetMode를 포함한 Output Log를 사용한다.
- 조기 반환 매크로는 C++ throw/catch나 실행을 중단하는 assert가 아니다. 조건을 한 번 평가하고 실패 시 현재 함수에서 반환한다. 반환값 표현식은 실패 시에만 평가한다.
- `do { ... } while (false)` 형태이므로 if/else 안에서도 하나의 문장처럼 사용하며 끝에 세미콜론을 붙인다.
- 예상치 못한 내부 상태/객체 누락에 사용한다. 잘못된 사용자 숫자 입력은 정상적으로 발생 가능한 상황이므로 명시적 분기와 Client 안내로 처리한다.
- 로그 인자에는 게임 상태 변경 같은 부작용을 넣지 않는다. 필수 정리 작업이나 상위 클래스 호출을 건너뛰지 않도록 반환 위치를 검토한다.

참고 코드의 중복 Error 로그와 전체 로그를 없애는 JDEBUG 분기를 그대로 옮기지 않았다. 미사용 BehaviorTree 의존성, 반환형별 중복 매크로, enum 문자열 매크로도 현재 필요하지 않아 추가하지 않았다.

현재 적용: 네 Actor 클래스의 M1 로그, GameMode PostLogin의 유효 객체 검사. 기존 World/NetMode/Authority 필드는 유지한다. 사용자 빌드 확인 대기이며, PIE에서는 기존 M1 로그가 함수명/줄번호와 함께 출력되는지 확인한다.
