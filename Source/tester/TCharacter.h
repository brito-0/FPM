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
	UPROPERTY(VisibleAnywhere)
	float WalkSpeed = 750.f;
	UPROPERTY(VisibleAnywhere)
	float RunSpeed = 950.f;
	UPROPERTY(VisibleAnywhere)
	float CrouchSpeed = 450.f;
	UPROPERTY(VisibleAnywhere)
	float MaxSpeed = 4515.f;

	// health
	const float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;

	// capsule
	// UPROPERTY()
	const float CapsuleRadius = 50.f;
	// UPROPERTY()
	const float CapsuleHeight = 96.f;
	// UPROPERTY()
	const float CrouchCapsuleHeight = 60.f;
	// offset to change the position of the capsule
	// UPROPERTY()
	const float CrouchOffset = 36.f;
	// length of the ray cast to check for collisions when increasing the size of the capsule
	// UPROPERTY()
	const float CrouchStandingLineLength = 140.f;

	UPROPERTY()
	bool bStuck = false;
	UPROPERTY()
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
	UPROPERTY()
	int16 PrevHeight;
	UPROPERTY(VisibleAnywhere)
	int16 JumpStartHeight;
	UPROPERTY()
	int16 JumpEndHeight;
	const int16 JumpMaxHeight = 850;
	const float BaseFallDamage = 25.f;
	UPROPERTY()
	bool bFalling = false;

	UPROPERTY()
	bool bMomentum = false;

	const float SpeedDecayRate = 5.f;
	const float SpeedDecayRateGround = 15.f;

	// attacks
	UPROPERTY()
	bool bCanAttack = true;
	FTimerHandle AttackHandle;
	const float AttackCooldown = 2.f;
	void SetCanAttackTrue() { bCanAttack = true; }

	void CharacterRangedAttack();
	const float RangedAttackCost = 15.f;
	const float RangedAttackRange = 1500.f;

	void CharacterMeleeAttack();
	const float MeleeAttackCost = 10.f;
	const float MeleeAttackRange = 500.f;
	void CharacterMeleeRecoil() const;
	const float MeleeAttackRecoilRange = -650.f;
	const float MeleeAttackRecoilRangeGround = -750.f;
	// const float MeleeAttackCooldown = 1.f;
	// const float MeleeAttack

	
	// heal
	void CharacterHeal();
	const float HealSpeedModifier = .6f;
	UPROPERTY(VisibleAnywhere, Category = Status)
	bool bHealing = false;
	FTimerHandle HealHandle;
	const float HealDuration = 3.5f;
	const float HealAmount = 25.f;
	void CharacterFinishHeal();
	void SetHealingFalse() { bHealing = false; }

	void CharacterChangeSpeed(const float Value) const;

	
	// teleport
	void InstantTeleport();
	bool CheckCollision(const FVector& TeleportLocation);
	const float TeleportMinRange = 450.f;
	const float TeleportMaxRange = 5000.f;
	const float TeleportLocationOffset = 50.f;
	bool bCanTeleport = true;
	FTimerHandle TeleportHandle;
	void SetCanTeleportTrue() { bCanTeleport = true; }
	const float TeleportCooldown = 5.f;

	const float TeleportSizeSide = 55.f;
	const float TeleportSizeStand = 197.f;
	const float TeleportSizeCrouch = 125.f;


	void Test();
	
};
