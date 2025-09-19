// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/BossArena.h"
#include "Components/BoxComponent.h"
#include "Enemy/Enemy.h"

ABossArena::ABossArena()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	Wall1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 1"));
	Walls.Add(Wall1);
	
	Wall2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 2"));
	Walls.Add(Wall2);
	
	Wall3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 3"));
	Walls.Add(Wall3);
	
	Wall4 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 4"));
	Walls.Add(Wall4);

	for (UBoxComponent* Wall : Walls)
	{
		Wall->SetupAttachment(GetRootComponent());
		Wall->SetCollisionObjectType(ECC_WorldStatic);
		Wall->SetCollisionResponseToAllChannels(ECR_Block);
		Wall->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		Wall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABossArena::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossArena::OnBoxOverlap);
}

void ABossArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Tick False
}

void ABossArena::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Knight")))
	{
		if (Boss) Boss->LookAtPlayer(OtherActor);
		
		for (UBoxComponent* Wall : Walls)
		{
			if (Wall)
			{
				Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
	}
}