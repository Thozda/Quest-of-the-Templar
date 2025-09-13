// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Knight.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/MainHUD.h"
#include "HUD/MainOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

AKnight::AKnight()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetWorldLocation(FVector(0.f, 60.f, 100.f));
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

void AKnight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKnight::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKnight::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AKnight::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AKnight::Equip);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AKnight::LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AKnight::HeavyAttack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AKnight::Dodge);
	}
}

void AKnight::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		InitialiseEnhancedInput(PlayerController);
		InitialiseOverlay(PlayerController);
	}

	Tags.Add(FName("Knight"));
}

void AKnight::InitialiseEnhancedInput(APlayerController* PlayerController)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(KnightMappingContext, 0);
	}
}

void AKnight::InitialiseOverlay(APlayerController* PlayerController)
{
	AMainHUD* HUD = Cast<AMainHUD>(PlayerController->GetHUD());
	if (HUD)
	{
		Overlay = HUD->GetOverlay();
		if (Overlay && Attributes)
		{
			Overlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			Overlay->SetStaminaBarPercent(1.f);
			Overlay->SetGoldText(0);
			Overlay->SetSoulsText(0);
		}
	}
}

void AKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && Overlay)
	{
		Attributes->RegenStamina(DeltaTime);
		Overlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

//
//Movement
//
void AKnight::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	const FVector2D DirectionValue = Value.Get<FVector2D>();
	if (GetController() && ((DirectionValue.X != 0.f) || (DirectionValue.Y != 0.f)))
	{
		//find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, DirectionValue.Y);

		//find out which way is Right
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, DirectionValue.X);
	}
}

void AKnight::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AKnight::Jump(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	ACharacter::Jump();
	if (ExertMetaSound && !KnightIsFalling())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExertMetaSound, GetActorLocation());
	}
}

bool AKnight::KnightIsFalling()
{
	return GetCharacterMovement()->IsFalling();
}

void AKnight::Dodge(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Attributes && Attributes->GetStamina() < Attributes->GetDodgeCost()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
	}
}

void AKnight::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

//
//Pickup
//
void AKnight::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AKnight::AddSouls(ASoul* Soul)
{
	if (Soul && Attributes && Overlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		Overlay->SetSoulsText(Attributes->GetSouls());
	}
}

void AKnight::AddGold(ATreasure* Treasure)
{
	if (Treasure && Attributes && Overlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		Overlay->SetGoldText(Attributes->GetGold());
	}
}

//
//Equip
//
void AKnight::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon) EquippedWeapon->Destroy();
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm()) Disarm();
		else if (CanArm()) Arm();
	}
}

void AKnight::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	OverlappingItem = nullptr;
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = Weapon;
}

bool AKnight::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_UnEquipped &&
		!KnightIsFalling();
}

bool AKnight::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_UnEquipped &&
		EquippedWeapon &&
		!KnightIsFalling();
}

void AKnight::Disarm()
{
	PlayArmMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_UnEquipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AKnight::Arm()
{
	PlayArmMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AKnight::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AKnight::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AKnight::FinishedArming()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AKnight::PlayArmMontage(const FName& SelectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ArmMontage)
	{
		AnimInstance->Montage_Play(ArmMontage);
		AnimInstance->Montage_JumpToSection(SelectionName, ArmMontage);
	}
}

//
//Attack
//
void AKnight::LightAttack(const FInputActionValue& Value)
{
	if (Attributes && Attributes->GetStamina() < Attributes->GetLightAttackCost()) return;
	if (CanAttack() && EquippedWeapon && Attributes && BaseAttack(PossibleLightAttacks, CurrentLightAttackIndex,
		LightComboResetTimerHandle, LightComboResetTime, [this]() { CurrentLightAttackIndex = 0; }))
	{
		ActionState = EActionState::EAS_Attacking;
		float Damage = EquippedWeapon->GetBaseDamage() * LightDamageMultiplier;
		EquippedWeapon->SetDamageAmount(Damage);
		Attributes->UseStamina(Attributes->GetLightAttackCost());
	}
}

void AKnight::HeavyAttack(const FInputActionValue& Value)
{
	if (Attributes && Attributes->GetStamina() < Attributes->GetHeavyAttackCost()) return;
	if (CanAttack() && EquippedWeapon && Attributes && BaseAttack(PossibleHeavyAttacks, CurrentHeavyAttackIndex,
		HeavyComboResetTimerHandle, HeavyComboResetTime, [this]() { CurrentHeavyAttackIndex = 0; }))
	{
		ActionState = EActionState::EAS_Attacking;
		float Damage = EquippedWeapon->GetBaseDamage() * HeavyDamageMultiplier;
		EquippedWeapon->SetDamageAmount(Damage);
		Attributes->UseStamina(Attributes->GetHeavyAttackCost());
	}
}

bool AKnight::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_UnEquipped &&
		!KnightIsFalling();
}

void AKnight::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

//
//Damage
//
void AKnight::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (IsAlive()) ActionState = EActionState::EAS_HitReaction;
	SetWeaponCanDamage(false);
}

void AKnight::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

float AKnight::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Attributes && Attributes->GetHealthPercent() <= 0.f) return DamageAmount;
	
	HandleDamage(DamageAmount);

	SetHUDHealth();

	if (Attributes && Attributes->GetHealthPercent() <= 0.f && !Cast<AWeapon>(DamageCauser)) GetHit_Implementation(GetActorLocation(), DamageCauser);
	return DamageAmount;
}

void AKnight::SetHUDHealth()
{
	if (Attributes && Overlay)
	{
		Overlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AKnight::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
}

int32 AKnight::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	TEnumAsByte<ECharacterDeathPose> Pose(Selection);
	if (Pose < ECharacterDeathPose::ECD_MAX)
	{
		CharacterDeathPose = Pose;
	}

	return Selection;
}