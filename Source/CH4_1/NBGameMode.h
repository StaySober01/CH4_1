#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameTypes.h"
#include "NBGameMode.generated.h"

class ANBPlayerController;

// 서버에서 게임 규칙과 진행을 담당한다.
UCLASS()
class CH4_1_API ANBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANBGameMode();
	virtual void PreLogin(const FString& Options, const FString& Address,
		const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole,
		const FString& Portal, const FString& Options,
		const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void ProcessChatMessage(ANBPlayerController* Sender, const FString& Message);
	void ProcessGuess(ANBPlayerController* Sender, const FString& Input, int32 SubmittedRoundId);

protected:
	virtual void BeginPlay() override;

private:
	void TryStartRound();
	void FinishRound(ENBRoundResult Result, ANBPlayerController* Winner = nullptr);
	bool HaveAllPlayersExhaustedAttempts() const;
	static constexpr int32 RequiredPlayers = 2;
	void GenerateRandomNumbers();
	bool CheckAnswer(const FString& Input, FNBGuessResult& OutResult) const;

	// GameMode는 서버에만 존재한다. 정답을 복제하거나 UI에 전달하지 않는다.
	FString SecretAnswer;

	// 접속 완료된 참가자와 서버가 부여한 이름. 이탈 즉시 라운드를 중단한다.
	UPROPERTY(Transient)
	TMap<TObjectPtr<ANBPlayerController>, FString> ChatPlayerNames;
};
