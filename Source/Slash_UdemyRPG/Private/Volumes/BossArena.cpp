// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/BossArena.h"
#include "Components/BoxComponent.h"

ABossArena::ABossArena()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	Wall1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 1"));
	Wall1->SetupAttachment(GetRootComponent());
	Wall1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Walls.Add(Wall1);
	
	Wall2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 2"));
	Wall2->SetupAttachment(GetRootComponent());
	Wall2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Walls.Add(Wall2);
	
	Wall3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 3"));
	
	Wall3->SetupAttachment(GetRootComponent());
	Wall3->SetCollisionResponseToAllChannels(ECR_Ignore);
	Walls.Add(Wall3);
	
	Wall4 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall 4"));
	Wall4->SetupAttachment(GetRootComponent());
	Wall4->SetCollisionResponseToAllChannels(ECR_Ignore);
	Walls.Add(Wall4);
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
		for (UBoxComponent* Wall : Walls)
		{
			if (Wall)
			{
				Wall->SetCollisionResponseToAllChannels(ECR_Block);
				Wall->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
			}
		}
	}
}

void ABossArena::BossDead()
{
	for (UBoxComponent* Wall : Walls)
	{
		if (Wall)
		{
			Wall->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
	}
}

