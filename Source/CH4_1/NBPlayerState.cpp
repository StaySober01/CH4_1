#include "NBPlayerState.h"

#include "NBUtility.h"
#include "Engine/World.h"

void ANBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}
