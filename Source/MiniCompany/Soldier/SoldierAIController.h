// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoldierAIController.generated.h"

/**
 * 
 */
UCLASS()
class MINICOMPANY_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()
	

	void OnPossess(APawn* InPawn) final;
};
