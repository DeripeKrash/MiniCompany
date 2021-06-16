// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_LookAtPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UMyBTTask_LookAtPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    AActor* actor = Cast<AActor>(BlackboardComp->GetValueAsObject("SelfActor"));

    FVector orientation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - actor->GetActorLocation();

    FRotator Rot = FRotationMatrix::MakeFromX(FVector(orientation.X, orientation.Y, 0.f)).Rotator();

    actor->SetActorRotation(Rot);


    return EBTNodeResult::Succeeded;
}