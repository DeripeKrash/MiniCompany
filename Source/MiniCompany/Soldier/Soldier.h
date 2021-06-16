// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Delegates/Delegate.h"
#include "Soldier.generated.h"


UCLASS()
class MINICOMPANY_API ASoldier : public ACharacter
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSoldierDeleguate);

	UPROPERTY(BlueprintAssignable)
	FSoldierDeleguate SoldierKillEvent;

	FTimerHandle StunTimer;

	// Next destination
	FVector GoToLocation;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree{ nullptr };

	bool AIUsed { false };

protected:
	// Internal
	// Material for Color
	class UMaterialInstanceDynamic* DynamicMaterialBody{nullptr};

	// Weapons
	class UStaticMeshComponent* Sword		{nullptr};

	class UStaticMeshComponent* Shield		{nullptr};

	class UStaticMeshComponent* Bow			{nullptr};

	// Collision box
	class UBoxComponent*		SwordBox	{nullptr};

	// Accessible from the inspector
	// Animation instance to trigger animations
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Lead")
	class ACompanyLeader*		Leader		{nullptr};

	UPROPERTY(BlueprintReadWrite)
	class USoldierAnim*			SoldierAnim	{nullptr};

	// Projectile launch on Line Special Attack
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<class AProjectile> projectileType{ nullptr };

public:
	/* ==== Data members ==== */
	// Radius that determine at which point the soldier stop moving
	UPROPERTY(EditAnywhere, Category="Move", meta=(UIMin="0.1"))
	float StopRadius						{5.f};
	float SquaredStopRadius;

	// Determine at which distance from the aimed point in the formation the soldier should Run
	UPROPERTY(EditAnywhere, Category="Move", meta=(UIMin="0.1"))
	float SprintRadius						{5.f};
	float SquaredSprintRadius;

	// Factor get the speed of the Leader and multiply it when the soldier Run
	UPROPERTY(EditAnywhere, Category="Move", meta=(UIMin="0.1"))
	float SprintFactor						{2.f};

	// Radius beyond which a soldier is considered too far from its leader
	UPROPERTY(EditAnywhere, Category="Move", meta=(UIMin="0.1"))
	float TPActivationRadius				{4000.f};
	float SquaredTPActivationRadius;

	UPROPERTY(EditAnywhere, Category = "Move", meta = (UIMin = "0.1"))
	float AiActivationRadius{ 2000.f };
	float SquaredAiActivationRadius;

	UPROPERTY(EditAnywhere, Category="Move")
	float StunDuration						{3.f};

	// Move properties
	bool PerfectFollow						{false};

protected:
	bool bIsAlive							{true};

public:
	ASoldier();

	void Tick(float DeltaTime) final;
	void BeginPlay() final;

	/* ==== Callbacks ==== */
	UFUNCTION()
	void SwordBeginOverlap(UPrimitiveComponent*	OverlappedComponent,
						   AActor*				OtherActor,
						   UPrimitiveComponent*	OtherComp,
						   int32				OtherBodyIndex,
						   bool					bFromSweep,
						   const FHitResult&	SweepResult);


	/* ==== Leader-related ==== */
	void SetMovementsFromLeader(class ACompanyLeader* _Leader);
	void DetachFromLeader();


	/* ==== Soldier actions ==== */
	void Move();

	UFUNCTION(BlueprintCallable)
	void MeleeAttack() noexcept;

	UFUNCTION(BlueprintCallable)
	void StopMeleeAttack() noexcept;

	UFUNCTION(BlueprintCallable)
	void RangedAttack();

	UFUNCTION(BlueprintCallable)
	void RaiseShield();

	UFUNCTION(BlueprintCallable)
	void LowerShield();
		
	UFUNCTION(BlueprintCallable)
	void ToggleSwordCollision(bool Toggle);

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile();

	UFUNCTION(BlueprintCallable)
	void GotHit(float Intensity = 1.f);

	UFUNCTION(BlueprintCallable)
	void Kill();

	UFUNCTION(BlueprintCallable)
	void Resurrect();


	/* ==== Setters ==== */
	void SetSoldierFromReferenceSoldier(ASoldier* soldier);
	void SetOrientation(AActor* Actor, enum FormationType Formation);
	void SetColor(FVector Color, float Alpha = 1.f);
	void SetGoToLocation(FVector Location);
	void EnterDash() noexcept;
	void ExitDash() noexcept;
	void EnterStun();
	void ExitStun();

	UFUNCTION(BlueprintCallable)
	void SetUseSword(bool Toggle);

	UFUNCTION(BlueprintCallable)
	void SetUseShield(bool Toggle);

	UFUNCTION(BlueprintCallable)
	void SetUseBow(bool Toggle);

	/* ==== Getters ==== */
	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const noexcept;

	UFUNCTION(BlueprintCallable)
	bool IsBlocking() const noexcept;

	// Inlined
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const noexcept
	{ return bIsAlive; }

	UFUNCTION(BlueprintCallable)
	class ACompanyLeader* GetLeader() const noexcept
	{ return Leader; }
};
