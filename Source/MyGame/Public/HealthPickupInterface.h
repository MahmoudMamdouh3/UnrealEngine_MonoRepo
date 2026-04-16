#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickupInterface.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class AHealthPickupInterface : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickupInterface();

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