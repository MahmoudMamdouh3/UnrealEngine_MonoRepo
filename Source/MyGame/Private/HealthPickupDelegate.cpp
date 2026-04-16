// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGame\Public\HealthPickupDelegate.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AHealthPickupDelegate::AHealthPickupDelegate()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->InitSphereRadius(60.f);
	
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickupDelegate::OnOverlapBegin);
}

void AHealthPickupDelegate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// DELEGATE METHOD: We broadcast to anyone listening. We pass the actor that touched us, and the heal amount.
		OnHealthPickup.Broadcast(OtherActor, HealAmount);
		
		Destroy(); // Delete the pickup
	}
}