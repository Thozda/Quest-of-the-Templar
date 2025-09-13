// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/Deathbox.h"

#include "Characters/Knight.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


ADeathbox::ADeathbox()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADeathbox::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ADeathbox::OnBoxOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ADeathbox::OnBoxEndOverlap);
}

void ADeathbox::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Knight")))
	{
		KnightInBox = OtherActor;
	}
}

void ADeathbox::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Knight")))
	{
		KnightInBox = nullptr;
	}
}

void ADeathbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (KnightInBox)
	{
		UGameplayStatics::ApplyDamage(KnightInBox, 100.f * DeltaTime, nullptr, this, UDamageType::StaticClass());
	}
}

