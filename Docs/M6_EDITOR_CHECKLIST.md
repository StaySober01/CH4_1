# M6 결과 공지 UI

상태: **사용자 검증 완료 보고.** 빌드·UMG 연결·2인 결과 표시 확인을 완료한 것으로 기록한다. 아래 목록은 재검증용 절차이며 개별 로그나 생성 순서별 재현 결과는 별도 미수집이다.

## 1. 빌드

에디터를 닫고 CH4_1Editor Win64 Development 빌드를 확인한다. 새 UCLASS와 PC의 UPROPERTY가 추가되었다. 이후 에디터를 연다.

## 2. WBP_NBResult 만들기

1. Content/UI에서 Widget Blueprint를 생성한다. 부모 클래스를 **NBResultWidget**으로 선택하고 이름은 **WBP_NBResult**로 지정한다. 일반 UserWidget으로 만들었다면 Class Settings의 Parent Class를 NBResultWidget으로 변경한다.
2. Designer에서 Canvas Panel을 루트로 두고 Text Block 하나를 추가한다.
3. Text Block 이름을 정확히 **ResultText**로 지정하고 **Is Variable**을 켠다. 타입은 Rich Text Block이 아닌 Text Block이다.
4. 화면 상단 중앙에 보이도록 Canvas Slot Anchor를 상단 중앙, Alignment를 X=0.5/Y=0, Position을 X=0/Y=40 정도로 설정한다. Auto Size를 켜고 글자 크기는 32 정도, 글자 정렬은 가운데로 설정한다. 색상과 위치는 기존 채팅창을 가리지 않게 조정한다.
5. 디자인 확인용 Text는 `Player 1 승리!`로 넣어도 된다. 런타임에는 C++가 상태에 맞게 덮어쓴다. Text Binding은 만들지 않는다.
6. 위젯의 Is Focusable은 끈다. Compile/Save 한다.

필수 이름은 ResultText 하나다. 배경 Border나 Size Box를 추가해도 된다. 위젯 생성, Add to Viewport, 표시/숨김, 결과 판정 이벤트를 Blueprint Graph에 중복 작성하지 않는다.

## 3. PlayerController 연결

1. BP_NBPlayerController를 연다.
2. Class Defaults에서 `Result Widget Class`를 검색한다(NB → UI).
3. **WBP_NBResult**를 지정하고 Compile/Save 한다.
4. 기존 Chat Widget Class는 WBP_NBChat 그대로 유지한다.

ResultWidgetClass를 빠뜨리면 서버 게임과 기존 채팅은 계속되지만 별도 결과 공지는 표시되지 않고 LogNB에 설정 오류가 남는다. 잘못된 설정을 정상 완료로 간주하지 않는다.

## 4. 2 Player 검증

기존 Listen Server 호스트 + 원격 Client 구성으로 양쪽 창을 확인한다. 각 승리/무승부 사례 사이에는 PIE를 다시 시작한다. M6에는 자동 리셋이 없다.

- [ ] 빌드 및 두 Blueprint Compile 성공, BindWidget/Class 설정 오류 없음.
- [ ] 1인 대기와 2인 Playing 중에는 결과 공지가 보이지 않음.
- [ ] 호스트가 정답 제출: 양쪽에 같은 `Player N 승리!` 공지 표시.
- [ ] 새 실행에서 원격 Client가 정답 제출: 원격 승자의 이름으로 양쪽 표시.
- [ ] 새 실행에서 두 명 모두 유효한 오답 세 번: 양쪽 `무승부!` 표시.
- [ ] 결과 후 숫자 입력은 잠기고 채팅 입력/버튼/스크롤은 계속 사용 가능. 공지가 마우스 클릭이나 키보드 포커스를 가로채지 않음.
- [ ] 기존 채팅 결과 기록도 그대로 한 번 표시. 위젯 공지는 여러 장 겹치지 않음.
- [ ] 결과 상태에서 3초 이상 기다려도 공지가 유지됨(자동 재시작은 M7).
- [ ] 별도 프로세스로 결과 표시 중 클라이언트만 종료: 호스트가 Waiting으로 바뀌면 공지가 사라짐. 재접속 새 라운드에서도 이전 결과가 보이지 않음.
- [ ] 게임 종료 후 공지 위젯이 남지 않음.

위젯보다 상태가 먼저 도착하는 경로는 C++ 생성 직후 현재 GS 조회로 처리한다. 해당 생성 순서를 실행 중 실제로 관찰하지 않았다면 **초기 조회 경로 정적 검토 완료, 순서별 실행 검증 미실시**로 구분한다. 검증을 위한 임시 Delay는 추가하지 않는다.

## 5. 처리 흐름과 설명할 질문

`서버 GM 결과 확정 → GS RoundState 변경 → 호스트 직접 알림 / 원격 OnRep → PC RefreshRoundUI → ResultWidget UpdateRoundState`

- 왜 결과 공지용 Multicast나 Client RPC를 추가하지 않았는가?
- 왜 위젯 생성 직후에도 현재 상태를 읽는가?
- 왜 채팅 문구 중복 방지 검사보다 먼저 결과 위젯을 갱신하는가?
- 왜 결과 위젯이 GameMode를 찾거나 승패를 계산하지 않는가?

표시 규칙: ShowingResult의 Win/Draw만 표시, 다른 단계나 None은 Collapsed. 표시할 때는 HitTestInvisible(자신과 자식 모두 마우스 히트 테스트 제외), 채팅보다 높은 ZOrder=10을 사용한다. 숫자 입력 허용 여부는 기존 채팅 위젯/서버 검사가 담당한다.
