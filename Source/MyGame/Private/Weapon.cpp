#include "public/Weapon.h"
#include "Components/StaticMeshComponent.h"

AWeapon::AWeapon()
{
	// Weapons generally don't need to tick every frame unless they have special logic
	PrimaryActorTick.bCanEverTick = false;

	// Create the visual mesh and make it the root component
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	// Turn off collision on the mesh itself so it doesn't bump into the player when equipped
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}