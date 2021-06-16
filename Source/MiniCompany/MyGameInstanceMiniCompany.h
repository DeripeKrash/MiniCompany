// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Delegates/Delegate.h"
#include <array>

#include "MyGameInstanceMiniCompany.generated.h"


UENUM(BluePrintType)
enum GameEndType
{
	VICTORY		UMETA(DisplayName="Victory"),
	DEATH		UMETA(DisplayName="Death"),
};


UCLASS()
class MINICOMPANY_API UMyGameInstanceMiniCompany : public UGameInstance
{
	GENERATED_BODY()

	FDelegateHandle TickDelegateHandle;
	TArray<class AEnemyLeader*> Leaders	{};
	class APlayerCharacter*		Player	{nullptr};
	

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameEvent);

	UPROPERTY(BlueprintAssignable)
	FGameEvent DeadEvent;

	UPROPERTY(BlueprintAssignable)
	FGameEvent VictoryEvent;

	FVector LastCheckpointLocation;
	FQuat	LastCheckpointRotation;

	GameEndType ResetOrigin; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta = (AllowPrivateAccess = "true"))
	int NbTry { 0 };

	/* Game Option */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option", meta = (AllowPrivateAccess = "true"))
	int32 HorizontalAxisFactor {1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option", meta = (AllowPrivateAccess = "true"))
	int32 VerticalAxisFactor {-1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option", meta = (AllowPrivateAccess = "true"))
	float MusicVolume	{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option", meta = (AllowPrivateAccess = "true"))
	float EffectVolume	{ 0.5f };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData", meta = (AllowPrivateAccess = "true"))
	bool InLobby {true};

	UFUNCTION(BlueprintCallable)
	void ApplyOption();

private:
	bool OnReset	{ true };
	bool SetStart	{ true };

	unsigned int NbEnemies				{0u};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Option", meta = (AllowPrivateAccess = "true"))
	int32		 CurrentCheckpoint		{-1};

	// Enemy Check Time
private:	float PlayerPosCheckCountDown		{0.f};
public:		float PlayerPosCheckTimer			{ 0.5f };

public:
	float CountDown						{0.f};
	float ResetTimeAtGameEnd			{5.f};


private:
	bool CheckTick(float DeltaTime);
	void CheckPositionOfPlayer();

public:
	// Events
	UFUNCTION(BlueprintImplementableEvent)
	void OnVictory();
	void OnVictory_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDefeat();
	void OnDefeat_Implementation();

	unsigned int GetNbEnemies() const;
	void SetPlayer(class APlayerCharacter* NewPlayer);
	class APlayerCharacter* GetPlayer() const;

	void SetCheckpoint(class ACheckpoint* Checkpoint);

	void AddEnemyLeader(class AEnemyLeader* NewLeader);
	void RemoveEnemyLeader(class AEnemyLeader* Leader);

	void EndGameScreen(GameEndType Status);
	bool ResetScreenCountdown(float DeltaTime);

	void TryDestroyEnemies();

	UFUNCTION(BlueprintCallable)
	void Reset(bool HardReset = false);

	UFUNCTION(BlueprintCallable)
	int GetCurrentCheckpoint() const
	{
		return CurrentCheckpoint;
	}
};
