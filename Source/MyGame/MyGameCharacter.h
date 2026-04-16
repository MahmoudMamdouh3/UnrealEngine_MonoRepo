// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "public/HealInterface.h"
#include "MyGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

// Forward declaration so the character knows the Weapon class exists
class AWeapon; 

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class AMyGameCharacter : public ACharacter, public IHealInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:
	/** Constructor */
	AMyGameCharacter();	

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	// Health Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = 50.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

public:
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	// -----------------------------------------------------
	// HEALING SYSTEM 
	// -----------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyHealing(float Amount);
	
	virtual void Heal_Implementation(float Amount) override;

	UFUNCTION()
	void HealFromDelegate(AActor* OverlappedActor, float Amount);
	
	// -----------------------------------------------------
	// WEAPON SYSTEM (BONUS TASK)
	// -----------------------------------------------------
	
	// The weapon currently held by the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* CurrentWeapon;

	// We use this to select a weapon blueprint in the editor to spawn for testing
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClassToTest;

	// Changes enum state and attaches to mesh
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(AWeapon* WeaponToEquip);

	// Changes enum state and detaches from mesh
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnequipWeapon();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};