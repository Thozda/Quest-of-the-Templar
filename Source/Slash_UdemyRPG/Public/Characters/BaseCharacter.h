// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;

UCLASS()
class SLASH_UDEMYRPG_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCanDamage(bool state);

protected:
	virtual void BeginPlay() override;

	virtual bool CanAttack();
	virtual void BaseAttack(TArray<int32> PossibleAttacks, int32 CurrentAttackIndex, FTimerHandle ComboResetTimerHandle, float ComboResetTime);
	virtual void PlayAttackMontage(const int32& Selection);

	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

private:

};
