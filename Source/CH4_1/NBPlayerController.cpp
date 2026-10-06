#include "NBPlayerController.h"

#include "NBUtility.h"
#include "Engine/World.h"



void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 서버 권한과 로컬 플레이어 여부는 서로 다른 개념이다.
	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d LocalController=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()),
		HasAuthority(), IsLocalController());

}
