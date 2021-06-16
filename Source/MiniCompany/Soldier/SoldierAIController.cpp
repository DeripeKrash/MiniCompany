// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAIController.h"
#include "Soldier.h"

void ASoldierAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASoldier* Soldier = Cast<ASoldier>(InPawn);

	if (Soldier)
	{
		RunBehaviorTree(Soldier->BehaviorTree);
	}
}