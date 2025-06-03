// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash_UdemyRPG/DebugMacros.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(0.f, 0.f, 200.f));

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	//DRAW_LINE(Location, Location + Forward * 100.0f);
	DRAW_SPHERE(Location);
	//DRAW_POINT(Location + Forward * 100.0f);
	DRAW_VECTOR_ARROW(Location, Location + Forward * 100.0f);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

