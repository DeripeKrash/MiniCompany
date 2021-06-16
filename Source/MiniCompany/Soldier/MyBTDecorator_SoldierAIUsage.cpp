// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTDecorator_SoldierAIUsage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Soldier.h"

bool UMyBTDecorator_SoldierAIUsage::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	ASoldier* Soldier{ Cast<ASoldier>(BlackboardComp->GetValueAsObject("SelfActor")) };

	return Soldier->AIUsed;
}