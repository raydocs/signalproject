#include "Report/ReportEditorWidget.h"

#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Engine/DataTable.h"

void UReportEditorWidget::InitializeForSlice(ASignalGameFlowManager* InFlowManager, ARouteStateManager* InRouteStateManager)
{
    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;

    BuildOptionList();
    RefreshSubmitState();
}

void UReportEditorWidget::BuildOptionList()
{
    DisplayedRows.Reset();

    if (ReportSentencesTable)
    {
        TArray<FST_ReportSentenceRow*> Rows;
        ReportSentencesTable->GetAllRows(TEXT("BuildOptionList"), Rows);

        const int32 ActiveDay = FlowManagerRef ? FlowManagerRef->CurrentDayIndex : 1;
        for (const FST_ReportSentenceRow* Row : Rows)
        {
            if (Row && (Row->DayIndex == ActiveDay || Row->DayIndex == 0))
            {
                DisplayedRows.Add(*Row);
            }
        }
    }

    BP_OnReportOptionsBuilt(DisplayedRows);
}

void UReportEditorWidget::HandleSentenceSelected(FName SentenceId)
{
    SelectedSentenceId = SentenceId;
    RefreshSubmitState();
}

void UReportEditorWidget::HandleSubmitClicked()
{
    RefreshSubmitState();
    if (!bCanSubmitState || !FlowManagerRef)
    {
        return;
    }

    FlowManagerRef->SubmitSliceReport(SelectedSentenceId);
}

void UReportEditorWidget::RefreshSubmitState()
{
    const bool bHasSentence = SelectedSentenceId != NAME_None;
    const bool bFlowAllows = FlowManagerRef && FlowManagerRef->CanOpenReport();
    const bool bHasRouteState = RouteStateManagerRef && RouteStateManagerRef->LastRouteChoice != E_RouteBranch::Neutral;

    bCanSubmitState = bHasSentence && bFlowAllows && bHasRouteState;
    BP_OnSubmitStateChanged(bCanSubmitState);
}
