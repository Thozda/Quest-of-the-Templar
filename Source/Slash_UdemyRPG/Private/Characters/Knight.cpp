// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Knight.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/BoxComponent.h"

// Sets default values
AKnight::AKnight()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetWorldLocation(FVector(0.f, 60.f, 100.f));
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AKnight::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(KnightMappingContext, 0);
		}
	}

	Tags.Add(FName("Knight"));
}

// Called every frame
void AKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

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

void AKnight::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		OverlappingItem = nullptr;
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{
		if (CanDisarm())
		{
			PlayArmMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_UnEquipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			PlayArmMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
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
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AKnight::Arm()
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

void AKnight::WeamponCanDamageTrue()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCanDamage(true);
	}
}

void AKnight::WeamponCanDamageFalse()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCanDamage(false);
	}
}

void AKnight::LightAttack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		int32 SelectedAttack = PossibleLightAttacks[CurrentLightAttackIndex];
		PlayAttackMontage(SelectedAttack);

		CurrentLightAttackIndex = (CurrentLightAttackIndex + 1) % PossibleLightAttacks.Num();

		GetWorldTimerManager().ClearTimer(LightComboResetTimerHandle);
		GetWorldTimerManager().SetTimer(LightComboResetTimerHandle, this, &AKnight::ResetLightAttackIndex, LightComboResetTime, false);
	}
}

void AKnight::HeavyAttack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;

		int32 SelectedAttack = PossibleHeavyAttacks[CurrentHeavyAttackIndex];
		PlayAttackMontage(SelectedAttack);

		CurrentHeavyAttackIndex = (CurrentHeavyAttackIndex + 1) % PossibleHeavyAttacks.Num();

		GetWorldTimerManager().ClearTimer(HeavyComboResetTimerHandle);
		GetWorldTimerManager().SetTimer(HeavyComboResetTimerHandle, this, &AKnight::ResetHeavyAttackIndex, HeavyComboResetTime, false);
	}
}



void AKnight::PlayAttackMontage(const int32& Selection)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		//const int32 Selection = FMath::RandRange(PossibleAttacks[0], PossibleAttacks[PossibleAttacks.Num() - 1]);
		FName RandomAttack = FName();
		switch (Selection)
		{
		case 0:
			RandomAttack = FName("HorizontalAttack");
			break;
		case 1:
			RandomAttack = FName("DownwardAttack");
			break;
		case 2:
			RandomAttack = FName("UpwardAttack");
			break;
		case 3:
			RandomAttack = FName("360Attack");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(RandomAttack, AttackMontage);
	}
}

bool AKnight::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_UnEquipped &&
		!KnightIsFalling();
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

void AKnight::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


// Called to bind functionality to input
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
	}
}