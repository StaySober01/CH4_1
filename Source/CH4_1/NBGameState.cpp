#include "NBGameState.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "NBPlayerController.h"
#include "Net/UnrealNetwork.h"

void ANBGameState::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
	NotifyRoundStateChanged();
}

void ANBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANBGameState, RoundState);
}

void ANBGameState::SetRoundState(const FNBRoundState& NewState)
{
	NB_RETURN_IF_FALSE(HasAuthority(), "Only the server can change round state.");
	RoundState = NewState;
	// C++ 서버 변경에는 OnRep가 자동 호출되지 않으므로 호스트도 직접 갱신한다.
	NotifyRoundStateChanged();
	ForceNetUpdate();
}

void ANBGameState::OnRep_RoundState()
{
	NotifyRoundStateChanged();
}

void ANBGameState::NotifyRoundStateChanged()
{
	NB_LOG("Round=%d Phase=%d Result=%d Winner=%s Authority=%d",
		RoundState.RoundId, static_cast<int32>(RoundState.Phase),
		static_cast<int32>(RoundState.Result), *RoundState.WinnerName, HasAuthority());
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANBPlayerController* Player = Cast<ANBPlayerController>(It->Get());
		if (IsValid(Player) && Player->IsLocalController())
		{
			Player->RefreshRoundUI();
		}
	}
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
