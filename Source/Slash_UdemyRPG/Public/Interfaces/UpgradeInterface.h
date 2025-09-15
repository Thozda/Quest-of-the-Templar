// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UpgradeInterface.generated.h"

class UCameraComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUpgradeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_UDEMYRPG_API IUpgradeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOverlappingCampfire(AActor* OverlappingCampfire);
	virtual UCameraComponent* GetPlayerCamera();
	virtual void SetCampfireNull();
	virtual FString CampfireText();
};
