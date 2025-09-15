// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Campfire.generated.h"

class IUpgradeInterface;
class UCampfireWidgetComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class SLASH_UDEMYRPG_API ACampfire : public AActor
{
	GENERATED_BODY()
	
public:	
	ACampfire();
	virtual void Tick(float DeltaTime) override;

	void UpgradeComplete();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere)
	UCampfireWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere, Category = "FX")
	UNiagaraSystem* UpgradeEffect;

	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* UpgradeSound;

private:	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	IUpgradeInterface* UpgradeInterface;
};
