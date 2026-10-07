#include "NBPlayerState.h"

#include "NBUtility.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

void ANBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG("World=%s Actor=%s NetMode=%d Authority=%d",
		*GetNameSafe(GetWorld()), *GetName(), static_cast<int32>(GetNetMode()), HasAuthority());
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANBPlayerState, UsedAttempts);
	DOREPLIFETIME(ANBPlayerState, MaxAttempts);
}

bool ANBPlayerState::TryConsumeAttempt()
{
	NB_RETURN_VALUE_IF_FALSE(HasAuthority(), false, "Only the server can consume attempts.");
	if (!HasRemainingAttempts())
	{
		return false;
	}

	++UsedAttempts;
	NB_LOG("Player=%s NetMode=%d Attempts=%d/%d", *GetPlayerName(),
		static_cast<int32>(GetNetMode()), UsedAttempts, MaxAttempts);
	// C++ 서버 변경은 OnRep를 자동 호출하지 않으므로 호스트에도 같은 알림을 보낸다.
	OnAttemptsChanged.Broadcast();
	ForceNetUpdate();
	return true;
}

void ANBPlayerState::OnRep_Attempts()
{
	NB_LOG("Player=%s NetMode=%d Attempts=%d/%d", *GetPlayerName(),
		static_cast<int32>(GetNetMode()), UsedAttempts, MaxAttempts);
	OnAttemptsChanged.Broadcast();
}
