// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UBoxComponent;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Breakable", meta=(AllowPrivateAccess="true"))
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Breakable", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollider;
	
private:	
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	USoundBase* BreakSound;

	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);

	void SpawnTreasure();

	bool bBroken = false;
	bool bHit = false;
};
