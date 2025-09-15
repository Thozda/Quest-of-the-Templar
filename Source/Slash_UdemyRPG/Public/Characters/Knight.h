// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "Interfaces/InteractInterface.h"
#include "Interfaces/UpgradeInterface.h"
#include "Knight.generated.h"

class ACampfire;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UMainOverlay;
class AItem;
class UAnimMontage;
class USoundBase;
class ASoul;
class ATreasure;

UCLASS()
class SLASH_UDEMYRPG_API AKnight : public ABaseCharacter, public IPickupInterface, public IInteractInterface, public IUpgradeInterface
{
	GENERATED_BODY()

public:
	AKnight();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override; 
	//Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	//Interact
	virtual void SetOverlappingActor(AActor* Actor) override;
	virtual UCameraComponent* GetDoorTextTarget() override;
	virtual FString DoorText() override;
	//Upgrades
	virtual void SetOverlappingCampfire(AActor* OverlappingCampfire) override;
	virtual UCameraComponent* GetPlayerCamera() override;
	virtual void SetCampfireNull() override;
	virtual FString CampfireText() override;
	//Pickups
	virtual void SetOverlappingItem(AActor* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditAnywhere)
	USoundBase* ExertMetaSound;

	//
	//Enhanced Input Variables
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* KnightMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DisarmAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;

	//
	//Anim Montages
	//
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishedArming();

	virtual void HitReactEnd() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleLightAttacks;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleHeavyAttacks;

	//
	//Damage
	//
	 
	virtual void Die_Implementation() override;
	virtual int32 PlayDeathMontage() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ECharacterDeathPose> CharacterDeathPose;


private:
	ECharacterState CharacterState = ECharacterState::ECS_UnEquipped;

	void InitialiseEnhancedInput(APlayerController* PlayerController);

	bool KnightIsFalling();

	//
	//Endgame
	//
	void InitialiseTeleportTarget();
	
	UPROPERTY(VisibleAnywhere, Category = "Endgame")
	int32 KeysHeld;

	UPROPERTY(EditAnywhere, Category = "Endgame")
	int32 KeysRequired = 2;

	UPROPERTY(VisibleAnywhere, Category = "Endgame")
	AActor* TeleportTarget;

	//
	//Components
	//
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AActor* OverlappingActor;

	//
	//Upgrades
	//
	UPROPERTY()
	ACampfire* Campfire;
	
	//
	//Overlay
	//
	void InitialiseOverlay(APlayerController* PlayerController);
	void SetHUDHealth();

	UPROPERTY()
	UMainOverlay* Overlay;

	//
	//Callbacks For Input
	//
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void LightAttack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);

	//
	//Animation Montages
	//
	void PlayArmMontage(const FName& SelectionName);

	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ArmMontage;

	//
	//Attack
	//
	virtual bool CanAttack() override;
	void EquipWeapon(AWeapon* Weapon);

	int32 CurrentLightAttackIndex = 0;
	int32 CurrentHeavyAttackIndex = 0;

	float LightComboResetTime = 5.f;
	float HeavyComboResetTime = 5.f;

	FTimerHandle LightComboResetTimerHandle;
	FTimerHandle HeavyComboResetTimerHandle;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState state) { ActionState = state; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE TEnumAsByte<ECharacterDeathPose> GetCharacterDeathPose() const { return CharacterDeathPose; }
};
