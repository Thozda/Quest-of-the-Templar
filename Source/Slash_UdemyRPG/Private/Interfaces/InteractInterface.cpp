// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/InteractInterface.h"

// Add default functionality here for any IInteractInterface functions that are not pure virtual.
void IInteractInterface::SetOverlappingActor(AActor* Actor)
{
}

UCameraComponent* IInteractInterface::GetDoorTextTarget()
{
	return nullptr;
}

FString IInteractInterface::DoorText()
{
	return FString();
}
