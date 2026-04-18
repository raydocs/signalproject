#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DesktopRootWidget.generated.h"

class AChatConversationManager;
class AMinigameManager;
class ARouteStateManager;
class ASignalGameFlowManager;
class UChatAppWidget;
class UReportEditorWidget;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UDesktopRootWidget : public UUserWidget
{
    GENERATED_BODY()

public:
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
    void RefreshAppAvailability();

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Desktop")
    UUserWidget* GetActiveAppWidget() const { return ActiveAppWidget; }

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
    TObjectPtr<UUserWidget> ActiveAppWidget;
};
