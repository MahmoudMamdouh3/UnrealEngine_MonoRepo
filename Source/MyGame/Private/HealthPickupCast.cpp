#include "MyGame/Public/HealthPickupCast.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyGameCharacter.h" // We must include this so the pickup knows about your player's healing function

AHealthPickupCast::AHealthPickupCast()
{
	PrimaryActorTick.bCanEverTick = false; // Pickups don't need to update every frame

	// 1. Setup the invisible collision sphere
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->InitSphereRadius(60.f);
	
	// Set it to only overlap with pawns (like our player)
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 2. Setup the visual mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // The sphere handles the collision, the mesh is just for looks

	// 3. Link our overlap function to the sphere's overlap event
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickupCast::OnOverlapBegin);
}

void AHealthPickupCast::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// CAST METHOD: We try to cast the generic "OtherActor" into our specific player class
	AMyGameCharacter* PlayerCharacter = Cast<AMyGameCharacter>(OtherActor);
	
	// If PlayerCharacter is not null, the cast worked! It was our player.
	if (PlayerCharacter)
	{
		PlayerCharacter->ApplyHealing(HealAmount); // Heal them
		Destroy(); // Delete the pickup from the world
	}
}