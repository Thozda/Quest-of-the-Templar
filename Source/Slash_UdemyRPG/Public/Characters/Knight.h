// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "Knight.generated.h"

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
class SLASH_UDEMYRPG_API AKnight : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AKnight();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override; 
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
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
	UInputAction* EquipAction;

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
	//Attack
	//
	UPROPERTY(EditAnywhere, Category = Combat)
	float LightDamageMultiplier = 0.8;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float HeavyDamageMultiplier = 1.2f;
	
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
	//Components
	//
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

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
	void Equip(const FInputActionValue& Value);
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

	//
	//Damage
	//

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState state) { ActionState = state; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE TEnumAsByte<ECharacterDeathPose> GetCharacterDeathPose() const { return CharacterDeathPose; }
};
