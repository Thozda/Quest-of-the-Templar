// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/MountainDoor.h"

#include "Components/BoxComponent.h"
#include "HUD/CampfireWidgetComponent.h"
#include "Interfaces/InteractInterface.h"

AMountainDoor::AMountainDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetCollisionObjectType(ECC_WorldStatic);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Box->SetupAttachment(GetRootComponent());
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	WidgetComponent = CreateDefaultSubobject<UCampfireWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(GetRootComponent());
}

void AMountainDoor::BeginPlay()
{
	Super::BeginPlay();

	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AMountainDoor::OnBoxBeginOverlap);
		Box->OnComponentEndOverlap.AddDynamic(this, &AMountainDoor::OnBoxEndOverlap);
	}
}

void AMountainDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InteractInterface && WidgetComponent)
	{
		WidgetComponent->FacePlayerCamera(InteractInterface->GetDoorTextTarget());
	}
}


void AMountainDoor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IInteractInterface>(OtherActor) && WidgetComponent)
	{
		InteractInterface = Cast<IInteractInterface>(OtherActor);
		InteractInterface->SetOverlappingActor(this);
		WidgetComponent->SetCampfireText(InteractInterface->DoorText());
	}
}

void AMountainDoor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<IInteractInterface>(OtherActor) && WidgetComponent)
	{
		InteractInterface->SetOverlappingActor(nullptr);
		InteractInterface = nullptr;
		WidgetComponent->SetCampfireText(FString());
	}
}
