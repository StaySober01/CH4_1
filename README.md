# CH4_1

UE 5.6 멀티플레이 숫자 야구 개인 과제. 서버 권한과 RPC/Replication 흐름을 작은 단계로 학습한다.

M1의 클래스 골격과 진단 유틸리티를 작성하고 사용자 제공 로그로 2인 PIE 클래스 배치를 확인했다. 정식 Editor Target 빌드 결과는 별도 확인 대기다. 채팅과 숫자 야구 규칙은 아직 미구현이다.

| 클래스 | 현재 작성 내용 |
| --- | --- |
| NBGameMode | PC/PS/GS 기본 클래스 지정, BeginPlay 및 PostLogin 로그 |
| NBPlayerController | 서버 권한과 로컬 컨트롤러 여부 로그 |
| NBPlayerState / NBGameState | 실행 월드와 서버 권한 로그 |

GameMode 생성자에서 엔진이 사용할 클래스 타입을 지정한다. HasAuthority와 IsLocalController는 서로 다른 개념이며, NBUtility.h를 이용해 함수명/줄번호와 함께 기록한다.

- [M1 에디터 및 PIE 확인](Docs/M1_EDITOR_CHECKLIST.md)
- [로그·조기 반환 유틸리티](Docs/NB_UTILITY.md)
- [작업 계획](TASKS.md)
- [설계](ARCHITECTURE.md)
- [작업 규칙](AGENTS.md)
