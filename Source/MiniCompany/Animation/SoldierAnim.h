#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SoldierAnim.generated.h"

UCLASS()
class MINICOMPANY_API USoldierAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// Proprietary Soldier
	UPROPERTY(BlueprintReadWrite, Category="Target")
	class ASoldier* Owner		{nullptr};

	// Blend parameters
	UPROPERTY(BlueprintReadWrite, Category="Blend parameters")
	float ForwardInputValue		{.0f};

	UPROPERTY(BlueprintReadWrite, Category="Blend parameters")
	float SideInputValue		{.0f};

	UPROPERTY(BlueprintReadWrite, Category="Blend parameters")
	float HitIntensity			{.0f};

	// Transition parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool AttackReceived			{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool LethalAttackReceived	{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool Stunned				{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool AttackBlocked			{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool BlockingOnGoing		{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool RangedAttackOnGoing	{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State machine")
	bool DashOnGoing			{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage state")
	bool AttackOnGoing			{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage state")
	bool KeepOnAttacking		{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage state")
	bool NextAttackBuffered		{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage state")
	bool PollNextAttack			{false};


	// Inherited
	void NativeBeginPlay() override;

public:
	// Inherited
	void NativeUpdateAnimation(float DeltaTime) final;

	// Called between AttackCheckEnd and AttackEnd notifies.
	// Pressing the attack button will set KeepOnAttacking to true, and so will
	// keeping the attack button pressed. This is to detect whether the player
	// wants to keep on attacking during a short lapse of time at the end of
	// their current attack
	UFUNCTION(BlueprintCallable)
	void ConfirmNextAttack() noexcept;

	// Getter used before launching the attack Anim Montage
	UFUNCTION(BlueprintCallable)
	bool IsStaggered() const noexcept;

	// Reset flags to interrupt an attack chain
	UFUNCTION(BlueprintCallable)
	void ResetAttackFlags() noexcept;

	FORCEINLINE bool IsAttacking() const noexcept
	{ return AttackOnGoing; }

	FORCEINLINE bool IsBlocking() const noexcept
	{ return BlockingOnGoing; }

	FORCEINLINE bool IsStunned() const noexcept
	{ return Stunned; }

	// State and transition update
	FORCEINLINE void MeleeAttack() noexcept
	{ KeepOnAttacking = AttackOnGoing = true; }

	FORCEINLINE void StopMeleeAttack() noexcept
	{ KeepOnAttacking = false; }

	FORCEINLINE void RangedAttack() noexcept
	{ RangedAttackOnGoing = true; }

	FORCEINLINE void ToggleShield(bool Toggle) noexcept
	{ BlockingOnGoing = Toggle; }

	FORCEINLINE void ToggleStunned(bool Toggle) noexcept
	{ Stunned = Toggle; }

	FORCEINLINE void ToggleDash(bool Toggle) noexcept
	{ DashOnGoing = Toggle; }

	FORCEINLINE void GotLethallyHit() noexcept
	{ LethalAttackReceived = true; }

	FORCEINLINE void GotHit(float Intensity = 1.f) noexcept
	{
		if (BlockingOnGoing)
			AttackBlocked = true;

		else
		{
			AttackReceived	= true;
			HitIntensity	= Intensity;
		}
	}
};