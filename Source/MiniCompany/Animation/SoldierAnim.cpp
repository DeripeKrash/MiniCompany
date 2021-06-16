#include "SoldierAnim.h"
#include "Animation/AnimMontage.h"
#include "../Soldier/Soldier.h"

void USoldierAnim::NativeBeginPlay()
{
    UAnimInstance::NativeBeginPlay();

    Owner = Cast<ASoldier>(TryGetPawnOwner());
}


void USoldierAnim::NativeUpdateAnimation(float DeltaTime)
{ Super::NativeUpdateAnimation(DeltaTime); }


void USoldierAnim::ConfirmNextAttack() noexcept
{
    if (AttackOnGoing && PollNextAttack && !NextAttackBuffered)
    {
        NextAttackBuffered = KeepOnAttacking;
    }
}


bool USoldierAnim::IsStaggered() const noexcept
{ return AttackReceived | LethalAttackReceived | AttackBlocked | BlockingOnGoing | Stunned; }


void USoldierAnim::ResetAttackFlags() noexcept
{ PollNextAttack = AttackOnGoing = NextAttackBuffered = KeepOnAttacking = false; }