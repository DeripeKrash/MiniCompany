#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MINICOMPANY_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	FTimerHandle timer;

	class ACompanyLeader const* OriginCompany;

	// Time the Arrow will stay on the field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float lifeDuration{ 5.f };

	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float speed {500.f};


	bool isInstanciable = { true };

protected:

	// Collision boxes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* hitBox{ nullptr };

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	virtual void ProjectileDestruction();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitProjectile(const class ACompanyLeader* OriginCompany, const FVector& OriginSoldierLocation, float Damage);

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor*					OtherActor,
			UPrimitiveComponent*	OtherComp,
			int32					OtherBodyIndex,
			bool					bFromSweep,
			const FHitResult&		SweepResult);

};
