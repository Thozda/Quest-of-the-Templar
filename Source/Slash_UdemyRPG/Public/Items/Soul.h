// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	ASoul();
	virtual void Tick(float DeltaTime) override; 
	
protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	void CalculateDesiredHeight();
	void MoveToDesiredHeight(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float DesiredHeight = 75.f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float Gravity = 30.f;

public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 Quantity) { Souls = Quantity; }
};
