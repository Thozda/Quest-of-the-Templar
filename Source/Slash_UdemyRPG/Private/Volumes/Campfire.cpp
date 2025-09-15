// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/Campfire.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "HUD/CampfireWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/UpgradeInterface.h"

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

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACampfire::OnSphereOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &ACampfire::OnSphereEndOverlap);
	}
}

void ACampfire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UpgradeInterface && WidgetComponent)
	{
		WidgetComponent->FacePlayerCamera(UpgradeInterface->GetPlayerCamera());
	}
}

void ACampfire::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UpgradeInterface = Cast<IUpgradeInterface>(OtherActor);
	if (UpgradeInterface && WidgetComponent)
	{
		WidgetComponent->SetCampfireText(UpgradeInterface->CampfireText());
		UpgradeInterface->SetOverlappingCampfire(this);
	}
}

void ACampfire::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UpgradeInterface = Cast<IUpgradeInterface>(OtherActor);
	if (UpgradeInterface && WidgetComponent)
	{
		WidgetComponent->SetCampfireText(FString());
		UpgradeInterface->SetCampfireNull();
		UpgradeInterface = nullptr;
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