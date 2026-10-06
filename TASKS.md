# 숫자 야구 작업 계획

기준일: 2026-10-06. 제출: 2026-10-12(월) 12:00, GitHub 저장소 하나(필수+도전)와 MP4 시연 영상.

## 상태와 완료 규칙

- `[x]`는 실제 수행 및 확인한 작업만 의미한다. 계획이나 예상 성공은 체크하지 않는다.
- 현재 상태: M0 완료. M1 코드/맵 연결 및 2인 PIE 클래스 생성 범위 확인 완료(사용자 제공 로그). 정식 Editor Target 빌드 결과는 별도 전달되지 않아 확인 대기로 남긴다.
- M2·M3 완료: 사용자 빌드/규칙 테스트/위젯/PIE 검증 완료 보고. 다음 단계 M4는 미착수. M1 당시의 별도 빌드 로그는 미수집 상태로 유지한다.
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

## M4. PlayerState 시도 횟수 — 미착수

- 목표: 플레이어별 사용 횟수와 최대 횟수를 독립 관리하고 표시.
- 수정 파일: NBPlayerState.h/.cpp, NBGameMode.h/.cpp, NBPlayerController.h/.cpp, NBGameState.h/.cpp, UI/NBChatWidget.h/.cpp, Content/UI/WBP_NBChat.uasset.
- 클래스 책임: PS 보관/Getter/서버 변경, GM 소비 여부 결정, UI 조회/표시.
- 실행 위치: 횟수 수정 서버, 표시 로컬.
- RPC/Replication: UsedAttempts/MaxAttempts RepNotify 등록. 판정 메시지는 해당 시점 횟수 스냅샷 포함.
- 완료 조건: 유효 입력만 1 증가, 3회 소진 후 거절, 일반 채팅/오입력은 유지. 호스트 UI도 갱신.
- 검증: C + P(A의 입력이 B 횟수를 바꾸지 않음, 0→1→2→3, 4번째 거절, 오입력 전후 비교) + H(`[사용/최대]`).
- [ ] M4-1 횟수 필드·Getter·복제 등록
- [ ] M4-2 서버 검증 후 정확히 한 번 소비
- [ ] M4-3 RepNotify/호스트 공통 갱신·초기 UI 조회
- [ ] M4-4 판정 메시지에 횟수 표시
- [ ] M4-5 빌드·2인 PIE 검증(소진 후 채팅/관전 유지 포함) — 수동 검증 필요

## M5. 라운드 상태·승리·무승부 — 미착수

- 목표: 서버가 승패를 한 번 확정하고 공개 상태로 유지.
- 수정 파일: NBGameMode.h/.cpp, NBGameState.h/.cpp, NBGameTypes.h, NBPlayerController.h/.cpp.
- 클래스 책임: GM 참가자/진행/승패, GS RoundId·단계·결과·승자 표시값 보관.
- 실행 위치: 판정 서버, 상태 조회 클라이언트.
- RPC/Replication: 라운드 상태 RepNotify; ServerSubmitGuess에 RoundId 전달/검증. 결과의 근거는 Multicast가 아닌 상태.
- 완료 조건: 먼저 처리된 3S 승리, 전원 소진만 무승부, 종료 이후 입력 거절. M6 전에는 결과 상태 유지.
- 검증: C + P(1·3번째 시도 승리, 한 명만 소진 시 계속, 전원 소진 무승부, 연속 제출 후 승자 불변, 입장/이탈) + H(로그/임시 채팅 표시).
- [ ] M5-1 확정 정책 적용: 2인 자동 시작·정원 초과 거절·이탈 시 중단/대기·재접속 후 새 라운드
- [ ] M5-2 Waiting/Playing/ShowingResult와 RoundId
- [ ] M5-3 승리 우선 판정, 이후 무승부 판정
- [ ] M5-4 종료 상태·이전 라운드 입력 차단
- [ ] M5-5 빌드·2인 PIE 검증 — 수동 검증 필요

## M6. 결과 공지 UI — 미착수

- 목표: 리셋 전에 모든 플레이어가 결과를 볼 수 있는 UI 완성.
- 수정 파일: UI/NBResultWidget.h/.cpp, NBPlayerController.h/.cpp, NBGameState.h/.cpp, Content/UI/WBP_NBResult.uasset, Content/Blueprints/BP_NBPlayerController.uasset.
- 클래스 책임: GS 상태 알림, PC 로컬 위젯 수명/연결, ResultWidget 표시.
- 실행 위치: 위젯은 로컬, 결과값은 서버 작성.
- RPC/Replication: 기존 라운드 RepNotify 활용, 결과 표시용 신규 RPC 불필요.
- 완료 조건: 승자/무승부 양쪽 표시, 위젯 늦은 생성에도 현재 상태 표시, 중복 생성 없음.
- 검증: C + P(호스트/원격 결과 일치) + H(승자 이름·무승부·입력 잠금·레이아웃).
- [ ] M6-1 결과 위젯 C++ 및 UMG 연결
- [ ] M6-2 상태 알림과 초기 스냅샷 연결
- [ ] M6-3 빌드·두 창 결과 표시 검증 — 수동 검증 필요

## M7. 결과 표시 후 게임 리셋 — 미착수

- 목표: 공지 시간을 확보한 뒤 동일 월드에서 새 라운드 시작.
- 수정 파일: NBGameMode.h/.cpp, NBGameState.h/.cpp, NBPlayerState.h/.cpp, NBPlayerController.h/.cpp, UI/NBChatWidget.h/.cpp, UI/NBResultWidget.h/.cpp.
- 클래스 책임: GM ResetGame/타이머, PS 횟수 초기화, GS 새 라운드 상태, UI 결과 닫기/입력 복구.
- 실행 위치: 초기화 서버, 표시 갱신 로컬.
- RPC/Replication: 기존 라운드/횟수 복제. 이전 RoundId 요청 거절.
- 완료 조건: 공지 후 횟수 0, 새 정답 생성 호출, RoundId 증가, 입력 재개. 무작위 정답은 우연히 이전과 같을 수 있으므로 숫자 불일치를 완료 기준으로 삼지 않는다.
- 검증: C + P(승리/무승부 후 각각 반복 라운드, 결과 중 입력, 이전 요청 거절, 인원 부족 시 Waiting) + H(공지 표시 시간·닫힘).
- [ ] M7-1 결과 3초 표시와 서버 리셋 타이머(정답 비공개)
- [ ] M7-2 ResetGame과 타이머/횟수/결과 초기화
- [ ] M7-3 새 라운드 및 UI 입력 복구
- [ ] M7-4 빌드·반복 2인 PIE 검증 — 수동 검증 필요

## M8. 필수 기능 통합 검증과 설명 문서 — 미착수

- 목표: 필수 기능을 완주하고 사용자가 코드 흐름을 설명할 수 있도록 정리.
- 수정 파일: README.md, TASKS.md, ARCHITECTURE.md. 실패 발견 시 관련 파일만 수정하고 원인 기록.
- 클래스 책임: 앞 단계 책임 유지; 신규 클래스 없음.
- 실행 위치: 기존 서버/클라이언트 흐름 전체.
- RPC/Replication: 기존 경로 회귀 검증; 추가 없음.
- 완료 조건: 필수 시나리오 실행 증거와 README 구현 과정·실행법·제약 기록.
- 검증: C/L 재확인은 코드 변경 또는 미검증 범위에 한정. P/H 필수 시나리오를 2명으로 완주, 가능하면 별도 프로세스/서버 전용 PIE 검증.
- [ ] M8-1 정상/오입력/횟수 소진/승리/무승부/리셋 회귀
- [ ] M8-2 원격 클라이언트 관점·호스트 관점 비교
- [ ] M8-3 README 소개·구현 과정·역할·RPC 흐름·실행 방법
- [ ] M8-4 미검증 항목과 실제 구조 갱신

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
