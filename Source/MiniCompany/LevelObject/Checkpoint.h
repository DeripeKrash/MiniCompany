// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <array>
#include "Checkpoint.generated.h"

UCLASS()
class MINICOMPANY_API ACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheckpoint();

	UPROPERTY(Category = "Checkpoint", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int32 PriorityCheckpoint {-1};

	UPROPERTY(Category = "Checkpoint", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class AEnemyLeader*> EssentialEnemies;

	UPROPERTY(Category = "Checkpoint", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool UnpassableWhileEssenstials{ false };


protected:

	// Collision boxes
	UPROPERTY(Category = "Checkpoint", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerBox{ nullptr };

	UPROPERTY(Category = "Checkpoint", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Mesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RemoveEnemy(class AEnemyLeader* enemy);

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32					OtherBodyIndex,
			bool					bFromSweep,
			const FHitResult& SweepResult);

};
