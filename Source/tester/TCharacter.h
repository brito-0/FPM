// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UENUM()
enum class ETCharacterState : uint8
{
	Normal,
	Crouch,
	Run,
	Dead
};

UCLASS()
class TESTER_API ATCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// get functions
	UFUNCTION(BlueprintCallable, Category = Camera)
	UCameraComponent* GetTCameraComponent() { return TCameraComponent; }
	UFUNCTION(BlueprintCallable, Category = CharacterState)
	ETCharacterState GetCharacterState() const { return CharacterState; }
	UFUNCTION(BlueprintCallable, Category = CharacterState)
	ETCharacterState GetPrevCharacterState() const { return PrevCharacterState; }
	UFUNCTION(BlueprintCallable, Category = CharacterStats)
	float GetCharacterMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintCallable, Category = CharacterStats)
	float GetCharacterCurrentHealth() const { return CurrentHealth; }
	UFUNCTION(BlueprintCallable, Category = CharacterStats)
	float GetCharacterCurrentSpeed() const { return GetCharacterMovement()->MaxWalkSpeed; };


	UFUNCTION(Blueprintable, Category = Character)
	bool CharacterTakeDamage(const float Damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/***/
	void MoveForward(const float Value);
	/***/
	void MoveRight(const float Value);

	/***/
	void Move(const FInputActionValue &Value);
	/***/
	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent *TCameraComponent;


	// input
	/** mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;
	/** move input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;
	/** look input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;
	/** crouch input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *CrouchAction;
	/** run input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *RunAction;
	/** jump input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;
	/** interact input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *InteractAction;
	/** ranged attack input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *RangedAction;
	/** melee attack input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MeleeAction;
	/** heal input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *HealAction;
	/** teleport input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *TeleportAction;
	/** TEST INPUT ACTION */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *TestAction;

	
	/***/
	UPROPERTY(VisibleAnywhere)
	ETCharacterState CharacterState = ETCharacterState::Crouch;
	/***/
	UPROPERTY(VisibleAnywhere)
	ETCharacterState PrevCharacterState;

	/***/
	void SetCharacterState(const ETCharacterState State);

	// state change functions
	void ChangeStateNormal() { SetCharacterState(ETCharacterState::Normal); }
	void ChangeStateCrouch() { SetCharacterState(ETCharacterState::Crouch); }
	void ChangeStateRun() { SetCharacterState(ETCharacterState::Run); }
	void ChangeStateDead() { SetCharacterState(ETCharacterState::Dead); }


	
	// character movement speeds
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float WalkSpeed = 750.f;
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float RunSpeed = 950.f;
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float CrouchSpeed = 450.f;
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float MaxSpeed = 4515.f;

	// health
	const float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;

	// capsule
	const float CapsuleRadius = 50.f;
	const float CapsuleHeight = 96.f;
	const float CrouchCapsuleHeight = 60.f;
	// offset to change the position of the capsule
	const float CrouchOffset = 36.f;
	// length of the ray cast to check for collisions when increasing the size of the capsule
	const float CrouchStandingLineLength = 140.f;

	UPROPERTY(VisibleAnywhere, Category = Stuck)
	bool bStuck = false;
	UPROPERTY(VisibleAnywhere, Category = Stuck)
	FVector StuckLoc;
	
	void CharacterCrouch();
	void CharacterUnCrouch();

	void CanStandDelay();
	void CanStand();

	bool CheckCapsule() const;

	FTimerHandle StopRunHandle;
	const float StopRunDelay = .3f;
	void CharacterStopRun();

	const float StandTimeDelay = .25f;

	void CapsuleChangeNormal();

	void CharacterJump();
	void CharacterJumpDamage();
	FTimerHandle JumpDamageHandle;


	// movement
	UPROPERTY()
	float PrevAngle;
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxTurnAngle = 44.f;
	UPROPERTY()
	int16 PrevHeight;
	UPROPERTY(VisibleAnywhere, Category = Movement)
	int16 JumpStartHeight;
	UPROPERTY()
	int16 JumpEndHeight;
	UPROPERTY(EditAnywhere, Category = Movement)
	int16 JumpMaxHeight = 850;
	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseFallDamage = 25.f;
	UPROPERTY(VisibleAnywhere, Category = Movement)
	bool bFalling = false;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	bool bMomentum = false;

	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedDecayRate = 5.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedDecayRateGround = 15.f;

	// attacks
	UPROPERTY(VisibleAnywhere, Category = Attack)
	bool bCanAttack = true;
	FTimerHandle AttackHandle;
	UPROPERTY(EditAnywhere, Category = Attack)
	float AttackCooldown = 2.f;
	void SetCanAttackTrue() { bCanAttack = true; }

	void CharacterRangedAttack();
	UPROPERTY(EditAnywhere, Category = Attack)
	float RangedAttackCost = 15.f;
	UPROPERTY(EditAnywhere, Category = Attack)
	float RangedAttackRange = 1500.f;

	void CharacterMeleeAttack();
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackCost = 10.f;
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRange = 500.f;
	void CharacterMeleeRecoil() const;
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRecoilRange = -650.f;
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRecoilRangeGround = -750.f;
	// const float MeleeAttackCooldown = 1.f;
	// const float MeleeAttack

	
	// heal
	void CharacterHeal();
	UPROPERTY(VisibleAnywhere, Category = Heal)
	bool bHealing = false;
	UPROPERTY(EditAnywhere, Category = Movement)
	float HealSpeedModifier = .6f;
	FTimerHandle HealHandle;
	UPROPERTY(EditAnywhere, Category = Heal)
	float HealDuration = 3.5f;
	UPROPERTY(EditAnywhere, Category = Heal)
	float HealAmount = 25.f;
	void CharacterFinishHeal();
	void SetHealingFalse() { bHealing = false; }

	void CharacterChangeSpeed(const float Value) const;

	
	// teleport
	void InstantTeleport();
	bool CheckCollision(const FVector& TeleportLocation);
	UPROPERTY(EditAnywhere, Category = Teleport)
	bool bCanTeleport = true;
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportMinRange = 450.f;
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportMaxRange = 5000.f;
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportLocationOffset = 50.f;
	FTimerHandle TeleportHandle;
	void SetCanTeleportTrue() { bCanTeleport = true; }
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportCooldown = 5.f;

	const float TeleportSizeSide = 55.f;
	const float TeleportSizeStand = 197.f;
	const float TeleportSizeCrouch = 125.f;


	void Test();
	
};
