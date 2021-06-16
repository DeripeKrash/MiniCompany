// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../MyGameInstanceMiniCompany.h"
#include "../Enemy/EnemyLeader.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup TriggerBox 
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->InitBoxExtent(FVector(20, 20, 20));
	TriggerBox->SetCollisionProfileName(TEXT("Pawn"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->Mobility = EComponentMobility::Movable;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::BeginOverlap);

	RootComponent = TriggerBox;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerBox);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < EssentialEnemies.Num(); i++)
	{
		if (EssentialEnemies[i])
		{
			EssentialEnemies[i]->Checkpoint = this;
		}
		else
		{
			EssentialEnemies.Remove(EssentialEnemies[i]);
			i--;
		}
	}

	if (Cast<UMyGameInstanceMiniCompany>(UGameplayStatics::GetGameInstance(GetWorld()))->GetCurrentCheckpoint() < PriorityCheckpoint && UnpassableWhileEssenstials && EssentialEnemies.Num())
	{
		TriggerBox->SetCollisionProfileName(TEXT("InvisibleWall"));
	}

}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::RemoveEnemy(AEnemyLeader* enemy)
{
	EssentialEnemies.Remove(enemy);

	if (UnpassableWhileEssenstials && EssentialEnemies.Num() == 0)
	{
		TriggerBox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	}
}

void ACheckpoint::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32				OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult& SweepResult)
{
	if (EssentialEnemies.Num() > 0)
	{
		return;
	}

	UMyGameInstanceMiniCompany* GameInstance{ Cast<UMyGameInstanceMiniCompany>(GetGameInstance()) };

	if (GameInstance)
	{
		if (GameInstance->NbTry == 0)
		{
			GameInstance->NbTry++;
		}
		GameInstance->SetCheckpoint(this);
	}
}