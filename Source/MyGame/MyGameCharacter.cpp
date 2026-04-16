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
#include "public/HealthPickupDelegate.h"
#include "public/Weapon.h" // Include the weapon so we can change its enum state

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

	// 1. Health Delegate Subscription
	TArray<AActor*> FoundPickups;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHealthPickupDelegate::StaticClass(), FoundPickups);

	for (AActor* PickupActor : FoundPickups)
	{
		if (AHealthPickupDelegate* DelegatePickup = Cast<AHealthPickupDelegate>(PickupActor))
		{
			DelegatePickup->OnHealthPickup.AddDynamic(this, &AMyGameCharacter::HealFromDelegate);
		}
	}

	// 2. Weapon Testing Logic
	// If the instructor assigned a weapon in the Blueprint, spawn it and equip it immediately so they can see it works!
	if (WeaponClassToTest)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClassToTest, GetActorLocation(), GetActorRotation(), SpawnParams);
		EquipWeapon(SpawnedWeapon);
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
	Health = FMath::Min(Health + Amount, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health is now: %f"), Health);
}

void AMyGameCharacter::Heal_Implementation(float Amount)
{
	ApplyHealing(Amount);
}

void AMyGameCharacter::HealFromDelegate(AActor* OverlappedActor, float Amount)
{
	if (OverlappedActor == this)
	{
		ApplyHealing(Amount); 
	}
}

// -----------------------------------------------------
// WEAPON SYSTEM FUNCTIONS
// -----------------------------------------------------

void AMyGameCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		CurrentWeapon = WeaponToEquip;

		// 1. Change the Enum State
		CurrentWeapon->CurrentState = EEquipState::Equipped;

		// 2. Attach the weapon to the right hand bone of the character's skeletal mesh
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r"));
		
		UE_LOG(LogTemp, Warning, TEXT("Weapon Equipped! State changed to Equipped."));
	}
}

void AMyGameCharacter::UnequipWeapon()
{
	if (CurrentWeapon)
	{
		// 1. Change the Enum State
		CurrentWeapon->CurrentState = EEquipState::Unequipped;

		// 2. Detach from the player
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		CurrentWeapon = nullptr; // Clear the reference
		
		UE_LOG(LogTemp, Warning, TEXT("Weapon Unequipped! State changed to Unequipped."));
	}
}