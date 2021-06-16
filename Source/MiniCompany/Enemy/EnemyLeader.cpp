// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyLeader.h"
#include "../MyGameInstanceMiniCompany.h"
#include "../Soldier/Soldier.h"
#include "../LevelObject/EnemyPatrolPoint.h"
#include "../LevelObject/Checkpoint.h"
#include "EnemyAIController.h"
#include "../Player/PlayerCharacter.h"

#include "TimerManager.h"

void AEnemyLeader::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = false;

	OriginalPosition = GetActorLocation();

	// So IsEnemy can stay public
	*const_cast<bool*>(&IsEnemy) = true;

	UMyGameInstanceMiniCompany* GameInstance{ Cast<UMyGameInstanceMiniCompany>(GetGameInstance()) };

	if (GameInstance)
		GameInstance->AddEnemyLeader(this);

	RandAlpha = FMath::RandRange(ColorMin.W, ColorMax.W);
	RandColor = FVector(FMath::RandRange(ColorMin.X, ColorMax.X), FMath::RandRange(ColorMin.Y, ColorMax.Y), FMath::RandRange(ColorMin.Z, ColorMax.Z));

	for (ASoldier* Soldier : Soldiers)
	{
		if (Soldier)
		{
			Soldier->SetColor(RandColor, RandAlpha);
		}
	}

	SquaredSeeRadius = SeeRadius * SeeRadius;
	SwitchPointRaduisSquared = SwitchPointRaduis * SwitchPointRaduis;
	SquaredRadiusSpecialDistanceAttack = RadiusSpecialDistanceAttack * RadiusSpecialDistanceAttack;
	SquaredRadiusFear = RadiusFear * RadiusFear;

	SetPatrolRoute();
}

void AEnemyLeader::SetPatrolRoute()
{
	for (int32 i = 0; i < PatrolPoints.Num(); i++)
	{
		PatrolPointsVector.Add(PatrolPoints[i]->GetActorLocation());
	}
}

void AEnemyLeader::SoldierDeath(class ASoldier* Soldier)
{
	if (IsDead)
	{
		return;
	}

	Super::SoldierDeath(Soldier);

}


void AEnemyLeader::LeaderDeath()
{
	if (IsDead)
	{
		return;
	}

	if (Checkpoint)
	{
		Checkpoint->RemoveEnemy(this);
	}

	Super::LeaderDeath();

	UMyGameInstanceMiniCompany* GameInstance{ Cast<UMyGameInstanceMiniCompany>(GetGameInstance()) };
	GameInstance->RemoveEnemyLeader(this);

	IsDead = true;
}

void AEnemyLeader::ApplyDamage(float Damage, bool* ThresholdPassed)
{
	Super::ApplyDamage(Damage, ThresholdPassed);

	StartInfiniteChase();
}

void AEnemyLeader::StartInfiniteChase()
{
	ChasePlayer = true;
	IsInfiniteChase = true;
	GetWorldTimerManager().ClearTimer(ChaseTimer);
	GetWorldTimerManager().SetTimer(ChaseTimer, this, &AEnemyLeader::EndInfiniteChase, EnemyInfiniteChaseDuration, false);
}

void AEnemyLeader::EndInfiniteChase()
{
	IsInfiniteChase = false;
}