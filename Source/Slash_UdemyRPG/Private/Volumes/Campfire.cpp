// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/Campfire.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/Knight.h"
#include "Components/SphereComponent.h"
#include "HUD/CampfireWidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ACampfire::ACampfire()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Sphere;
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionObjectType(ECC_WorldStatic);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);

	WidgetComponent = CreateDefaultSubobject<UCampfireWidgetComponent>(TEXT("Campfire Widget Component"));
	WidgetComponent->SetupAttachment(GetRootComponent());
}

void ACampfire::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACampfire::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ACampfire::OnSphereEndOverlap);
}

void ACampfire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Knight && WidgetComponent)
	{
		WidgetComponent->FacePlayerCamera(Knight);
	}
}

void ACampfire::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (WidgetComponent && OtherActor && OtherActor->ActorHasTag("Knight"))
	{
		if (Cast<AKnight>(OtherActor))
		{
			Knight = Cast<AKnight>(OtherActor);
			WidgetComponent->SetCampfireText(Knight->CampfireText());
			Knight->Campfire = this;
		}
	}
}

void ACampfire::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (WidgetComponent && OtherActor && OtherActor->ActorHasTag("Knight"))
	{
		WidgetComponent->SetCampfireText(FString());
		Knight->Campfire = nullptr;
		Knight = nullptr;
	}
}

void ACampfire::UpgradeComplete()
{
	if (WidgetComponent)
	{
		WidgetComponent->SetCampfireText(FString("Upgrade Complete"));
	}
	if (UpgradeEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, UpgradeEffect, GetActorLocation());
	}
	if (UpgradeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UpgradeSound, GetActorLocation());
	}
}