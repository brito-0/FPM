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


	/**
	 * reduces the value of @p CurrentHealth and changes @p CharacterState to Dead if it falls bellow 0.
	 *
	 * @param Damage amount to be reduced from @p CurrentHealth
	 * @return @code true@endcode if successful @n @code false@endcode if unsuccessful | NOT IMPLEMENTED |
	 */
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

	/**
	 * changes @p PrevCharacterState to @p CharacterState and changes @p CharacterState to @p State.
	 * 
	 * also applies other changes depending on @p State.
	 * 
	 * @param State new state
	 */
	void SetCharacterState(const ETCharacterState State);

	// state change functions
	void ChangeStateNormal() { SetCharacterState(ETCharacterState::Normal); }
	void ChangeStateCrouch() { SetCharacterState(ETCharacterState::Crouch); }
	void ChangeStateRun() { SetCharacterState(ETCharacterState::Run); }
	void ChangeStateDead() { SetCharacterState(ETCharacterState::Dead); }


	
	// character movement speeds
	/** default: 750.0 */
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float WalkSpeed = 750.f;
	/** default: 950.0 */
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float RunSpeed = 950.f;
	/** default: 450.0 */
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float CrouchSpeed = 450.f;
	/** default: 4515.0 */
	UPROPERTY(EditAnywhere, Category = MovementSpeeds)
	float MaxSpeed = 4515.f;

	// health
	/** default: 100.0 */
	UPROPERTY(EditAnywhere, Category = Health)
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere, Category = Health)
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

	/**
	 * changes @p CharacterState to Crouch and reduces the size of the collision capsule.
	 */
	void CharacterCrouch();
	/**
	 * """""
	 */
	void CharacterUnCrouch();

	/**
	 * runs @code CanStand@endcode after @p StandTimeDelay has passed.
	 */
	void CanStandDelay();
	/**
	 * """""
	 */
	void CanStand();

	/**
	 * line trace upwards from four sides of the characters collision capsule.
	 * 
	 * @return @code true@endcode if none of the line traces collides \n @code false@endcode otherwise
	 */
	bool CheckCapsule() const;

	FTimerHandle StopRunHandle;
	const float StopRunDelay = .3f;
	/**
	 * changes @p CharacterState to Normal after @p StopRunDelay.
	 */
	void CharacterStopRun();

	const float StandTimeDelay = .25f;

	/**
	 * changes @p CharacterState to Normal or Run and increases the size of the collision capsule.
	 */
	void CapsuleChangeNormal();

	/**
	 * checks if the character needs to take damage from a previous fall.
	 *
	 * makes the character jump.
	 */
	void CharacterJump();
	/**
	 * calculates the damage to deal based on the distance fallen.
	 */
	void CharacterJumpDamage();
	FTimerHandle JumpDamageHandle;


	// movement
	UPROPERTY()
	float PrevAngle;
	/** default: 44.0 */
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxTurnAngle = 44.f;
	UPROPERTY()
	int16 PrevHeight;
	UPROPERTY(VisibleAnywhere, Category = Movement)
	int16 JumpStartHeight;
	UPROPERTY()
	int16 JumpEndHeight;
	/** default: 850 */
	UPROPERTY(EditAnywhere, Category = Movement)
	int16 JumpMaxHeight = 850;
	/** default: 25.0 */
	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseFallDamage = 25.f;
	// UPROPERTY(VisibleAnywhere, Category = Movement)
	// bool bFalling = false;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	bool bMomentum = false;

	/** default: 5.0 */
	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedDecayRate = 5.f;
	/** default: 15.0 */
	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedDecayRateGround = 15.f;

	// attacks
	UPROPERTY(VisibleAnywhere, Category = Attack)
	bool bCanAttack = true;
	FTimerHandle AttackHandle;
	/** default: 2.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float AttackCooldown = 2.f;
	void SetCanAttackTrue() { bCanAttack = true; }

	/**
	 * | NOT FULLY IMPLEMENTED |
	 * 
	 */
	void CharacterRangedAttack();
	/** default: 15.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float RangedAttackCost = 15.f;
	/** default: 1500.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float RangedAttackRange = 1500.f;

	/**
	 * | NOT FULLY IMPLEMENTED |
	 *
	 * deals damage to the character and runs @code CharacterMeleeRecoil@endcode.
	 */
	void CharacterMeleeAttack();
	/** default: 10.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackCost = 10.f;
	/** default: 500.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRange = 500.f;
	/**
	 * calculates impulse and applies it to the character.
	 */
	void CharacterMeleeRecoil() const;
	/** NEGATIVE VALUE | default: -650.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRecoilRange = -650.f;
	/** NEGATIVE VALUE | default: -750.0 */
	UPROPERTY(EditAnywhere, Category = Attack)
	float MeleeAttackRecoilRangeGround = -750.f;
	// const float MeleeAttackCooldown = 1.f;
	// const float MeleeAttack

	
	// heal
	/**
	 * reduces the speed of the character and runs @code CharacterFinishHeal@endcode after @p HealDuration.
	 */
	void CharacterHeal();
	UPROPERTY(VisibleAnywhere, Category = Heal)
	bool bHealing = false;
	/** default: 0.6 */
	UPROPERTY(EditAnywhere, Category = Movement)
	float HealSpeedModifier = .6f;
	FTimerHandle HealHandle;
	/** default: 3.5 */
	UPROPERTY(EditAnywhere, Category = Heal)
	float HealDuration = 3.5f;
	/** default: 25.0 */
	UPROPERTY(EditAnywhere, Category = Heal)
	float HealAmount = 25.f;
	/**
	 * increases @p CurrentHealth by @p HealAmount and returns speed to the normal value depending on @p CharacterState.
	 */
	void CharacterFinishHeal();
	void SetHealingFalse() { bHealing = false; }

	/**
	 * changes the current speed of the character to @p Value.
	 * 
	 * if @p bHealing is @code true@endcode @p HealSpeedModifier is applied to @p Value.
	 * 
	 * @param Value new speed value
	 */
	void CharacterChangeSpeed(const float Value) const;

	
	// teleport
	/**
	 * """""
	 */
	void InstantTeleport();
	/**
	 * checks every direction at @p TeleportLocation for collisions to make sure the character collision capsule fits.
	 * 
	 * both standing and crouching capsule sizes are used in the check, if only the crouched fits @p CharacterState is changed to Crouch.
	 * 
	 * @param TeleportLocation location to check for collisions
	 * @return @code true@endcode if there are no collision at @p TeleportLocation \n @code false@endcode otherwise
	 */
	bool CheckCollision(const FVector& TeleportLocation);
	/** default: true */
	UPROPERTY(EditAnywhere, Category = Teleport)
	bool bCanTeleport = true;
	/** default: 450.0 */
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportMinRange = 450.f;
	/** default: 5000.0 */
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportMaxRange = 5000.f;
	/** default: 50.0 */
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportLocationOffset = 50.f;
	FTimerHandle TeleportHandle;
	void SetCanTeleportTrue() { bCanTeleport = true; }
	/** default: 5.0 */
	UPROPERTY(EditAnywhere, Category = Teleport)
	float TeleportCooldown = 5.f;

	const float TeleportSizeSide = 55.f;
	const float TeleportSizeStand = 197.f;
	const float TeleportSizeCrouch = 125.f;


	/**
	 * | TEST FUNCTION |
	 */
	void Test();
	
};
