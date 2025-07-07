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
}

// Called every frame
void AKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKnight::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Attacking) return;
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
	ACharacter::Jump();
	if (!FallingCheck())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExertMetaSound, GetActorLocation());
	}
}

bool AKnight::FallingCheck()
{
	return GetCharacterMovement()->IsFalling();
}

void AKnight::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
}

void AKnight::Attack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage();
	}
}

bool AKnight::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_UnEquipped;
}

void AKnight::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 3);
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
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AKnight::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AKnight::Attack);
	}
}

