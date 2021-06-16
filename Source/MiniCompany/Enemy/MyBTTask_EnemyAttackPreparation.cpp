// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_EnemyAttackPreparation.h"
#include "EnemyLeader.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UMyBTTask_EnemyAttackPreparation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AEnemyLeader* EnemyLeader{ Cast<AEnemyLeader>(BlackboardComp->GetValueAsObject("SelfActor")) };

	if (EnemyLeader)
	{
		BlackboardComp->SetValueAsFloat("AttackPreparationDuration", EnemyLeader->AttackPreparationDuration);
		BlackboardComp->SetValueAsFloat("AttackWaitDuration", EnemyLeader->AttackWaitDuration);
	
		EnemyLeader->PreparingAttack = true;
	}

	return EBTNodeResult::Succeeded;
}