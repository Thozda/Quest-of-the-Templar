// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FBatchLODGeneratorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
	void AddBatchLODGenerationMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets);
	void OnExecuteBatchLODGeneration(TArray<FAssetData> SelectedAssets);

private:
	void ExecuteBatchLODGeneration();
	void OnExecuteBatchLODGeneration();
};
