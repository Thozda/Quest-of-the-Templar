// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/BossArena.h"
#include "Components/BoxComponent.h"
#include "Enemy/Enemy.h"
#include "Characters/Knight.h"
#include "Components/SaveSystemComponent.h"
#include "GameMode/SlashGameMode.h"

class ASlashGameMode;

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
	if (OtherActor && Cast<AKnight>(OtherActor))
	{
		if (Boss) Boss->LookAtPlayer(OtherActor);

		Cast<AKnight>(OtherActor)->PlayBossMusic(BossMusic);
		
		ASlashGameMode* GameMode = Cast<ASlashGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode && GameMode->GetSaveSystem()) GameMode->GetSaveSystem()->SaveGame();
		else UE_LOG(LogTemp, Warning, TEXT("Failed to get game mode : Boss Arena"));
		
		for (UBoxComponent* Wall : Walls)
		{
			if (Wall)
			{
				Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
	}
}