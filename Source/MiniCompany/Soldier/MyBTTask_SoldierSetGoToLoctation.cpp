// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_SoldierSetGoToLoctation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Soldier.h"


EBTNodeResult::Type UMyBTTask_SoldierSetGoToLoctation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	ASoldier* Soldier{ Cast<ASoldier>(BlackboardComp->GetValueAsObject("SelfActor")) };

	BlackboardComp->SetValueAsVector("GoToLocation", Soldier->GoToLocation);

	return EBTNodeResult::Succeeded;
}