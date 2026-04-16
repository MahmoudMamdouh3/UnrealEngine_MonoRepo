#include "MyGame/Public/HealthPickupInterface.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyGame/Public/HealInterface.h" // We include the interface contract, not the character!

AHealthPickupInterface::AHealthPickupInterface()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Setup the sphere
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->InitSphereRadius(60.f);
	
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 2. Setup the mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	// 3. Link the overlap event
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickupInterface::OnOverlapBegin);
}

void AHealthPickupInterface::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ensure the OtherActor exists, then ask: "Do you implement the HealInterface?"
	// Note: We use UHealInterface here because Unreal's reflection system uses the 'U' prefix for checking
	if (OtherActor && OtherActor->Implements<UHealInterface>())
	{
		// If yes, execute the interface function! 
		// Note: We use IHealInterface here because 'I' is the prefix used for actually calling functions
		IHealInterface::Execute_Heal(OtherActor, HealAmount);
		
		Destroy(); // Remove pickup
	}
}