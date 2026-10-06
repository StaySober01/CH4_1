# 숫자 야구 아키텍처

작성 기준: 2026-10-06. M1 클래스 골격과 진단 유틸리티를 작성하고 사용자 제공 로그로 2인 PIE 클래스 배치를 확인했다. 정식 빌드 결과는 별도 확인 대기다. 채팅과 숫자 야구 규칙은 아직 미구현이다.

## 확정 요구사항 (2026-10-06 사용자 승인)

| 항목 | 확정 내용 |
| --- | --- |
| 인원/시작 | 2인 전용. 두 명 접속 후 자동 시작. 정원 초과 접속은 서버에서 거절 |
| 서버 | Listen Server 호스트 + 원격 클라이언트가 기본. 별도 서버 실행은 추가 검증 |
| 입력 UI | 같은 화면에서 일반 채팅 입력창과 숫자 제출창을 분리 |
| 숫자 형식 | ASCII 1~9 중 중복 없는 정확히 세 자리. 공백을 자동 제거하지 않으며 0·문자·중복 거절 |
| 공개 범위 | 제출된 유효 숫자·S/B/OUT·사용 횟수를 모두에게 공개. 입력 오류는 제출자만 안내 |
| 기회 소진 | 숫자 제출 차단, 일반 채팅과 관전 허용 |
| 결과/리셋 | 결과 3초 표시 후 자동 재시작. 비밀 정답은 결과 공지에도 공개하지 않음 |
| 이탈 | 진행 중 한 명이 나가면 라운드 중단 후 Waiting. 다시 두 명이 되면 새 라운드 시작 |
| 최종 범위 | 필수 기능 검증 후 도전 턴제·타이머까지 구현 |
| 턴 | 접속 순서로 선공 결정, 턴당 20초. 유효 제출 또는 시간 초과 시 기회 한 번 소비 후 전환 |
| 오입력 | 기회 유지, 턴 유지, 남은 시간은 계속 흐름 |
| 표시 이름 | 서버가 Player 1 / Player 2 부여. 별도 닉네임 입력 기능 없음 |
| 라이브러리 | 단일 모듈 안에 UBlueprintFunctionLibrary 하나 작성 |
| 이름 | 프로젝트/모듈 CH4_1 유지, 신규 게임 클래스 접두사 NB |
| 역할 분담 | Codex: C++·문서·정적 검토·검증 절차 안내. 사용자: 빌드 확인, 맵·Blueprint·UMG 편집 및 PIE 확인. Codex는 별도 요청 없이 빌드하지 않음 |

입장 제한은 2인 전용 요구를 적용하기 위한 정책이다. 재접속 시 남아 있는 플레이어의 이름과 겹치지 않게 빈 이름을 부여하며, 선공은 이름이 아닌 현재 접속 순서를 기준으로 한다. 필수 단계의 자유 입력 경쟁을 먼저 검증한 뒤 도전 단계에서 턴제로 확장한다. 별도 모드 선택 UI는 현재 범위에 추가하지 않는다.

## 1. 현재 확인한 프로젝트

| 항목 | 확인 결과 |
| --- | --- |
| 엔진/모듈 | CH4_1.uproject: EngineAssociation 5.6, Runtime 모듈 CH4_1 |
| C++ | 기본 모듈에 LogNB 선언/정의 추가. NBGameMode/PlayerController/PlayerState/GameState 골격과 생성 로그 작성 |
| 빌드 | CH4_1.Build.cs: Core, CoreUObject, Engine, InputCore, EnhancedInput. UMG 의존성은 아직 없음 |
| Target | Game, Editor 두 Target. 별도 Server Target 없음 |
| 콘텐츠 | 사용자 생성 L_NumberBaseball.umap, BP_NBGameMode.uasset, BP_NBPlayerController.uasset 확인 |
| 시작 맵 | GameDefaultMap 및 EditorStartupMap이 /Game/Maps/L_NumberBaseball.L_NumberBaseball로 설정됨 |
| 게임 클래스 설정 | GlobalDefaultGameMode에 BP_NBGameMode 설정. 사용자 PIE 로그로 클래스 적용 확인 |
| 입력 | EnhancedInput 기본 클래스 설정. 게임 전용 입력 코드 없음 |
| 문서 | README에 M1 구현 범위와 에디터 체크리스트 링크 추가 |
| 환경 | 사용자 로그로 Listen Server + Client 실행 확인. 정식 빌드 결과 및 프로세스 설정은 별도 미확인 |
| 저장소 | 분석 시작 시 git status --short 출력 없음 |

Saved/Intermediate/Binaries 파일은 실행 성공의 증거로 삼지 않는다. 사용자가 설정한 맵/BP의 클래스 적용은 제공된 PIE 로그로 확인했다. M1은 클래스 골격 및 로그만 포함하며 게임 RPC/복제 프로퍼티는 없다.

## 2. 최종 목표 클래스 구조 (현재 네 Actor 클래스 골격만 작성)

현재 공통 진단 도구는 `NBUtility.h`다. LogNB에 함수명/줄번호를 포함하는 로그와 조건 실패 시 조기 반환 매크로를 제공한다. 화면 출력은 기본적으로 꺼져 있다. 순수 C++ 헤더로 서버/클라이언트 호출 위치에서 실행하며, 네트워크 전송이나 게임 판정을 담당하지 않는다. 사용법은 `Docs/NB_UTILITY.md`에 기록했다.

경로는 `Source/CH4_1/` 기준이며 헤더/구현 쌍은 `.h/.cpp`로 표기한다. 별도 네트워크 매니저, Subsystem, 서비스 계층, 이동용 Character는 현재 요구에 필요하지 않다.

| 클래스 / 예정 파일 | 책임 | 존재·실행 위치 |
| --- | --- | --- |
| ANBGameMode : AGameModeBase / NBGameMode.h/.cpp | 정답 생성, 요청 검증, 판정 호출, 기회 소비 지시, 승패/리셋/턴/타이머 제어 | 서버에만 존재 |
| ANBPlayerController / NBPlayerController.h/.cpp | 입력 RPC, 소유자 안내, 로컬 위젯 생성·연결 | 서버의 각 플레이어 인스턴스 + 해당 소유 클라이언트 |
| ANBPlayerState / NBPlayerState.h/.cpp | UsedAttempts, MaxAttempts와 Getter, 서버 변경 및 복제 알림 | 서버 + 모든 클라이언트 |
| ANBGameState : AGameStateBase / NBGameState.h/.cpp | 공개 라운드 상태·결과·턴·마감 시각 복제, 전체 채팅 이벤트 전달 | 서버 + 모든 클라이언트 |
| UNBChatWidget / UI/NBChatWidget.h/.cpp | 입력·채팅·횟수·현재 턴·타이머 표시 | 각 로컬 플레이어 |
| UNBResultWidget / UI/NBResultWidget.h/.cpp | 승리/무승부 공지 표시 | 각 로컬 플레이어 |
| UNBNumberBaseballLibrary : UBlueprintFunctionLibrary / NBNumberBaseballLibrary.h/.cpp | 입력 검증과 S/B 비교의 순수 계산 | 판정 시 서버 GameMode에서 호출, 독립 계산 테스트 가능 |
| NBGameTypes.h | 여러 클래스가 공유해야 하는 판정/라운드 상태 구조체와 enum만 정의 | 값 타입 |

사용자 결정에 따라 라이브러리는 UBlueprintFunctionLibrary 하나로 구현한다. 평가표가 의도한 형태는 확인되지 않았으나 프로젝트의 구현 방식은 확정했다. GameMode에 GenerateRandomNumbers(), CheckAnswer(), ResetGame()이라는 규칙 진입점을 두고, CheckAnswer가 순수 계산 라이브러리를 호출한다. 입력 검증 및 비교 외의 진행 로직은 분산하지 않는다.

에셋 제안: `Content/Maps/L_NumberBaseball.umap`, `Content/Blueprints/BP_NBGameMode.uasset`, `Content/Blueprints/BP_NBPlayerController.uasset`, `Content/UI/WBP_NBChat.uasset`, `Content/UI/WBP_NBResult.uasset`. Blueprint는 클래스 기본값 및 화면 배치에 사용하고 핵심 규칙은 C++에 둔다. 필요한 단계에서만 생성한다.

## 3. 데이터 소유와 수정 권한


여기서 데이터 보관과 네트워크 owning connection을 구분한다. 클라이언트가 PlayerController를 소유한다는 것은 요청 RPC의 경로를 가진다는 뜻이며, 서버 게임 상태를 임의로 수정할 권한이 있다는 뜻이 아니다.

| 데이터 | 보관 주체 | 수정 권한 | 전송 방식 |
| --- | --- | --- | --- |
| 입력 중인 문자열 | 로컬 Widget | 로컬 사용자 | 제출 시에만 PC Server RPC |
| 비밀 정답 3자리 | GameMode | 서버 | 복제하지 않음 |
| UsedAttempts / MaxAttempts(기본 3) | PlayerState | 서버 | 모든 클라이언트로 RepNotify |
| 남은 기회 | PlayerState Getter | 파생값 | MaxAttempts - UsedAttempts, 별도 복제 안 함 |
| RoundId / 단계 / 결과 / 승자 이름·ID | GameState의 라운드 상태 구조체 | 서버 | RepNotify; 결과 표시의 기준 |
| 이번 라운드 참가자 목록 | GameMode | 서버 | 서버 판정용. 표시용 플레이어 정보는 PlayerState 사용 |
| 현재 턴 플레이어 / TurnId / TurnEndServerTime | GameState의 턴 상태 구조체 | 서버 | RepNotify, M9/M10에서 추가 |
| 타이머 핸들 / 현재 턴 처리 여부 | GameMode | 서버 | 복제하지 않음 |
| 채팅·숫자 판정 메시지 | 서버 생성 → 로컬 로그 UI | 서버가 발신자/결과 확정 | GameState NetMulticast, 지속 기록 복제 안 함 |
| 잘못된 입력 안내 | 해당 PC → 로컬 UI | 서버 판단 | owning Client RPC |

승자 표시 정보는 서버 PlayerState에서 가져온 값으로 라운드 결과에 저장해 접속 종료 후에도 공지에 사용할 수 있게 한다. 정답은 클라이언트 메시지나 복제 상태에 포함하지 않는다.

## 4. 입력과 네트워크 흐름

```text
로컬 WBP_NBChat
  → 소유 NBPlayerController: 제출 텍스트 + 현재 RoundId (도전 시 TurnId)
  → ServerSubmitGuess [Server, Reliable]
서버의 NBPlayerController
  → 자신에게 연결된 PlayerState 식별 (발신자가 보낸 ID 사용 안 함)
  → NBGameMode::CheckAnswer / 제출 처리 (서버 일반 함수 호출)
  → 단계·참가 여부·기회·라운드·입력 검증 (도전 시 턴/마감도 검증)
      실패: ClientShowInputError [Client, Reliable] → 제출자 UI; 기회 유지
      성공: 규칙 라이브러리로 S/B 계산
          → PlayerState UsedAttempts를 정확히 1 증가
          → 3S 승리 우선 확인, 그다음 전체 기회 소진 무승부 확인
          → GameState 공개 결과/단계 변경
          → GameState MulticastReceiveMessage [NetMulticast, Reliable]
          → PlayerState/GameState Replication
각 클라이언트
  → 메시지 이벤트는 채팅 표시
  → RepNotify는 현재 상태를 읽어 횟수/결과 UI 갱신
```

일반 채팅은 `ServerSendChat` → GameMode의 길이/내용 확인 → GameState Multicast 경로를 사용하며 기회를 소비하지 않는다. 같은 화면에서 일반 채팅 입력창과 숫자 제출창을 분리한다. `abc`를 숫자 제출로 보냈다면 일반 채팅으로 우회하지 않고 잘못된 입력으로 안내한다. 일반 채팅창에 입력한 `123`은 채팅이며 숫자 시도가 아니다.

GameMode에는 클라이언트 수신 인스턴스가 없으므로 GameMode Multicast를 설계하지 않는다. PC도 다른 클라이언트에는 존재하지 않으므로 전체 전송은 GameState에서 한다. NetMulticast는 현재 연결되어 있고 Actor를 수신할 수 있는 클라이언트의 일회성 이벤트용이다. 늦게 들어온 플레이어에게 과거 채팅을 재생하지 않는다. 결과·횟수는 복제 상태에서 복원한다.

RPC와 서로 다른 Actor 상태의 도착 순서는 가정하지 않는다. 채팅 판정 메시지에는 서버가 계산한 UsedAttempts/MaxAttempts와 RoundId를 함께 담아 `[사용/최대]`를 일관되게 표시한다. 현재 횟수 UI는 PlayerState를 읽는다. 라운드/턴 관련 값은 작은 구조체로 묶되 서로 다른 Actor 사이의 원자적 갱신까지 보장한다고 가정하지 않는다.

Listen Server의 로컬 UI는 C++ 서버 상태 변경 시 공통 갱신 경로로 갱신한다. 원격 클라이언트는 OnRep에서 같은 갱신 경로를 사용한다. 위젯 생성 시 현재 상태를 읽고, GameState/PlayerState가 아직 준비되지 않으면 생명주기 콜백에서 연결한다. 서버 전용 실행에서는 위젯을 만들지 않는다.

## 5. 규칙과 상태 전이

- 정답: 서버가 1~9에서 중복 없이 3개 생성. 숫자 문자열 또는 배열 중 한 표현을 선택하여 중복 저장하지 않는다.
- 입력: 정확히 ASCII '1'~'9' 세 글자, 중복 금지. 0, 부호, 소수점, 공백, 전각 숫자 등을 거절한다. UI가 검증해도 서버에서 재검증한다.
- 사용 횟수는 0 → 3 증가한다. UI `[1/3]`은 사용 횟수이며 '남은 기회 2/3'와 혼동하지 않는다.
- 유효한 시도만 소비한다. 잘못된 입력, 기회 소진 후 제출, 이전 라운드/턴 요청은 소비하지 않는다.
- 3번째 시도에서 3S라면 승리다. 무승부 확인은 승리 확인 다음에 한다.
- 필수 모드는 자유 입력 경쟁이다. 서버가 먼저 정상 처리한 3S를 승리로 확정하고 이후 요청은 종료 상태 검사로 거절한다.
- 단계: Waiting → Playing → ShowingResult → (초기화) Playing 또는 Waiting.
- 2인 전용이며 두 명 접속 후 시작한다. 정원이 찬 상태의 추가 접속은 거절한다. 한 명이 이탈하면 라운드를 중단하고 Waiting으로 돌아간다. 다시 두 명이 되면 횟수와 정답을 초기화한 새 라운드를 시작한다. 이탈 자체를 승리/무승부로 기록하지 않는다. 호스트 종료는 Listen Server 종료이므로 클라이언트 연결도 끊기며 호스트 이전은 구현하지 않는다.
- 결과는 3초 표시 후 서버 타이머가 초기화한다. 즉시 리셋하면 복제 전에 결과가 사라질 수 있으므로 ShowingResult를 유지한다.
- 리셋은 같은 월드에서 입력 차단 → 이전 타이머 정리 → 횟수 초기화 → 새 정답 생성 → RoundId 증가 → 참가자 확정 → 재개 순서로 처리한다. 맵 재로드는 필요하지 않다.
- 도전 모드는 서버가 턴을 정한다. 성공한 유효 입력 또는 시간 초과 중 한 경로만 기회를 소비하고 턴을 끝낸다. 잘못된 입력은 턴을 유지하고 시간도 초기화하지 않는다.
- TurnId와 처리 플래그로 동일 턴의 중복 처리를 막는다. 서버 현재 시각이 마감 시각 이상이면 늦은 입력을 거절하고 아직 미처리인 시간 초과를 한 번만 처리한다.
- 종료 시각은 서버가 계산/복제한다. UI는 `max(0, TurnEndServerTime - GameState.GetServerWorldTimeSeconds())`로 표시한다. 매 프레임 남은 시간을 RPC로 보내지 않는다. 화면 간 프레임 단위 차이는 가능하며 최종 판정은 서버 기준이다.
- 턴 시간은 20초다. 각 라운드는 현재 참가자의 접속 순서로 선공을 정한다. 소비 완료 플레이어는 건너뛰고, 승패 판정 후에만 다음 턴으로 간다.

## 6. 구현 시 주의할 사항

1. 게임 정책과 역할 분담은 위 확정 요구사항을 따른다. 화면 색상·배치 같은 세부 디자인은 UI 단계에서 정한다.
2. 에디터 작업은 사용자가 수행한다. Codex는 필요한 클래스/변수 이름, 설정 순서, 테스트 입력과 기대 결과를 안내하며 사용자 확인을 근거로 수동 검증 결과를 기록한다.
3. 결과 메시지와 PlayerState 복제는 서로 다른 시점에 도착할 수 있다. 메시지의 횟수 스냅샷과 현재 상태 UI를 구분하고 실제 양쪽 화면을 검증해야 한다.
4. UE 헤더 변경 후 Live Coding 결과만으로 UHT/링크 성공을 판단하지 않는다. 필요 시 사용자가 Editor를 닫은 정식 빌드로 검증한다. 결과 전달 전에는 사용자 빌드 확인 대기로 기록한다.
5. PIE 설정, 맵 World Settings override, Blueprint 클래스 연결, 포커스 및 BindWidget 연결은 C++ 컴파일로 보장할 수 없다.
6. Listen Server 호스트만 테스트하면 owning connection, RepNotify, 원격 UI 초기화 오류를 놓칠 수 있다. 최소 호스트+원격 클라이언트를 검증한다.

## 7. 참고 근거

프로젝트 사실은 로컬 파일 확인 결과다. 아래는 네트워크 설계에 참고한 Epic 공식 문서이며 실제 UE 5.6 API 시그니처는 구현 시 설치된 엔진 헤더와 대조한다.

- [Game Mode and Game State](https://dev.epicgames.com/documentation/en-us/unreal-engine/game-mode-and-game-state-in-unreal-engine): GameMode의 서버 전용 역할과 GameState의 공개 상태.
- [Remote Procedure Calls](https://dev.epicgames.com/documentation/unreal-engine/remote-procedure-calls-in-unreal-engine?lang=en-US): RPC 실행 대상과 owning connection.
- [APlayerController](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/APlayerController): 서버 및 소유 클라이언트의 인스턴스 범위.
