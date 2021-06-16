// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerMiniCompany.generated.h"

/**
 * 
 */
UCLASS()
class MINICOMPANY_API APlayerControllerMiniCompany : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

};
