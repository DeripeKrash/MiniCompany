// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceMiniCompany.h"

#include "./Player/PlayerCharacter.h"
#include "./Enemy/EnemyLeader.h"
#include "Engine/World.h"
#include "Containers/Ticker.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "./Soldier/Soldier.h"

#include "./LevelObject/Checkpoint.h"

void UMyGameInstanceMiniCompany::ApplyOption()
{
	if (Player)
	{
		Player->VerticalAxisFactor = VerticalAxisFactor;
		Player->HorizontalAxisFactor = HorizontalAxisFactor;
	}
}

void UMyGameInstanceMiniCompany::SetPlayer(APlayerCharacter* NewPlayer)
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	TickDelegateHandle.Reset();
	Player = NewPlayer;

	ApplyOption();

	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMyGameInstanceMiniCompany::CheckTick));

	OnReset = false;

	if (NbTry > 0)
	{
		Player->SetActorLocation(LastCheckpointLocation);
		Player->SetActorRotation(LastCheckpointRotation);

		SetStart = true;

		for (int32 i = 0; i < Player->Soldiers.Num(); i++)
		{
			Player->Soldiers[i]->SetActorLocation(Player->GetActorLocation());
		}
	}

	LastCheckpointLocation = NewPlayer->GetActorLocation();
	LastCheckpointRotation = NewPlayer->GetActorRotation().Quaternion();
}


APlayerCharacter* UMyGameInstanceMiniCompany::GetPlayer() const
{
	return Player;
}

void UMyGameInstanceMiniCompany::SetCheckpoint(ACheckpoint* Checkpoint)
{
	if (OnReset)
		return;

	if (CurrentCheckpoint < Checkpoint->PriorityCheckpoint)
	{
		CurrentCheckpoint = Checkpoint->PriorityCheckpoint;
		LastCheckpointLocation = Checkpoint->GetActorLocation();
		LastCheckpointRotation = Checkpoint->GetActorRotation().Quaternion();

		Player->ResetSoldiers();
	
		TryDestroyEnemies();
	}
}

void UMyGameInstanceMiniCompany::AddEnemyLeader(AEnemyLeader* NewLeader)
{
	Leaders.Add(NewLeader);
}


void UMyGameInstanceMiniCompany::RemoveEnemyLeader(AEnemyLeader* Leader)
{
	Leaders.Remove(Leader);

	if (Leaders.Num() <= 0)
	{
		EndGameScreen(GameEndType::VICTORY);
	}
}


unsigned int UMyGameInstanceMiniCompany::GetNbEnemies() const
{
	return Leaders.Num();
}


bool UMyGameInstanceMiniCompany::CheckTick(float DeltaTime)
{
	PlayerPosCheckCountDown += DeltaTime;

	if (SetStart)
	{
		TryDestroyEnemies();
		SetStart = false;
	}


	if (PlayerPosCheckCountDown >= PlayerPosCheckTimer)
	{
		CheckPositionOfPlayer();
		PlayerPosCheckCountDown = 0.f;
	}

	return true;
}


void UMyGameInstanceMiniCompany::CheckPositionOfPlayer()
{
	const FVector PlayerLocation{Player->GetActorLocation()};

	for (int32 i = 0; i < Leaders.Num(); i++)
	{
		if (Leaders[i]->CheckpointNb >= CurrentCheckpoint && !Leaders[i]->IsInfiniteChase)
		{
			if ((PlayerLocation - Leaders[i]->GetActorLocation()).SizeSquared() <= Leaders[i]->SquaredSeeRadius)
			{
				Leaders[i]->ChasePlayer = true;
			}
			else
			{
				Leaders[i]->ChasePlayer = false;
			}
		}
	}
}


void UMyGameInstanceMiniCompany::EndGameScreen(GameEndType Status)
{
	switch (Status)
	{
		case GameEndType::DEATH:
			DeadEvent.Broadcast();
			OnDefeat();
			ResetOrigin = GameEndType::DEATH;
			break;
		
		case GameEndType::VICTORY:
			VictoryEvent.Broadcast();
			OnVictory();
			NbTry = -1;
			CurrentCheckpoint = -1;
			ResetOrigin = GameEndType::VICTORY;
			break;
		
		default:
			return;
	}

	OnReset = true;
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	TickDelegateHandle.Reset();
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMyGameInstanceMiniCompany::ResetScreenCountdown));
}


bool UMyGameInstanceMiniCompany::ResetScreenCountdown(float DeltaTime)
{
	CountDown += DeltaTime;

	if (CountDown >= ResetTimeAtGameEnd)
	{
		TickDelegateHandle.Reset();
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
		
		if (ResetOrigin == GameEndType::VICTORY)
		{
			Reset(true);
			UGameplayStatics::OpenLevel(GetWorld(), "Credit");
		}
		else
		{
			Reset(false);
		}
		FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
		return false;
	}

	return true;
}

void UMyGameInstanceMiniCompany::TryDestroyEnemies()
{
	if (OnReset)
	{
		return;
	}

	for (int32 i = 0; i < Leaders.Num(); i++)
	{
		if (Leaders[i]->CheckpointNb < CurrentCheckpoint)
		{
			AEnemyLeader* leader = Leaders[i];

			RemoveEnemyLeader(leader);
			leader->Destroy();
			i--;
		}
	}
}

void UMyGameInstanceMiniCompany::Reset(bool HardReset)
{
	SetStart = true;
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	TickDelegateHandle.Reset();
	Leaders.Empty();
	CountDown = 0;

	if (HardReset)
	{
		NbTry = 0;
		CurrentCheckpoint = -1;
	}
	else
	{
		NbTry++;
	}

}