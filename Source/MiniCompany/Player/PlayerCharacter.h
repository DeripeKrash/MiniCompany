// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Leader/CompanyLeader.h"
#include "Delegates/Delegate.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class MINICOMPANY_API APlayerCharacter : public ACompanyLeader
{
	GENERATED_BODY()

protected:
	FQuat StartMoveCam;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeleguate);

	UPROPERTY(BlueprintAssignable)
	FPlayerDeleguate PlayerSwitchFormationEvent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 HorizontalAxisFactor	{1};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 VerticalAxisFactor	{1};


protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CameraReset", meta=(AllowPrivateAccess="true"))
	float TimeCamOnReset 	{.1f};
	float CompletionMoveCam {.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CameraReset", meta=(AllowPrivateAccess="true"))
	float PitchCamReset		{345.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	bool ResetCam 			{false};
	bool Lock				{false};
	
	virtual void BeginPlay() final;

	void ControllerYawInput(float Val);
	void ControllerPitchInput(float Val);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void ResetCamera();
	void ResetCamera(float deltaTime);

	void Fire();
	void Hire();

	void StartLock();
	void EndLock();

	virtual void SwitchFormationNext() override;
	virtual void SwitchFormationPrevious() override;

public:
	APlayerCharacter();

	void SetPlayer();

	virtual void Tick(float DeltaTime) final;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final;

	void SoldierDeath(ASoldier* soldier) final;
	void LeaderDeath() final; 

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
