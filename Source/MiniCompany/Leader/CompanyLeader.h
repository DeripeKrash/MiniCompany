#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Delegates/Delegate.h"

#include "CompanyLeader.generated.h"

UENUM(BluePrintType)
enum FormationType
{
	TRIANGLE	UMETA(DisplayName="TRIANGLE"),
	LINE		UMETA(DisplayName="LINE"),
	CIRCLE		UMETA(DisplayName="CIRCLE")
};


UCLASS()
class MINICOMPANY_API ACompanyLeader : public ACharacter
{
	GENERATED_BODY()

public:
	/* ==== Delegates ==== */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FControllerDeleguate);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCompanyAttackDeleguate, const ACompanyLeader*, Attacker, const ACompanyLeader*, Defender);

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerMove;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerJump;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerJumpStop;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerMeleeAttack;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerStopMeleeAttack;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerRaiseShield;

	UPROPERTY(BlueprintAssignable)
	FControllerDeleguate ControllerLowerShield;

	UPROPERTY(BlueprintAssignable)
	FCompanyAttackDeleguate DamageEvent;


	/* ==== Company Creation ====*/

	FTimerHandle deathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Company settings", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class ASoldier> BaseSoldier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Company settings", meta=(AllowPrivateAccess="true"))
	TArray<class ASoldier*> Soldiers{};

	/* ==== Leader Gestion =====*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings", meta = (AllowPrivateAccess = "true"))
	float RaduisTpToClosestSoldier{ 500 };

	/* ==== Dash settings ==== */
	FTimerHandle DashTimer;

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Dash", meta=(UIMin="0"))
	float DashSpeed					{5000.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Dash", meta=(UIMin="0"))
	float DashAcceleration			{5000.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Dash", meta=(UIMin="0"))
	float DashDuration				{.5f};

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Dash", meta=(UIMin="0"))
	float DashForce					{500.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Dash", meta=(UIMin="0"))
	float DashUpForce				{100.f};

	/* ==== Company settings ==== */
	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0"))
	unsigned int MinimumSoldierNb	{1u};

	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0"))
	unsigned int MaximumSoldierNb	{9u};

	unsigned int NbSoldierAlive;
	

	/* ==== Formation settings ==== */
	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0.0001"))
	float DistanceSoldierTriangle	{300.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0.0001"))
	float DistanceSoldierCircle		{200.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0.0001"))
	float DistanceSoldierLine		{300.f};


	/* ==== Soldiers movement settings ==== */
	// Radius that determines at which point soldiers stop moving
	UPROPERTY(EditAnywhere, Category="Company settings|Formation", meta=(UIMin="0.1"))
	float StopRadius				{5.f};

	// Determines at which distance from the aimed point in the Formation the soldier should run
	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Movements", meta=(UIMin="0.1"))
	float SprintRadius				{5.f};

	// Movement speed multiplying factor when sprinting
	// TODO (Sami, Denis): clarify what "sprint" should be
	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Movements", meta=(UIMin="0.1"))
	float SprintFactor				{2.f};

	// Radius beyond which soldiers are considered too far from the leader, and TPed back their position in the Formation
	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Movements", meta=(UIMin="0.1"))
	float TPActivationRadius		{4000.f};

	UPROPERTY(EditAnywhere, Category = "Company settings|Actions|Movements", meta = (UIMin = "0.1"))
	float AiActivationRadius{ 2000.f };

	
	/* ==== Sprint settings ==== */
	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Movements", meta=(UIMin="0"))
	float SprintSpeed				{5000.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Actions|Movements", meta=(UIMin="0"))
	float SprintAcceleration		{5000.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Company settings|Actions|Movements", meta = (UIMin = "0"))
	float OriginalSpeed;
	float OriginalAcceleration;

protected:
	/* ==== Health, defense and attack settings ==== */
	// Accessible from the inspector
	UPROPERTY(EditAnywhere, Category="Company settings|Health & defense")
	float MaxHealth					{18.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Health & defense")
	float InvicibilityDurationAfterAttack{1.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Health & defense", meta=(UIMin=".0", UIMax="1."))
	float BlockingSlowFactor		{.5f};

	UPROPERTY(EditAnywhere, Category="Company settings|Attack")
	float MeleeAttackDamage			{1.f};

	UPROPERTY(EditAnywhere, Category="Company settings|Attack")
	float RangedAttackDamage		{.15f};
	

	// Armor
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company settings|Health & defense", meta = (UIMin = ".0"))
	int32 MaxArmorValue	{0};
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Company settings|Health & defense", meta = (UIMin = ".0"))
	int32 ArmorValue	{0};

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company settings|Health & defense")
	bool CanRestoreArmorAfterBreak	{false};

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company settings|Health & defense", meta = (UIMin = ".0"))
	float RestoreArmorAfterBreakTimer	{ 0.1f };

	UPROPERTY(EditAnywhere, Category = "Company settings|Health & defense", meta = (UIMin = ".1"))
	float ResetArmorTimer{ 0.1f };

	bool ArmorActive	{true};
	FTimerHandle ArmorTimer;

	// Attack type
	bool LastAttackIsSpecial{ false };

	/* ==== Attack Strength And Weakness ==== */
	// Strength
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Strength", meta = (AllowPrivateAccess = "true"))
	float StrengthFactorCircle	{1.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Strength", meta = (AllowPrivateAccess = "true"))
	float StrengthFactorLine{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Strength", meta = (AllowPrivateAccess = "true"))
	float StrengthFactorTriangle{ 1.f };

	// Weakness
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Weakness", meta = (AllowPrivateAccess = "true"))
	float WeaknessFactorCircle{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Weakness", meta = (AllowPrivateAccess = "true"))
	float WeaknessFactorLine{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Company settings|Damage Multiplier| Weakness", meta = (AllowPrivateAccess = "true"))
	float WeaknessFactorTriangle{ 1.f };


	// Internal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Company settings|Health & defense")
	float Health;
	float HealthThresholdOffset;
	float NextHealthThreshold;
	float InvicibilityBeginTime;

public:
	/* ==== State and behavior ==== */
	UPROPERTY(EditAnywhere, Category="Company settings")
	float TimeBeforeDestroy			{5.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Company Settings|Formation")
	TEnumAsByte<FormationType> Formation{FormationType::CIRCLE};

	UPROPERTY(EditAnywhere, Category="Company Settings|Formation")
	bool ArmyFollowPerfect			{false};

	UPROPERTY(EditAnywhere, Category="Company Settings|Actions|Dash")
	bool ResetVelocityAtStartDash	{false};

	UPROPERTY(EditAnywhere, Category="Company Settings|Actions|Dash")
	bool ResetVelocityAfterDash		{false};

	UPROPERTY(EditAnywhere, Category = "Company Settings|Actions|Dash", meta = (UIMin = ".0", UIMax = "1."))
	float ResetVelocityFactor		{ 0.5f };

	UPROPERTY(EditAnywhere, Category = "Company Settings|Actions|Dash")
	bool CanDashWhileJumping		{false};

	UPROPERTY(BlueprintReadOnly, Category="Company Settings|Actions|Dash")
	bool IsDashing					{false};


	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company Settings|Actions|SpecialCooldown")
	float CooldownTriangle					{ 1.f };
	
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company Settings|Actions|SpecialCooldown")
	float CooldownLine						{ 1.f };

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company Settings|Actions|SpecialCooldown")
	bool CanSpecialLine					{ true };

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Company Settings|Actions|SpecialCooldown")
	bool CanSpecialTriangle				{ true };
	
	
	FTimerHandle SpecialTimerLine;

	FTimerHandle SpecialTimerTriangle;

	const bool IsEnemy				{false};

protected:
	virtual void BeginPlay() override;

	void RefillArmy();

	virtual void ApplyDamage(float Damage, bool* ThresholdPassed = nullptr);

public:
	ACompanyLeader();

	// Event
	UFUNCTION(BlueprintImplementableEvent)
	void OnCompanyDeath();
	void OnCompanyDeath_Implementation();

	// Static formation creation
	static void CreateCircleFormation(unsigned int Number, unsigned int NbDeadSoldier, float DistanceFromReference, AActor* Controller, TArray<FVector>& ListLocation);
	static void CreateTriangleFormation(unsigned int Number, float DistanceFromReference, AActor* Controller, TArray<FVector>& ListLocation);
	static void CreateLineFormation(unsigned int Number, float DistanceFromReference, AActor* Controller, TArray<FVector>& ListLocation);
	
	void CheckLeaderDistanceRelativeToSoldiers();

	// Inherited
	virtual void Tick(float DeltaTime) override;
	
	// Leader's soldier management
	void SendOrdersTo(class ASoldier& Soldier);
	void StopSendingOrdersTo(class ASoldier& Soldier);
	void SortSoldierList();
	float GetFormationRadius();

	/* ==== Company actions ==== */
	// Formation
	void SetFormation();
	void CreateFormation(unsigned int		Division,
						 unsigned int		Number,
						 float				DistanceFromReference,
						 AActor*			Controller,
						 TArray<FVector>&	ListLocation);
	virtual void SwitchFormationNext();
	virtual void SwitchFormationPrevious();

	// Jump and move
	void ControllerJumpStart();
	void ControllerJumpEnd();

	// Health, defense and attack
	UFUNCTION(BlueprintCallable)
	void MeleeAttack();

	UFUNCTION(BlueprintCallable)
	void StopMeleeAttack();

	UFUNCTION(BlueprintCallable)
	void RaiseShield();

	UFUNCTION(BlueprintCallable)
	void LowerShield();

	void ReceiveAttack(const ACompanyLeader* Attacker, ASoldier* LastSoldierHit);

	void ReceiveAttack(const class AProjectile* Attacker, ASoldier* LastSoldierHit);

	bool ArmorBreak(FormationType CheckedFormation = FormationType::LINE, bool IsAttackSpecial = true);
	void ArmorReset();

	float GetStrengthFactor(FormationType CheckedFormation) const;
	float GetWeaknessFactor(FormationType CheckedFormation) const;

	static float GetDamageBetweenCompanies(const ACompanyLeader* Attacker, const ACompanyLeader* Defender, float Damage = 1.f);

	// Function For AI
	FVector GetClosetSoldierLocation(FVector ReferencePoint);

	// Leader Death
	virtual void LeaderDeath();
	virtual void TimerLeaderDeath();

	// Dash
	void StartDash();
	void StopDash();

	// Sprint
	void StartSprint();
	void StopSprint();

	// Special attacks
	void SpecialAttack();
	void SpecialAttackTriangle();
	void SpecialAttackLine();

	void SpecialAttackTimerTriangle();
	void SpecialAttackTimerLine();

	void SetSpecialAttackOnCooldown();


	float SpecialAttackCooldown();
	
	/* ==== Event and delegates ==== */
	virtual void SoldierDeath(class ASoldier* Soldier);
	
	void ResetSoldiers();
	
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	float GetSpecialCooldownFactor();

	/* ==== Other getters ==== */
	FORCEINLINE bool IsAlive() const noexcept
	{ return Health != .0f; }

	FORCEINLINE float GetRangedAttackDamage() const noexcept
	{ return RangedAttackDamage; }
};