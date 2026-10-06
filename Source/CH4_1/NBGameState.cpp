#include "NBGameState.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBPlayerController.h"

void ANBGameState::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}

void ANBGameState::MulticastReceiveMessage_Implementation(const FString& Message)
{
	// 이 월드의 로컬 PC에만 전달한다. 서버의 원격 플레이어 PC에는 표시하지 않는다.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANBPlayerController* Player = Cast<ANBPlayerController>(It->Get());
		if (IsValid(Player) && Player->IsLocalController())
		{
			Player->ReceiveChatMessage(Message);
		}
	}
}
