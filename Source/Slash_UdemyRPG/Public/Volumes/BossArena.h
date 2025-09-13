// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossArena.generated.h"

class UBoxComponent;

UCLASS()
class SLASH_UDEMYRPG_API ABossArena : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossArena();
	virtual void Tick(float DeltaTime) override;

	void BossDead();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Wall1;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Wall2;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Wall3;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Wall4;
	
private:	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	TArray<UBoxComponent*> Walls;
};
