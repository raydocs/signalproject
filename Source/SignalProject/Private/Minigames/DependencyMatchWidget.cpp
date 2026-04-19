#include "Minigames/DependencyMatchWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Minigames/MinigameManager.h"

#define LOCTEXT_NAMESPACE "DependencyMatchWidget"

TSharedRef<SWidget> UDependencyMatchWidget::RebuildWidget()
{
    EnsureDefaultLayout();
    return Super::RebuildWidget();
}

void UDependencyMatchWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EnsureDefaultLayout();
    RefreshDefaultLayout();
    BP_OnPauseChanged(bIsPaused);
    BP_OnStressStateChanged(CurrentStressState);
}

void UDependencyMatchWidget::InitializeForSlice(AMinigameManager* InMinigameManager)
{
    MinigameManagerRef = InMinigameManager;
    RefreshDefaultLayout();
}

void UDependencyMatchWidget::BuildGrid()
{
    RefreshDefaultLayout();
    BP_OnGridBuildRequested();
}

void UDependencyMatchWidget::HandleTileClicked(int32 /*TileId*/)
{
}

void UDependencyMatchWidget::SetPaused(bool bInPaused)
{
    bIsPaused = bInPaused;
    CurrentStressState.bIsPaused = bIsPaused;
    RefreshDefaultLayout();
    BP_OnPauseChanged(bIsPaused);
}

void UDependencyMatchWidget::ApplyStressState(const FST_StressTestViewState& InState)
{
    CurrentStressState = InState;
    bIsPaused = CurrentStressState.bIsPaused;
    EnsureDefaultLayout();
    RefreshDefaultLayout();
    BP_OnStressStateChanged(CurrentStressState);
}

void UDependencyMatchWidget::HandleSingleDrawClicked()
{
    if (!MinigameManagerRef)
    {
        return;
    }

    MinigameManagerRef->HandleSingleDraw();
}

void UDependencyMatchWidget::HandleTenDrawClicked()
{
    if (!MinigameManagerRef)
    {
        return;
    }

    MinigameManagerRef->HandleTenDraw();
}

void UDependencyMatchWidget::HandleSingleDrawButtonPressed()
{
    HandleSingleDrawClicked();
}

void UDependencyMatchWidget::HandleTenDrawButtonPressed()
{
    HandleTenDrawClicked();
}

void UDependencyMatchWidget::EnsureDefaultLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StressTestRoot"));
    RootBorder->SetPadding(FMargin(20.0f));
    RootBorder->SetBrushColor(FLinearColor(0.07f, 0.08f, 0.10f, 0.96f));
    WidgetTree->RootWidget = RootBorder;

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StressTestLayout"));
    RootBorder->SetContent(Layout);

    HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StressTestHeader"));
    HeaderText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UVerticalBoxSlot* HeaderSlot = Layout->AddChildToVerticalBox(HeaderText))
    {
        HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    OutputTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StressTestOutput"));
    OutputTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.90f, 0.96f, 1.0f)));
    if (UVerticalBoxSlot* OutputSlot = Layout->AddChildToVerticalBox(OutputTextBlock))
    {
        OutputSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    StatusTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StressTestStatus"));
    StatusTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.67f, 0.84f, 1.0f, 1.0f)));
    if (UVerticalBoxSlot* StatusSlot = Layout->AddChildToVerticalBox(StatusTextBlock))
    {
        StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
    }

    SingleDrawButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SingleDrawButton"));
    UTextBlock* SingleLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SingleDrawLabel"));
    SingleLabel->SetText(LOCTEXT("SingleDrawLabel", "Single Draw"));
    SingleLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    SingleDrawButton->SetContent(SingleLabel);
    SingleDrawButton->OnClicked.AddDynamic(this, &UDependencyMatchWidget::HandleSingleDrawButtonPressed);
    if (UVerticalBoxSlot* SingleSlot = Layout->AddChildToVerticalBox(SingleDrawButton))
    {
        SingleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    TenDrawButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("TenDrawButton"));
    UTextBlock* TenLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TenDrawLabel"));
    TenLabel->SetText(LOCTEXT("TenDrawLabel", "Ten Draw"));
    TenLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    TenDrawButton->SetContent(TenLabel);
    TenDrawButton->OnClicked.AddDynamic(this, &UDependencyMatchWidget::HandleTenDrawButtonPressed);
    Layout->AddChildToVerticalBox(TenDrawButton);
}

void UDependencyMatchWidget::RefreshDefaultLayout()
{
    if (HeaderText)
    {
        HeaderText->SetText(FText::Format(
            LOCTEXT("StressHeaderFormat", "Pressure Test Console | Draws: {0} | Points: {1} | Coins: {2}"),
            FText::AsNumber(CurrentStressState.TotalDraws),
            FText::AsNumber(CurrentStressState.StressPoints),
            FText::AsNumber(CurrentStressState.RemainingCurrency)));
    }

    if (OutputTextBlock)
    {
        OutputTextBlock->SetText(CurrentStressState.OutputText.IsEmpty() ? LOCTEXT("StressOutputFallback", "Latest draw output will appear here.") : CurrentStressState.OutputText);
    }

    if (StatusTextBlock)
    {
        StatusTextBlock->SetText(CurrentStressState.StatusText.IsEmpty() ? LOCTEXT("StressStatusFallback", "Pressure test idle.") : CurrentStressState.StatusText);
    }

    if (SingleDrawButton)
    {
        SingleDrawButton->SetIsEnabled(CurrentStressState.bCanSingleDraw);
    }

    if (TenDrawButton)
    {
        TenDrawButton->SetIsEnabled(CurrentStressState.bCanTenDraw);
    }
}

#undef LOCTEXT_NAMESPACE
