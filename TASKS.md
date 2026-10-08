# 숫자 야구 작업 계획

기준일: 2026-10-06. 제출: 2026-10-12(월) 12:00, GitHub 저장소 하나(필수+도전)와 MP4 시연 영상.

## 상태와 완료 규칙

- `[x]`는 실제 수행 및 확인한 작업만 의미한다. 계획이나 예상 성공은 체크하지 않는다.
- 현재 상태: M0 완료. M1 코드/맵 연결 및 2인 PIE 클래스 생성 범위 확인 완료(사용자 제공 로그). 정식 Editor Target 빌드 결과는 별도 전달되지 않아 확인 대기로 남긴다.
- M2~M5 사용자 검증 완료 보고. M5 이탈·재접속은 별도 프로세스 실행 안내 후 사용자 완료 보고를 받았다. M6 사용자 검증 완료. M7 사용자 기능 검증 완료(종료 크래시 제외). M1 당시의 별도 빌드 로그는 미수집 상태로 유지한다.
- M1~M8 필수 기반/기능, M9~M10 도전, M11 제출 정리. 한 번에 하나의 milestone만 진행한다.
- 파일 경로 표에서 `.h/.cpp`는 두 파일을 뜻하고, C++ 경로는 `Source/CH4_1/` 기준이다. 에셋과 설정은 루트 기준이다.
- 각 milestone 종료 시 아래 검증 기록 양식을 채운다. 실행 검증이 남으면 완료 처리하지 않는다.
- 2026-10-06: 사용자가 추천 요구사항을 승인했다. 확정 내용은 ARCHITECTURE.md의 '확정 요구사항'을 따른다. 라이브러리는 UBlueprintFunctionLibrary 하나이며, 최종 범위는 도전 턴제·타이머까지다.
- 담당: Codex는 C++·문서·정적 검토 및 검증 절차 안내, 사용자는 빌드 확인과 모든 맵/Blueprint/UMG 작업 및 PIE 검증. Codex는 별도 요청이 없으면 빌드를 실행하지 않는다. 아래 수정 파일의 에셋은 사용자 작업 대상이다. 사용자 결과를 받기 전에는 빌드/실행 검증을 완료 처리하지 않는다.

## 공통 검증 방법

**C — 컴파일(사용자 담당):** 사용자가 UE 5.6의 CH4_1Editor Win64 Development 빌드로 UHT, C++ 및 링크를 확인한다. Codex는 빌드를 실행하지 않고, 결과를 전달받기 전까지 '사용자 빌드 확인 대기'로 기록한다. 엔진 Build.bat 경로 존재만 확인했으며 현재 빌드는 미실행이다. 아래 모든 milestone의 C/빌드 항목에 이 담당 구분을 적용한다.

**L — 계산 검증:** 네트워크와 분리된 입력 검증/S·B 판정에 작은 Automation Test를 사용한다. 구현 코드를 그대로 반복한 테스트 대신 경계값과 기대 결과를 검증한다.

**P — 실행:** 최소 2 Player PIE, Listen Server 호스트 1 + 원격 Client 1, 각각 별도 창으로 확인한다. 발신자, NetMode, RoundId, UsedAttempts 등 필요한 개발 로그로 서버 변경을 추적한다. 후속 통합 검증에서는 가능하면 별도 프로세스 PIE 및 dedicated server + 2 clients도 확인한다. 후자는 현재 Server Target 존재를 전제로 하지 않으며 환경 지원부터 확인한다.

**H — 사람 확인:** 맵/클래스 기본값, 위젯 연결·포커스·레이아웃, 양쪽 창 메시지·결과·타이머 표시. 직접 확인하지 못하면 반드시 **수동 검증 필요**라고 기록한다.

## M0. 현재 구조 분석과 설계 — 완료(문서 단계)

- 목표: 실제 프로젝트 확인, 책임/흐름/단계/검증 계획 기록.
- 수정 파일: 루트 AGENTS.md, TASKS.md, ARCHITECTURE.md 신규 생성.
- 클래스 책임: 현재 모듈만 존재함을 확인하고 예정 책임을 정의.
- 실행 위치: 개발 시 정적 분석, 게임 Server/Client 실행 없음.
- RPC/Replication: 설계만 정의, 추가 없음.
- 완료 조건: 기존 소스/설정/콘텐츠 확인 및 세 문서 작성. 기능 코드 변경 없음.
- 검증: 파일 읽기, 문서 교차 검토, git diff/status. C/P/H는 이번 단계에서 미실행.
- [x] M0-1 기존 소스·Target·Build.cs·설정·README 확인
- [x] M0-2 Content 및 Git 상태 확인
- [x] M0-3 서버 권한과 네트워크 흐름 설계
- [x] M0-4 세 작업 문서 및 검증 기준 작성
- [x] M0-5 추천 요구사항·라이브러리 형태·에디터 작업 분담 확정 및 문서 반영

## M1. 최소 게임 클래스 연결과 PIE 기반 — PIE 검증 완료, 정식 빌드 결과 확인 대기

- 목표: 실제 게임 클래스가 올바른 위치에 생성되는지 먼저 확인.
- 수정 파일: NBGameMode.h/.cpp, NBPlayerController.h/.cpp, NBPlayerState.h/.cpp, NBGameState.h/.cpp, Config/DefaultEngine.ini, Content/Maps/L_NumberBaseball.umap, Content/Blueprints/BP_NBGameMode.uasset, BP_NBPlayerController.uasset.
- 클래스 책임: GameMode가 PC/PS/GS 클래스 지정, 각 클래스는 최소 골격과 확인용 로그만 담당.
- 실행 위치: GameMode 서버 전용, PC 서버/소유 클라이언트, PS/GS 서버/클라이언트.
- RPC/Replication: 신규 게임 RPC/프로퍼티 없음. 엔진 기본 Actor 생성 범위 확인.
- 완료 조건: 맵에 커스텀 클래스가 적용되고 호스트/원격 클라이언트 구분 확인.
- 검증: C + P(2인 접속·클래스/NetMode 로그) + H(World Settings override, 시작 맵).
- [x] M1-1 NB 접두사의 네 클래스 골격 추가 및 GameMode 기본 클래스 지정
- [x] M1-1a 사용자 요청: NBUtility.h 로그/조기 반환 유틸리티 추가, 기존 M1 로그 적용 및 사용법 문서 작성(빌드 미검증)
- [x] M1-2 테스트 맵·Blueprint 기본값 및 GameMode 연결 — 에셋/Config와 사용자 실행 로그로 실제 클래스 적용 확인
- [ ] M1-3 Editor Target 빌드 — 사용자 빌드 확인 대기
- [x] M1-4 2 Player PIE에서 클래스 생성 범위 검증 — 사용자 제공 Listen Server/Client 로그로 확인

## M2. 기본 멀티플레이 채팅 — 완료(사용자 검증)

- 목표: 소유 클라이언트 → 서버 → 모든 플레이어 메시지 흐름 확립.
- 수정 파일: NBPlayerController.h/.cpp, NBGameMode.h/.cpp, NBGameState.h/.cpp, UI/NBChatWidget.h/.cpp, CH4_1.Build.cs, Content/UI/WBP_NBChat.uasset, Content/Blueprints/BP_NBPlayerController.uasset.
- 클래스 책임: Widget 입력/표시, PC RPC/위젯 생성, GM 서버 메시지 검증, GS 전체 이벤트 전달.
- 실행 위치: UI 로컬, 발신자 확인/메시지 구성 서버, 수신 UI 각 로컬.
- RPC/Replication: ServerSendChat, GS MulticastReceiveMessage; 잘못된 입력은 ClientShowInputError. 모두 낮은 빈도 Reliable. 채팅 이력 복제 없음.
- 완료 조건: 양쪽이 각각 보낸 메시지가 양쪽에 한 번씩 표시, 발신자는 서버 기준.
- 검증: C + P(호스트→클라이언트, 클라이언트→호스트, 빈 입력·과도한 길이) + H(Enter 제출/포커스/중복 표시).
- [x] M2-1a UMG/SlateCore 의존성 및 위젯 입력 C++ 연결 작성
- [x] M2-1b 사용자 WBP_NBChat 생성 및 BP_NBPlayerController의 ChatWidgetClass 연결 — 사용자 검증 완료 보고
- [x] M2-2 소유 PC의 Client → Server RPC 코드 작성
- [x] M2-3 서버에서 Player 1 / Player 2 이름 부여 및 발신자·문자열 처리 코드 작성
- [x] M2-4 GameState Multicast → 로컬 채팅 표시 코드 작성
- [x] M2-5 빌드 및 2 Player PIE 검증 — 사용자 검증 완료 보고, 문제 없음

## M3. 정답 생성·입력 검증·판정 — 완료(사용자 검증)

- 목표: 서버에서만 정답을 만들고 유효한 숫자의 S/B/OUT을 계산.
- 수정 파일: NBGameMode.h/.cpp, NBPlayerController.h/.cpp, NBNumberBaseballLibrary.h/.cpp, NBGameTypes.h, UI/NBChatWidget.h/.cpp, Content/UI/WBP_NBChat.uasset, Tests/NBNumberBaseballTests.cpp.
- 클래스 책임: GM 정답/판정 진입점, 라이브러리 순수 검증·비교, PC 제출 통로, Widget 채팅과 숫자 제출 구분.
- 실행 위치: 최종 검증/판정 서버. UI는 요청/표시만.
- RPC/Replication: ServerSubmitGuess, ClientShowInputError, 기존 Multicast 메시지. 정답 복제 금지.
- 완료 조건: 유효 입력의 정확한 판정과 잘못된 입력 안내. 이 단계에서는 횟수 제한·승패 미구현.
- 검증: C + L + P(같은 정답 기준 양쪽 결과) + H(채팅/숫자 UX).
- [x] M3-1 서버 GenerateRandomNumbers 및 개발 로그 코드 작성
- [x] M3-2 정확히 3자리·1~9·중복 금지 검사 코드 작성
- [x] M3-3 CheckAnswer 및 순수 계산 라이브러리 코드 작성
- [x] M3-4a 별도 숫자 입력창용 C++ 바인딩, 제출 RPC와 개인 오류 안내 연결
- [x] M3-4b 사용자 WBP_NBChat에 GuessInput/GuessButton 추가 및 연결 확인
- [x] M3-5a 입력 오류·고정 판정 사례·504개 유효 숫자 자기 비교 Automation Test 코드 작성(미실행)
- [x] M3-5 정답 386: 127→OUT, 167→0S1B, 123→0S1B, 368→1S2B, 638→0S3B, 386→3S0B 테스트 — 사용자 검증 완료 보고
- [x] M3-6 빈 값/12/1234/112/012/abc/1a3/공백/전각 숫자 거절, 반복 생성 시 범위·중복 없음 확인 — 사용자 검증 완료 보고
- [x] M3-7 빌드·계산 테스트·2인 PIE — 사용자 검증 완료 보고

## M4. PlayerState 시도 횟수 — 완료(사용자 검증)

- 목표: 플레이어별 사용 횟수와 최대 횟수를 독립 관리하고 표시.
- 수정 파일: NBPlayerState.h/.cpp, NBGameMode.cpp, NBPlayerController.h/.cpp, UI/NBChatWidget.h/.cpp. 사용자 수정 대상: Content/UI/WBP_NBChat.uasset. 기존 GS Multicast를 재사용하므로 GS 수정 없음.
- 클래스 책임: PS 보관/Getter/서버 변경, GM 소비 여부 결정, UI 조회/표시.
- 실행 위치: 횟수 수정 서버, 표시 로컬.
- RPC/Replication: UsedAttempts/MaxAttempts RepNotify 등록. 판정 메시지는 해당 시점 횟수 스냅샷 포함.
- 완료 조건: 유효 입력만 1 증가, 3회 소진 후 거절, 일반 채팅/오입력은 유지. 호스트 UI도 갱신.
- 검증: C + P(A의 입력이 B 횟수를 바꾸지 않음, 0→1→2→3, 4번째 거절, 오입력 전후 비교) + H(`[사용/최대]`).
- [x] M4-1 횟수 필드·Getter·복제 등록 코드 작성
- [x] M4-2 서버 검증 후 정확히 한 번 소비하는 코드 작성
- [x] M4-3 RepNotify/호스트 공통 갱신·초기 UI 조회 코드 작성
- [x] M4-4 판정 메시지에 횟수 표시 코드 작성
- [x] M4-4a 사용자 WBP_NBChat에 AttemptsText 추가 및 연결 확인 — 사용자 완료 보고
- [x] M4-5 빌드·2인 PIE 검증(소진 후 채팅/관전 유지 포함) — 사용자 완료 보고

## M5. 라운드 상태·승리·무승부 — 완료(사용자 검증)

- 목표: 서버가 승패를 한 번 확정하고 공개 상태로 유지.
- 수정 파일: NBGameMode.h/.cpp, NBGameState.h/.cpp, NBGameTypes.h, NBPlayerController.h/.cpp, NBPlayerState.h/.cpp, UI/NBChatWidget.h/.cpp. PS는 재접속 새 라운드의 횟수 초기화, Widget은 단계와 횟수를 함께 확인하는 입력 차단을 담당. 에셋 추가 없음.
- 클래스 책임: GM 참가자/진행/승패, GS RoundId·단계·결과·승자 표시값 보관.
- 실행 위치: 판정 서버, 상태 조회 클라이언트.
- RPC/Replication: 라운드 상태 RepNotify; ServerSubmitGuess에 RoundId 전달/검증. 결과의 근거는 Multicast가 아닌 상태.
- 완료 조건: 먼저 처리된 3S 승리, 전원 소진만 무승부, 종료 이후 입력 거절. M7 자동 재시작 구현 전에는 결과 상태 유지.
- 검증: C + P(1·3번째 시도 승리, 한 명만 소진 시 계속, 전원 소진 무승부, 연속 제출 후 승자 불변, 입장/이탈) + H(로그/임시 채팅 표시).
- [x] M5-1 확정 정책 코드 작성: 2인 자동 시작·정원 초과 거절·이탈 시 중단/대기·재접속 후 새 라운드
- [x] M5-2 Waiting/Playing/ShowingResult와 RoundId 복제 코드 작성
- [x] M5-3 승리 우선 판정, 이후 무승부 판정 코드 작성
- [x] M5-4 종료 상태·이전 라운드 입력 차단 코드 작성
- [x] M5-4a 초기 상태 조회·호스트/원격 상태 알림·기존 채팅창 결과 표시·횟수와 단계 결합 코드 작성
- [x] M5-4b 정적 검토 및 Docs/M5_EDITOR_CHECKLIST.md 작성
- [x] M5-5 빌드·2인 실행 검증 — 사용자 완료 보고. 이탈·재접속은 PIE 외 별도 프로세스 실행 안내 후 확인

## M6. 결과 공지 UI — 완료(사용자 검증)

- 목표: 리셋 전에 모든 플레이어가 결과를 볼 수 있는 UI 완성.
- 수정 파일: UI/NBResultWidget.h/.cpp, NBPlayerController.h/.cpp. GS는 M5 알림을 그대로 재사용하여 수정 없음. 사용자 작업: Content/UI/WBP_NBResult.uasset, Content/Blueprints/BP_NBPlayerController.uasset.
- 클래스 책임: GS 상태 알림, PC 로컬 위젯 수명/연결, ResultWidget 표시.
- 실행 위치: 위젯은 로컬, 결과값은 서버 작성.
- RPC/Replication: 기존 라운드 RepNotify 활용, 결과 표시용 신규 RPC 불필요.
- 완료 조건: 승자/무승부 양쪽 표시, 위젯 늦은 생성에도 현재 상태 표시, 중복 생성 없음.
- 검증: C + P(호스트/원격 결과 일치) + H(승자 이름·무승부·입력 잠금·레이아웃).
- [x] M6-1a 결과 위젯 C++ 및 로컬 PC 생성/정리 코드 작성
- [x] M6-1b 사용자 WBP_NBResult 생성, ResultText 바인딩, ResultWidgetClass 지정 — 사용자 완료 보고
- [x] M6-2 상태 알림과 초기 스냅샷 연결, 채팅 입력을 막지 않는 표시/숨김 코드 작성
- [x] M6-2a 정적 검토 및 Docs/M6_EDITOR_CHECKLIST.md 작성
- [x] M6-3 빌드·두 창 결과 표시 검증 — 사용자 완료 보고

## M7. 결과 표시 후 게임 리셋 — 기능 검증 완료(종료 크래시 별도 미해결)

- 목표: 공지 시간을 확보한 뒤 동일 월드에서 새 라운드 시작.
- 수정 파일: NBGameMode.h/.cpp, NBPlayerController.cpp, UI/NBChatWidget.h/.cpp. GS/PS/결과 위젯은 기존 복제·초기화·표시 경로 재사용, 에셋 변경 없음.
- 클래스 책임: GM ResetGame/타이머, PS 횟수 초기화, GS 새 라운드 상태, UI 결과 닫기/입력 복구.
- 실행 위치: 초기화 서버, 표시 갱신 로컬.
- RPC/Replication: 기존 라운드/횟수 복제. 이전 RoundId 요청 거절.
- 완료 조건: 공지 후 횟수 0, 새 정답 생성 호출, RoundId 증가, 입력 재개. 무작위 정답은 우연히 이전과 같을 수 있으므로 숫자 불일치를 완료 기준으로 삼지 않는다.
- 검증: C + P(승리/무승부 후 각각 반복 라운드, 결과 중 입력, 이전 요청 거절, 인원 부족 시 Waiting) + H(공지 표시 시간·닫힘).
- [x] M7-1 결과 3초 표시와 서버 단발 타이머 코드 작성(정답 비공개)
- [x] M7-2 ResetGame의 예약 RoundId/단계 확인, 이탈/종료 타이머 정리, 기존 횟수/결과 초기화 연결
- [x] M7-3 새 라운드 및 기존 UI 입력 복구 연결, 새 RoundId에서 숫자 입력 텍스트 초기화
- [x] M7-4 빌드·반복 2인 기능 검증 — 사용자 완료 보고, 종료 크래시는 별도 미해결 이슈

## M8. 필수 기능 통합 검증과 설명 문서 — 완료(사용자 검증)

- 목표: 필수 기능을 완주하고 사용자가 코드 흐름을 설명할 수 있도록 정리.
- 수정 파일: README.md, TASKS.md, ARCHITECTURE.md, AGENTS.md, Docs/M8_INTEGRATION_CHECKLIST.md. 이번 단계 C++/에셋/설정 변경 없음.
- 클래스 책임: 앞 단계 책임 유지; 신규 클래스 없음.
- 실행 위치: 기존 서버/클라이언트 흐름 전체.
- RPC/Replication: 기존 경로 회귀 검증; 추가 없음.
- 완료 조건: 필수 시나리오 실행 증거와 README 구현 과정·실행법·제약 기록.
- 검증: C/L 재확인은 코드 변경 또는 미검증 범위에 한정. P/H 필수 시나리오를 2명으로 완주, 가능하면 별도 프로세스/서버 전용 PIE 검증.
- [x] M8-1 정상/오입력/횟수 소진/승리/무승부/리셋 회귀 — 사용자 완료 보고
- [x] M8-2 원격 클라이언트 관점·호스트 관점 비교 — 사용자 완료 보고
- [x] M8-3 README 소개·구현 과정·역할·RPC 흐름·실행 방법 작성
- [x] M8-4 실제 구조 정적 대조, 미검증 항목/종료 크래시 구분, 통합 체크리스트 작성

## M9. 도전: 타이머 없는 턴 제어 — 미착수

- 목표: 현재 플레이어의 유효 입력만 허용하고 한 번만 턴 전환.
- 수정 파일: NBGameMode.h/.cpp, NBGameState.h/.cpp, NBGameTypes.h, NBPlayerController.h/.cpp, UI/NBChatWidget.h/.cpp, Content/UI/WBP_NBChat.uasset.
- 클래스 책임: GM 턴 순서/처리 플래그, GS 현재 플레이어/TurnId, UI 턴 표시.
- 실행 위치: 서버가 턴 결정/권한 검사, 로컬은 표시/입력 보조 차단.
- RPC/Replication: 턴 상태 RepNotify, ServerSubmitGuess에 TurnId 추가/검증.
- 완료 조건: 다른 사람 입력 거절, 잘못된 입력은 같은 턴 유지, 유효 입력 한 번에 한 번 소비/전환, 소진 플레이어 건너뛰기.
- 검증: C + P(다른 사람 제출, 중복 제출, 이전 TurnId, 오입력, 마지막 기회 승리, 이탈) + H(현재 턴 표시).
- [ ] M9-1 접속 순서 기준 선공·현재 플레이어·TurnId·처리 플래그
- [ ] M9-2 서버 권한 검사와 유효 입력 후 전환
- [ ] M9-3 UI 표시/입력 보조 차단
- [ ] M9-4 빌드·2인 PIE 검증 — 수동 검증 필요

## M10. 도전: 서버 타이머와 시간 초과 — 미착수

- 목표: 서버 기준 마감 시각과 시간 초과 기회 소비 구현.
- 수정 파일: NBGameMode.h/.cpp, NBGameState.h/.cpp, NBGameTypes.h, UI/NBChatWidget.h/.cpp, Content/UI/WBP_NBChat.uasset.
- 클래스 책임: GM 서버 타이머/마감 판정, GS 종료 시각, Widget 동기화된 서버 시각으로 남은 시간 표시.
- 실행 위치: 시간 초과/기회 소비 서버, 카운트다운 표시 각 로컬.
- RPC/Replication: TurnEndServerTime을 턴 상태와 복제, 시간 표시용 Tick RPC 없음.
- 완료 조건: 무입력 한 번 소비/전환, 0초 이후 제출 거절, 제출과 타임아웃 겹쳐도 한 번만 소비. 리셋/이탈 시 이전 타이머 무효화.
- 검증: C + P(무입력, 마감 직전/직후 입력, 늦게 실행된 타이머, 오입력 후 만료, 마지막 기회 초과 무승부, 리셋) + H(두 창 카운트다운). 가능하면 네트워크 지연 조건 추가.
- [ ] M10-1 턴당 20초의 서버 종료 시각·타이머 설정(오입력 시 시간 유지)
- [ ] M10-2 클라이언트 표시 시간 계산
- [ ] M10-3 공통 턴 종료 경로로 중복 소비 방지
- [ ] M10-4 빌드·경계 시점 2인 PIE 검증 — 수동 검증 필요

## M11. 제출 자료 완성 — 미착수

- 목표: 실제 구현 범위와 검증 내용을 설명하고 재현 가능한 제출물 준비.
- 수정 파일: README.md, TASKS.md, ARCHITECTURE.md, 시연 MP4(저장 위치는 제작 시 결정).
- 클래스 책임: 신규 클래스 없음.
- 실행 위치: 시연은 서버+클라이언트, 문서 작업은 개발 환경.
- RPC/Replication: 최종 구현 설명, 추가 없음.
- 완료 조건: 필수/도전 구현 범위·실행 절차·검증 결과·한계 기록, GitHub 저장소 링크와 재생 가능한 MP4 준비.
- 검증: H(시연 영상 실제 재생, 두 플레이어 동작과 결과 확인), 문서/저장소 누락 점검. 제출 사이트 업로드는 별도 사용자 지시 범위에 따름.
- [ ] M11-1 필수/도전 체크리스트 최종 정리
- [ ] M11-2 모든 핵심 클래스와 네트워크 흐름을 사용자 관점으로 설명
- [ ] M11-3 시연 녹화 및 MP4 재생 확인
- [ ] M11-4 단일 저장소 링크와 제출 파일 확인

## 단계별 검증 기록 양식

```text
Milestone / 날짜:
변경 파일과 이유:
컴파일: 실행 명령 / 성공·실패·미실행 / 근거
계산 테스트: 사례 / 결과 / 근거
PIE: 인원 / NetMode / 프로세스 구성 / 테스트 입력 / 양쪽 결과
Editor 수동 확인: 확인 항목 / 확인자 / 결과 또는 '수동 검증 필요'
기존 동작 회귀:
남은 오류·미확정 사항:
설명할 질문: 왜 이 클래스인가? 누가 수정하는가? 왜 이 RPC/복제인가?
완료 여부:
```

### M0 기록

- 기존 모듈·설정·콘텐츠 및 Git 추적 파일을 읽고 세 문서를 작성했다.
- 소스·설정·에셋 변경 없음. 컴파일/계산 테스트/PIE 미실행(문서 단계).
- M1 이후 모든 Editor 동작은 수동 검증 필요이며 성공으로 간주하지 않는다.
- 당시 다음 작업: M1 최소 클래스 연결과 2 Player PIE 기반 확인.

### M1 코드 작업 기록 (2026-10-06)

- NBGameMode, NBPlayerController, NBPlayerState, NBGameState의 .h/.cpp를 추가했다.
- NBGameMode 생성자에 PC/PS/GS 기본 클래스를 지정했다. 기존 CH4_1.h/.cpp에 LogNB 선언/정의를 추가했다.
- 각 BeginPlay는 World/Actor/NetMode/Authority를 기록한다. PC는 LocalController도 기록하며, GameMode PostLogin은 서버 접속과 PC 클래스를 기록한다.
- 설치된 UE 5.6 헤더에서 PostLogin, 클래스 지정 필드, IsLocalController 및 ENetMode 정의를 확인했다. generated.h 위치, 선언/정의 및 Super 호출을 정적으로 검토했다.
- 빌드: 사용자 요청에 따라 미실행, 사용자 빌드 확인 대기. PIE: 수동 검증 필요.
- 에셋/Config/Build.cs 변경 없음. 에디터 단계와 기대 결과는 Docs/M1_EDITOR_CHECKLIST.md에 기록했다.
- 설명할 질문: GameMode 생성자에서 클래스를 지정하는 이유는? Authority와 LocalController는 어떻게 다른가? 왜 GameMode 로그는 클라이언트에 없는가?
- 완료 여부: 코드 작성만 완료. M1-2~M1-4 사용자 확인 전 M1 전체는 미완료.

### M1 사용자 실행 결과 확인

- 사용자 제공 LogNB 로그: NetMode=2 서버에서 GameMode 1, GameState 1, PC 2, PS 2; NetMode=3 클라이언트에서 GameMode 없음, GameState 1, PC 1, PS 2 확인.
- 서버 PC는 Authority=1이며 호스트 LocalController=1, 원격 플레이어 LocalController=0. 클라이언트 자신의 PC는 Authority=0, LocalController=1로 기대값과 일치.
- 서버 PostLogin 두 건에서 BP_NBPlayerController 적용 확인. L_NumberBaseball 맵과 BP_NBGameMode 적용도 로그로 확인.
- Content의 맵/BP 두 개와 Config의 시작 맵/기본 GameMode 설정을 읽기 전용으로 확인했다. 사용자 에셋/설정은 수정하지 않았다.
- PIE 실행 및 클래스 배치는 검증 완료. 정확한 프로세스 설정과 정식 빌드 명령/성공 결과는 로그만으로 확정하지 않으며 M1-3은 확인 대기로 유지한다.

### M2 코드 작업 기록

- PC: 로컬 위젯 생성, ServerSendChat, ClientShowInputError, UI 생성 전 메시지 임시 보관(최대 100건).
- GM: PostLogin에서 빈 Player 번호 부여, 서버 전용 이름 표와 PlayerState 이름 설정, Logout 정리. ProcessChatMessage가 원문 200자 제한/공백뿐인 입력/제어 문자를 검사하고 서버 이름으로 메시지를 만든다.
- GS: Reliable NetMulticast를 수신한 월드의 로컬 PC에만 전달. 서버의 원격 PC에 재전송하거나 발신 UI에서 먼저 표시하지 않아 중복 표시를 피한다.
- UI: BindWidget 네 개, 초기화 시 한 번만 이벤트 연결, Enter 또는 버튼 제출, 최근 100건 표시 및 스크롤. 숫자 제출 UI는 M3에 남겨둔다.
- 정적 검토: UE 5.6의 UMG 이벤트 시그니처·초기화 순서·포커스 API·GM GetGameState 확인. RPC 선언과 구현, BindWidget 이름, 서버/로컬 분기 확인.
- 빌드/실행: 수행하지 않음. 사용자 빌드 확인 대기 / 에디터·PIE 수동 검증 필요.
- 에디터 절차: Docs/M2_EDITOR_CHECKLIST.md. 사용자 기존 에셋·Config와 .github는 수정하지 않음.
- 설명할 질문: 왜 Widget에서 직접 Server RPC를 호출하지 않는가? 왜 Multicast는 GS에 있는가? 왜 발신자 이름을 클라이언트로부터 받지 않는가?
- 완료 여부: M2 코드 작성만 완료, M2 전체는 미완료.

### M2 include 경로 오류 수정

- 사용자 빌드에서 UI/NBChatWidget.h를 찾지 못하는 오류 보고.
- 생성된 CH4_1.Shared.rsp에는 Source 경로만 있고 Source/CH4_1 모듈 루트 검색 경로가 없는 것을 확인했다. 하위 UI 폴더의 .cpp에서 모듈 루트 기준 include를 사용한 것이 원인이다.
- CH4_1.Build.cs에 PrivateIncludePaths.Add(ModuleDirectory)를 추가했다. 기존 파일 구조를 유지하며 모듈 내부 컴파일에 필요한 헤더 검색 기준을 명시한다.
- 사용자 빌드 재확인 대기. Codex는 빌드하지 않았다.

### M2 사용자 검증 완료

- 사용자 보고: 'M2 검증 완료. 문제 없음'. 이를 근거로 M2 빌드·위젯 연결·PIE 검증 완료를 기록한다.
- Content/UI/WBP_NBChat.uasset 존재와 BP_NBPlayerController 변경 상태를 확인했다. 에셋은 수정하지 않았다.
- 개별 테스트 로그/스크린샷은 별도 수집하지 않았으며 Codex가 직접 빌드 또는 PIE를 실행한 것은 아니다.
- M2 완료. 다음 M3는 정답 생성·입력 검증·S/B/OUT 판정이며 아직 시작하지 않았다.

### M3 코드 작업 기록

- 사용자 다음 단계 요청으로 착수. GameMode BeginPlay에서 1~9 후보 중 3개를 뽑고 제거하여 중복 없는 정답을 생성한다. 개발 서버 Output Log에만 정답을 남기며 Shipping에서는 해당 로그를 제외한다.
- UNBNumberBaseballLibrary 하나에 ValidateInput/EvaluateGuess를 작성했다. FNBGuessResult는 S/B를 보관하고 OUT 또는 nSnB 문자열을 만든다. 잘못된 인수는 false이며 OUT과 구분한다.
- UI GuessInput/GuessButton → 소유 PC ServerSubmitGuess → GM ProcessGuess/CheckAnswer → 기존 GS MulticastReceiveMessage 경로. 오류는 기존 ClientShowInputError를 사용한다.
- 정답은 private FString이며 RPC/Replication/위젯에 전달하지 않는다. 클라이언트에서 정답/판정 결과를 보내지 않는다.
- Tests/NBNumberBaseballTests.cpp 작성: 입력 오류, 오류/결과 초기화, 386 기준 사례, 유효 504개 숫자의 자기 비교 3S0B. 실행하지 않았으므로 테스트 통과로 기록하지 않는다.
- 정적 검토: RPC 선언/구현, generated.h 순서, BindWidget 이름/타입, 정답 참조 범위, diff 공백 검사. 빌드·자동화 테스트·PIE는 사용자 담당이며 확인 대기.
- M3에서는 PIE 시작 시 한 번 생성, 횟수 제한 없이 판정만 수행한다. 3S도 게임 종료/리셋하지 않는다. 2인 자동 시작·라운드 식별자는 M5, 횟수는 M4, 리셋은 M7에서 구현한다.
- 사용자 절차: Docs/M3_EDITOR_CHECKLIST.md. 기존 에셋/Config는 수정하지 않았다.
- 설명할 질문: 순수 계산 라이브러리와 서버 권한은 어떻게 다른가? 입력 오류를 왜 OUT과 구분하는가? 뽑은 후보를 제거하면 왜 중복이 생기지 않는가?

### M3 사용자 검증 완료

- 사용자 보고: '모두 검증 완료'. M3 안내 범위의 빌드·규칙 테스트·위젯 연결·2인 PIE 검증을 완료한 것으로 기록한다.
- 개별 실행 로그는 별도 수집하지 않았으며 Codex가 직접 실행한 결과가 아니다. 위 코드 작성 시점의 대기 기록은 이 사용자 확인으로 해소한다.
- M3 완료. 다음 M4는 PlayerState 시도 횟수 관리이며 아직 착수하지 않았다.

### M4 코드 작업 기록

- PS에 private UsedAttempts=0, MaxAttempts=3과 Getter/HasRemainingAttempts/TryConsumeAttempt를 추가했다. 두 필드를 RepNotify/DOREPLIFETIME로 모든 클라이언트에 복제한다.
- GM에서 제출자의 PS를 확인하고 기회 소진을 거절한다. 유효성·판정·GS 유효성 확인 후 TryConsumeAttempt를 한 번만 호출한다. 실패한 입력이나 일반 채팅은 소비하지 않는다.
- TryConsumeAttempt는 서버 권한과 남은 기회를 검사한 뒤 증가시킨다. 서버 로그와 로컬 알림을 남기고 ForceNetUpdate로 복제를 요청한다. ForceNetUpdate가 즉시 전달을 보장하는 것은 아니다.
- 원격 PS의 OnRep와 서버 변경이 같은 OnAttemptsChanged 알림을 보낸다. 이 델리게이트는 로컬 알림이지 RPC/NetMulticast가 아니다.
- 로컬 PC는 BeginPlay/OnRep_PlayerState에서 자신의 PS에 연결하고 현재 값을 읽는다. PS가 바뀌거나 EndPlay이면 기존 연결을 해제한다. 위젯보다 먼저 알림이 오거나 초기 기본값으로 OnRep가 발생하지 않는 경우에도 초기 표시를 갱신한다.
- UI에 AttemptsText를 추가하고 숫자 입력/버튼만 기회 소진 시 비활성화한다. 채팅은 유지하고 소진 시 포커스를 채팅으로 옮긴다. PS 준비 전에는 준비 중으로 표시하고 숫자 제출을 차단한다.
- 판정 메시지는 제출 당시 PS 횟수를 문자열에 포함한다. UI의 현재 횟수는 PS 복제값을 읽으며, 메시지 도착 순서와 결합하지 않는다.
- 정적 검토: UE 5.6 OnRep_PlayerState/복제 함수 시그니처와 위젯 활성 API, 서버 증가 위치, 선언/구현/BindWidget 이름, diff 공백 확인. 빌드/PIE 미실행, 사용자 확인 대기.
- Docs/M4_EDITOR_CHECKLIST.md에 경계값·호스트/클라이언트·채팅 회귀 검증을 기록했다. 에셋/Config 및 기존 계산 라이브러리는 변경하지 않았다.
- 설명할 질문: 왜 호스트에 별도 변경 알림이 필요한가? UI 차단과 서버 차단은 왜 둘 다 필요한가? 왜 메시지에 횟수를 함께 넣는가?
- M4는 승패/리셋을 구현하지 않는다. 3S라도 남은 기회가 있으면 계속 제출 가능하고, 두 사람 모두 소진하면 PIE 재시작 전까지 채팅만 가능하다.

### M4 사용자 검증 완료

- 사용자 '검증 완료' 보고를 근거로 M4 빌드/위젯 연결/일반 2인 PIE 검증 완료를 기록한다. Codex가 직접 실행한 것은 아니다.
- 지연 조건에서 실제 4번째 RPC를 도착시킨 추가 방어 테스트의 개별 결과는 제공되지 않았다. 해당 경계 테스트까지 직접 관찰한 것으로 주장하지 않는다.
- 다음 M5는 미착수. 이번에는 M1~M4 처리 흐름을 설명하며 새 기능은 추가하지 않는다.

### M5 코드 작업 기록 (2026-10-07)

- 사용자 다음 단계 요청에 따라 M5만 작업했다. 시작 시 Git 변경 사항은 없었다.
- GM PreLogin/Login 정원 검사, BeginPlay/PostLogin의 2인 시작, Logout 대기 전환을 작성했다. 기존 참가자/이름 표를 재사용하며 정답은 실제 라운드 시작 시 생성한다.
- PS ResetAttempts를 추가했다. 이탈 후 재접속 새 라운드에 필요하여 M7의 횟수 초기화 부분만 M5에서 사용한다. 결과 후 자동 리셋은 추가하지 않았다.
- FNBRoundState를 GS에서 RepNotify하며 서버 변경과 원격 OnRep의 UI 알림을 공유한다. PC의 초기 조회와 GS 생명주기 알림으로 생성 순서를 처리한다.
- 숫자 RPC에 RoundId를 추가했다. 서버는 참가 여부/라운드/단계/횟수/형식을 확인하고 3S 승리를 우선 확정한다. 한 명만 소진하면 계속하고 두 명 모두 소진한 경우에만 무승부다.
- 결과를 기존 채팅창에 표시하고 숫자 입력 활성 조건을 Playing과 남은 기회의 AND로 결합했다. 공개 결과는 Multicast 이력이 아닌 GS 스냅샷을 기준으로 한다.
- 정적 검토: 설치 UE 5.6 GameModeBase.h의 PreLogin/Login 시그니처, PreLoginAsync→PreLogin 호출, LevelActor.cpp의 Login→PostLogin 연속 처리, AActor::BeginPlay 상태 완료를 확인했다. RPC 선언/구현/호출 인자, DOREPLIFETIME/Super, 비밀 정답 참조 범위, UI 두 상태 결합 및 git diff --check를 확인했다.
- 빌드/Automation/PIE는 실행하지 않았다. 사용자 빌드 확인 대기 및 수동 검증 필요. 기존 규칙 계산 코드는 변경하지 않았다.
- Docs/M5_EDITOR_CHECKLIST.md에 시작/승리/마지막 기회/무승부/정원/재접속/서버 방어 검증을 분리했다. UI 차단 확인을 서버 거절 경로 실행 증거로 간주하지 않는다.
- AGENTS.md, TASKS.md, ARCHITECTURE.md, README.md를 실제 M5 범위로 갱신했다. 에셋·Config 변경 없음.
- 설명할 질문: 왜 결과는 GS 상태인가? 마지막 시도에서 왜 승리부터 검사하는가? 왜 RoundId와 서버 단계 검사가 함께 필요한가?
- 완료 여부: 코드/정적 검토 완료, milestone 실행 검증 대기. M6 미착수.

### M5 사용자 검증 완료

- 사용자 '검증 완료' 보고를 근거로 M5 일반 검증 완료를 기록한다. PIE 창 종료 시 전체 세션이 종료되는 문제에 대해 별도 호스트/클라이언트 프로세스 실행을 안내했고, 이어서 완료 보고를 받았다.
- Codex가 빌드/실행한 결과가 아니며 개별 실행 로그는 수집하지 않았다. 이전 RoundId 강제 제출이나 종료 후 지연 RPC 등 별도 방어 경계 테스트의 실행 여부는 확인되지 않았다.
- 위 코드 작성 시점의 검증 대기 기록은 이 사용자 확인으로 해소한다. M5 완료, M6 결과 공지 UI는 다음 단계 요청 전까지 미착수다.

### M6 코드 작업 기록

- 시작 시 Git 변경 사항 없음. 기존 GS 상태 알림과 PC 초기 조회를 재사용했다.
- UNBResultWidget의 ResultText에 서버 스냅샷의 승자 이름/무승부를 표시한다. ShowingResult의 Win/Draw만 표시하고 다른 상태는 텍스트를 비우고 숨긴다.
- PC BeginPlay에서 로컬 결과 위젯을 한 번 생성하여 ZOrder=10으로 추가한다. 생성 직후 현재 상태 조회, 이후 RefreshRoundUI 갱신, EndPlay 제거를 연결했다. 채팅 문구 중복 검사 전에 결과 위젯을 갱신한다.
- 결과 위젯은 HitTestInvisible로 표시하여 기존 채팅 클릭을 막지 않는다. GameMode/규칙/RPC/GS 코드와 에셋은 변경하지 않았다. ResultWidgetClass 누락 시 명확한 오류 로그를 남기며 기존 채팅 초기화는 계속한다.
- 정적 검토: generated.h/include/BindWidget, 선언·정의·호출, 초기 조회 및 숨김/정리 경로, UE 5.6 AddToPlayerScreen 시그니처와 HitTestInvisible 정의, git diff --check 확인.
- 빌드/Editor/PIE 미실행. 사용자 작업은 Docs/M6_EDITOR_CHECKLIST.md의 WBP_NBResult 생성·ResultWidgetClass 연결·양쪽 표시/입력/이탈 검증이다. 수동 검증 필요.
- M6 코드 작성 완료, 실행 검증 대기. M7 자동 재시작 미착수.

### M6 사용자 검증 완료

- 사용자 '검증 완료' 보고를 근거로 M6 빌드·UMG 연결·2인 결과 표시 검증 완료를 기록한다. Codex가 직접 실행한 결과는 아니다.
- 개별 실행 로그와 UI 생성/상태 도착 순서별 재현 결과는 별도로 수집하지 않았다. 해당 경계 사례까지 직접 관찰했다고 주장하지 않는다.
- 위 코드 작성 시점의 검증 대기는 사용자 확인으로 해소한다. M6 완료, M7 미착수. 이번에는 문서만 갱신한다.

### M7 코드 작업 기록

- 시작 시 Git 변경 사항 없음. 서버 FinishRound에서 3초 단발 타이머를 예약하고 라운드 ID를 delegate 인자로 보관한다.
- ResetGame은 서버 권한, 예약 ID, ShowingResult를 확인한다. 2명이면 공통 StartRound에서 횟수 0 → 정답 생성 → 새 RoundId/Playing 공개 순서로 처리한다. 최초/재접속 시작도 같은 함수를 사용한다.
- 이탈/EndPlay/새 라운드 시작에서 타이머를 해제한다. 참가자 부족 시 Waiting이며 혼자 재시작하지 않는다. 결과 UI 숨김 및 횟수 UI 복구는 기존 복제 경로를 재사용한다.
- PC는 새 RoundId를 받으면 숫자창만 비운다. 채팅 이력과 채팅 입력은 유지한다. 신규 RPC나 에셋 변경 없음.
- 정적 검토: UE 5.6 TimerManager의 delegate SetTimer/ClearTimer 시그니처, 선언/정의, ID/단계 검사, 취소/종료 경로, 초기화 순서 및 git diff --check 확인. 빌드/PIE 미실행, 사용자 검증 대기.
- Docs/M7_EDITOR_CHECKLIST.md 작성. M5/M6의 결과 유지 검증은 당시 기록이며 M7에서는 3초 후 재시작을 확인한다. M8 미착수.

### M7 종료 크래시 조사

- 사용자 종료 크래시 보고에 따라 저장된 호스트/클라이언트 로그와 엔진 소스를 읽었다. 이탈 후 Waiting 및 재접속 R2 시작은 로그로 확인했다.
- 원격 클라이언트의 엔진 종료 중 스레드풀 참조 수 assertion을 확인했다. 타이머가 직접 원인이라는 증거는 없으며 렌더링 비동기 작업 종료 연관성을 비교 검증할 예정이다.
- 상세: Docs/M7_SHUTDOWN_CRASH.md. 코드/엔진 설정 변경 없음. M7 완료 처리하지 않음.
- 사용자 비교 시험 보고: 진행 중 종료 및 -noraytracing 적용 종료 모두 동일 크래시 미재현. 원인 미확정 상태로 기록하며, 레이트레이싱 원인 확정이나 크래시 수정 완료로 간주하지 않는다. 원래 옵션의 결과 대기 중 이탈/재접속 및 M7 전체 완료 확인은 대기한다.
- 후속 사용자 보고: 최초 크래시와 동일한 상황에서도 크래시 미재현. 원래 조건 재시험 결과를 기록했다. 종료 이슈는 원인 미확정·미재현 상태로 남기며 M7 전체 기능 완료 확인은 별도 대기한다. 추가 코드 변경 없음.

### M7 사용자 기능 검증 완료

- 사용자 '일단 이 크래시를 제외하고 검증 완료했어' 보고를 근거로 M7 기능 검증을 완료 처리한다. Codex 직접 실행 결과가 아니다.
- 종료 크래시는 원인 미확정·현재 미재현인 미해결 이슈로 별도 유지한다. 수정 완료나 종료 안정성 보장으로 해석하지 않는다.
- 앞선 M7 검증 대기 기록은 이 보고로 해소한다. M8 필수 기능 통합 검증과 설명 문서는 미착수다.

### M8 정적 검토·문서 작업 기록

- 시작 시 M7 코드/문서 변경 사항이 남아 있었으며 그대로 보존했다. 이번 단계는 문서만 변경했다.
- GM/PC/PS/GS/위젯/라이브러리/기존 계산 테스트를 읽고 서버 검증·소비·승패·리셋·UI 복원 경로를 대조했다. 새 코드 수정 필요 사항은 발견하지 않았다.
- README에 규칙/조작, PIE 및 별도 프로세스 실행, 실제 RPC와 복제 흐름, 라운드 수명, 단계별 구현 과정과 검증 한계를 정리했다. ARCHITECTURE의 초기 골격 기준 표를 현재 구조로 갱신했다.
- Docs/M8_INTEGRATION_CHECKLIST.md에 2인 연속 라운드 시나리오와 코드 설명 질문을 작성했다. 기존 단계별 사용자 결과는 유지하며 M8-1/2는 새 사용자 통합 확인 전까지 대기한다.
- 빌드/Automation/PIE 미실행. C++ 변경이 없어 기존 빌드/계산 검증의 반복 실행은 요구하지 않는다. 문서 링크와 git diff --check를 확인한다. M9 미착수.

### M8 사용자 검증 완료 (2026-10-08)

- 사용자 '검증 완료' 보고를 근거로 M8 체크리스트의 2인 통합 시나리오 1~8 및 양쪽 결과 확인을 완료 처리한다. Codex가 직접 실행한 결과는 아니며 개별 로그는 별도 미수집이다.
- 종료 크래시는 원인 미확정·현재 미재현인 별도 미해결 이슈로 유지한다. 강제 이전 ID/지연 RPC 등 별도 경계 시험의 실행까지 확인한 것으로 해석하지 않는다.
- M8 완료. M9 타이머 없는 턴 제어는 다음 단계 요청 전까지 미착수. 이번 변경은 문서만이다.
