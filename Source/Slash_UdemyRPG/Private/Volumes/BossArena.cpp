// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/BossArena.h"
#include "Components/BoxComponent.h"
#include "Enemy/Enemy.h"
#include "Characters/Knight.h"
#include "Components/SaveSystemComponent.h"
#include "GameMode/SlashGameMode.h"
#include "Kismet/GameplayStatics.h"

class ASlashGameMode;

ABossArena::ABossArena()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic); // Or leave default
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);          // Character
	TriggerBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);   // Boss
	
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
    // Check if boss is invalid or dead
    if (Boss == nullptr || !IsValid(Boss) || Boss->IsActorBeingDestroyed() || 
        Boss->GetAttributes() == nullptr || Boss->IsDead())
    {
        // Try to find the boss in the world
        if (!BossClass) return;
        
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), BossClass, FoundActors);

        if (FoundActors.Num() > 0)
        {
            Boss = Cast<AEnemy>(FoundActors[0]);
        }

        // If still no boss, clean up and destroy arena
        if (!Boss)
        {
            if (AKnight* Knight = Cast<AKnight>(OtherActor))
            {
                Knight->StopBossMusic();
            }
            Destroy();
            return;
        }
        
        // Boss was found, continue normal flow
    }
    
    // Check if player entered the arena
    AKnight* Knight = Cast<AKnight>(OtherActor);
    if (Knight && TriggerBox && BossMusic && Boss)
    {
        // Disable trigger box
        TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
        TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        
        // Boss looks at player
        Boss->LookAtPlayer(OtherActor);

        // Start boss music
        Knight->PlayBossMusic(BossMusic);
        
        /* Save the game
        if (ASlashGameMode* GameMode = Cast<ASlashGameMode>(GetWorld()->GetAuthGameMode()))
        {
            if (GameMode->GetSaveSystem())
            {
                //GameMode->GetSaveSystem()->SaveGame();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get game mode : Boss Arena"));
        }*/
        
        // Activate arena walls
        for (UBoxComponent* Wall : Walls)
        {
            if (Wall)
            {
                Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }
        }
    }
}