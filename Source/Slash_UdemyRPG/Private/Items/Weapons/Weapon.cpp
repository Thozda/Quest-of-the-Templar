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

	Damage = BaseDamage;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanDamage)
	{
		WeaponBoxTrace();

		if (BoxHit.GetActor() && !IgnoreActors.Contains(BoxHit.GetActor())) DealDamage();
	}
}

//
//Equip
//
void AWeapon::Equip(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocket);
	DisableSphereCollision();
	PlayEquipSound();
	DeactivateLootParticles();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocket)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocket);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound) UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::DeactivateLootParticles()
{
	if (ItemEffect) ItemEffect->Deactivate();
}

//
//Damage
//
void AWeapon::WeaponBoxTrace()
{
	BoxHit = FHitResult();

	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);
	ActorsToIgnore.AddUnique(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceSize,
		BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);
}

void AWeapon::DealDamage()
{
	IgnoreActors.AddUnique(BoxHit.GetActor());

	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && BoxHit.GetActor()->ActorHasTag(TEXT("Enemy"))) return;
	if (GetOwner()->ActorHasTag(TEXT("Knight")) && BoxHit.GetActor()->ActorHasTag(TEXT("Knight"))) return;

	UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(),
		this, UDamageType::StaticClass());
	
	ExecuteGetHit();

	CreateFields(BoxHit.ImpactPoint);
}

void AWeapon::ExecuteGetHit()
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}
