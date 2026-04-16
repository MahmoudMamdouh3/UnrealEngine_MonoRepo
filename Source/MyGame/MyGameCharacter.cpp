// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MyGame.h"
#include "Kismet/GameplayStatics.h"
#include "Public/HealthPickupDelegate.h" // Needed for the delegate pickup broadcast

AMyGameCharacter::AMyGameCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AMyGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	// DELEGATE METHOD: Find all Delegate Pickups in the world when the game starts
	TArray<AActor*> FoundPickups;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHealthPickupDelegate::StaticClass(), FoundPickups);

	// Loop through them and subscribe our character to their broadcast
	for (AActor* PickupActor : FoundPickups)
	{
		if (AHealthPickupDelegate* DelegatePickup = Cast<AHealthPickupDelegate>(PickupActor))
		{
			DelegatePickup->OnHealthPickup.AddDynamic(this, &AMyGameCharacter::HealFromDelegate);
		}
	}
}

void AMyGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyGameCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMyGameCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyGameCharacter::Look);
	}
	else
	{
		UE_LOG(LogMyGame, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AMyGameCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMyGameCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMyGameCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMyGameCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMyGameCharacter::DoJumpStart()
{
	Jump();
}

void AMyGameCharacter::DoJumpEnd()
{
	StopJumping();
}

void AMyGameCharacter::ApplyHealing(float Amount)
{
	// Increases health by the amount, but ensures it never goes above MaxHealth
	Health = FMath::Min(Health + Amount, MaxHealth);
    
	UE_LOG(LogTemp, Warning, TEXT("Health is now: %f"), Health);
}

void AMyGameCharacter::Heal_Implementation(float Amount)
{
	// INTERFACE METHOD: Uses the existing healing logic
	ApplyHealing(Amount);
}

void AMyGameCharacter::HealFromDelegate(AActor* OverlappedActor, float Amount)
{
	// DELEGATE METHOD: Did WE touch the pickup that just broadcasted?
	if (OverlappedActor == this)
	{
		ApplyHealing(Amount); 
	}
}