// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/UpgradeInterface.h"

// Add default functionality here for any IUpgradeInterface functions that are not pure virtual.
void IUpgradeInterface::SetOverlappingCampfire(AActor* OverlappingCampfire)
{
}

UCameraComponent* IUpgradeInterface::GetPlayerCamera()
{
	return nullptr;
}

void IUpgradeInterface::SetCampfireNull()
{
}

FString IUpgradeInterface::CampfireText()
{
	return FString();
}
