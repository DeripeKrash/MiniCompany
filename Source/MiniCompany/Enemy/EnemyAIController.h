#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyLeader.h"
#include "EnemyAIController.generated.h"


UCLASS()
class MINICOMPANY_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	void OnPossess(APawn* InPawn) final;
};
