// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	//
	//Pickup
	//
	void Equip(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocket);
protected:
	virtual void BeginPlay() override;

	//
	//Physics
	//
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:
	//
	//Pickup
	//
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateLootParticles();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	//
	//Damage
	//
	void WeaponBoxTrace();
	void DealDamage();
	void ExecuteGetHit();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BaseDamage = 25.f;

	FHitResult BoxHit;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceSize = FVector(15.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	TArray<AActor*> IgnoreActors;

	bool bCanDamage = false;

public:
	FORCEINLINE void EmptyIgnoreActors() { IgnoreActors.Empty(); }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCanDamage(bool state) { bCanDamage = state; }
	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }
	FORCEINLINE void SetDamageAmount(float Amount) { Damage = Amount; }
};
