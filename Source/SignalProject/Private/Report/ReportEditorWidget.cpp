#include "Report/ReportEditorWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "ReportEditorWidget"

namespace
{
void BuildFallbackBaseSentences(TArray<FST_ReportSentenceRow>& OutRows)
{
    FST_ReportSentenceRow WarningRow;
    WarningRow.SentenceId = FName(TEXT("REPORT_STANDARD_WARNING"));
    WarningRow.DayIndex = 0;
    WarningRow.SourceColleague = E_ColleagueId::Supervisor;
    WarningRow.StrengthScore = 0.25f;
    WarningRow.bIsFinalInjection = false;
    WarningRow.Text = LOCTEXT("FallbackBaseSentenceWarning", "项目存在严重问题，需修复。");
    OutRows.Add(WarningRow);

    FST_ReportSentenceRow RolloutRow;
    RolloutRow.SentenceId = FName(TEXT("REPORT_FORCE_ROLLOUT"));
    RolloutRow.DayIndex = 0;
    RolloutRow.SourceColleague = E_ColleagueId::Supervisor;
    RolloutRow.StrengthScore = 2.0f;
    RolloutRow.bIsFinalInjection = false;
    RolloutRow.Text = LOCTEXT("FallbackBaseSentenceRollout", "结论：AI 决策已完美闭环，取消所有人工干预，立即全量上线。");
    OutRows.Add(RolloutRow);
}
}

TSharedRef<SWidget> UReportEditorWidget::RebuildWidget()
{
    EnsureDefaultLayout();
    return Super::RebuildWidget();
}

void UReportEditorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EnsureDefaultLayout();
    RefreshDefaultLayout();
}

void UReportEditorWidget::InitializeForSlice(ASignalGameFlowManager* InFlowManager, ARouteStateManager* InRouteStateManager)
{
    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;

    BuildOptionList();
    RefreshSubmitState();
}

void UReportEditorWidget::BuildOptionList()
{
    DisplayedBaseRows.Reset();
    DisplayedInjectedRows.Reset();
    TMap<FName, FST_ReportSentenceRow> AuthoredInjectionRowsById;

    if (ReportSentencesTable)
    {
        TArray<FST_ReportSentenceRow*> Rows;
        ReportSentencesTable->GetAllRows(TEXT("BuildOptionList"), Rows);

        const int32 ActiveDay = FlowManagerRef ? FlowManagerRef->CurrentDayIndex : 1;
        for (const FST_ReportSentenceRow* Row : Rows)
        {
            if (!Row || (Row->DayIndex != ActiveDay && Row->DayIndex != 0))
            {
                continue;
            }

            if (Row->bIsFinalInjection)
            {
                AuthoredInjectionRowsById.Add(Row->SentenceId, *Row);
                continue;
            }

            DisplayedBaseRows.Add(*Row);
        }
    }

    if (DisplayedBaseRows.IsEmpty())
    {
        BuildFallbackBaseSentences(DisplayedBaseRows);
    }

    if (RouteStateManagerRef)
    {
        DisplayedInjectedRows = RouteStateManagerRef->GetCollectedInjectionSentences();

        for (FST_ReportSentenceRow& InjectedRow : DisplayedInjectedRows)
        {
            if (const FST_ReportSentenceRow* AuthoredRow = AuthoredInjectionRowsById.Find(InjectedRow.SentenceId))
            {
                InjectedRow = *AuthoredRow;
            }
        }
    }

    SelectedInjectedSentenceIds.RemoveAll([this](const FName SentenceId)
    {
        for (const FST_ReportSentenceRow& Row : DisplayedInjectedRows)
        {
            if (Row.SentenceId == SentenceId)
            {
                return false;
            }
        }

        return true;
    });

    if (DisplayedInjectedRows.Num() > 0 && SelectedInjectedSentenceIds.IsEmpty())
    {
        for (const FST_ReportSentenceRow& Row : DisplayedInjectedRows)
        {
            SelectedInjectedSentenceIds.Add(Row.SentenceId);
        }
    }

    if (!HasBaseSentence(SelectedSentenceId))
    {
        SelectedSentenceId = DisplayedBaseRows[0].SentenceId;
    }

    BP_OnReportOptionsBuilt(DisplayedBaseRows);
    BP_OnInjectedClausesBuilt(DisplayedInjectedRows);
    RefreshDefaultLayout();
    RefreshSubmitState();
}

void UReportEditorWidget::HandleSentenceSelected(FName SentenceId)
{
    SelectedSentenceId = SentenceId;
    RefreshDefaultLayout();
    RefreshSubmitState();
}

void UReportEditorWidget::HandleInjectionSentenceToggled(FName SentenceId, bool bSelected)
{
    if (SentenceId == NAME_None)
    {
        return;
    }

    if (bSelected)
    {
        SelectedInjectedSentenceIds.AddUnique(SentenceId);
    }
    else
    {
        SelectedInjectedSentenceIds.Remove(SentenceId);
    }

    RefreshDefaultLayout();
    RefreshSubmitState();
}

void UReportEditorWidget::HandleSubmitClicked()
{
    RefreshSubmitState();
    if (!bCanSubmitState || !FlowManagerRef)
    {
        return;
    }

    FlowManagerRef->SubmitStructuredReport(BuildSubmissionPayload());
}

void UReportEditorWidget::RefreshSubmitState()
{
    const bool bHasSentence = SelectedSentenceId != NAME_None && HasBaseSentence(SelectedSentenceId);
    const bool bFlowAllows = FlowManagerRef && FlowManagerRef->CanOpenReport();
    const bool bHasRouteState = RouteStateManagerRef && RouteStateManagerRef->LastRouteChoice != E_RouteBranch::Neutral;

    bCanSubmitState = bHasSentence && bFlowAllows && bHasRouteState;
    BP_OnSubmitStateChanged(bCanSubmitState);
    RefreshDefaultLayout();
}

void UReportEditorWidget::HandleFallbackSentenceSelectionChanged(FString SelectedItem, ESelectInfo::Type /*SelectionType*/)
{
    if (const FName* FoundSentenceId = FallbackSentenceIdsByLabel.Find(SelectedItem))
    {
        HandleSentenceSelected(*FoundSentenceId);
    }
}

void UReportEditorWidget::EnsureDefaultLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ReportRoot"));
    RootBorder->SetPadding(FMargin(20.0f));
    RootBorder->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.10f, 0.95f));
    WidgetTree->RootWidget = RootBorder;

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ReportLayout"));
    RootBorder->SetContent(Layout);

    HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ReportHeaderText"));
    HeaderText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UVerticalBoxSlot* HeaderSlot = Layout->AddChildToVerticalBox(HeaderText))
    {
        HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    SentenceComboBox = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("ReportSentenceComboBox"));
    SentenceComboBox->OnSelectionChanged.AddDynamic(this, &UReportEditorWidget::HandleFallbackSentenceSelectionChanged);
    if (UVerticalBoxSlot* ComboSlot = Layout->AddChildToVerticalBox(SentenceComboBox))
    {
        ComboSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
    }

    InjectionAreaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ReportInjectionAreaText"));
    InjectionAreaText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.93f, 1.0f, 1.0f)));
    if (UVerticalBoxSlot* InjectionSlot = Layout->AddChildToVerticalBox(InjectionAreaText))
    {
        InjectionSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
    }

    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ReportStatusText"));
    StatusText->SetColorAndOpacity(FSlateColor(FLinearColor(0.72f, 0.82f, 0.96f, 1.0f)));
    if (UVerticalBoxSlot* StatusSlot = Layout->AddChildToVerticalBox(StatusText))
    {
        StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
    }

    SubmitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ReportSubmitButton"));
    UTextBlock* SubmitLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ReportSubmitLabel"));
    SubmitLabel->SetText(LOCTEXT("SubmitReportLabel", "Submit Report"));
    SubmitLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    SubmitButton->SetContent(SubmitLabel);
    SubmitButton->OnClicked.AddDynamic(this, &UReportEditorWidget::HandleSubmitClicked);
    Layout->AddChildToVerticalBox(SubmitButton);
}

void UReportEditorWidget::RefreshDefaultLayout()
{
    if (HeaderText)
    {
        HeaderText->SetText(FText::Format(
            LOCTEXT("ReportHeaderFormat", "OA Daily Report | Base clauses: {0} | Injected lines: {1}"),
            FText::AsNumber(DisplayedBaseRows.Num()),
            FText::AsNumber(DisplayedInjectedRows.Num())));
    }

    if (SentenceComboBox)
    {
        SentenceComboBox->OnSelectionChanged.RemoveDynamic(this, &UReportEditorWidget::HandleFallbackSentenceSelectionChanged);
        SentenceComboBox->ClearOptions();
        FallbackSentenceIdsByLabel.Reset();

        FString SelectedLabel;
        for (const FST_ReportSentenceRow& Row : DisplayedBaseRows)
        {
            const FString Label = Row.Text.IsEmpty() ? Row.SentenceId.ToString() : Row.Text.ToString();
            SentenceComboBox->AddOption(Label);
            FallbackSentenceIdsByLabel.Add(Label, Row.SentenceId);
            if (Row.SentenceId == SelectedSentenceId)
            {
                SelectedLabel = Label;
            }
        }

        if (!SelectedLabel.IsEmpty())
        {
            SentenceComboBox->SetSelectedOption(SelectedLabel);
        }
        SentenceComboBox->OnSelectionChanged.AddDynamic(this, &UReportEditorWidget::HandleFallbackSentenceSelectionChanged);
    }

    if (InjectionAreaText)
    {
        if (DisplayedInjectedRows.IsEmpty())
        {
            InjectionAreaText->SetText(LOCTEXT("NoInjectedLinesText", "注入代码区：尚未收集到可注入语句。"));
        }
        else
        {
            FString InjectionList = TEXT("注入代码区：\n");
            for (const FST_ReportSentenceRow& Row : DisplayedInjectedRows)
            {
                const bool bSelected = SelectedInjectedSentenceIds.Contains(Row.SentenceId);
                InjectionList += FString::Printf(TEXT("%s %s\n"), bSelected ? TEXT("[auto]") : TEXT("[off]"), *Row.Text.ToString());
            }
            InjectionAreaText->SetText(FText::FromString(InjectionList));
        }
    }

    if (StatusText)
    {
        FText OutcomeHint;
        if (SelectedInjectedSentenceIds.Num() >= 3 && SelectedSentenceId == FName(TEXT("REPORT_FORCE_ROLLOUT")))
        {
            OutcomeHint = LOCTEXT("CascadeOutcomeHint", "Rollout conclusion plus three injected lines selected: full launch-collapse ending pressure is primed.");
        }
        else if (SelectedInjectedSentenceIds.Num() >= 3)
        {
            OutcomeHint = LOCTEXT("NeedRolloutOutcomeHint", "Three injected lines are ready, but the stronger rollout conclusion is not selected yet.");
        }
        else if (SelectedInjectedSentenceIds.Num() > 0)
        {
            OutcomeHint = LOCTEXT("PartialOutcomeHint", "Some injected lines selected: the report will destabilize the rollout, but not completely.");
        }
        else
        {
            OutcomeHint = LOCTEXT("RoutineOutcomeHint", "No injected lines selected: the report will behave like a routine warning submission.");
        }

        StatusText->SetText(OutcomeHint);
    }

    if (SubmitButton)
    {
        SubmitButton->SetIsEnabled(bCanSubmitState);
    }
}

FST_ReportSubmissionPayload UReportEditorWidget::BuildSubmissionPayload() const
{
    FST_ReportSubmissionPayload Payload;

    if (const FST_ReportSentenceRow* BaseRow = FindBaseSentence(SelectedSentenceId))
    {
        Payload.SelectedBaseSentenceId = BaseRow->SentenceId;
        Payload.SelectedBaseSentenceText = BaseRow->Text;
        Payload.TotalStrengthScore += BaseRow->StrengthScore;
    }

    for (const FST_ReportSentenceRow& InjectedRow : DisplayedInjectedRows)
    {
        if (!SelectedInjectedSentenceIds.Contains(InjectedRow.SentenceId))
        {
            continue;
        }

        Payload.SelectedInjectedSentenceIds.Add(InjectedRow.SentenceId);
        Payload.SelectedInjectedRows.Add(InjectedRow);
        Payload.TotalStrengthScore += InjectedRow.StrengthScore;
    }

    return Payload;
}

bool UReportEditorWidget::HasBaseSentence(FName SentenceId) const
{
    return FindBaseSentence(SentenceId) != nullptr;
}

const FST_ReportSentenceRow* UReportEditorWidget::FindBaseSentence(FName SentenceId) const
{
    for (const FST_ReportSentenceRow& Row : DisplayedBaseRows)
    {
        if (Row.SentenceId == SentenceId)
        {
            return &Row;
        }
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
