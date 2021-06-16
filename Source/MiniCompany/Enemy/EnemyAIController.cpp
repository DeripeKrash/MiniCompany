#include "EnemyAIController.h"

#include "EnemyLeader.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyLeader* Enemy = Cast<AEnemyLeader>(InPawn);

	if (Enemy)
	{
		RunBehaviorTree(Enemy->BehaviorTree);
	}
}
