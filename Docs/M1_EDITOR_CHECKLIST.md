# M1 사용자 빌드 및 에디터 확인

상태: **사용자 제공 로그로 맵 연결 및 2인 PIE 클래스 배치 확인 완료**. 정식 Editor Target 빌드 결과는 별도 확인 대기. 아래 표의 클래스 수와 권한 기대값은 제공된 로그와 일치했다.

## 1. 빌드

1. 에디터를 닫는다. 이번에는 새 UCLASS 헤더가 추가되었으므로 정식 빌드로 확인한다.
2. 필요하면 CH4_1.uproject에서 Visual Studio 프로젝트 파일을 재생성한다.
3. CH4_1.sln을 열고 Development Editor / Win64 구성으로 프로젝트를 빌드한다(IDE 플랫폼 표기가 Win64인 경우). CH4_1Editor 대상의 UHT, C++ 컴파일 및 링크가 성공해야 한다.
4. 실패하면 최초 오류와 관련 파일/줄 번호를 공유한다. 성공 전에는 에디터 단계로 넘어가지 않는다.
5. 성공하면 CH4_1.uproject를 연다.

Codex는 이번 작업에서 빌드 명령을 실행하지 않았다.

## 2. Blueprint와 테스트 맵

1. Content 아래 Maps, Blueprints 폴더를 만든다.
2. 부모 클래스 NBPlayerController로 Blueprint Class를 만들고 `Content/Blueprints/BP_NBPlayerController`로 저장한다. All Classes 검색 또는 C++ Classes의 NBPlayerController에서 파생 Blueprint를 생성한다.
3. 부모 클래스 NBGameMode로 `Content/Blueprints/BP_NBGameMode`를 만든다.
4. BP_NBGameMode의 Class Defaults에서 다음 값을 확인/설정하고 Compile 및 Save한다.

| 항목 | 값 |
| --- | --- |
| Player Controller Class | BP_NBPlayerController |
| Player State Class | NBPlayerState |
| Game State Class | NBGameState |

PlayerState/GameState는 별도 Blueprint가 필요 없다. C++ 생성자에 기본값을 지정했지만 기존 Blueprint 기본값과 실제 적용값은 에디터에서 확인해야 한다.

5. 기본 바닥과 조명이 있는 Basic 레벨을 만들고 `Content/Maps/L_NumberBaseball`로 저장한다. M1에서는 엔진 기본 Pawn 동작을 유지하므로 Player Start 두 개를 바닥 위에 충분히 떨어뜨려 배치한다. 기존 Player Start가 있으면 재사용한다.
6. World Settings의 GameMode Override를 BP_NBGameMode로 지정한다.
7. Project Settings → Maps & Modes에서 Default GameMode를 BP_NBGameMode, Editor Startup Map과 Game Default Map을 L_NumberBaseball로 설정한다. 맵과 설정을 저장한다.

이번 단계에는 위젯이나 별도 Character를 만들지 않는다. 게임 UI가 없는 것은 현재 구현 범위상 정상이다.

## 3. 2 Player PIE

1. L_NumberBaseball을 연다.
2. Play 설정에서 Number of Players를 2, Net Mode를 Play As Listen Server로 설정한다.
3. New Editor Window (PIE)로 실행한다. 최초 확인은 Run Under One Process를 켜도 된다.
4. Output Log에서 `LogNB`를 검색한다. 한 프로세스에서는 서버/클라이언트 로그가 섞이므로 `World`와 `NetMode`를 함께 확인한다.

로그의 NetMode 숫자는 엔진 ENetMode 값이다: 0=Standalone, 1=DedicatedServer, 2=ListenServer, 3=Client. 이번 테스트에는 2와 3이 보여야 한다.

NBUtility 적용 후 로그 접두사는 `[ANBGameMode::BeginPlay:줄번호]`처럼 함수명과 줄번호를 자동으로 포함한다. 아래 표는 함수명 기준이며 World/NetMode/Authority 등 본문 필드는 동일하다. 화면 출력은 기본적으로 꺼져 있다.

| 로그 | Listen Server 월드 기대 | 원격 Client 월드 기대 |
| --- | --- | --- |
| GameMode::BeginPlay | 1개, Authority=1 | 없음 |
| GameMode::PostLogin | 호스트/원격 접속 총 2개 | 없음 |
| PlayerController::BeginPlay | 플레이어별 총 2개, Authority=1 | 자기 PC만 1개, Authority=0 |
| PlayerState::BeginPlay | 플레이어별 총 2개, Authority=1 | 복제 도착 후 총 2개, Authority=0 |
| GameState::BeginPlay | 1개, Authority=1 | 1개, Authority=0 |

서버 PC의 LocalController는 호스트 1 / 원격 플레이어 0, 원격 클라이언트 자신의 PC는 1을 기대한다. BeginPlay는 초기화 시점의 스냅샷이며 다른 Actor의 복제 완료 순서를 보장하지 않는다. 다른 값이 보이면 로그 전체와 실행 설정을 공유하여 생명주기/연결 상태를 확인한다. PostLogin의 서버 접속 로그도 함께 비교한다.

로그 순서, Actor 이름 접미사, PIE 월드 번호 자체를 고정값으로 검사하지 않는다. 이전 실행 로그를 지우고 한 번의 실행을 기준으로 비교한다. NetMode=0만 보이면 Standalone으로 실행했는지 확인한다.

## 4. 결과 공유 및 완료 판정

- 빌드 성공/실패
- 두 창 접속 여부
- GameMode, PC, PS, GS의 LogNB 로그
- 사용한 PIE 설정과 오류/경고

위 결과를 확인한 후 TASKS.md의 M1-2~M1-4를 체크한다. 코드만 존재한다고 M1 전체를 완료 처리하지 않는다. 접속 인원 제한과 자동 라운드 시작은 M5 작업이며 이번 확인 범위가 아니다.

설명해 볼 질문: 왜 GameMode가 클래스 타입을 지정하는가? 호스트의 PC에서 Authority와 LocalController가 모두 참일 수 있는 이유는? 클라이언트는 왜 상대 PC 없이 상대 PlayerState를 볼 수 있는가?
