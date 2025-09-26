// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TargetPoint.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000.f;
	PawnSensing->SetPeripheralVisionAngle(30.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	if (PatrolTargets.Num() > 0) PatrolTarget = PatrolTargets[PatrolPointIndex];
	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);
	SetHealthBarVisibility(false);
	SetHealthBarPercent();
	InitializeWeapon();
}

void AEnemy::InitializeWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClasses.Num() > 0)
	{
		TSubclassOf<AWeapon> RandomWeapon = WeaponClasses[FMath::RandRange(0, WeaponClasses.Num() - 1)];
		if (!RandomWeapon)
		{
			InitializeWeapon();
			return;
		}

		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(RandomWeapon);

		DefaultWeapon->Equip(GetMesh(), WeaponSocket, this, this);
		EquippedWeapon = DefaultWeapon;

		if (SecondaryWeaponSocket != FName("Null"))
		{
			DefaultWeapon = World->SpawnActor<AWeapon>(RandomWeapon);
			DefaultWeapon->Equip(GetMesh(), SecondaryWeaponSocket, this, this);
			SecondaryEquippedWeapon = DefaultWeapon;

		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius) && !GetWorldTimerManager().IsTimerActive(PatrolTimer))
	{
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished,
			FMath::RandRange(PatrolWaitMin, PatrolWaitMax));
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (!IsChasing() && IsOutsideAttackRadius())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Patrolling && SeenPawn->ActorHasTag(FName("Knight")) &&
		!SeenPawn->ActorHasTag(FName("Dead")))
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		ChaseTarget();
	}
}

void AEnemy::LookAtPlayer(const AActor* Player)
{
	if (Player)
	{
		FVector Direction = Player->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = Direction.Rotation();
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;
		SetActorRotation(NewRotation);
	}
}

void AEnemy::SetPatrolTarget(FString TargetName)
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> AllPatrolPoints;
	UGameplayStatics::GetAllActorsOfClass(World, ATargetPoint::StaticClass(), AllPatrolPoints);
	if (AllPatrolPoints.Num() == 0) return;

	for (AActor* PatrolPoint : AllPatrolPoints)
	{
		if (PatrolPoint->GetName() == TargetName)
		{
			PatrolTarget = PatrolPoint;
			return;
		}
	}
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::PatrolTimerFinished()
{
	if (NewPatrolTarget())
	{
		MoveToTarget(PatrolTarget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Set Patrol Target"))
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	EnemyController->MoveTo(MoveRequest);
}

bool AEnemy::NewPatrolTarget()
{
	if (PatrolTargets.Num() <= 0) return false;

	PatrolPointIndex = (PatrolPointIndex + 1) % PatrolTargets.Num();
	PatrolTarget = PatrolTargets[PatrolPointIndex];
	return true;
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	if (CombatTarget)
	{
		MoveToTarget(CombatTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Combat Target"))
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	SetHealthBarVisibility(false);
}

void AEnemy::SetSecondaryWeaponCanDamage(bool state)
{
	if (SecondaryEquippedWeapon)
	{
		SecondaryEquippedWeapon->EmptyIgnoreActors();
		SecondaryEquippedWeapon->SetCanDamage(state);
	}
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::Attack()
{
	if (BaseAttack(PossibleAttacks, CurrentAttackIndex, ComboResetTimerHandle, ComboResetTime,
		[this]() { CurrentAttackIndex = 0; } ))
	{
		EnemyState = EEnemyState::EES_Engaged;

		AttackResetTime = FMath::RandRange(AttackResetTimeMin, AttackResetTimeMax);
		GetWorldTimerManager().SetTimer(AttackResetTimer, this, &AEnemy::AttackEnd, AttackResetTime);
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
	GetWorldTimerManager().ClearTimer(AttackResetTimer);
}

void AEnemy::AttackResetComboExtention()
{
	GetWorldTimerManager().ClearTimer(AttackResetTimer);
	AttackResetTime = FMath::RandRange(AttackResetTimeMin, AttackResetTimeMax);
	GetWorldTimerManager().SetTimer(AttackResetTimer, this, &AEnemy::AttackEnd, AttackResetTime);}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_NoState;
	return DamageAmount;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) SetHealthBarVisibility(true);
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);
	StopAttackMontage();
	SetWeaponCanDamage(false);
	//if (IsInsideAttackRadius() && !IsDead()) StartAttackTimer();
}

void AEnemy::HitReactEnd()
{
	CheckCombatTarget();
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	EnemyState = EEnemyState::EES_Dead;

	ClearAttackTimer();
	SetHealthBarVisibility(false);
	GetWorldTimerManager().ClearTimer(AttackResetTimer);
	DisableCapsule();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetLifeSpan(DeathLifeSpan);
	SetWeaponCanDamage(false);
	SpawnSoul();
	SpawnKey();
	if (BossArena) BossArena->Destroy();
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::SetHealthBarVisibility(bool Visibility)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(Visibility);
	}
}

void AEnemy::SetHealthBarPercent()
{
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			int32 Souls = FMath::RandRange(Attributes->GetMinSouls(), Attributes->GetMaxSouls());
			SpawnedSoul->SetSouls(Souls);
			SpawnedSoul->SetOwner(this);
			SpawnedSoul->GetSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

void AEnemy::SpawnKey()
{
	UWorld* World = GetWorld();
	if (World && Key)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		World->SpawnActor<AActor>(Key, SpawnLocation, GetActorRotation());
	}
}
