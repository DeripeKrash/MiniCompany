// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Leader/CompanyLeader.h"

#include "Perception/PawnSensingComponent.h"

#include "EnemyLeader.generated.h"

UCLASS()
class MINICOMPANY_API AEnemyLeader : public ACompanyLeader
{
	GENERATED_BODY()
		// Color
		FVector RandColor;

public:
	UPROPERTY(EditAnywhere, Category = "AI")
		class UBehaviorTree* BehaviorTree{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setting AI")
		int32 CheckpointNb {0};

	class ACheckpoint* Checkpoint{ nullptr };

private:
	// Color
	float RandAlpha;


public:

	/* ==== AI Setting  ==== */

	FVector OriginalPosition; // Used has a reference position when the enemy doesn't have patrol point

	bool ChasePlayer{ false }; // determine if the enemy is chasing the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting AI")
		float SeeRadius{ 5000.f };
	float SquaredSeeRadius;

	/* The Basic Action of an Enemy when he see the player is to go to the closest soldier and do an melee Attack */

	// If True the enemy will do a special attack on close combat
	UPROPERTY(EditAnywhere, Category = "Setting AI") 
		bool SpecialAttackOnCac{ false };

	// If True the enemy will launch special Attacks at a determined distance / The enemy will try to keep distance with the player
	UPROPERTY(EditAnywhere, Category = "Setting AI")
		bool DistanceSpecial{ false };

	// If DistanceSpecial == true and GoCacIfPlayerCloseEnough == true the enemy will attack the player instead of running away
	UPROPERTY(EditAnywhere, Category = "Setting AI")
		bool GoCacIfPlayerCloseEnough{ false };

	// Distance from which the enemy will start his special Attack if DistanceSpecial is True
	UPROPERTY(EditAnywhere, Category = "Setting AI", meta = (UIMin = "0"))
		float RadiusSpecialDistanceAttack{ 500.f };
	float SquaredRadiusSpecialDistanceAttack;

	// Distance from which the enemy will run away if DistanceSpecial is True
	UPROPERTY(EditAnywhere, Category = "Setting AI", meta = (UIMin = "0"))
		float RadiusFear{ 450.f };
	float SquaredRadiusFear;
	
	// When the Enemy is hit he will start chasing the Player regardless of the distance
	UPROPERTY(EditAnywhere, Category = "Setting AI", meta = (UIMin = "0"))
	float EnemyInfiniteChaseDuration{ 5.f };
	
	FTimerHandle ChaseTimer;
	bool IsInfiniteChase{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting AI")
		TArray <class AEnemyPatrolPoint*>	PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting AI")
		float SwitchPointRaduis{ 50.f };
	float SwitchPointRaduisSquared;

	int32 CurrentPoint{ 0 };
	TArray <FVector>	PatrolPointsVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting AI")
	float AttackPreparationDuration	{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting AI")
	float AttackWaitDuration		{ 1.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setting AI")
	bool  PreparingAttack			{false}; // Used for UI

	/* ==== Cosmetic Setting ==== */

	// A color is randomly selected for the soldier between the 2 colors input
	UPROPERTY(EditAnywhere, Category = "Setting Color", meta = (UIMin = "0", UIMax = "1"))
	FVector4 ColorMax {1.f, 1.f, 1.f, 1.f};
	UPROPERTY(EditAnywhere, Category = "Setting Color", meta = (UIMin = "0", UIMax = "1"))
	FVector4 ColorMin {0.f, 0.f, 0.f, 0.f};

	void SetPatrolRoute();

private:
	bool IsDead{ false };

public:
	void BeginPlay() final;
	
	void SoldierDeath(class ASoldier* Soldier) final;
	virtual void LeaderDeath() override;

	virtual void ApplyDamage(float Damage, bool* ThresholdPassed = nullptr) override;

	void StartInfiniteChase(); // Infinite Chase is used when an Enemy is attacked by the player so it can chase him regardless of distance
	void EndInfiniteChase();
};
