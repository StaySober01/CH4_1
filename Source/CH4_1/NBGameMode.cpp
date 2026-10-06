#include "NBGameMode.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBGameState.h"
#include "NBPlayerController.h"
#include "NBPlayerState.h"

ANBGameMode::ANBGameMode()
{
	// 엔진이 플레이어와 월드를 초기화할 때 사용할 기본 클래스다.
	PlayerControllerClass = ANBPlayerController::StaticClass();
	PlayerStateClass = ANBPlayerState::StaticClass();
	GameStateClass = ANBGameState::StaticClass();
}

void ANBGameMode::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}

void ANBGameMode::PostLogin(APlayerController* NewPlayer)
{
	NB_RETURN_IF_FALSE(IsValid(NewPlayer), "PostLogin received an invalid PlayerController.");

	Super::PostLogin(NewPlayer);

	// 서버에서 접속을 확인한다. 인원 제한과 시작 판정은 M5에서 추가한다.
	NB_LOG("World=%s PlayerController=%s Class=%s NetMode=%d LocalController=%d",
		*GetNameSafe(GetWorld()), *GetNameSafe(NewPlayer),
		*NewPlayer->GetClass()->GetName(),
		static_cast<int32>(GetNetMode()), NewPlayer->IsLocalController());
}
