#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickupDelegate.generated.h"

// 1. Declare the delegate signature ABOVE the class. 
// It passes two pieces of info: Who touched it, and how much to heal.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthPickupSignature, AActor*, OverlappedActor, float, HealAmount);

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class AHealthPickupDelegate : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickupDelegate();

	// 2. Create the actual delegate instance so the player can subscribe to it
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthPickupSignature OnHealthPickup;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	float HealAmount = 25.f;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};