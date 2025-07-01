#include "BatchLODGeneratorModule.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MeshUtilities.h"
#include "StaticMeshResources.h"
#include "StaticMeshEditorModule.h"
#include "Engine/StaticMesh.h"
#include "Logging/LogMacros.h"
#include "PropertyEditorPermissionList.h"  // Required for FPermissionListOwners

#include "BatchLODGeneratorEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "BatchLODGeneratorModule"

// ------------------

class SMyComboBoxDialog : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyComboBoxDialog) {}
        SLATE_ARGUMENT(TArray<FName>, Options)
        SLATE_ARGUMENT(TFunction<void(FName)>, OnSelected)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        Options = InArgs._Options;
        OnSelected = InArgs._OnSelected;

        for (const FName& Name : Options)
        {
            ComboBoxItems.Add(MakeShared<FName>(Name));
        }

        ChildSlot
            [
                SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SComboBox<TSharedPtr<FName>>)
                            .OptionsSource(&ComboBoxItems)
                            .OnGenerateWidget(this, &SMyComboBoxDialog::GenerateComboBoxItem)
                            .OnSelectionChanged(this, &SMyComboBoxDialog::OnComboBoxSelectionChanged)
                            [
                                SAssignNew(SelectedTextBlock, STextBlock)
                                    .Text(FText::FromString("Select an option"))
                            ]
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SButton)
                            .Text(FText::FromString("OK"))
                            .OnClicked(this, &SMyComboBoxDialog::OnOkClicked)
                    ]
            ];
    }

private:
    TArray<FName> Options;
    TArray<TSharedPtr<FName>> ComboBoxItems;
    TSharedPtr<FName> SelectedItem;
    TSharedPtr<STextBlock> SelectedTextBlock;
    TFunction<void(FName)> OnSelected;

    TSharedRef<SWidget> GenerateComboBoxItem(TSharedPtr<FName> Item)
    {
        return SNew(STextBlock).Text(FText::FromName(*Item));
    }

    void OnComboBoxSelectionChanged(TSharedPtr<FName> NewSelection, ESelectInfo::Type SelectInfo)
    {
        SelectedItem = NewSelection;
        if (SelectedTextBlock.IsValid() && SelectedItem.IsValid())
        {
            SelectedTextBlock->SetText(FText::FromName(*SelectedItem));
        }
    }

    FReply OnOkClicked()
    {
        if (OnSelected && SelectedItem.IsValid())
        {
            OnSelected(*SelectedItem);
        }
        FSlateApplication::Get().DismissAllMenus();
        return FReply::Handled();
    }
};

void ShowMyComboBoxDialog(const TArray<FName>& Options, TFunction<void(FName)> OnSelected)
{
    TSharedRef<SWindow> Dialog = SNew(SWindow)
        .Title(FText::FromString("Select an Option"))
        .ClientSize(FVector2D(300, 100))
        [
            SNew(SMyComboBoxDialog)
                .Options(Options)
                .OnSelected(OnSelected)
        ];

    FSlateApplication::Get().AddWindow(Dialog);
}

//--------------

void FBatchLODGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FBatchLODGeneratorEditorModeCommands::Register();

	// Register the context menu extension
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	MenuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FBatchLODGeneratorModule::OnExtendContentBrowserAssetSelectionMenu));
}

void FBatchLODGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FBatchLODGeneratorEditorModeCommands::Unregister();
}

TSharedRef<FExtender> FBatchLODGeneratorModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
    TSharedRef<FExtender> Extender(new FExtender());

    // Add the "Generate LODs" option to the context menu
    Extender->AddMenuExtension(
        "CommonAssetActions",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateRaw(this, &FBatchLODGeneratorModule::AddBatchLODGenerationMenuEntry, SelectedAssets)
    );

    return Extender;
}

void FBatchLODGeneratorModule::AddBatchLODGenerationMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT("GenerateLODs", "Generate LODs"),
        LOCTEXT("GenerateLODsTooltip", "Generate LODs for selected static meshes"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FBatchLODGeneratorModule::OnExecuteBatchLODGeneration, SelectedAssets))
    );
}

void FBatchLODGeneratorModule::OnExecuteBatchLODGeneration(TArray<FAssetData> SelectedAssets)
{
    TArray<FName> LODGroupNames;
    UStaticMesh::GetLODGroups(LODGroupNames);

    ShowMyComboBoxDialog(LODGroupNames, [SelectedAssets](FName SelectedName)
        {
            UE_LOG(LogTemp, Log, TEXT("User selected: %s"), *SelectedName.ToString());

            for (const FAssetData& AssetData : SelectedAssets)
            {
                if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset()))
                {
                    FName LODGroupName = SelectedName;

                    StaticMesh->bAutoComputeLODScreenSize = true;
                    StaticMesh->LODGroup = LODGroupName;

                    StaticMesh->SetLODGroup(LODGroupName);

                    StaticMesh->GenerateLodsInPackage();
                }
            }

        });

}
 
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBatchLODGeneratorModule, BatchLODGeneratorEditorMode)
