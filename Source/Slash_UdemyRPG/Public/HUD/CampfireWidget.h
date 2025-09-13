// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampfireWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API UCampfireWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;
};
