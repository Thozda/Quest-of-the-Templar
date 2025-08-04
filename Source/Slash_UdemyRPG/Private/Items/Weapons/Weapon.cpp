// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/Knight.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Slash_UdemyRPG/DebugMacros.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	/*
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetBoxExtent(FVector(15.f, 15.f, 75.f));
	WeaponBox->SetWorldLocation(FVector(0.f, 0.f, 90.f));
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	*/

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceStart->SetWorldLocation(FVector(0.f, 0.f, 15.f));

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
	BoxTraceEnd->SetWorldLocation(FVector(0.f, 0.f, 160.f));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanDamage)
	{
		WeaponBoxTrace();

		if (BoxHit.GetActor() && !IgnoreActors.Contains(BoxHit.GetActor()))
		{
			DealDamage();
		}
	}
}


//
//Equip
//
void AWeapon::Equip(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator)
{
	AttachMeshToSocket(InParent, InSocket);
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (PickupEffect)
	{
		PickupEffect->Deactivate();
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocket)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocket);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

//
//Damage
//
/*void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	*

	if (!bCanDamage) return;

	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	FHitResult BoxHit;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(15.f, 15.f, 15.f), BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::ForDuration, BoxHit, true);

	if (BoxHit.GetActor() && !IgnoreActors.Contains(BoxHit.GetActor()))
	{
		IgnoreActors.AddUnique(BoxHit.GetActor());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *BoxHit.GetActor()->GetName());

		hits++;
		UE_LOG(LogTemp, Warning, TEXT("Hits: %i"), hits);

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
		}

		CreateFields(BoxHit.ImpactPoint);

		DRAW_SPHERE_HIT(BoxHit.ImpactPoint);

		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
	}
	*/
//}*/

void AWeapon::WeaponBoxTrace()
{
	BoxHit = FHitResult();

	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(15.f, 15.f, 15.f), BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, BoxHit, true);
}

void AWeapon::DealDamage()
{
	IgnoreActors.AddUnique(BoxHit.GetActor());
	//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *BoxHit.GetActor()->GetName());

	hits++;
	//UE_LOG(LogTemp, Warning, TEXT("Hits: %i"), hits);

	UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
	
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
	}

	CreateFields(BoxHit.ImpactPoint);

	//DRAW_SPHERE_HIT(BoxHit.ImpactPoint);
}
