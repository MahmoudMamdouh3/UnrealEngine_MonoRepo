#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// 1. Define the Enum for the Equip State
UENUM(BlueprintType)
enum class EEquipState : uint8
{
	Unequipped UMETA(DisplayName = "Unequipped"),
	Equipped UMETA(DisplayName = "Equipped")
};

// 2. Define the Struct for Weapon Data
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float FireRate = 0.5f;
};

// 3. The actual Weapon Class
UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	// The visual mesh for the weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* WeaponMesh;

	// The current equip state of this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon State")
	EEquipState CurrentState = EEquipState::Unequipped;

	// The stats for this weapon using the struct we made above
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	FWeaponData WeaponStats;
};