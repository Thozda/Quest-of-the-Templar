// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MountainDoor.generated.h"

class IInteractInterface;
class UCampfireWidgetComponent;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class SLASH_UDEMYRPG_API AMountainDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMountainDoor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere)
	UCampfireWidgetComponent* WidgetComponent;
	
	IInteractInterface* InteractInterface;
};
