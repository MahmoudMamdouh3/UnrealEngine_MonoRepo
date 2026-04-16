#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickupCast.generated.h"

// Forward declarations so the compiler knows these exist
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class AHealthPickupCast : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickupCast();

protected:
	// The visual 3D model
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UStaticMeshComponent* MeshComp;

	// The invisible bubble that detects the player
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* SphereComp;

	// How much health to give
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float HealAmount = 25.f;

	// The function that fires when something touches the sphere
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};