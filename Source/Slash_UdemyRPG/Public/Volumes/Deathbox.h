// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Deathbox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class SLASH_UDEMYRPG_API ADeathbox : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeathbox();
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	
	//
	//Components
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Box;
private:
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	AActor* KnightInBox;
};
