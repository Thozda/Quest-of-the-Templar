// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchLODGeneratorEditorMode.h"
#include "BatchLODGeneratorEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "BatchLODGeneratorEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/BatchLODGeneratorSimpleTool.h"
#include "Tools/BatchLODGeneratorInteractiveTool.h"

// step 2: register a ToolBuilder in FBatchLODGeneratorEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "BatchLODGeneratorEditorMode"

const FEditorModeID UBatchLODGeneratorEditorMode::EM_BatchLODGeneratorEditorModeId = TEXT("EM_BatchLODGeneratorEditorMode");

FString UBatchLODGeneratorEditorMode::SimpleToolName = TEXT("BatchLODGenerator_ActorInfoTool");
FString UBatchLODGeneratorEditorMode::InteractiveToolName = TEXT("BatchLODGenerator_MeasureDistanceTool");


UBatchLODGeneratorEditorMode::UBatchLODGeneratorEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UBatchLODGeneratorEditorMode::EM_BatchLODGeneratorEditorModeId,
		LOCTEXT("ModeName", "BatchLODGenerator"),
		FSlateIcon(),
		true);
}


UBatchLODGeneratorEditorMode::~UBatchLODGeneratorEditorMode()
{
}


void UBatchLODGeneratorEditorMode::ActorSelectionChangeNotify()
{
}

void UBatchLODGeneratorEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FBatchLODGeneratorEditorModeCommands& SampleToolCommands = FBatchLODGeneratorEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UBatchLODGeneratorSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UBatchLODGeneratorInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UBatchLODGeneratorEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FBatchLODGeneratorEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UBatchLODGeneratorEditorMode::GetModeCommands() const
{
	return FBatchLODGeneratorEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
