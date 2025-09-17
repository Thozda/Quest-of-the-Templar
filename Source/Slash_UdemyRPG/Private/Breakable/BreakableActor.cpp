// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(RootComponent);

	// Configure limits: only Z free
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.f);

	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnBreak);

	// Constrain the geometry collection against the world
	PhysicsConstraint->SetConstrainedComponents(GeometryCollection, NAME_None,
		nullptr, NAME_None); // nullptr = world
}



// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Material1)
	{
		DissolveAmount = FMath::Clamp(DissolveAmount + DeltaTime * DissolveSpeed, -1, 1);
		Material1->SetScalarParameterValue(FName("Dissolve"), 1.f * DissolveAmount);
	}

	if (Material2)
	{
		DissolveAmount = FMath::Clamp(DissolveAmount + DeltaTime * DissolveSpeed, -1, 1);
		Material2->SetScalarParameterValue(FName("Dissolve"), 1.f * DissolveAmount);
	}
}

void ABreakableActor::OnBreak(const FChaosBreakEvent& BreakEvent)
{
	if (bBroken) return;
	bBroken = true;

	//Breakable Fracture Event
	SetLifeSpan(5.f);

	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, GetActorLocation());
	}

	SpawnTreasure();

	if (GeometryCollection)
	{
		Material1 = GeometryCollection->CreateAndSetMaterialInstanceDynamic(0);
		Material2 = GeometryCollection->CreateAndSetMaterialInstanceDynamic(1);
	}

	if (PhysicsConstraint)
	{
		// Configure limits: only Z free
		PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.f);
		PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.f);
		PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.f);

		PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 0.f);
		PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0.f);
		PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Free, 0.f);
	}
}

void ABreakableActor::SpawnTreasure()
{
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 50.f;

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);

		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bHit) return;
	bHit = true;

	//Sword hit Breakable
}