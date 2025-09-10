// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"

#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

ASoul::ASoul()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	CalculateDesiredHeight();
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveToDesiredHeight(DeltaTime);
}

void ASoul::CalculateDesiredHeight()
{
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.AddUnique(EObjectTypeQuery::ObjectTypeQuery1);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetOwner());
	
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false,
	                                                ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	DesiredHeight = HitResult.ImpactPoint.Z + DesiredHeight;
}

void ASoul::MoveToDesiredHeight(float DeltaTime)
{
	if (GetActorLocation().Z > DesiredHeight)
	{
		FVector DeltaLocation = FVector(0.f, 0.f, -Gravity) * DeltaTime;
		AddActorWorldOffset(DeltaLocation);
	}
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if(PickupInterface)
	{
		PickupInterface->AddSouls(this);
		SpawnPickupEffects();
		Destroy();
	}
}