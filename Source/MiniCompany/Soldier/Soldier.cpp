#include "Soldier.h"

#include "../Animation/SoldierAnim.h"
#include "../Leader/CompanyLeader.h"
#include "../Enemy/EnemyLeader.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Projectile.h"
#include "TimerManager.h"

// Sets default values
ASoldier::ASoldier()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ASoldier::SwordBeginOverlap(UPrimitiveComponent*	OverlappedComponent,
								 AActor*				OtherActor,
								 UPrimitiveComponent*	OtherComp,
								 int32					OtherBodyIndex,
								 bool					bFromSweep,
								 const FHitResult&		SweepResult)
{
	ASoldier* Soldier{Cast<ASoldier>(OtherActor)};

	if (Soldier && Soldier->Leader != Leader
		&& Leader->IsEnemy != Soldier->Leader->IsEnemy
		&& OtherComp == Soldier->GetCapsuleComponent())
	{
		if (Soldier->SoldierAnim->IsBlocking())
		{
			Soldier->GotHit();
			EnterStun();
			return;
		}

		else if (Soldier->SoldierAnim->IsStunned())
		{
			Soldier->ExitStun();
		}

		Soldier->Leader->ReceiveAttack(Leader, Soldier);
	}
}


void ASoldier::BeginPlay()
{
	Super::BeginPlay();

	Sword		= Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Sword")));
	SwordBox	= Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("SwordBox")));
	Shield		= Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Shield")));
	Bow			= Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Bow")));

	if (SwordBox)
	{
		SwordBox->OnComponentBeginOverlap.AddDynamic(this, &ASoldier::SwordBeginOverlap);
	}

	checkf(GetMesh(), TEXT("This soldier does not have a mesh"));

	SoldierAnim = Cast<USoldierAnim>(GetMesh()->GetAnimInstance());
	checkf(SoldierAnim, TEXT("This soldier does not have an animation instance"));

	// Only has a bow
	if (!(Sword || Shield) && Bow)
	{
		Bow->SetVisibility(true, true);
		Bow->SetActive(true);
	}

	// Has at least a sword and/or a shield
	else
	{
		SetUseSword(Sword != nullptr);
		SetUseShield(Shield != nullptr);
	}

	// Disable weapon interactions
	LowerShield();
	ToggleSwordCollision(false);
}


void ASoldier::SetSoldierFromReferenceSoldier(ASoldier* soldier)
{
	projectileType = soldier->projectileType;
}


void ASoldier::SetColor(FVector Color, float Alpha)
{
	if (!DynamicMaterialBody)
	{
		DynamicMaterialBody = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), NULL);
		GetMesh()->SetMaterial(0, DynamicMaterialBody);
	}

	DynamicMaterialBody->SetVectorParameterValue(TEXT("EditColor"), Color);
	DynamicMaterialBody->SetScalarParameterValue(TEXT("EditAlpha"), Alpha);
}


void ASoldier::EnterDash() noexcept
{
	if (Sword)
	{
		SoldierAnim->ToggleDash(true);
	}
}


void ASoldier::ExitDash() noexcept
{
	if (Sword)
	{
		SoldierAnim->ToggleDash(false);
		ToggleSwordCollision(false);
	}
}


void ASoldier::EnterStun()
{
	if (GetWorldTimerManager().GetTimerRemaining(StunTimer) > .0f)
		return;

	SoldierAnim->ToggleStunned(true);
	ToggleSwordCollision(false);
	GetMovementComponent()->SetActive(false);

	GetWorldTimerManager().SetTimer(StunTimer, this, &ASoldier::ExitStun, StunDuration, false);
}


void ASoldier::ExitStun()
{
	GetWorldTimerManager().ClearTimer(StunTimer);

	SoldierAnim->ToggleStunned(false);
	GetMovementComponent()->SetActive(true);
}


void ASoldier::SetUseSword(bool Toggle)
{
	if (!Sword)
		return;

	Sword->SetVisibility(Toggle, true);
	Sword->SetActive(Toggle);

	if (!Toggle)
	{
		ToggleSwordCollision(false);
	}
}


void ASoldier::SetUseShield(bool Toggle)
{
	if (Shield)
	{
		Shield->SetVisibility(Toggle, true);
		Shield->SetActive(Toggle);
	}
}


void ASoldier::SetUseBow(bool Toggle)
{
	if (Bow && (Sword || Shield))
	{
		Bow->SetVisibility(Toggle, true);
		Bow->SetActive(Toggle);
	}
}


void ASoldier::Tick(float DeltaTime)
{
	if (!Leader)
	{
		Destroy();
	}

	Super::Tick(DeltaTime);

	if (bIsAlive)
		Move();
}


void ASoldier::SetMovementsFromLeader(ACompanyLeader* _Leader)
{
	Leader = _Leader;

	GetCharacterMovement()->MaxWalkSpeed	= Leader->GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = Leader->GetCharacterMovement()->MaxAcceleration;
	GetCharacterMovement()->JumpZVelocity	= Leader->GetCharacterMovement()->JumpZVelocity;
	PerfectFollow							= Leader->ArmyFollowPerfect;
	TPActivationRadius						= Leader->TPActivationRadius;
	SprintRadius							= Leader->SprintRadius;
	StopRadius								= Leader->StopRadius;
	SprintFactor							= Leader->SprintFactor;
	AiActivationRadius						= Leader->AiActivationRadius;
	SquaredTPActivationRadius				= TPActivationRadius * TPActivationRadius;
	SquaredSprintRadius						= SprintRadius * SprintRadius;
	SquaredStopRadius						= StopRadius * StopRadius;
	SquaredAiActivationRadius				= AiActivationRadius * AiActivationRadius;
}


void ASoldier::DetachFromLeader()
{
	if (!Leader)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed	= .0f;
	GetCharacterMovement()->MaxAcceleration = .0f;
	GetCharacterMovement()->JumpZVelocity	= .0f;

	Leader->StopSendingOrdersTo(*this);
}


void ASoldier::SetGoToLocation(FVector Location)
{
	if (bIsAlive)
	{
		GoToLocation = Location;
	}
}


void ASoldier::SetOrientation(AActor* Actor, FormationType Formation)
{
	if (!bIsAlive || SoldierAnim->IsStunned())
	{
		return;
	}

	FVector Orientation;

	if (Formation == FormationType::CIRCLE)
	{
		Orientation = (GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal();
	}

	else
	{
		Orientation = Actor->GetActorForwardVector();
	}

	const FRotator Rotator{FRotationMatrix::MakeFromX(FVector(Orientation.X, Orientation.Y, 0.f)).Rotator()};

	SetActorRotation(Rotator);
}


void ASoldier::Move()
{
	if (PerfectFollow)
	{
		SetActorLocation(GoToLocation, true);
	}

	const float heightDiff{GetActorLocation().Z - GoToLocation.Z};

	GoToLocation.Z = GetActorLocation().Z;

	const float ToLocationDist2{(GetActorLocation() - GoToLocation).SizeSquared()};

	if (ToLocationDist2 < SquaredAiActivationRadius && abs(heightDiff) < AiActivationRadius)
	{
		AIUsed = false;

		// Set the walk speed depending on distance to target location
		if (ToLocationDist2 > SquaredSprintRadius)
		{
			GetCharacterMovement()->MaxWalkSpeed = Leader->GetCharacterMovement()->MaxWalkSpeed * 2;
			GetCharacterMovement()->MaxAcceleration = Leader->GetCharacterMovement()->MaxAcceleration * SprintFactor;
		}

		else
		{
			GetCharacterMovement()->MaxWalkSpeed = Leader->GetCharacterMovement()->MaxWalkSpeed;
			GetCharacterMovement()->MaxAcceleration = Leader->GetCharacterMovement()->MaxAcceleration;
		}

		//  Set new soldier position
		if (ToLocationDist2 <= SquaredStopRadius)
		{
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				SetActorLocation(GoToLocation, true, nullptr, ETeleportType::TeleportPhysics);
			}
			GetCharacterMovement()->Velocity = Leader->GetCharacterMovement()->Velocity;
		}
		else
		{
			AddMovementInput((GoToLocation - GetActorLocation()).GetSafeNormal(), 1.f);
		}
	}

	else if (ToLocationDist2 < SquaredTPActivationRadius && abs(heightDiff) < TPActivationRadius)
	{
		AIUsed = true;
	}

	else
	{
		AIUsed = false;

		SetActorLocation(GoToLocation - FVector(0.f, 0.f, heightDiff));
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	}
}


void ASoldier::MeleeAttack() noexcept
{
	if (SwordBox)
	{
		SoldierAnim->MeleeAttack();
	}
}


void ASoldier::StopMeleeAttack() noexcept
{
	SoldierAnim->StopMeleeAttack();
}


void ASoldier::RangedAttack()
{
	if (Bow)
	{
		SoldierAnim->RangedAttack();
	}
}


void ASoldier::RaiseShield()
{
	if (Shield)
	{
		SoldierAnim->ToggleShield(true);
		ToggleSwordCollision(false);
	}
}


void ASoldier::LowerShield()
{
	if (Shield)
	{
		SoldierAnim->ToggleShield(false);
	}
}


void ASoldier::ToggleSwordCollision(bool Toggle)
{
	if (SwordBox)
	{
		SwordBox->SetGenerateOverlapEvents(Toggle);
		SwordBox->SetActive(Toggle);
	}
}


void ASoldier::LaunchProjectile()
{
	if (Bow && projectileType)
	{
		AProjectile* newProjectile = Cast<AProjectile>(GetWorld()->SpawnActor(projectileType));

		newProjectile->InitProjectile(Leader, GetActorLocation(), Leader->GetRangedAttackDamage());

		Leader->SetSpecialAttackOnCooldown();
	}
}


void ASoldier::GotHit(float Intensity)
{
	SoldierAnim->GotHit(Intensity);
}


void ASoldier::Kill()
{
	bIsAlive = false;

	SoldierAnim->GotLethallyHit();
	
	Leader->SoldierDeath(this);

	// Disable any kind of collision with sword
	if (SwordBox)
	{
		SwordBox->OnComponentBeginOverlap.RemoveDynamic(this, &ASoldier::SwordBeginOverlap);
		SwordBox->SetGenerateOverlapEvents(false);
		SwordBox->Deactivate();
	}

	if (Shield)
	{
		SoldierAnim->ToggleShield(false);
	}

	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	// Completely disable this Soldier
	for (UActorComponent* Component : GetComponents())
	{
		Component->Deactivate();
	}

	// Only leave the mesh
	GetMesh()->Activate();
}


void ASoldier::Resurrect()
{
	if (bIsAlive)
	{
		return;
	}

	// Turn back on every Component
	for (UActorComponent* Component : GetComponents())
	{
		Component->Activate();
	}

	Leader->SendOrdersTo(*this);

	// Disable any kind of collision with shield/sword
	if (Sword)
	{
		SwordBox->OnComponentBeginOverlap.AddDynamic(this, &ASoldier::SwordBeginOverlap);
		SwordBox->Activate();
		SwordBox->SetGenerateOverlapEvents(true);
	}

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	Leader->NbSoldierAlive++;
}


bool ASoldier::IsAttacking() const noexcept
{ return SoldierAnim->IsAttacking(); }


bool ASoldier::IsBlocking() const noexcept
{ return SoldierAnim->IsBlocking(); }