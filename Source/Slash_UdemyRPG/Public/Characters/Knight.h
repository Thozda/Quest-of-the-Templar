// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Knight.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class USoundBase;
class AWeapon;

UCLASS()
class SLASH_UDEMYRPG_API AKnight : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKnight();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCanDamage(bool state);
	//void SetWeaponCanDamage(ECollisionEnabled::Type CollisionEnabled);
	

protected:
	// Called when the game starts or when spawned
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

	//
	//Callbacks For Input
	//

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void LightAttack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);

	//
	//Play Montage Functions
	//

	void PlayAttackMontage(const int32& Selection);
	void PlayArmMontage(const FName& SelectionName);
	
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	bool CanAttack();
	bool KnightIsFalling();

	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishedArming();

	UFUNCTION(BlueprintCallable)
	void WeamponCanDamageTrue();

	UFUNCTION(BlueprintCallable)
	void WeamponCanDamageFalse();

private:
	ECharacterState CharacterState = ECharacterState::ECS_UnEquipped;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	//
	//Animation Montages
	//
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ArmMontage;

	//
	//Attack
	//
	int32 CurrentLightAttackIndex = 0;
	FORCEINLINE void ResetLightAttackIndex() { CurrentLightAttackIndex = 0; }
	int32 CurrentHeavyAttackIndex = 0;
	FORCEINLINE void ResetHeavyAttackIndex() { CurrentHeavyAttackIndex = 0; }

	float ComboResetTime = 5.f;

	FTimerHandle ComboResetTimerHandle;

	TArray<int32> PossibleLightAttacks = { 0, 1 };
	TArray<int32> PossibleHeavyAttacks = { 2, 3 };

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
