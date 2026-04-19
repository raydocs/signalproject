#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "SignalSliceTypes.h"
#include "ReportEditorWidget.generated.h"

class ARouteStateManager;
class ASignalGameFlowManager;
class UButton;
class UComboBoxString;
class UDataTable;
class UTextBlock;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UReportEditorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void InitializeForSlice(ASignalGameFlowManager* InFlowManager, ARouteStateManager* InRouteStateManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void BuildOptionList();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void HandleSentenceSelected(FName SentenceId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void HandleInjectionSentenceToggled(FName SentenceId, bool bSelected);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void HandleSubmitClicked();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Report")
    void RefreshSubmitState();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Report")
    void BP_OnReportOptionsBuilt(const TArray<FST_ReportSentenceRow>& Rows);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Report")
    void BP_OnInjectedClausesBuilt(const TArray<FST_ReportSentenceRow>& Rows);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Report")
    void BP_OnSubmitStateChanged(bool bCanSubmit);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> ReportSentencesTable;

private:
    UFUNCTION()
    void HandleFallbackSentenceSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    void EnsureDefaultLayout();
    void RefreshDefaultLayout();
    FST_ReportSubmissionPayload BuildSubmissionPayload() const;
    bool HasBaseSentence(FName SentenceId) const;
    const FST_ReportSentenceRow* FindBaseSentence(FName SentenceId) const;

private:
    UPROPERTY(Transient)
    TObjectPtr<ASignalGameFlowManager> FlowManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    FName SelectedSentenceId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    TArray<FST_ReportSentenceRow> DisplayedBaseRows;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    TArray<FST_ReportSentenceRow> DisplayedInjectedRows;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    TArray<FName> SelectedInjectedSentenceIds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Report", meta = (AllowPrivateAccess = "true"))
    bool bCanSubmitState = false;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> HeaderText;

    UPROPERTY(Transient)
    TObjectPtr<UComboBoxString> SentenceComboBox;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> InjectionAreaText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> StatusText;

    UPROPERTY(Transient)
    TObjectPtr<UButton> SubmitButton;

    TMap<FString, FName> FallbackSentenceIdsByLabel;
};
