// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UMainOverlay;

UCLASS()
class SLASH_UDEMYRPG_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UMainOverlay> OverlayClass;

	UPROPERTY()
	UMainOverlay* Overlay;
public:
	FORCEINLINE UMainOverlay* GetOverlay() const { return Overlay; }
};
