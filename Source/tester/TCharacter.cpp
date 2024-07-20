// Fill out your copyright notice in the Description page of Project Settings.


#include "TCharacter.h"
#include "Camera/CameraComponent.h"
#include "Chaos/Character/CharacterGroundConstraintContainer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATCharacter::ATCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set the size of the capsule
	GetCapsuleComponent()->InitCapsuleSize(CapsuleRadius,CapsuleHeight);

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanSwim = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanFly = false;
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// jump
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = .1f;

	// camera
	TCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TCameraComponent->SetupAttachment(RootComponent);
	TCameraComponent->SetRelativeLocation(FVector(-5.f,0.f,65.f));
	TCameraComponent->bUsePawnControlRotation = true;

	SetActorTickInterval(.1f);
}

// Called when the game starts or when spawned
void ATCharacter::BeginPlay()
{
	Super::BeginPlay();


	GetWorld()->DebugDrawTraceTag = "TraceTag";
	
	ChangeStateNormal();
	CurrentHealth = MaxHealth;
}

// Called every frame
void ATCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetCharacterState() == ETCharacterState::Crouch)
	{
		if (bMomentum)
		{
			// check for changes in dir
			const FRotator TempDir = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity);
			if (!PrevAngle) PrevAngle = TempDir.Yaw;
			if (!PrevHeight) static_cast<int16>(GetActorLocation().Z);
			if (abs(abs(PrevAngle)-abs(TempDir.Yaw)) > 44.f)
			{
				bMomentum = false;
				GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
				return;
			}

			// check for movement
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				// UE_LOG(LogTemp,Warning,TEXT("%d|%d|%f"),PrevHeight,static_cast<int16>(GetActorLocation().Z),(PrevHeight-static_cast<int16>(GetActorLocation().Z))*2.7f);

				// increase speed if player is going down and decrease if going up
				if (PrevHeight != static_cast<int16>(GetActorLocation().Z))
				{
					GetCharacterMovement()->MaxWalkSpeed += (PrevHeight-static_cast<int16>(GetActorLocation().Z))*2.7f;
					if (GetCharacterMovement()->MaxWalkSpeed > MaxSpeed) GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
					
					// if (GetCharacterMovement()->MaxWalkSpeed+(PrevHeight-static_cast<int16>(GetActorLocation().Z))*2.7f >= MaxSpeed)
					// 	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
					// else
					// 	GetCharacterMovement()->MaxWalkSpeed += (PrevHeight-static_cast<int16>(GetActorLocation().Z))*2.7f;
				}
			}
			else GetCharacterMovement()->MaxWalkSpeed -= SpeedDecayRate;

			// decrease speed slightly if neither
				// if speed is smaller or equal to crouch speed set bMomentum to false

			if (GetCharacterMovement()->MaxWalkSpeed - SpeedDecayRate <= CrouchSpeed)
			{
				bMomentum = false;
				GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			}
			else GetCharacterMovement()->MaxWalkSpeed -= GetCharacterMovement()->IsMovingOnGround() ? SpeedDecayRateGround : SpeedDecayRate;

			PrevAngle = TempDir.Yaw;
			PrevHeight = static_cast<int16>(GetActorLocation().Z);
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			PrevAngle = NULL;
			PrevHeight = NULL;
		}

		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			if (!JumpStartHeight)
			{
				JumpStartHeight = static_cast<int16>(GetActorLocation().Z);
				UE_LOG(LogTemp,Warning,TEXT("JUMP>%d"),JumpStartHeight);
			}

			JumpEndHeight = static_cast<int16>(GetActorLocation().Z);
			if (GetWorldTimerManager().IsTimerActive(JumpDamageHandle)) GetWorldTimerManager().ClearTimer(JumpDamageHandle);
			GetWorldTimerManager().SetTimer(JumpDamageHandle,this,&ATCharacter::CharacterJumpDamage,.1f,false);
		}
	}
}


void ATCharacter::SetCharacterState(const ETCharacterState State)
{
	if (GetWorldTimerManager().IsTimerActive(StopRunHandle)) GetWorldTimerManager().ClearTimer(StopRunHandle);

	if (GetCharacterState() == ETCharacterState::Dead || GetCharacterState() == State) return;

	PrevCharacterState = GetCharacterState();

	switch (State)
	{
	case ETCharacterState::Normal:
		{
			CharacterState = ETCharacterState::Normal;
			
			// if (GetCharacterState() == ETCharacterState::Crouch) CharacterUnCrouch();
			// GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			CharacterChangeSpeed(WalkSpeed);
			GetCharacterMovement()->JumpZVelocity = 350.f;

			// kill momentum if player stands up
			bMomentum = false;
			
			UE_LOG(LogTemp,Warning,TEXT("Player -> Normal"));
			break;
		}
	case ETCharacterState::Crouch:
		{
			CharacterState = ETCharacterState::Crouch;

			PrevAngle = NULL;
			PrevHeight = NULL;

			// if PrevCharacterState was run give a boost to the character in the direction of movement and set momentum to true
			if (GetPrevCharacterState() == ETCharacterState::Run)
			{
				bMomentum = true;
				// if (!bHealing) GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed+RunSpeed;
				// else GetCharacterMovement()->MaxWalkSpeed = (CrouchSpeed+RunSpeed)*HealSpeedModifier;
				CharacterChangeSpeed(CrouchSpeed+RunSpeed);
			}
			else CharacterChangeSpeed(CrouchSpeed);
			
			UE_LOG(LogTemp,Warning,TEXT("Player -> Crouch"));
			break;
		}
	case ETCharacterState::Run:
		{
			CharacterState = ETCharacterState::Run;

			// if (GetPrevCharacterState() == ETCharacterState::Crouch) CharacterUnCrouch();
			if (GetPrevCharacterState() == ETCharacterState::Crouch) CapsuleChangeNormal();
			// GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			CharacterChangeSpeed(RunSpeed);
			GetCharacterMovement()->JumpZVelocity = 400.f;

			// kill momentum if player stands up
			bMomentum = false;
			
			UE_LOG(LogTemp,Warning,TEXT("Player -> Run"));
			break;
		}
	case ETCharacterState::Dead:
		{
			CharacterState = ETCharacterState::Dead;

			// set health to 0

			CharacterChangeSpeed(0.f);
			GetCharacterMovement()->SetJumpAllowed(false);
			bMomentum = false;
			// TCameraComponent->bUsePawnControlRotation = false;
			
			UE_LOG(LogTemp,Warning,TEXT("Player -> Dead"));
			break;
		}
	}
}


bool ATCharacter::CharacterTakeDamage(const float Damage)
{
	// cancel healing if attacked
	if (bHealing)
	{
		if (GetWorldTimerManager().IsTimerActive(HealHandle)) GetWorldTimerManager().ClearTimer(HealHandle);
		SetHealingFalse();
		UE_LOG(LogTemp,Warning,TEXT("HEAL CANCELED"));
	}

	if (GetCharacterCurrentHealth()-Damage <= 0.f)
	{
		CurrentHealth = 0.f;
		ChangeStateDead();
		return true;
	}

	CurrentHealth -= Damage;
	UE_LOG(LogTemp,Warning,TEXT("%f DAMAGE TAKEN"),Damage);

	// return false;
	return true;
}


// Called to bind functionality to input
void ATCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


void ATCharacter::MoveForward(const float Value)
{
	if (!Controller) return;

	// find out which way is forward
	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0,Rot.Yaw,0);

	// get forward vector
	const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Dir,Value);
}

void ATCharacter::MoveRight(const float Value)
{
	if (!Controller || Value == 0.f) return;

	// find out which way is right
	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0,Rot.Yaw,0);

	// get right vector
	const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Dir,Value);
}

void ATCharacter::CharacterCrouch()
{
	
}

void ATCharacter::CharacterUnCrouch()
{
	
}

void ATCharacter::CanStandDelay()
{
	
}

void ATCharacter::CanStand()
{
	
}

void ATCharacter::CapsuleChangeNormal()
{
	
}

bool ATCharacter::CheckCapsule()
{
	return false;
}

void ATCharacter::CharacterStopRun()
{
	
}

void ATCharacter::CharacterJump()
{
	
}

void ATCharacter::CharacterJumpDamage()
{
	
}

void ATCharacter::CharacterRangedAttack()
{
	
}

void ATCharacter::CharacterMeleeAttack()
{
	
}

void ATCharacter::CharacterMeleeRecoil()
{
	
}

void ATCharacter::CharacterHeal()
{
	
}

void ATCharacter::CharacterFinishHeal()
{
	
}

void ATCharacter::CharacterChangeSpeed(const float Value)
{
	
}

void ATCharacter::InstantTeleport()
{
	
}

bool ATCharacter::CheckCollision(const FVector TeleportLocation)
{
	return false;
}



void ATCharacter::Test()
{
	
}
