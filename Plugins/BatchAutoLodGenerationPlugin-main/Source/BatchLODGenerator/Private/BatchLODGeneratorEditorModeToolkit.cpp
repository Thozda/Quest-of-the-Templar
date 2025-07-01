// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchLODGeneratorEditorModeToolkit.h"
#include "BatchLODGeneratorEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "BatchLODGeneratorEditorModeToolkit"

FBatchLODGeneratorEditorModeToolkit::FBatchLODGeneratorEditorModeToolkit()
{
}

void FBatchLODGeneratorEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FBatchLODGeneratorEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FBatchLODGeneratorEditorModeToolkit::GetToolkitFName() const
{
	return FName("BatchLODGeneratorEditorMode");
}

FText FBatchLODGeneratorEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "BatchLODGeneratorEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
