// Fill out your copyright notice in the Description page of Project Settings.


#include "TCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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


	// // add input mapping context
	// if (const APlayerController *PlayerController = Cast<APlayerController>(Controller))
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	// 	{
	// 		Subsystem->AddMappingContext(DefaultMappingContext,0);
	// 	}
	// }
	
	
	ChangeStateNormal();
	CurrentHealth = GetCharacterMaxHealth();

	if (MeleeAttackRecoilRange > 0) MeleeAttackRecoilRange = -650.f;
	if (MeleeAttackRecoilRangeGround > 0) MeleeAttackRecoilRangeGround = -750.f;
}

// Called every frame
void ATCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetCharacterState() == ETCharacterState::Crouch)
	{
		if (bMomentum)
		{
			// check for changes in dir
			const FRotator TempDir = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity);
			if (!PrevAngle) PrevAngle = TempDir.Yaw;
			if (!PrevHeight) PrevHeight = static_cast<int16>(GetActorLocation().Z);
			
			if (abs(abs(PrevAngle)-abs(TempDir.Yaw)) > MaxTurnAngle)
			{
				bMomentum = false;
				// GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
				CharacterChangeSpeed(CrouchSpeed);
				return;
			}

			// check for movement
			const int16 CurrHeight = static_cast<int16>(GetActorLocation().Z);
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				// UE_LOG(LogTemp,Warning,TEXT("%d|%d|%f"),PrevHeight,CurrHeight,(PrevHeight-CurrHeight)*2.7f);

				// increase speed if player is going down and decrease if going up
				if (PrevHeight != CurrHeight)
				{
					GetCharacterMovement()->MaxWalkSpeed += (PrevHeight-CurrHeight)*2.7f;
					// if (GetCharacterMovement()->MaxWalkSpeed > MaxSpeed) GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
					if (GetCharacterCurrentSpeed() > MaxSpeed) CharacterChangeSpeed(MaxSpeed);

					
					// if (GetCharacterMovement()->MaxWalkSpeed+(PrevHeight-CurrHeight)*2.7f >= MaxSpeed)
					// 	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
					// else
					// 	GetCharacterMovement()->MaxWalkSpeed += (PrevHeight-CurrHeight)*2.7f;
				}
			}
			else GetCharacterMovement()->MaxWalkSpeed -= SpeedDecayRate;

			// decrease speed slightly if neither
			// if speed is smaller or equal to crouch speed set bMomentum to false
			// if (GetCharacterMovement()->MaxWalkSpeed - SpeedDecayRate <= CrouchSpeed)
			if (GetCharacterCurrentSpeed() - SpeedDecayRate <= CrouchSpeed)
			{
				bMomentum = false;
				// GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
				CharacterChangeSpeed(CrouchSpeed);
			}
			else GetCharacterMovement()->MaxWalkSpeed -= GetCharacterMovement()->IsMovingOnGround() ? SpeedDecayRateGround : SpeedDecayRate;

			PrevAngle = TempDir.Yaw;
			PrevHeight = CurrHeight;
		}
		else
		{
			// GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			CharacterChangeSpeed(CrouchSpeed);
			PrevAngle = NULL;
			PrevHeight = NULL;
		}
	}

	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		if (!JumpStartHeight)
		{
			JumpStartHeight = static_cast<int16>(GetActorLocation().Z);
			UE_LOG(LogTemp,Warning,TEXT("JUMP>%d"),JumpStartHeight);
			return;
		}

		JumpEndHeight = static_cast<int16>(GetActorLocation().Z);
		if (GetWorldTimerManager().IsTimerActive(JumpDamageHandle)) GetWorldTimerManager().ClearTimer(JumpDamageHandle);
		GetWorldTimerManager().SetTimer(JumpDamageHandle,this,&ATCharacter::CharacterJumpDamage,.1f,false);
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
			// prevents the player from changing state when crouching under a platform
			if (GetPrevCharacterState() == ETCharacterState::Crouch)
				if (CheckCapsule())
				{
					CapsuleChangeNormal();
				}
				else return;

			
			CharacterState = ETCharacterState::Run;
			
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

			CurrentHealth = 0.f;
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

	check(PlayerInputComponent);

	
	// ///////////////////////////////////////////// ENHANCED INPUT /////////////////////////////////////////////
	
	// if (UEnhancedInputComponent *Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	// {
	// 	// move
	// 	Input->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ATCharacter::Move);
	//
	// 	// look
	// 	Input->BindAction(LookAction,ETriggerEvent::Triggered,this,&ATCharacter::Look);
	//
	// 	// crouch
	// 	Input->BindAction(CrouchAction,ETriggerEvent::Triggered,this,&ATCharacter::CharacterCrouch);
	// 	Input->BindAction(CrouchAction,ETriggerEvent::Completed,this,&ATCharacter::CharacterUnCrouch);
	//
	// 	// run
	// 	Input->BindAction(RunAction,ETriggerEvent::Triggered,this,&ATCharacter::ChangeStateRun);
	// 	Input->BindAction(RunAction,ETriggerEvent::Completed,this,&ATCharacter::CharacterStopRun);
	//
	// 	// jump
	// 	Input->BindAction(JumpAction,ETriggerEvent::Triggered,this,&ATCharacter::Jump);
	// 	Input->BindAction(JumpAction,ETriggerEvent::Completed,this,&ATCharacter::StopJumping);
	//
	// 	// interact
	//
	//
	// 	// ranged attack
	// 	Input->BindAction(RangedAction,ETriggerEvent::Triggered,this,&ATCharacter::CharacterRangedAttack);
	//
	// 	// melee attack
	// 	Input->BindAction(MeleeAction,ETriggerEvent::Triggered,this,&ATCharacter::CharacterMeleeAttack);
	//
	// 	// heal attack
	// 	Input->BindAction(HealAction,ETriggerEvent::Triggered,this,&ATCharacter::CharacterHeal);
	//
	// 	// teleport attack
	// 	Input->BindAction(TeleportAction,ETriggerEvent::Triggered,this,&ATCharacter::InstantTeleport);
	//
	//
	// 	
	// 	// TEST attack
	// 	Input->BindAction(TestAction,ETriggerEvent::Triggered,this,&ATCharacter::Test);
	// }

	// ///////////////////////////////////////////// ENHANCED INPUT /////////////////////////////////////////////


	
	// movement and mouse
	PlayerInputComponent->BindAxis("MoveForward",this,&ATCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ATCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ATCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&ATCharacter::AddControllerPitchInput);

	// crouch
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ATCharacter::CharacterCrouch);
	PlayerInputComponent->BindAction("Crouch",IE_Released,this,&ATCharacter::CharacterUnCrouch);

	// run
	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&ATCharacter::ChangeStateRun);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&ATCharacter::CharacterStopRun);

	// jump
	// PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ATCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ATCharacter::CharacterJump);
	PlayerInputComponent->BindAction("Jump",IE_Released,this,&ATCharacter::StopJumping);

	// interact
	// PlayerInputComponent->BindAction("Interact",IE_Pressed,this,&ATCharacter::);
	
	// ranged attack
	PlayerInputComponent->BindAction("RangedAttack",IE_Pressed,this,&ATCharacter::CharacterRangedAttack);
	
	// melee attack
	PlayerInputComponent->BindAction("MeleeAttack",IE_Pressed,this,&ATCharacter::CharacterMeleeAttack);

	// heal
	PlayerInputComponent->BindAction("Heal",IE_Pressed,this,&ATCharacter::CharacterHeal);

	// teleport
	PlayerInputComponent->BindAction("Teleport",IE_Pressed,this,&ATCharacter::InstantTeleport);

	
	
	// test
	PlayerInputComponent->BindAction("TEST",IE_Pressed,this,&ATCharacter::Test);
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

void ATCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// find which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// get right vector
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ATCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	// add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}


void ATCharacter::CharacterCrouch()
{
	if (GetCharacterState() != ETCharacterState::Normal && GetCharacterState() != ETCharacterState::Run) return;

	ChangeStateCrouch();

	// change capsule size
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius,CrouchCapsuleHeight);
	FVector CapsuleLoc = GetCapsuleComponent()->GetRelativeLocation();
	CapsuleLoc.Z -= CrouchOffset;
	GetCapsuleComponent()->SetRelativeLocation(CapsuleLoc);
}

void ATCharacter::CharacterUnCrouch()
{
	if (GetCharacterState() != ETCharacterState::Crouch) return;

	if (CheckCapsule())
		CapsuleChangeNormal();
	else
		if (!bStuck)
		{
			bStuck = true;
			CanStandDelay();
		}
}

void ATCharacter::CanStandDelay()
{
	StuckLoc = GetActorLocation();
	FTimerHandle CanStandHandle;
	GetWorldTimerManager().SetTimer(CanStandHandle,this,&ATCharacter::CanStand,StandTimeDelay,false);
}

void ATCharacter::CanStand()
{
	// if the location is different from the previous location check for collision
	if (const FVector CharacterPos = GetActorLocation(); CharacterPos.X != StuckLoc.X || CharacterPos.Y != StuckLoc.Y)
	{
		if (CheckCapsule())
		{
			bStuck = false;
			CapsuleChangeNormal();
		}
		else CanStandDelay();
	}
	else CanStandDelay();
}

void ATCharacter::CapsuleChangeNormal()
{
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius,CapsuleHeight);
	FVector CapsuleLoc = GetCapsuleComponent()->GetRelativeLocation();
	CapsuleLoc.Z += CrouchOffset;
	GetCapsuleComponent()->SetRelativeLocation(CapsuleLoc);

	// change state
	if (GetCharacterState() == ETCharacterState::Run) ChangeStateRun();
	else ChangeStateNormal();
}

bool ATCharacter::CheckCapsule() const
{
	FHitResult CSResult;
	FCollisionQueryParams CSQueryP;
	CSQueryP.TraceTag = "TraceTag";
	FCollisionResponseParams CSResponseP;

	// capsule corner vectors
	const FVector FrontStart = GetActorLocation()+GetActorForwardVector()*49;
	const FVector FrontEnd = FrontStart+GetActorUpVector()*CrouchStandingLineLength;
	const FVector BackStart = GetActorLocation()+GetActorForwardVector()*-49;
	const FVector BackEnd = BackStart+GetActorUpVector()*CrouchStandingLineLength;
	const FVector LeftStart = GetActorLocation()+GetActorRightVector()*-49;
	const FVector LeftEnd = LeftStart+GetActorUpVector()*CrouchStandingLineLength;
	const FVector RightStart = GetActorLocation()+GetActorRightVector()*49;
	const FVector RightEnd = RightStart+GetActorUpVector()*CrouchStandingLineLength;

	// check the corners
	return !GetWorld()->LineTraceSingleByChannel(CSResult,FrontStart,FrontEnd,ECC_Visibility,CSQueryP,CSResponseP)
			&& !GetWorld()->LineTraceSingleByChannel(CSResult,BackStart,BackEnd,ECC_Visibility,CSQueryP,CSResponseP)
			&& !GetWorld()->LineTraceSingleByChannel(CSResult,LeftStart,LeftEnd,ECC_Visibility,CSQueryP,CSResponseP)
			&& !GetWorld()->LineTraceSingleByChannel(CSResult,RightStart,RightEnd,ECC_Visibility,CSQueryP,CSResponseP);
}

void ATCharacter::CharacterStopRun()
{
	if (GetCharacterState() != ETCharacterState::Run) return;
	GetWorldTimerManager().SetTimer(StopRunHandle,this,&ATCharacter::ChangeStateNormal,StopRunDelay,false);
}

void ATCharacter::CharacterJump()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) return;

	// prevents the player from delaying jump damage
	if (JumpStartHeight && GetWorldTimerManager().IsTimerActive(JumpDamageHandle))
	{
		GetWorldTimerManager().ClearTimer(JumpDamageHandle);
		CharacterJumpDamage();
	}
	
	JumpStartHeight = static_cast<int16>(GetActorLocation().Z);
	UE_LOG(LogTemp,Warning,TEXT("JUMP>%d"),JumpStartHeight);

	ATCharacter::Jump();
}

void ATCharacter::CharacterJumpDamage()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) return;

	if (const int16 JumpDist = JumpStartHeight-JumpEndHeight; JumpDist >= JumpMaxHeight)
	{
		const float FallDamage = BaseFallDamage+(JumpDist-JumpMaxHeight)*.015f;
		CharacterTakeDamage(FallDamage);

		UE_LOG(LogTemp,Warning,TEXT("JUMP DAMAGE TAKEN"));
	}

	JumpStartHeight = NULL;
	// bFalling = false;
}

void ATCharacter::CharacterRangedAttack()
{
	if (GetCharacterState() == ETCharacterState::Dead || bHealing || !bCanAttack) return;
	
	CharacterTakeDamage(RangedAttackCost);

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackHandle,this,&ATCharacter::SetCanAttackTrue,AttackCooldown,false);

	FHitResult CSResult;
	FCollisionQueryParams CSQueryP;
	CSQueryP.TraceTag = "TraceTag";
	FCollisionResponseParams CSResponseP;

	if (GetWorld()->LineTraceSingleByChannel(CSResult,TCameraComponent->GetComponentLocation(),
		(TCameraComponent->GetComponentLocation()+TCameraComponent->GetForwardVector()*RangedAttackRange),ECC_Visibility,CSQueryP,CSResponseP))
	{
		UE_LOG(LogTemp,Warning,TEXT("RANGED ATTACK - HIT"));
		return;
	}

	UE_LOG(LogTemp,Warning,TEXT("RANGED ATTACK - MISS"));
}

void ATCharacter::CharacterMeleeAttack()
{
	if (GetCharacterState() == ETCharacterState::Dead || bHealing || !bCanAttack) return;

	CharacterTakeDamage(MeleeAttackCost);

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackHandle,this,&ATCharacter::SetCanAttackTrue,AttackCooldown,false);

	CharacterMeleeRecoil();
	UE_LOG(LogTemp,Warning,TEXT("MELEE ATTACK - MISS"));
}

void ATCharacter::CharacterMeleeRecoil() const
{
	// push character backwards

	const FVector Impulse = GetCharacterMovement()->IsMovingOnGround() ?
		TCameraComponent->GetForwardVector()*MeleeAttackRecoilRangeGround : TCameraComponent->GetForwardVector()*MeleeAttackRecoilRange;

	UE_LOG(LogTemp,Warning,TEXT("X>%f\nY>%f\nZ>%f"),Impulse.X,Impulse.Y,Impulse.Z);
	DrawDebugSphere(GetWorld(),Impulse,25.f,12,FColor::Black,false,5.f);

	GetCharacterMovement()->AddImpulse(Impulse,true);
}

void ATCharacter::CharacterHeal()
{
	if (GetCharacterState() == ETCharacterState::Dead || bHealing || CurrentHealth == MaxHealth) return;

	bHealing = true;
	GetWorldTimerManager().SetTimer(HealHandle,this,&ATCharacter::CharacterFinishHeal,HealDuration,false);

	GetCharacterMovement()->MaxWalkSpeed *= HealSpeedModifier;

	UE_LOG(LogTemp,Warning,TEXT("HEAL"));
}

void ATCharacter::CharacterFinishHeal()
{
	if (GetCharacterState() == ETCharacterState::Dead) return;

	SetHealingFalse();

	// heal character
	if (CurrentHealth+HealAmount > MaxHealth) CurrentHealth = MaxHealth;
	else CurrentHealth += HealAmount;

	// return speed to normal
	if (GetCharacterState() == ETCharacterState::Normal) CharacterChangeSpeed(WalkSpeed);
	else if (GetCharacterState() == ETCharacterState::Run) CharacterChangeSpeed(RunSpeed);
	// else if (GetCharacterState() == ETCharacterState::Crouch) CharacterChangeSpeed(CrouchSpeed);

	UE_LOG(LogTemp,Warning,TEXT("HEAL FINISHED"));
}

void ATCharacter::CharacterChangeSpeed(const float Value) const
{
	if (!bHealing) GetCharacterMovement()->MaxWalkSpeed = Value;
	else GetCharacterMovement()->MaxWalkSpeed = Value*HealSpeedModifier;
}

void ATCharacter::InstantTeleport()
{
	if (GetCharacterState() == ETCharacterState::Dead || bHealing || !bCanTeleport) return;

	FHitResult CSResult;
	FCollisionQueryParams CSQueryP;
	CSQueryP.TraceTag = "TraceTag";
	FCollisionResponseParams CSResponseP;

	if (GetWorld()->LineTraceSingleByChannel(CSResult,TCameraComponent->GetComponentLocation(),
		(TCameraComponent->GetComponentLocation()+TCameraComponent->GetForwardVector()*TeleportMaxRange),ECC_Visibility,CSQueryP,CSResponseP))
	{
		if (CSResult.Distance < TeleportMinRange)
		{
			UE_LOG(LogTemp,Warning,TEXT("TELEPORT - TOO CLOSE"));
			return;
		}

		if (CheckCollision(TCameraComponent->GetComponentLocation()+TCameraComponent->GetForwardVector()*(CSResult.Distance-TeleportLocationOffset)))
		{
			bCanTeleport = false;
			GetWorldTimerManager().SetTimer(TeleportHandle,this,&ATCharacter::SetCanTeleportTrue,TeleportCooldown,false);

			SetActorLocation(CSResult.Location,false);
		}

		UE_LOG(LogTemp,Warning,TEXT("TELEPORT - HIT"));
		return;
	}

	UE_LOG(LogTemp,Warning,TEXT("TELEPORT - MISS"));
}

bool ATCharacter::CheckCollision(const FVector &TeleportLocation)
{
	FHitResult CSResult;
	FCollisionQueryParams CSQueryP;
	CSQueryP.TraceTag = "TraceTag";
	CSQueryP.AddIgnoredActor(this);
	FCollisionResponseParams CSResponseP;

	uint8 SideCount = 0;

	// check sides
		//forward
	SideCount += GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+(GetActorForwardVector()*TeleportSizeSide)),
		ECC_Visibility,CSQueryP,CSResponseP) ? 1 : 0;
		// back
	SideCount += GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+(GetActorForwardVector()*TeleportSizeSide*-1)),
		ECC_Visibility,CSQueryP,CSResponseP) ? 1 : 0;
		// left
	SideCount += GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+(GetActorRightVector()*TeleportSizeSide*-1)),
		ECC_Visibility,CSQueryP,CSResponseP) ? 1 : 0;
		// right
	SideCount += GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+(GetActorRightVector()*TeleportSizeSide)),
		ECC_Visibility,CSQueryP,CSResponseP) ? 1 : 0;

	UE_LOG(LogTemp,Warning,TEXT("SIDE COUNT>%d"),SideCount);

	if (SideCount >= 3) return false;

	// check above and below standing
	if (GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+GetActorUpVector()*TeleportSizeStand),ECC_Visibility,CSQueryP,CSResponseP)
		&& GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+GetActorUpVector()*TeleportSizeStand*-1),ECC_Visibility,CSQueryP,CSResponseP))
	{
		// check above and below crouched
		if (GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+GetActorUpVector()*TeleportSizeCrouch),ECC_Visibility,CSQueryP,CSResponseP)
			&& GetWorld()->LineTraceSingleByChannel(CSResult,TeleportLocation,(TeleportLocation+GetActorUpVector()*TeleportSizeCrouch*-1),ECC_Visibility,CSQueryP,CSResponseP))
			return false;

		// make player crouch
		CharacterCrouch();

		FTimerHandle TeleportCrouchHandle;
		GetWorldTimerManager().SetTimer(TeleportCrouchHandle,this,&ATCharacter::CharacterUnCrouch,.5f,false);
		// CharacterUnCrouch();
	}
	
	return true;
}



void ATCharacter::Test()
{
	// test


	
}


// if player jumps right after landing the fall damage is delayed until the character reaches the ground for the second time
