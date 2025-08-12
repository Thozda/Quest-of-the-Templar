// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash_UdemyRPG/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/Knight.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(100.f);
	Sphere->SetWorldLocation(FVector(0.f, 0.f, 50.f));

	PickupEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup Effect"));
	PickupEffect->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return FMath::Sin(RunningTime * Frequency) * Amplitude;
}

float AItem::TransformedCos()
{
	return FMath::Cos(RunningTime * Frequency) * Amplitude;
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKnight* Knight = Cast<AKnight>(OtherActor);
	if (Knight)
	{
		Knight->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AKnight* Knight = Cast<AKnight>(OtherActor);
	if (Knight)
	{
		Knight->SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

