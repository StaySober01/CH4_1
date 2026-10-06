# CH4_1

UE 5.6 멀티플레이 숫자 야구 개인 과제. 서버 권한과 RPC/Replication 흐름을 이해하고 설명할 수 있도록 작은 단계로 구현한다.

현재 **M1의 2인 PIE 클래스 배치 확인 완료**, **M2 채팅 사용자 검증 완료(문제 없음)** 상태다. 숫자 판정, 시도 횟수, 승패와 턴 기능은 아직 없다.

| 클래스 | 현재 작성 내용 |
| --- | --- |
| NBGameMode | 기본 클래스 지정, 서버 발신자 이름 부여 및 채팅 검증 |
| NBPlayerController | 로컬 위젯 생성, ServerSendChat, ClientShowInputError |
| NBGameState | MulticastReceiveMessage로 각 로컬 플레이어에게 전달 |
| NBPlayerState | 골격 및 기존 PlayerName 사용. 시도 횟수는 미구현 |
| NBChatWidget | 입력/전송 버튼/최근 100건 표시. UMG 에셋 연결 필요 |

GameMode가 생성할 클래스 타입을 지정하면 엔진의 기존 생성 흐름을 사용할 수 있다. PlayerController를 클라이언트에서 직접 생성하지 않는다. 서버 권한(HasAuthority)과 로컬 플레이어 여부(IsLocalController)는 구분하여 기록한다.

- [M1 빌드·에디터·PIE 확인 절차](Docs/M1_EDITOR_CHECKLIST.md)
- [M2 채팅 위젯·PIE 확인 절차](Docs/M2_EDITOR_CHECKLIST.md)
- [로그·조기 반환 유틸리티 사용법](Docs/NB_UTILITY.md)
- [진행 상태](TASKS.md)
- [클래스와 네트워크 설계](ARCHITECTURE.md)
- [작업 규칙](AGENTS.md)
