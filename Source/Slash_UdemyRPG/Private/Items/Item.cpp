// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash_UdemyRPG/DebugMacros.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

float AItem::TransformedSin()
{
	return FMath::Sin(RunningTime * Frequency) * Amplitude;
}

float AItem::TransformedCos()
{
	return FMath::Cos(RunningTime * Frequency) * Amplitude;
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	//AddActorWorldRotation(FRotator(-180.f * DeltaTime, 0.f, 0.f));
}

