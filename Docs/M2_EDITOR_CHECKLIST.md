# M2 채팅 위젯 연결 및 검증

**사용자 검증 완료(문제 없음)**. 사용자의 'M2 검증 완료. 문제 없음' 보고를 근거로 빌드·에디터·PIE 검증 완료를 기록했다. 아래 절차는 이후 회귀 검증에 사용한다. 개별 실행 로그는 별도 수집하지 않았으며 Codex가 직접 실행한 것은 아니다.

## 1. 사용자 빌드

에디터를 닫고 Development Editor / Win64로 빌드한다. 새 UCLASS/RPC와 UMG/SlateCore 의존성이 추가되었다. 필요하면 프로젝트 파일을 재생성한다. 실패하면 첫 오류부터 관련 메시지를 공유한다. 성공 후 에디터를 연다.

## 2. WBP_NBChat 생성

1. Content/UI 폴더에서 Widget Blueprint를 생성한다. 부모 클래스를 NBChatWidget으로 선택하고 이름을 WBP_NBChat으로 지정한다.
2. 이미 일반 UserWidget으로 만들었다면 Class Settings의 Parent Class를 NBChatWidget으로 변경한다.
3. Designer에서 아래 계층을 만든다. 필수 네 위젯은 이름과 타입을 정확히 맞추고 Is Variable을 켠다.

```text
Canvas Panel
└ Border (채팅 배경)
   └ Vertical Box
      ├ Scroll Box        이름: ChatScrollBox
      │  └ Text           이름: ChatHistoryText
      └ Horizontal Box
         ├ Editable Text Box  이름: ChatInput
         └ Button             이름: SendButton
            └ Text            내용: 전송
```

| 이름 | C++ 타입 | Designer에서 선택할 위젯 |
| --- | --- | --- |
| ChatInput | UEditableTextBox | Editable Text Box (Multi-Line이나 Editable Text가 아님) |
| SendButton | UButton | Button |
| ChatScrollBox | UScrollBox | Scroll Box |
| ChatHistoryText | UTextBlock | Text (Rich Text Block이 아님) |

4. 빠른 검증용 배치 예: Border를 좌상단에서 X=30, Y=30, 크기 620×400으로 배치한다. 배경은 어두운 색, 글자는 밝은 색을 사용한다.
5. Vertical Box 안에서 ChatScrollBox 슬롯의 Size는 Fill, Horizontal Box 슬롯은 Auto로 지정한다. Horizontal Box 안 ChatInput 슬롯은 Fill, SendButton 슬롯은 Auto로 지정한다.
6. ChatHistoryText는 Auto Wrap Text를 켜고 기본 Text를 비운다. ChatInput의 Hint Text는 `채팅 내용을 입력하세요`로 설정한다.
7. Compile 및 Save한다. BindWidget 관련 오류가 나면 부모 클래스, 이름, 타입을 먼저 확인한다.

이벤트 그래프에 Create Widget, OnClicked, OnTextCommitted 또는 메시지 추가 로직을 별도로 작성하지 않는다. 이번 단계에서는 C++이 생성과 이벤트 연결을 담당한다. 숫자 제출창은 M3에서 추가한다.

## 3. PlayerController 연결

1. BP_NBPlayerController → Class Defaults에서 `Chat Widget Class`를 검색한다(NB → UI 범주).
2. WBP_NBChat을 지정하고 Compile/Save한다.
3. BP_NBGameMode의 Player Controller Class가 BP_NBPlayerController인지 재확인한다.
4. L_NumberBaseball을 열고 Number of Players=2, Net Mode=Play As Listen Server, New Editor Window (PIE)로 실행한다.

각 로컬 PC가 자신의 위젯을 만든다. 입력 모드는 UIOnly이며 마우스가 표시된다. 별도로 Level Blueprint에서 위젯을 만들면 중복 표시되므로 만들지 않는다. UIOnly에서 Esc가 게임 종료에 반응하지 않으면 에디터 툴바의 Stop 버튼을 사용한다.

## 4. 양쪽 창 검증

| 작업 | 기대 결과 |
| --- | --- |
| 호스트에서 `안녕` 입력 후 Enter | 양쪽에 `Player 1: 안녕` 한 번씩 |
| 클라이언트에서 `반가워` 입력 후 전송 클릭 | 양쪽에 `Player 2: 반가워` 한 번씩 |
| 입력창에서 버튼으로 포커스 이동 후 클릭 | 포커스 이동 자체로 제출되지 않고 클릭으로 한 번만 전송 |
| 채팅창에 `123` 전송 | 일반 채팅으로 표시. 숫자 판정/횟수 소비 없음 |
| 빈 입력 또는 공백만 입력 후 제출 | 제출자에게만 `[안내] 채팅 내용을 입력해주세요.` |
| ASCII `a` 200개 붙여넣기 후 제출 | 양쪽에 정상 표시 |
| ASCII `a` 201개 붙여넣기 후 제출 | 제출자에게만 200자 제한 안내, 상대에게 전달 안 됨 |
| `  안녕  ` 제출 | 앞뒤 공백 제거 후 표시 |
| 길게 여러 번 대화 | 줄바꿈 표시 및 아래로 스크롤, 최근 100건만 유지 |
| 양쪽 창 번갈아 Enter/버튼 전송 | 호스트·클라이언트 메시지 누락/중복 없이 각 한 번 표시 |

일반 입력창은 제어 문자 입력을 막을 수 있다. 서버의 제어 문자 방어는 코드에 있지만 UI에서 해당 문자열이 실제 전송되지 않았다면 실행 검증했다고 기록하지 않는다. 입력창은 제출 직후 비워지며 거절된 내용도 자동 복구하지 않는다.

서버 Output Log에서 `Accepted chat: Sender=Player ... Length=...`가 정상 제출마다 한 번 찍히는지 확인한다. 서버 로그 성공만으로 양쪽 UI 수신 성공을 판단하지 않는다. `Set ChatWidgetClass` 오류는 BP 클래스 기본값 누락, `BindWidget` 오류는 위젯 이름/타입 연결 문제를 의미한다.

## 5. 공유할 결과

- 빌드 성공/실패, 위젯 Compile 성공 여부
- 호스트→클라이언트, 클라이언트→호스트 각각 한 번씩 표시되는지
- Enter와 버튼 전송, 빈 값/201자 안내가 제출자에게만 보이는지
- 오류가 있으면 관련 LogNB 및 Blueprint 오류 메시지

M2에서는 채팅 구현만 검증한다. 정원 제한, 숫자 입력, 시도 횟수, 결과, 리셋, 턴은 후속 milestone 작업이다.
