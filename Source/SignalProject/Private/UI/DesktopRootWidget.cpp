#include "UI/DesktopRootWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Minigames/MinigameManager.h"
#include "Report/ReportEditorWidget.h"
#include "UI/ChatAppWidget.h"

void UDesktopRootWidget::NativeConstruct()
{
    Super::NativeConstruct();
    EnsureDebugShell();
    RefreshDebugStatus();
}

void UDesktopRootWidget::InitializeForSlice(
    ASignalGameFlowManager* InFlowManager,
    ARouteStateManager* InRouteStateManager,
    AChatConversationManager* InChatConversationManager,
    AMinigameManager* InMinigameManager)
{
    if (FlowManagerRef)
    {
        FlowManagerRef->OnPhaseChanged.RemoveDynamic(this, &UDesktopRootWidget::HandlePhaseChanged);
    }

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->OnMessagesUpdated.RemoveDynamic(this, &UDesktopRootWidget::HandleChatMessagesUpdated);
    }

    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;
    ChatConversationManagerRef = InChatConversationManager;
    MinigameManagerRef = InMinigameManager;

    if (FlowManagerRef)
    {
        FlowManagerRef->OnPhaseChanged.AddDynamic(this, &UDesktopRootWidget::HandlePhaseChanged);
    }

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->OnMessagesUpdated.AddDynamic(this, &UDesktopRootWidget::HandleChatMessagesUpdated);
    }

    if (MinigameManagerRef)
    {
        MinigameManagerRef->RegisterDesktopRoot(this);
    }

    RefreshAppAvailability();
    RefreshDebugStatus();
}

void UDesktopRootWidget::OpenApp(FName AppTag)
{
    if (AppTag == FName(TEXT("Chat")))
    {
        if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() != E_GamePhase::ChatActive)
        {
            FlowManagerRef->RequestPhaseChange(E_GamePhase::ChatActive);
            return;
        }

        if (!ChatAppWidgetInstance && ChatAppWidgetClass)
        {
            ChatAppWidgetInstance = CreateWidget<UChatAppWidget>(GetWorld(), ChatAppWidgetClass);
            if (ChatAppWidgetInstance)
            {
                ChatAppWidgetInstance->InitializeForSlice(ChatConversationManagerRef, RouteStateManagerRef);
            }
        }

        ShowAppWidget(ChatAppWidgetInstance);
        return;
    }

    if (AppTag == FName(TEXT("Report")))
    {
        if (!FlowManagerRef || !FlowManagerRef->CanOpenReport())
        {
            RefreshAppAvailability();
            return;
        }

        if (FlowManagerRef->GetCurrentPhase() != E_GamePhase::ReportPhase)
        {
            FlowManagerRef->RequestPhaseChange(E_GamePhase::ReportPhase);
            return;
        }

        if (!ReportEditorWidgetInstance && ReportEditorWidgetClass)
        {
            ReportEditorWidgetInstance = CreateWidget<UReportEditorWidget>(GetWorld(), ReportEditorWidgetClass);
            if (ReportEditorWidgetInstance)
            {
                ReportEditorWidgetInstance->InitializeForSlice(FlowManagerRef, RouteStateManagerRef);
            }
        }

        ShowAppWidget(ReportEditorWidgetInstance);
    }
}

void UDesktopRootWidget::ShowAppWidget(UUserWidget* AppWidget)
{
    ActiveAppWidget = AppWidget;
    RefreshDebugStatus();
    BP_OnAppWidgetRequested(AppWidget);
}

void UDesktopRootWidget::RefreshAppAvailability()
{
    const bool bCanOpenReport = FlowManagerRef && FlowManagerRef->CanOpenReport();
    if (DebugReportButton)
    {
        DebugReportButton->SetIsEnabled(bCanOpenReport);
    }
    RefreshDebugStatus();
    BP_OnAppAvailabilityChanged(bCanOpenReport);
}

void UDesktopRootWidget::HandlePhaseChanged(E_GamePhase /*PreviousPhase*/, E_GamePhase /*NewPhase*/)
{
    RefreshAppAvailability();
}

void UDesktopRootWidget::HandleChatMessagesUpdated()
{
    RefreshAppAvailability();
}

void UDesktopRootWidget::HandleDebugOpenChatClicked()
{
    OpenApp(FName(TEXT("Chat")));
}

void UDesktopRootWidget::HandleDebugOpenReportClicked()
{
    OpenApp(FName(TEXT("Report")));
}

bool UDesktopRootWidget::EnsureDebugShell()
{
    if (!WidgetTree)
    {
        return false;
    }

    UWidget* ExistingRoot = WidgetTree->RootWidget;
    if (!DebugRootOverlay)
    {
        const bool bAlreadyUsingDebugOverlay =
            ExistingRoot &&
            ExistingRoot->GetFName() == TEXT("DesktopDebugOverlayRoot") &&
            ExistingRoot->IsA(UOverlay::StaticClass());

        if (bAlreadyUsingDebugOverlay)
        {
            DebugRootOverlay = Cast<UOverlay>(ExistingRoot);
        }
        else
        {
            DebugRootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("DesktopDebugOverlayRoot"));
            WidgetTree->RootWidget = DebugRootOverlay;

            if (ExistingRoot)
            {
                if (UOverlaySlot* ContentSlot = DebugRootOverlay->AddChildToOverlay(ExistingRoot))
                {
                    ContentSlot->SetHorizontalAlignment(HAlign_Fill);
                    ContentSlot->SetVerticalAlignment(VAlign_Fill);
                }
            }

            UE_LOG(
                LogTemp,
                Display,
                TEXT("Desktop debug overlay attached (preserved existing root: %s)."),
                ExistingRoot ? TEXT("true") : TEXT("false"));
        }
    }

    if (!DebugRootOverlay || DebugPhaseText || DebugAppText)
    {
        return DebugRootOverlay != nullptr;
    }

    UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DebugDesktopPanel"));
    Panel->SetPadding(FMargin(16.0f));
    Panel->SetBrushColor(FLinearColor(0.05f, 0.08f, 0.12f, 0.88f));
    if (UOverlaySlot* PanelSlot = DebugRootOverlay->AddChildToOverlay(Panel))
    {
        PanelSlot->SetHorizontalAlignment(HAlign_Left);
        PanelSlot->SetVerticalAlignment(VAlign_Top);
        PanelSlot->SetPadding(FMargin(24.0f, 24.0f, 0.0f, 0.0f));
    }

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DebugDesktopLayout"));
    Panel->SetContent(Layout);

    DebugPhaseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugPhaseText"));
    DebugPhaseText->SetText(FText::FromString(TEXT("Desktop Debug Shell")));
    DebugPhaseText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    Layout->AddChildToVerticalBox(DebugPhaseText);

    DebugAppText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugAppText"));
    DebugAppText->SetColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.85f, 1.0f, 1.0f)));
    if (UVerticalBoxSlot* AppSlot = Layout->AddChildToVerticalBox(DebugAppText))
    {
        AppSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 12.0f));
    }

    UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("DebugButtonRow"));
    Layout->AddChildToVerticalBox(ButtonRow);

    DebugChatButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugChatButton"));
    UTextBlock* ChatLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugChatLabel"));
    ChatLabel->SetText(FText::FromString(TEXT("Open Chat")));
    ChatLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugChatButton->SetContent(ChatLabel);
    DebugChatButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleDebugOpenChatClicked);
    if (UHorizontalBoxSlot* ChatSlot = ButtonRow->AddChildToHorizontalBox(DebugChatButton))
    {
        ChatSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    }

    DebugReportButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugReportButton"));
    UTextBlock* ReportLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugReportLabel"));
    ReportLabel->SetText(FText::FromString(TEXT("Open Report")));
    ReportLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugReportButton->SetContent(ReportLabel);
    DebugReportButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleDebugOpenReportClicked);
    ButtonRow->AddChildToHorizontalBox(DebugReportButton);

    return true;
}

void UDesktopRootWidget::RefreshDebugStatus()
{
    if (!DebugPhaseText || !DebugAppText)
    {
        return;
    }

    const UEnum* PhaseEnum = StaticEnum<E_GamePhase>();
    const FString PhaseName =
        FlowManagerRef && PhaseEnum
            ? PhaseEnum->GetNameStringByValue(static_cast<int64>(FlowManagerRef->GetCurrentPhase()))
            : TEXT("Uninitialized");

    DebugPhaseText->SetText(FText::FromString(FString::Printf(TEXT("Desktop Debug Shell\nPhase: %s"), *PhaseName)));

    FString AppText = TEXT("Active App: None");
    if (ActiveAppWidget)
    {
        AppText = FString::Printf(TEXT("Active App Widget: %s"), *GetNameSafe(ActiveAppWidget));
    }
    else if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() == E_GamePhase::ChatActive)
    {
        AppText = TEXT("Active App Widget: Chat requested (widget may still be visually empty)");
    }
    else if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() == E_GamePhase::ReportPhase)
    {
        AppText = TEXT("Active App Widget: Report requested (widget may still be visually empty)");
    }

    DebugAppText->SetText(FText::FromString(AppText));
}
