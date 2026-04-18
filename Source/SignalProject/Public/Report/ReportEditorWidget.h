#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "ReportEditorWidget.generated.h"

class ARouteStateManager;
class ASignalGameFlowManager;
class UDataTable;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UReportEditorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void InitializeForSlice(ASignalGameFlowManager* InFlowManager, ARouteStateManager* InRouteStateManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void BuildOptionList();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void HandleSentenceSelected(FName SentenceId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void HandleSubmitClicked();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void RefreshSubmitState();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Report")
    void BP_OnReportOptionsBuilt(const TArray<FST_ReportSentenceRow>& Rows);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Report")
    void BP_OnSubmitStateChanged(bool bCanSubmit);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> ReportSentencesTable;

private:
    UPROPERTY(Transient)
    TObjectPtr<ASignalGameFlowManager> FlowManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    FName SelectedSentenceId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    TArray<FST_ReportSentenceRow> DisplayedRows;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    bool bCanSubmitState = false;
};
