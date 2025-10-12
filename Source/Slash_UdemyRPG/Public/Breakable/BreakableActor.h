// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UPhysicsConstraintComponent;
class USoundBase;

UCLASS()
class SLASH_UDEMYRPG_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

private:
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	USoundBase* BreakSound;

	UPROPERTY()
	UMaterialInstanceDynamic* Material1;
	UMaterialInstanceDynamic* Material2;

	UPROPERTY(VisibleAnywhere)
	float DissolveAmount = -1.f;
	
	UPROPERTY(VisibleAnywhere)
	float DissolveSpeed = 0.3f;
	
	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);

	void SpawnTreasure();

	bool bBroken = false;
	bool bHit = false;

public:
	FORCEINLINE bool GetIsBroken() const { return bBroken; }
};
