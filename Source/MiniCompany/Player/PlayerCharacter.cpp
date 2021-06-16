// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "../MyGameInstanceMiniCompany.h"
#include "../Soldier/Soldier.h"

#include "PlayerControllerMiniCompany.h"
#include "GameFramework/PlayerInput.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UMyGameInstanceMiniCompany* GameInstance{ Cast<UMyGameInstanceMiniCompany>(GetGameInstance()) };

	if (GameInstance)
		GameInstance->SetPlayer(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ResetCam)
	{
		ResetCamera(DeltaTime);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::ControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::ControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	// Leader input
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACompanyLeader::MeleeAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACompanyLeader::StopMeleeAttack);

	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ACompanyLeader::RaiseShield);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ACompanyLeader::LowerShield);

	PlayerInputComponent->BindAction("Special", IE_Pressed, this, &ACompanyLeader::SpecialAttack);

	PlayerInputComponent->BindAction("NextFormation", IE_Pressed, this, &APlayerCharacter::SwitchFormationNext);
	PlayerInputComponent->BindAction("PreviousFormation", IE_Pressed, this, &APlayerCharacter::SwitchFormationPrevious);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACompanyLeader::ControllerJumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACompanyLeader::ControllerJumpEnd);

	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &APlayerCharacter::StartLock);
	PlayerInputComponent->BindAction("Lock", IE_Released, this, &APlayerCharacter::EndLock);


	PlayerInputComponent->BindAction("ResetCamera", IE_Released, this, &APlayerCharacter::ResetCamera);
}

void APlayerCharacter::ControllerYawInput(float Val)
{
	AddControllerYawInput(Val * HorizontalAxisFactor);
}

void APlayerCharacter::ControllerPitchInput(float Val)
{
	AddControllerPitchInput(Val * VerticalAxisFactor);
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(HorizontalAxisFactor * Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	if (Lock)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		SetActorRotation(YawRotation);
	}
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(VerticalAxisFactor * Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::ResetCamera()
{
	if (ResetCam)
	{
		return;
	}

	ResetCam = true;

	FQuat temp = FQuat::Slerp(Controller->GetControlRotation().Quaternion(), GetActorRotation().Quaternion(), 0.5f);

	FRotator rotTemp(temp);

	StartMoveCam = Controller->GetControlRotation().Quaternion();
	FRotator rot = GetActorRotation();
	rot.Pitch = 90.f;
	CompletionMoveCam = 0.f;

}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != .0f) && !Lock && !IsDashing)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != .0f) && !Lock && !IsDashing)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void APlayerCharacter::Fire()
{
	Soldiers[FMath::RandRange(1, Soldiers.Num() - 1)]->Kill();
	SortSoldierList();
}


void APlayerCharacter::Hire()
{
	Soldiers[FMath::RandRange(1, Soldiers.Num() - 1)]->Resurrect();
	SortSoldierList();
}


void APlayerCharacter::StartLock()
{
	Lock = true;
}
void APlayerCharacter::EndLock()
{
	Lock = false;
}


void APlayerCharacter::ResetCamera(float DeltaTime)
{
	CompletionMoveCam += DeltaTime;

	FRotator Rot = GetActorRotation();
	Rot.Pitch = PitchCamReset;

	FQuat TempQuat;

	if (CompletionMoveCam >= TimeCamOnReset)
	{
		TempQuat = FQuat::Slerp(StartMoveCam, Rot.Quaternion(), 1.f);

		ResetCam = false;
	}
	else
	{
		TempQuat = FQuat::Slerp(StartMoveCam, Rot.Quaternion(), CompletionMoveCam / TimeCamOnReset);
	}

	Controller->SetControlRotation(TempQuat.Rotator());
}


void APlayerCharacter::SwitchFormationNext()
{
	Super::SwitchFormationNext();

	PlayerSwitchFormationEvent.Broadcast();
}

void APlayerCharacter::SwitchFormationPrevious()
{
	Super::SwitchFormationPrevious();

	PlayerSwitchFormationEvent.Broadcast();
}

void APlayerCharacter::SoldierDeath(ASoldier* Soldier)
{
	Super::SoldierDeath(Soldier);
}


void APlayerCharacter::LeaderDeath()
{
	Super::LeaderDeath();

	UMyGameInstanceMiniCompany* GameInstance{ Cast<UMyGameInstanceMiniCompany>(GetGameInstance()) };

	GameInstance->EndGameScreen(GameEndType::DEATH);
}
