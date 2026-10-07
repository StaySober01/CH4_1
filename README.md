# CH4_1

UE 5.6 멀티플레이 숫자 야구 개인 과제. 서버 권한과 RPC/Replication 흐름을 이해하고 설명할 수 있도록 작은 단계로 구현한다.

현재 **M1 PIE 및 M2~M6 사용자 검증 완료** 상태다. 채팅·숫자 판정·개인별 3회 제한·2인 자동 시작·승패 상태 복제와 결과 위젯 C++를 구현했다. 결과 위젯 UMG 연결도 사용자 확인을 마쳤으며, 결과 후 자동 재시작·턴 기능은 아직 없다.

| 클래스 | 현재 작성 내용 |
| --- | --- |
| NBGameMode | 2인 접속 정책, 서버 정답/입력/횟수/승패 처리, 이탈 시 대기 |
| NBPlayerController | 로컬 위젯 생성, 채팅/숫자 Server RPC, RoundId 전달, 개인 안내·상태 표시 |
| NBGameState | 일회성 메시지 Multicast, 라운드/결과 스냅샷 RepNotify |
| NBPlayerState | 서버 시도 횟수 소비/초기화, UsedAttempts/MaxAttempts 복제 및 변경 알림 |
| NBChatWidget | 채팅과 숫자 제출 분리, 최근 100건 표시, 라운드 단계와 횟수에 따른 입력 차단 |
| NBNumberBaseballLibrary | 상태를 변경하지 않는 입력 검증 및 S/B 계산 |
| FNBGuessResult | Strikes/Balls 및 OUT/nSnB 표시 문자열 |
| FNBRoundState | RoundId, Waiting/Playing/ShowingResult, 결과, 승자 이름/ID |
| NBResultWidget | 서버 라운드 스냅샷의 승자/무승부 표시, 대기/진행 시 숨김 |

GameMode가 생성할 클래스 타입을 지정하면 엔진의 기존 생성 흐름을 사용할 수 있다. PlayerController를 클라이언트에서 직접 생성하지 않는다. 서버 권한(HasAuthority)과 로컬 플레이어 여부(IsLocalController)는 구분하여 기록한다.

- [M1 빌드·에디터·PIE 확인 절차](Docs/M1_EDITOR_CHECKLIST.md)
- [M2 채팅 위젯·PIE 확인 절차](Docs/M2_EDITOR_CHECKLIST.md)
- [M3 숫자 제출 위젯·규칙 테스트·PIE 확인](Docs/M3_EDITOR_CHECKLIST.md)
- [M4 시도 횟수 표시·복제·입력 차단 검증](Docs/M4_EDITOR_CHECKLIST.md)
- [M5 라운드 시작·승패·접속 정책 검증](Docs/M5_EDITOR_CHECKLIST.md)
- [M6 결과 공지 위젯 생성·연결·검증](Docs/M6_EDITOR_CHECKLIST.md)
- [로그·조기 반환 유틸리티 사용법](Docs/NB_UTILITY.md)
- [진행 상태](TASKS.md)
- [클래스와 네트워크 설계](ARCHITECTURE.md)
- [작업 규칙](AGENTS.md)
