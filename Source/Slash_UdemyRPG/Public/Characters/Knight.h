// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Knight.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class USoundBase;

UCLASS()
class SLASH_UDEMYRPG_API AKnight : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKnight();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	//Anim Montages
	//
	void PlayArmMontage(const FName& SelectionName);
	
	virtual void AttackEnd() override;

	virtual bool CanAttack() override;

	bool KnightIsFalling();

	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishedArming();

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleLightAttacks;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleHeavyAttacks;

private:
	ECharacterState CharacterState = ECharacterState::ECS_UnEquipped;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//
	//Animation Montages
	//
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ArmMontage;

	//
	//Attack
	//
	int32 CurrentLightAttackIndex = 0;
	int32 CurrentHeavyAttackIndex = 0;

	float LightComboResetTime = 5.f;
	float HeavyComboResetTime = 5.f;

	FTimerHandle LightComboResetTimerHandle;
	FTimerHandle HeavyComboResetTimerHandle;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState state) { ActionState = state; }
};
