// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_EnemyAttack.h"
#include "EnemyLeader.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UMyBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent*	BlackboardComp	{OwnerComp.GetBlackboardComponent()};
	AEnemyLeader*			Leader			{Cast<AEnemyLeader>(BlackboardComp->GetValueAsObject("SelfActor"))};
	ACompanyLeader*			PlayerLeader	{ Cast<ACompanyLeader>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };


	Leader->PreparingAttack = false;

	if (Leader->DistanceSpecial)
	{
		if ((Leader->GetActorLocation() - PlayerLeader->GetActorLocation()).SizeSquared() > pow((PlayerLeader->GetFormationRadius() + Leader->GetFormationRadius()), 2))
		{
			Leader->SpecialAttack();
		}
		else if (Leader->SpecialAttackOnCac)
		{
			Leader->SpecialAttack();
		}
		else
		{
			Leader->MeleeAttack();
			Leader->StopMeleeAttack();
		}
	}
	if ((Leader->GetActorLocation() - PlayerLeader->GetActorLocation()).SizeSquared() < pow((PlayerLeader->GetFormationRadius() + Leader->GetFormationRadius()) * 1.5f, 2))
	{
		if (Leader->SpecialAttackOnCac)
		{
			Leader->SpecialAttack();
		}
		else
		{
			Leader->MeleeAttack();
			Leader->StopMeleeAttack();
		}
	}
	return EBTNodeResult::Succeeded;
}
