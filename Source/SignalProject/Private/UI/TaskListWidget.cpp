#include "UI/TaskListWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#define LOCTEXT_NAMESPACE "TaskListWidget"

TSharedRef<SWidget> UTaskListWidget::RebuildWidget()
{
    EnsureDefaultLayout();
    return Super::RebuildWidget();
}

void UTaskListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EnsureDefaultLayout();
    RefreshDefaultLayout();
    BP_OnTaskListChanged(CurrentDayTitle, CurrentTasks);
}

void UTaskListWidget::ApplyTaskListState(const FText& InDayTitle, const TArray<FST_DesktopTaskRecord>& InTasks)
{
    CurrentDayTitle = InDayTitle;
    CurrentTasks = InTasks;

    EnsureDefaultLayout();
    RefreshDefaultLayout();
    BP_OnTaskListChanged(CurrentDayTitle, CurrentTasks);
}

void UTaskListWidget::EnsureDefaultLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("NativeTaskListRoot"));
    RootBorder->SetPadding(FMargin(20.0f));
    RootBorder->SetBrushColor(FLinearColor(0.08f, 0.09f, 0.12f, 0.96f));
    WidgetTree->RootWidget = RootBorder;

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("NativeTaskListLayout"));
    RootBorder->SetContent(Layout);

    DayTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NativeTaskDayTitle"));
    DayTitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UVerticalBoxSlot* TitleSlot = Layout->AddChildToVerticalBox(DayTitleText))
    {
        TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
    }

    TaskListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("NativeTaskListBox"));
    Layout->AddChildToVerticalBox(TaskListBox);
}

void UTaskListWidget::RefreshDefaultLayout()
{
    if (DayTitleText)
    {
        DayTitleText->SetText(CurrentDayTitle.IsEmpty() ? LOCTEXT("FallbackDayTitle", "Day 1") : CurrentDayTitle);
    }

    if (!TaskListBox)
    {
        return;
    }

    TaskListBox->ClearChildren();

    for (const FST_DesktopTaskRecord& Task : CurrentTasks)
    {
        UTextBlock* TaskLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        const FString StatusPrefix = Task.bIsCompleted ? TEXT("[x] ") : TEXT("[ ] ");
        TaskLine->SetText(FText::FromString(StatusPrefix + Task.TaskText.ToString()));
        TaskLine->SetColorAndOpacity(FSlateColor(Task.bIsCompleted ? FLinearColor(0.65f, 1.0f, 0.72f, 1.0f) : FLinearColor(0.86f, 0.90f, 0.96f, 1.0f)));

        if (UVerticalBoxSlot* TaskSlot = TaskListBox->AddChildToVerticalBox(TaskLine))
        {
            TaskSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
        }
    }
}

#undef LOCTEXT_NAMESPACE
