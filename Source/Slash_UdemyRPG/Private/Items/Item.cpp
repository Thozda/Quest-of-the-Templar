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
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float MovementRate = 70.f;
	float RotationRate = 15.f;

	AddActorWorldOffset(FVector(MovementRate, 0.f, 0.f) * DeltaTime);
	AddActorWorldRotation(FRotator(RotationRate, 0.f, 0.f) * DeltaTime);
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_ARROW_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);
}

