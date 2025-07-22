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
	//Pickup
	//
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//
	//Damage
	//
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//
	//Physics
	//
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:
	//
	//Pickup
	//
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	//
	//Damage
	//
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	FHitResult BoxHit;

	void WeaponBoxTrace();
	void DealDamage();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	TArray<AActor*> IgnoreActors;

	int hits = 0;

	bool bCanDamage = false;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE void EmptyIgnoreActors() { IgnoreActors.Empty(); }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCanDamage(bool state) { bCanDamage = state; }
};
