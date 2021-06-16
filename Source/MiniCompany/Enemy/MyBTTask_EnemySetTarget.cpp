// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_EnemySetTarget.h"
#include "EnemyLeader.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UMyBTTask_EnemySetTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AEnemyLeader* enemyLeader{ Cast<AEnemyLeader>(BlackboardComp->GetValueAsObject("SelfActor")) };

	if (!enemyLeader->ChasePlayer)
	{
		return Patrol(BlackboardComp, enemyLeader);
	}

	ACompanyLeader* playerLeader{ Cast<ACompanyLeader>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };

	if (enemyLeader->DistanceSpecial)
	{
		return DistanceAttack(BlackboardComp, enemyLeader, playerLeader);
	}
	else
	{
		return ClassicAttack(BlackboardComp, enemyLeader, playerLeader);
	}

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UMyBTTask_EnemySetTarget::Patrol(UBlackboardComponent* BlackboardComp, AEnemyLeader* enemyLeader)
{
	if (enemyLeader->PatrolPointsVector.Num() > 0)
	{
		FVector AimVector{ enemyLeader->PatrolPointsVector[enemyLeader->CurrentPoint] };
		AimVector.Z = enemyLeader->GetActorLocation().Z;

		if ((enemyLeader->GetActorLocation() - AimVector).SizeSquared() > enemyLeader->SwitchPointRaduisSquared)
		{
			BlackboardComp->SetValueAsVector("Target", enemyLeader->PatrolPointsVector[enemyLeader->CurrentPoint]);
		}
		else // Manage Which point must be used has GotoLocation While Patroling
		{
			if (enemyLeader->CurrentPoint + 1 < enemyLeader->PatrolPointsVector.Num())
			{
				enemyLeader->CurrentPoint++;
			}
			else
			{
				enemyLeader->CurrentPoint = 0;
			}
		}
	}
	else
	{
		BlackboardComp->SetValueAsVector("Target", enemyLeader->OriginalPosition);
	}

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UMyBTTask_EnemySetTarget::ClassicAttack(UBlackboardComponent* BlackboardComp, AEnemyLeader* enemyLeader, ACompanyLeader* playerLeader)
{
	// if the Enemy is to close to the Player he while not move to keep clarity in the fights
	if ((playerLeader->GetActorLocation() - enemyLeader->GetActorLocation()).SizeSquared() <= powf(playerLeader->GetFormationRadius() + enemyLeader->GetFormationRadius(), 2))
	{
		BlackboardComp->SetValueAsVector("Target", enemyLeader->GetActorLocation());

		return EBTNodeResult::Succeeded;
	}

	FVector ClosestSoldierLocation {playerLeader->GetClosetSoldierLocation(enemyLeader->GetActorLocation())};
	ClosestSoldierLocation.Z = enemyLeader->GetActorLocation().Z;

	FVector PlayerEnemy{ enemyLeader->GetActorLocation() - ClosestSoldierLocation};
	FVector Direction{ PlayerEnemy.GetSafeNormal() };


	FVector goToLocation{ ClosestSoldierLocation + Direction * 1.1f * enemyLeader->GetFormationRadius() };

	goToLocation.Z = enemyLeader->GetActorLocation().Z;

	BlackboardComp->SetValueAsVector("Target", goToLocation);

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UMyBTTask_EnemySetTarget::DistanceAttack(UBlackboardComponent* BlackboardComp, AEnemyLeader* enemyLeader, ACompanyLeader* playerLeader)
{
	FVector PlayerEnemy{ enemyLeader->GetActorLocation() - playerLeader->GetActorLocation() };
	FVector Direction{ PlayerEnemy.GetSafeNormal() };
	float Distance{ PlayerEnemy.SizeSquared() };

	if (enemyLeader->SquaredRadiusSpecialDistanceAttack >= Distance && enemyLeader->SquaredRadiusFear < Distance)
	{
		BlackboardComp->SetValueAsVector("Target", enemyLeader->GetActorLocation());
	}
	else if (enemyLeader->SquaredRadiusFear > Distance)
	{
		if (!enemyLeader->GoCacIfPlayerCloseEnough)
		{
			FVector goToLocation{ playerLeader->GetActorLocation()
			+ Direction * enemyLeader->RadiusSpecialDistanceAttack };

			goToLocation.Z = enemyLeader->GetActorLocation().Z;
			BlackboardComp->SetValueAsVector("Target", goToLocation);
		}
		else
		{
			if ((playerLeader->GetActorLocation() - enemyLeader->GetActorLocation()).SizeSquared() <= powf(playerLeader->GetFormationRadius() + enemyLeader->GetFormationRadius(), 2))
			{
				BlackboardComp->SetValueAsVector("Target", enemyLeader->GetActorLocation());
			}
			else
			{
				FVector goToLocation{ playerLeader->GetActorLocation()
				+ Direction * (playerLeader->GetFormationRadius() + enemyLeader->GetFormationRadius()) };

				goToLocation.Z = enemyLeader->GetActorLocation().Z;

				BlackboardComp->SetValueAsVector("Target", goToLocation);
			}
		}
	}

	return EBTNodeResult::Succeeded;
}