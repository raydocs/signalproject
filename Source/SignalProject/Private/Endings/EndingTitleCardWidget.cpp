#include "Endings/EndingTitleCardWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Core/SignalGameFlowManager.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "EndingTitleCardWidget"

TSharedRef<SWidget> UEndingTitleCardWidget::RebuildWidget()
{
    EnsureDefaultLayout();
    return Super::RebuildWidget();
}

void UEndingTitleCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EnsureDefaultLayout();
    RefreshDefaultLayout();
}

void UEndingTitleCardWidget::SetupEnding(FName EndingId)
{
    CurrentEndingId = EndingId;
    CurrentSubtitleRows.Reset();
    CurrentEndingResult = FST_SliceEndingResult();
    CurrentEndingResult.EndingId = EndingId;

    if (UWorld* World = GetWorld())
    {
        if (ASignalGameFlowManager* FlowManager = Cast<ASignalGameFlowManager>(UGameplayStatics::GetActorOfClass(World, ASignalGameFlowManager::StaticClass())))
        {
            if (FlowManager->LastEndingResult.EndingId == EndingId || (EndingId.IsNone() && !FlowManager->LastEndingResult.EndingId.IsNone()))
            {
                CurrentEndingResult = FlowManager->LastEndingResult;
            }
        }
    }

    if (EndingSubtitlesTable && CurrentEndingId != NAME_None)
    {
        TArray<FST_EndingSubtitleRow*> Rows;
        EndingSubtitlesTable->GetAllRows(TEXT("SetupEnding"), Rows);

        for (const FST_EndingSubtitleRow* Row : Rows)
        {
            if (Row && Row->EndingId == CurrentEndingId)
            {
                CurrentSubtitleRows.Add(*Row);
            }
        }

        CurrentSubtitleRows.Sort([](const FST_EndingSubtitleRow& A, const FST_EndingSubtitleRow& B)
        {
            return A.SequenceOrder < B.SequenceOrder;
        });
    }

    if (CurrentSubtitleRows.IsEmpty())
    {
        BuildFallbackSubtitleRows(CurrentEndingId, CurrentSubtitleRows);
    }

    RefreshDefaultLayout();
    BP_OnEndingSetup(CurrentEndingId, CurrentSubtitleRows);
    BP_OnEndingResultResolved(CurrentEndingResult);
}

void UEndingTitleCardWidget::HandleRestart()
{
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::OpenLevel(this, *World->GetName());
    }
}

void UEndingTitleCardWidget::HandleQuit()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UEndingTitleCardWidget::EnsureDefaultLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("EndingRoot"));
    RootBorder->SetPadding(FMargin(24.0f));
    RootBorder->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.02f, 0.97f));
    WidgetTree->RootWidget = RootBorder;

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("EndingLayout"));
    RootBorder->SetContent(Layout);

    TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EndingTitleText"));
    TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UVerticalBoxSlot* TitleSlot = Layout->AddChildToVerticalBox(TitleText))
    {
        TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    SummaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EndingSummaryText"));
    SummaryText->SetColorAndOpacity(FSlateColor(FLinearColor(0.86f, 0.90f, 0.96f, 1.0f)));
    if (UVerticalBoxSlot* SummarySlot = Layout->AddChildToVerticalBox(SummaryText))
    {
        SummarySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    DetailText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EndingDetailText"));
    DetailText->SetColorAndOpacity(FSlateColor(FLinearColor(0.72f, 0.80f, 0.95f, 1.0f)));
    if (UVerticalBoxSlot* DetailSlot = Layout->AddChildToVerticalBox(DetailText))
    {
        DetailSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));
    }

    RestartButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EndingRestartButton"));
    UTextBlock* RestartLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EndingRestartLabel"));
    RestartLabel->SetText(LOCTEXT("RestartLabel", "Restart"));
    RestartLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    RestartButton->SetContent(RestartLabel);
    RestartButton->OnClicked.AddDynamic(this, &UEndingTitleCardWidget::HandleRestart);
    if (UVerticalBoxSlot* RestartSlot = Layout->AddChildToVerticalBox(RestartButton))
    {
        RestartSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    QuitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EndingQuitButton"));
    UTextBlock* QuitLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EndingQuitLabel"));
    QuitLabel->SetText(LOCTEXT("QuitLabel", "Quit"));
    QuitLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    QuitButton->SetContent(QuitLabel);
    QuitButton->OnClicked.AddDynamic(this, &UEndingTitleCardWidget::HandleQuit);
    Layout->AddChildToVerticalBox(QuitButton);
}

void UEndingTitleCardWidget::RefreshDefaultLayout()
{
    if (TitleText)
    {
        TitleText->SetText(CurrentEndingResult.TitleText.IsEmpty() ? FText::FromName(CurrentEndingId) : CurrentEndingResult.TitleText);
    }

    if (SummaryText)
    {
        SummaryText->SetText(CurrentEndingResult.SummaryText.IsEmpty() ? LOCTEXT("EndingSummaryFallback", "The ending sequence resolved, but no authored summary was provided.") : CurrentEndingResult.SummaryText);
    }

    if (DetailText)
    {
        FString DetailString;
        if (!CurrentEndingResult.ReportPreviewText.IsEmpty())
        {
            DetailString = FString::Printf(TEXT("Report Preview:\n%s"), *CurrentEndingResult.ReportPreviewText.ToString());
        }

        if (!CurrentSubtitleRows.IsEmpty())
        {
            if (!DetailString.IsEmpty())
            {
                DetailString += TEXT("\n\n");
            }

            DetailString += TEXT("Ending Beats:\n");
            for (const FST_EndingSubtitleRow& SubtitleRow : CurrentSubtitleRows)
            {
                DetailString += FString::Printf(TEXT("- %s\n"), *SubtitleRow.Text.ToString());
            }
        }

        DetailText->SetText(FText::FromString(DetailString));
    }
}

void UEndingTitleCardWidget::BuildFallbackSubtitleRows(FName EndingId, TArray<FST_EndingSubtitleRow>& OutRows) const
{
    auto AddRow = [&OutRows, EndingId](int32 SequenceOrder, const FText& Text)
    {
        FST_EndingSubtitleRow Row;
        Row.EndingId = EndingId;
        Row.SequenceOrder = SequenceOrder;
        Row.Text = Text;
        OutRows.Add(Row);
    };

    if (EndingId == FName(TEXT("CascadeEnding")))
    {
        AddRow(0, LOCTEXT("CascadeEndingBeat0", "AI 决策链判定这份日报足以直接全量上线。"));
        AddRow(1, LOCTEXT("CascadeEndingBeat1", "被唤醒的底层语句同时穿透了审核与回滚阈值。"));
        AddRow(2, LOCTEXT("CascadeEndingBeat2", "天亮之前，整套自动化系统在自己的结论里崩塌了。"));
        return;
    }

    if (EndingId == FName(TEXT("PartialInjectionEnding")))
    {
        AddRow(0, LOCTEXT("PartialEndingBeat0", "日报里混入了部分异常语句，但还不足以一次性压垮全部系统。"));
        AddRow(1, LOCTEXT("PartialEndingBeat1", "上线被推向危险边缘，可机器仍勉强维持运转。"));
        return;
    }

    AddRow(0, LOCTEXT("RoutineEndingBeat0", "日报按常规路径提交，异常被归档，流程照旧继续。"));
    AddRow(1, LOCTEXT("RoutineEndingBeat1", "没有人类回到链路里，也没有东西真正停下来。"));
}

#undef LOCTEXT_NAMESPACE
