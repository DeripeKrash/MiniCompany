// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "../Leader/CompanyLeader.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Soldier.h"
#include "TimerManager.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	hitBox->InitBoxExtent(FVector(10, 6e0, 10));
	hitBox->SetCollisionProfileName(TEXT("Pawn"));
	hitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	hitBox->Mobility = EComponentMobility::Movable;

	hitBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::BeginOverlap);

	RootComponent = hitBox;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(hitBox);

}


void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + speed * DeltaTime * GetActorForwardVector(), true);
}


void AProjectile::ProjectileDestruction()
{
	Destroy();
}


void AProjectile::InitProjectile(ACompanyLeader const* OriginCompany_, const FVector& OriginSoldierLocation, float Damage_)
{
	OriginCompany = OriginCompany_;
	SetActorLocation(OriginSoldierLocation);
	SetActorRotation(OriginCompany->GetActorRotation());

	Damage = Damage_;

	GetWorldTimerManager().SetTimer(timer, this, &AProjectile::ProjectileDestruction, lifeDuration, false);
}


void AProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
							   AActor*				OtherActor,
							   UPrimitiveComponent* OtherComp,
							   int32				OtherBodyIndex,
							   bool					bFromSweep,
							   const FHitResult&	SweepResult)
{
	ASoldier* Soldier{Cast<ASoldier>(OtherActor)};

	if (Soldier && Soldier->GetLeader()->IsEnemy != OriginCompany->IsEnemy
		&& OtherComp == Soldier->GetCapsuleComponent())
	{
		Soldier->GetLeader()->ReceiveAttack(this, Soldier);

		// Disable this object's ticking
		SetActorTickEnabled(false);

		// Remove any interaction with the hitbox
		hitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AProjectile::BeginOverlap);
		hitBox->SetGenerateOverlapEvents(false);
		hitBox->SetActive(false);
		hitBox->DestroyComponent();

		// Attach to hit Soldier
		Mesh->AttachToComponent(Soldier->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}