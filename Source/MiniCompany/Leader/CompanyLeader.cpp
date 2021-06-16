// Fill out your copyright notice in the Description page of Project Settings.


#include "CompanyLeader.h"

#include "../Soldier/Soldier.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Soldier/Projectile.h"
#include "TimerManager.h"


ACompanyLeader::ACompanyLeader()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ACompanyLeader::BeginPlay()
{
	Super::BeginPlay();

	HealthThresholdOffset = MaxHealth / (float)MaximumSoldierNb;
	NextHealthThreshold = MaxHealth - HealthThresholdOffset;

	Health = MaxHealth;

	RefillArmy();

	OriginalSpeed			= GetCharacterMovement()->MaxWalkSpeed;
	OriginalAcceleration	= GetCharacterMovement()->MaxAcceleration;

	ArmorValue = MaxArmorValue;

	if (ArmorValue <= 0)
	{
		ArmorActive = false;
	}
}

void ACompanyLeader::RefillArmy()
{
	if (BaseSoldier)
	{
		while (Soldiers.Num() < (int32)MaximumSoldierNb)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ASoldier* NewSoldier = GetWorld()->SpawnActor<ASoldier>(BaseSoldier, SpawnParams);

			NewSoldier->SetActorLocation(GetActorLocation());
			SendOrdersTo(*NewSoldier);
			Soldiers.Add(NewSoldier);
		}
		NbSoldierAlive = Soldiers.Num();
	}
}

void ACompanyLeader::ApplyDamage(float Damage, bool* ThresholdPassed)
{
	const float NewHealth		{Health - Damage};
	bool		PassedThreshold	{false};

	// Is it a lethal attack?
	if (NewHealth <= .0f)
	{
		Health = .0f;
		PassedThreshold = true;
	}

	// Non-lethal
	else
	{
		// Did health drop under the next threshold?
		if (NewHealth <= NextHealthThreshold)
		{
			NextHealthThreshold -= HealthThresholdOffset;

			if (NextHealthThreshold < .0f)
				NextHealthThreshold = .0f;

			PassedThreshold = true;
		}

		// Apply changes
		Health = NewHealth;
	}

	if (ThresholdPassed)
		*ThresholdPassed = PassedThreshold;
}

FVector ACompanyLeader::GetClosetSoldierLocation(FVector ReferencePoint)
{
	FVector Result = Soldiers[0]->GetActorLocation();
	float ClosestSquaredDistance = (Soldiers[0]->GetActorLocation() - ReferencePoint).SizeSquared();

	int32 res = 0;

	for (int32 i = 1; i < Soldiers.Num(); i++)
	{
		if (!Soldiers[i]->IsAlive())
		{
			break;
		}
		if ((Soldiers[i]->GetActorLocation() - ReferencePoint).SizeSquared() < ClosestSquaredDistance)
		{
			Result = Soldiers[i]->GetActorLocation();
			ClosestSquaredDistance = (Soldiers[i]->GetActorLocation() - ReferencePoint).SizeSquared();
			res = i;
		}
	}

	return Result;
}

void ACompanyLeader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetFormation();
	CheckLeaderDistanceRelativeToSoldiers();

	if (IsDashing)
	{
		AddMovementInput(GetActorForwardVector(), DashAcceleration);
	}
}

void ACompanyLeader::CheckLeaderDistanceRelativeToSoldiers()
{
	unsigned int nb{ 0 };

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (Soldiers[i]->IsAlive() && (Soldiers[i]->GetActorLocation() - GetActorLocation()).Size() > RaduisTpToClosestSoldier)
		{
			nb++;
		}
	}

	if (nb >= NbSoldierAlive)
	{
		SetActorLocation(GetClosetSoldierLocation(GetActorLocation()));
	}
}


void ACompanyLeader::SendOrdersTo(ASoldier& Soldier)
{
	ControllerJump.AddDynamic(&Soldier, &ACharacter::Jump);
	ControllerJumpStop.AddDynamic(&Soldier, &ACharacter::StopJumping);
	ControllerMeleeAttack.AddDynamic(&Soldier, &ASoldier::MeleeAttack);
	ControllerStopMeleeAttack.AddDynamic(&Soldier, &ASoldier::StopMeleeAttack);
	ControllerRaiseShield.AddDynamic(&Soldier, &ASoldier::RaiseShield);
	ControllerLowerShield.AddDynamic(&Soldier, &ASoldier::LowerShield);

	Soldier.SetMovementsFromLeader(this);
}


void ACompanyLeader::StopSendingOrdersTo(ASoldier& Soldier)
{
	ControllerJump.RemoveDynamic(&Soldier, &ACharacter::Jump);
	ControllerJumpStop.RemoveDynamic(&Soldier, &ACharacter::StopJumping);
	ControllerMeleeAttack.RemoveDynamic(&Soldier, &ASoldier::MeleeAttack);
	ControllerStopMeleeAttack.RemoveDynamic(&Soldier, &ASoldier::StopMeleeAttack);
	ControllerRaiseShield.RemoveDynamic(&Soldier, &ASoldier::RaiseShield);
	ControllerLowerShield.RemoveDynamic(&Soldier, &ASoldier::LowerShield);
}


void ACompanyLeader::ControllerJumpStart()
{
	ControllerJump.Broadcast();
}


void ACompanyLeader::ControllerJumpEnd()
{
	ControllerJumpStop.Broadcast();
}


void ACompanyLeader::SwitchFormationNext()
{
	switch (Formation)
	{
		case FormationType::LINE:
			Formation = FormationType::TRIANGLE;
			
			break;

		case FormationType::TRIANGLE:
			Formation = FormationType::CIRCLE;
			break;

		case FormationType::CIRCLE:
			LowerShield();
			Formation = FormationType::LINE;
			break;
	}
}


void ACompanyLeader::SwitchFormationPrevious()
{
	switch (Formation)
	{
		case FormationType::LINE:
			Formation = FormationType::CIRCLE;
			break;

		case FormationType::TRIANGLE:
			Formation = FormationType::LINE;
			break;

		case FormationType::CIRCLE:
			LowerShield();
			Formation = FormationType::TRIANGLE;
			break;
	}
}


void ACompanyLeader::SetFormation()
{
	TArray<FVector> List;

	switch (Formation)
	{
		case FormationType::LINE:
			CreateLineFormation(Soldiers.Num(), DistanceSoldierLine, this, List);
			break;

		case FormationType::TRIANGLE:
			CreateTriangleFormation(Soldiers.Num(), DistanceSoldierTriangle, this, List);
			break;

		case FormationType::CIRCLE:
			CreateCircleFormation(Soldiers.Num(), Soldiers.Num() - NbSoldierAlive, (DistanceSoldierCircle / MaximumSoldierNb) * NbSoldierAlive , this, List);
			break;
	}

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		Soldiers[i]->SetGoToLocation(List[i]);
		Soldiers[i]->SetOrientation(this, Formation);
	}
}


void ACompanyLeader::CreateFormation(unsigned int Division, unsigned int Number, float DistanceFromReference, AActor* Reference, TArray<FVector>& Locations)
{
	const FVector ReferenceForward	{Reference->GetActorForwardVector()};
	const FVector ReferenceLocation	{Reference->GetActorLocation()};

	unsigned int Total{0u};

	float Add = (Division % 2 == 1) ? .0f : -.5f;

	for (unsigned int e = 1; e < (Number / Division) + 1; e++)
	{

		for (unsigned int i = 0; i < Division; i++)
		{
			const FVector SetVector{ReferenceForward.RotateAngleAxis((360.f / (Division)*Add) + (360.f / Division) * i, FVector::UpVector)};

			Locations.Add(ReferenceLocation + SetVector.GetSafeNormal() * (DistanceFromReference * e));

			Total++;

			if (Total >= Number)
				return;
		}

		for (unsigned int i = 0; i < Division; i++)
		{
			const FVector SetVector{ReferenceForward.RotateAngleAxis((360.f / (Division)*Add) + (360.f / (Division) * 0.5f) + (360.f / Division) * i, FVector::UpVector)};

			Locations.Add(ReferenceLocation + SetVector.GetSafeNormal() * ((DistanceFromReference / 2) * e));

			Total++;

			if (Total >= Number)
				return;
		}
	}
}


void ACompanyLeader::MeleeAttack()
{
	LastAttackIsSpecial = false;
	ControllerMeleeAttack.Broadcast();
}


void ACompanyLeader::StopMeleeAttack()
{
	ControllerStopMeleeAttack.Broadcast();
}


void ACompanyLeader::RaiseShield()
{
	if (Formation == FormationType::CIRCLE)
	{
		ControllerRaiseShield.Broadcast();

		GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed * BlockingSlowFactor;
	}
}


void ACompanyLeader::LowerShield()
{
	ControllerLowerShield.Broadcast();
	
	if (!IsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
	}
}


void ACompanyLeader::ReceiveAttack(const ACompanyLeader* Attacker, ASoldier* LastSoldierHit)
{
	// Is this leader still active?
	if (!IsAlive() || LastSoldierHit->IsBlocking())
	{
		return;
	}

	const float TimeSeconds{ GetWorld()->TimeSeconds };

	// Is the company immune to damage for now?
	if (TimeSeconds - InvicibilityBeginTime < InvicibilityDurationAfterAttack)
	{
		return;
	}

	if (ArmorBreak(Attacker->Formation, Attacker->LastAttackIsSpecial))
	{
		if (LastSoldierHit->IsBlocking())
		{
			LastSoldierHit->GotHit(.25f);
		}

		else
		{
			bool ThresholdPassed{ false };
			ApplyDamage(GetDamageBetweenCompanies(Attacker, this), &ThresholdPassed);
			DamageEvent.Broadcast(Attacker, this);

			ThresholdPassed ? LastSoldierHit->Kill() : LastSoldierHit->GotHit(.25f);
		}
	}
}


void ACompanyLeader::ReceiveAttack(const AProjectile* Projectile, ASoldier* LastSoldierHit)
{
	// Is this leader still active?
	if (!IsAlive())
	{
		return;
	}

	if (ArmorBreak())
	{
		if (LastSoldierHit->IsBlocking())
		{
			LastSoldierHit->GotHit(.25f);
		}

		else
		{
			bool ThresholdPassed{false};
			ApplyDamage(Projectile->Damage, &ThresholdPassed);

			ThresholdPassed ? LastSoldierHit->Kill() : LastSoldierHit->GotHit(.25f);
		}
	}
}

bool ACompanyLeader::ArmorBreak(FormationType CheckedFormation, bool IsAttackSpecial)
{
	if (CheckedFormation == FormationType::TRIANGLE && IsAttackSpecial)
	{
		ArmorValue--;
	}


	if (!ArmorActive)
	{
		return true;
	}
	else if (ArmorValue <= 0)
	{
		ArmorActive = 0;
		GetWorldTimerManager().ClearTimer(ArmorTimer);

		if (CanRestoreArmorAfterBreak)
		{
			GetWorldTimerManager().ClearTimer(ArmorTimer);
			GetWorldTimerManager().SetTimer(ArmorTimer, this, &ACompanyLeader::ArmorReset, RestoreArmorAfterBreakTimer, false);
		}

		return true;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ArmorTimer);
		GetWorldTimerManager().SetTimer(ArmorTimer, this, &ACompanyLeader::ArmorReset, ResetArmorTimer, false);
	}
	return false;
}

void ACompanyLeader::ArmorReset()
{
	ArmorActive = true;
	ArmorValue = MaxArmorValue;
}

void ACompanyLeader::CreateCircleFormation(unsigned int Number, unsigned int NbDeadSoldier, float DistanceFromReference, AActor* Reference, TArray<FVector>& Locations)
{
	const FVector ReferenceForward	{Reference->GetActorForwardVector()};
	const FVector ReferenceLocation	{Reference->GetActorLocation()};

	for (unsigned int i = 0; i < Number; i++)
	{
		if (i < Number - NbDeadSoldier)
		{
			const FVector SetVector{ReferenceForward.RotateAngleAxis((360.f / (Number - NbDeadSoldier)) * i, FVector::UpVector)};
			Locations.Add((SetVector * DistanceFromReference) + ReferenceLocation);
		}

		else
		{
			Locations.Add(ReferenceLocation);
		}
	}
}


void ACompanyLeader::CreateTriangleFormation(unsigned int Number, float DistanceFromReference, AActor* Reference, TArray<FVector>& Locations)
{
	if (Number == 0u)
	{
		return;
	}

	const FVector ReferenceForward	{Reference->GetActorForwardVector()};
	const FVector ReferenceLocation	{Reference->GetActorLocation()};
	const FVector BottomRight		{(ReferenceForward.RotateAngleAxis(360.f / 3.f, FVector::UpVector) * DistanceFromReference) + ReferenceLocation};
	const FVector BottomLeft		{(ReferenceForward.RotateAngleAxis(-360.f / 3.f, FVector::UpVector) * DistanceFromReference) + ReferenceLocation};
	const FVector InitialLocation	{(ReferenceForward * DistanceFromReference) + ReferenceLocation};

	FVector SetVector;

	if (Number >= 1)
	{
		Locations.Add(InitialLocation);
	}
	if (Number >= 2)
	{
		SetVector = InitialLocation + (BottomRight - InitialLocation) * 1.f / 3.f;
		Locations.Add(SetVector);
	}
	if (Number >= 3)
	{
		SetVector = InitialLocation + (BottomLeft - InitialLocation) * 1.f / 3.f;
		Locations.Add(SetVector);
	}
	if (Number >= 4)
	{
		SetVector = InitialLocation + (BottomLeft - InitialLocation) * 2.f / 3.f;
		Locations.Add(SetVector);
	}
	if (Number >= 5)
	{
		SetVector = InitialLocation + (BottomRight - InitialLocation) * 2.f / 3.f;
		Locations.Add(SetVector);
	}
	if (Number >= 6)
	{
		SetVector = ReferenceForward.RotateAngleAxis(360.f / 3.f, FVector::UpVector);
		Locations.Add((SetVector * DistanceFromReference) + ReferenceLocation);
	}
	if (Number >= 7)
	{
		SetVector = ReferenceForward.RotateAngleAxis(-360.f / 3.f, FVector::UpVector);
		Locations.Add((SetVector * DistanceFromReference) + ReferenceLocation);
	}
	if (Number >= 8)
	{
		Locations.Add(ReferenceLocation);
	}
	if (Number >= 9)
	{
		Locations.Add(ReferenceLocation + ReferenceForward * (DistanceFromReference * -.5f));
	}
}


void ACompanyLeader::CreateLineFormation(unsigned int Number, float DistanceFromReference, AActor* Reference, TArray<FVector>& Locations)
{
	if (Number == 0u)
		return;

	const FVector ReferenceForward	{Reference->GetActorForwardVector()};
	const FVector ReferenceLocation {Reference->GetActorLocation()};
	const FVector Front				{(ReferenceForward * DistanceFromReference * .5f) + ReferenceLocation};
	const FVector Right				{ReferenceForward.RotateAngleAxis(90.f, FVector::UpVector) * DistanceFromReference};
	const FVector Bottom			{(ReferenceForward * DistanceFromReference * -.5f) + ReferenceLocation};
	FVector	SetVector;

	if (Number >= 1)
		Locations.Add((ReferenceForward * DistanceFromReference * .5f) + ReferenceLocation);

	if (Number >= 2)
	{
		SetVector = Front + Right * .5f;
		Locations.Add(SetVector);
	}
	if (Number >= 3)
	{
		SetVector = Front + Right * -.5f;
		Locations.Add(SetVector);
	}
	if (Number >= 4)
	{
		SetVector = Front + Right * 1.f;
		Locations.Add(SetVector);
	}
	if (Number >= 5)
	{
		SetVector = Front + Right * -1.f;
		Locations.Add(SetVector);
	}
	if (Number >= 6)
	{
		SetVector = Bottom + Right * .25f;
		Locations.Add(SetVector);
	}
	if (Number >= 7)
	{
		SetVector = Bottom + Right * -.25f;
		Locations.Add(SetVector);
	}
	if (Number >= 8)
	{
		SetVector = Bottom + Right * .75f;
		Locations.Add(SetVector);
	}
	if (Number >= 9)
	{
		SetVector = Bottom + Right * -.75f;
		Locations.Add(SetVector);
	}
}


void ACompanyLeader::SortSoldierList()
{
	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (!Soldiers[i]->IsAlive())
		{
			for (int32 e = Soldiers.Num() - 1 ; e > i; e--)
			{
				if (Soldiers[e]->IsAlive())
				{
					Soldiers.Swap(e, i);
					break;
				}
			}
		}
	}
}


float ACompanyLeader::GetFormationRadius()
{
	switch (Formation)
	{
		case FormationType::LINE:
			return DistanceSoldierLine;

		case FormationType::TRIANGLE:
			return DistanceSoldierTriangle;

		case FormationType::CIRCLE:
			return DistanceSoldierCircle;

		default:
			return .0f;
	}
}


void ACompanyLeader::SoldierDeath(ASoldier* soldier)
{
	NbSoldierAlive--;
	StopSendingOrdersTo(*soldier);
	SortSoldierList();

	if (NbSoldierAlive < MinimumSoldierNb)
	{
		LeaderDeath();
	}
}


void ACompanyLeader::LeaderDeath()
{
	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (Soldiers[i]->IsAlive())
		{
			Soldiers[i]->Kill();
			i--;
		}
	}

	GetCharacterMovement()->Deactivate();

	GetWorldTimerManager().ClearTimer(DashTimer);
	GetWorldTimerManager().SetTimer(DashTimer, this, &ACompanyLeader::TimerLeaderDeath, TimeBeforeDestroy, false);

	OnCompanyDeath();
}


void ACompanyLeader::TimerLeaderDeath()
{
	Destroy();
}


void ACompanyLeader::StartDash()
{
	if (IsDashing || (!CanDashWhileJumping && !GetCharacterMovement()->IsMovingOnGround()))
	{
		return;
	}

	if (ResetVelocityAtStartDash)
	{
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	
		for (int32 i = 0; i < Soldiers.Num(); i++)
		{
			Soldiers[i]->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		}
	}
	
	GetCharacterMovement()->Velocity = GetActorForwardVector() * DashSpeed;

	GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
	GetCharacterMovement()->MaxAcceleration = DashAcceleration;

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (Soldiers[i]->IsAlive())
		{
			Soldiers[i]->GetCharacterMovement()->Velocity = GetActorForwardVector() * DashSpeed;
		}
	}

	IsDashing = true;

	GetWorldTimerManager().ClearTimer(DashTimer);
	GetWorldTimerManager().SetTimer(DashTimer, this, &ACompanyLeader::StopDash, DashDuration, false);

	for (ASoldier* Soldier : Soldiers)
	{
		Soldier->EnterDash();
	}
}


void ACompanyLeader::StopDash()
{
	if (!IsDashing)
	{
		return;
	}

	IsDashing = false;

	if (ResetVelocityAfterDash)
	{
		GetCharacterMovement()->Velocity *= ResetVelocityFactor;

		for (int32 i = 0; i < Soldiers.Num(); i++)
		{
			if (Soldiers[i]->IsAlive())
				Soldiers[i]->GetCharacterMovement()->Velocity *= ResetVelocityFactor;
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
	GetCharacterMovement()->MaxAcceleration = OriginalAcceleration;

	for (ASoldier* Soldier : Soldiers)
	{
		Soldier->ExitDash();
	}
}


void ACompanyLeader::StartSprint()
{
	if (IsDashing)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed	= SprintSpeed;
	GetCharacterMovement()->MaxAcceleration	= SprintAcceleration;
}


void ACompanyLeader::StopSprint()
{
	if (IsDashing)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed	= OriginalSpeed;
	GetCharacterMovement()->MaxAcceleration	= OriginalAcceleration;
}


void ACompanyLeader::SpecialAttack()
{
	switch (Formation)
	{
		case FormationType::LINE:
			if (CanSpecialLine)
			{
				SpecialAttackLine();
			}
			break;

		case FormationType::TRIANGLE:
			if (CanSpecialTriangle)
			{
				SpecialAttackTriangle();

				CanSpecialTriangle = false;
				GetWorldTimerManager().ClearTimer(SpecialTimerTriangle);
				GetWorldTimerManager().SetTimer(SpecialTimerTriangle, this, &ACompanyLeader::SpecialAttackTimerTriangle, SpecialAttackCooldown(), false);
			}
			break;

		default:
			break;
	}
}


void ACompanyLeader::SpecialAttackTriangle()
{
	StartDash();
	LastAttackIsSpecial = true;

	CanSpecialTriangle = true;
}


void ACompanyLeader::SpecialAttackLine()
{
	for (const ASoldier* Soldier : Soldiers)
	{
		if (Soldier->IsAttacking())
			return;
	}

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (Soldiers[i]->IsAlive())
		{
			Soldiers[i]->RangedAttack();
		}
	}
}


void ACompanyLeader::SpecialAttackTimerTriangle()
{
	CanSpecialTriangle = true;
}


void ACompanyLeader::SpecialAttackTimerLine()
{
	CanSpecialLine = true;
}


void ACompanyLeader::SetSpecialAttackOnCooldown()
{
	if (CanSpecialLine)
	{
		CanSpecialLine = false;
		GetWorldTimerManager().ClearTimer(SpecialTimerLine);
		GetWorldTimerManager().SetTimer(SpecialTimerLine, this, &ACompanyLeader::SpecialAttackTimerLine, SpecialAttackCooldown(), false);
	}
}


float ACompanyLeader::SpecialAttackCooldown()
{
	switch (Formation)
	{
	case FormationType::LINE:
		return CooldownLine;

	case FormationType::TRIANGLE:
		return CooldownTriangle + DashDuration;
	}

	return 0.f;
}

float ACompanyLeader::GetStrengthFactor(FormationType CheckedFormation) const
{
	switch (CheckedFormation)
	{
	case FormationType::LINE:
		return StrengthFactorLine;

	case FormationType::TRIANGLE:
		return StrengthFactorTriangle;
	case FormationType::CIRCLE:
		return StrengthFactorCircle;
	}

	return 0.f;
}

float ACompanyLeader::GetWeaknessFactor(FormationType CheckedFormation) const
{
	switch (CheckedFormation)
	{
	case FormationType::LINE:
		return WeaknessFactorLine;

	case FormationType::TRIANGLE:
		return WeaknessFactorTriangle;

	case FormationType::CIRCLE:
		return WeaknessFactorCircle;
	}

	return 0.f;
}

float ACompanyLeader::GetDamageBetweenCompanies(const ACompanyLeader* Attacker, const ACompanyLeader* Defender, float Damage)
{
	return Attacker->GetStrengthFactor(Defender->Formation) * Defender->GetWeaknessFactor(Attacker->Formation) * Damage;
}

void ACompanyLeader::ResetSoldiers()
{
	Health = MaxHealth;

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		if (!Soldiers[i]->IsAlive())
		{
			ASoldier* soldier = Soldiers[i];
			Soldiers.Remove(soldier);
			soldier->Destroy();
			i--;
		}
	}
	RefillArmy();
	NextHealthThreshold = MaxHealth - HealthThresholdOffset;
}


void ACompanyLeader::Destroyed()
{
	Super::Destroyed();

	for (int32 i = 0; i < Soldiers.Num(); i++)
	{
		Soldiers[i]->Destroy();
	}
}

float ACompanyLeader::GetSpecialCooldownFactor()
{
	switch (Formation)
	{
	case FormationType::LINE:
		return (GetWorldTimerManager().GetTimerElapsed(SpecialTimerLine) /  CooldownLine);

	case FormationType::TRIANGLE:
		return (GetWorldTimerManager().GetTimerElapsed(SpecialTimerTriangle) / (CooldownTriangle + DashDuration));
	}

	return 1.f;
}