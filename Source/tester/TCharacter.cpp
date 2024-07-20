// Fill out your copyright notice in the Description page of Project Settings.


#include "TCharacter.h"
#include "Camera/CameraComponent.h"
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

}


void ATCharacter::SetCharacterState(const ETCharacterState State)
{
	
}


bool ATCharacter::CharacterTakeDamage(const float Damage)
{
	return false;
}


// Called to bind functionality to input
void ATCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ATCharacter::MoveForward(const float Value)
{
	
}

void ATCharacter::MoveRight(const float Value)
{
	
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
