#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "DesktopRootWidget.generated.h"

class AChatConversationManager;
class AMinigameManager;
class ARouteStateManager;
class ASignalGameFlowManager;
class UButton;
class UChatAppWidget;
class UOverlay;
class UPanelWidget;
class UReportEditorWidget;
class UTaskListWidget;
class UTextBlock;
class UUserWidget;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UDesktopRootWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void InitializeForSlice(
        ASignalGameFlowManager* InFlowManager,
        ARouteStateManager* InRouteStateManager,
        AChatConversationManager* InChatConversationManager,
        AMinigameManager* InMinigameManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void OpenApp(FName AppTag);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void ShowAppWidget(UUserWidget* AppWidget);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void ShowTaggedAppWidget(FName AppTag, UUserWidget* AppWidget);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void ClearHostedApp(UUserWidget* AppWidget);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void CloseActiveApp();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Desktop")
    void RefreshAppAvailability();

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Desktop")
    UUserWidget* GetActiveAppWidget() const { return ActiveAppWidget; }

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Desktop")
    FName GetActiveAppTag() const { return ActiveAppTag; }

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Desktop")
    void BP_OnAppWidgetRequested(UUserWidget* AppWidget);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Desktop")
    void BP_OnAppAvailabilityChanged(bool bCanOpenReport);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Desktop")
    TSubclassOf<UChatAppWidget> ChatAppWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Desktop")
    TSubclassOf<UReportEditorWidget> ReportEditorWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Desktop")
    TSubclassOf<UTaskListWidget> TaskListWidgetClass;

private:
    UFUNCTION()
    void HandlePhaseChanged(E_GamePhase PreviousPhase, E_GamePhase NewPhase);

    UFUNCTION()
    void HandleChatMessagesUpdated();

    UFUNCTION()
    void HandleOpenChatClicked();

    UFUNCTION()
    void HandleOpenTasksClicked();

    UFUNCTION()
    void HandleOpenStressTestClicked();

    UFUNCTION()
    void HandleOpenReportClicked();

    UFUNCTION()
    void HandleCloseActiveAppClicked();

    void BindOptionalShellButtons();
    void UnbindOptionalShellButtons();
    bool EnsureDebugShell();
    void RefreshDebugStatus();
    void RefreshTaskListState();
    bool IsDesktopShellPhase(E_GamePhase Phase) const;
    UPanelWidget* ResolveAppContentHost() const;

private:
    UPROPERTY(Transient)
    TObjectPtr<ASignalGameFlowManager> FlowManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<UChatAppWidget> ChatAppWidgetInstance;

    UPROPERTY(Transient)
    TObjectPtr<UReportEditorWidget> ReportEditorWidgetInstance;

    UPROPERTY(Transient)
    TObjectPtr<UTaskListWidget> TaskListWidgetInstance;

    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> ActiveAppWidget;

    UPROPERTY(Transient)
    FName ActiveAppTag = NAME_None;

    UPROPERTY(Transient)
    FName PendingAppTag = NAME_None;

    UPROPERTY(Transient)
    FText CurrentDayTitle;

    UPROPERTY(Transient)
    TArray<FST_DesktopTaskRecord> CurrentTasks;

    UPROPERTY(Transient)
    bool bHasOpenedChatAppThisDay = false;

    UPROPERTY(Transient)
    int32 CachedTaskDayIndex = INDEX_NONE;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> AppContentHost;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> ChatAppButton;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> TasksAppButton;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> StressTestAppButton;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> ReportAppButton;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> CloseAppButton;

    UPROPERTY(Transient)
    TObjectPtr<UOverlay> DebugRootOverlay;

    UPROPERTY(Transient)
    TObjectPtr<UPanelWidget> DebugAppContentHost;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> DebugPhaseText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> DebugAppText;

    UPROPERTY(Transient)
    TObjectPtr<UButton> DebugChatButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> DebugTasksButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> DebugStressTestButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> DebugReportButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> DebugCloseButton;
};
