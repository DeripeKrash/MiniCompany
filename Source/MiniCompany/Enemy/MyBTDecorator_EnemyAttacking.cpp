// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTDecorator_EnemyAttacking.h"

#include "EnemyLeader.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UMyBTDecorator_EnemyAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp{ OwnerComp.GetBlackboardComponent() };
	AEnemyLeader* Leader{ Cast<AEnemyLeader>(BlackboardComp->GetValueAsObject("SelfActor")) };

	return (Leader->ChasePlayer);
}