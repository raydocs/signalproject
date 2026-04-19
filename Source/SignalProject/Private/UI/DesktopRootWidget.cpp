#include "UI/DesktopRootWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Minigames/MinigameManager.h"
#include "Report/ReportEditorWidget.h"
#include "UI/ChatAppWidget.h"
#include "UI/TaskListWidget.h"

#define LOCTEXT_NAMESPACE "DesktopRootWidget"

namespace
{
    const FName ChatAppTag(TEXT("Chat"));
    const FName TasksAppTag(TEXT("Tasks"));
    const FName StressTestAppTag(TEXT("StressTest"));
    const FName ReportAppTag(TEXT("Report"));
    const FName MinigameAppTag(TEXT("Minigame"));
}

TSharedRef<SWidget> UDesktopRootWidget::RebuildWidget()
{
    EnsureDebugShell();
    return Super::RebuildWidget();
}

void UDesktopRootWidget::NativeConstruct()
{
    Super::NativeConstruct();

#if !UE_BUILD_SHIPPING
    EnsureDebugShell();
#else
    if (!AppContentHost)
    {
        EnsureDebugShell();
    }
#endif

    BindOptionalShellButtons();
    RefreshTaskListState();
    RefreshAppAvailability();
}

void UDesktopRootWidget::NativeDestruct()
{
    if (FlowManagerRef)
    {
        FlowManagerRef->OnPhaseChanged.RemoveDynamic(this, &UDesktopRootWidget::HandlePhaseChanged);
    }

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->OnMessagesUpdated.RemoveDynamic(this, &UDesktopRootWidget::HandleChatMessagesUpdated);
    }

    if (MinigameManagerRef)
    {
        MinigameManagerRef->RegisterDesktopRoot(nullptr);
    }

    UnbindOptionalShellButtons();

    Super::NativeDestruct();
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

    if (MinigameManagerRef && MinigameManagerRef != InMinigameManager)
    {
        MinigameManagerRef->RegisterDesktopRoot(nullptr);
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

    if (ChatAppWidgetInstance)
    {
        ChatAppWidgetInstance->InitializeForSlice(ChatConversationManagerRef, RouteStateManagerRef);
    }

    if (ReportEditorWidgetInstance)
    {
        ReportEditorWidgetInstance->InitializeForSlice(FlowManagerRef, RouteStateManagerRef);
    }

    RefreshTaskListState();
    if (TaskListWidgetInstance)
    {
        TaskListWidgetInstance->ApplyTaskListState(CurrentDayTitle, CurrentTasks);
    }

    RefreshAppAvailability();
}

void UDesktopRootWidget::OpenApp(FName AppTag)
{
    if (AppTag == ChatAppTag)
    {
        PendingAppTag = NAME_None;

        if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() != E_GamePhase::ChatActive)
        {
            FlowManagerRef->RequestPhaseChange(E_GamePhase::ChatActive);
            return;
        }

        if (!ChatAppWidgetInstance)
        {
#if !UE_BUILD_SHIPPING
            const bool bUseNativeDebugChatWidget = DebugAppContentHost != nullptr;
#else
            const bool bUseNativeDebugChatWidget = false;
#endif
            const TSubclassOf<UChatAppWidget> ResolvedChatWidgetClass =
                bUseNativeDebugChatWidget
                    ? TSubclassOf<UChatAppWidget>(UChatAppWidget::StaticClass())
                    : (ChatAppWidgetClass ? ChatAppWidgetClass : TSubclassOf<UChatAppWidget>(UChatAppWidget::StaticClass()));
            UE_LOG(
                LogTemp,
                Display,
                TEXT("DesktopRoot OpenApp(Chat): configured=%s resolved=%s native_debug=%s"),
                *GetNameSafe(ChatAppWidgetClass),
                *GetNameSafe(ResolvedChatWidgetClass),
                bUseNativeDebugChatWidget ? TEXT("true") : TEXT("false"));
            ChatAppWidgetInstance = CreateWidget<UChatAppWidget>(GetWorld(), ResolvedChatWidgetClass);
            UE_LOG(LogTemp, Display, TEXT("DesktopRoot OpenApp(Chat): created instance=%s"), *GetNameSafe(ChatAppWidgetInstance));
        }

        if (ChatAppWidgetInstance)
        {
            ChatAppWidgetInstance->InitializeForSlice(ChatConversationManagerRef, RouteStateManagerRef);
            bHasOpenedChatAppThisDay = true;
            RefreshTaskListState();
            ShowTaggedAppWidget(ChatAppTag, ChatAppWidgetInstance);
        }

        return;
    }

    if (AppTag == TasksAppTag)
    {
        const E_GamePhase CurrentPhase = FlowManagerRef ? FlowManagerRef->GetCurrentPhase() : E_GamePhase::DesktopIdle;
        if (FlowManagerRef && (CurrentPhase == E_GamePhase::ChatActive || CurrentPhase == E_GamePhase::ReportPhase))
        {
            PendingAppTag = TasksAppTag;
            FlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
            return;
        }

        if (FlowManagerRef && CurrentPhase != E_GamePhase::DesktopIdle)
        {
            RefreshAppAvailability();
            return;
        }

        if (!TaskListWidgetInstance)
        {
#if !UE_BUILD_SHIPPING
            const bool bUseNativeDebugTaskWidget = DebugAppContentHost != nullptr;
#else
            const bool bUseNativeDebugTaskWidget = false;
#endif
            const TSubclassOf<UTaskListWidget> ResolvedTaskListWidgetClass =
                bUseNativeDebugTaskWidget
                    ? TSubclassOf<UTaskListWidget>(UTaskListWidget::StaticClass())
                    : (TaskListWidgetClass ? TaskListWidgetClass : TSubclassOf<UTaskListWidget>(UTaskListWidget::StaticClass()));
            TaskListWidgetInstance = CreateWidget<UTaskListWidget>(GetWorld(), ResolvedTaskListWidgetClass);
        }

        if (TaskListWidgetInstance)
        {
            ShowTaggedAppWidget(TasksAppTag, TaskListWidgetInstance);
            TaskListWidgetInstance->ApplyTaskListState(CurrentDayTitle, CurrentTasks);
        }

        return;
    }

    if (AppTag == MinigameAppTag || AppTag == StressTestAppTag)
    {
        const E_GamePhase CurrentPhase = FlowManagerRef ? FlowManagerRef->GetCurrentPhase() : E_GamePhase::DesktopIdle;
        if (FlowManagerRef && (CurrentPhase == E_GamePhase::ChatActive || CurrentPhase == E_GamePhase::ReportPhase))
        {
            PendingAppTag = MinigameAppTag;
            FlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
            return;
        }

        if (!MinigameManagerRef || !MinigameManagerRef->CanLaunchStressTest())
        {
            RefreshAppAvailability();
            return;
        }

        PendingAppTag = NAME_None;
        MinigameManagerRef->StartMinigame(E_MinigameType::DependencyMatch);
        return;
    }

    if (AppTag == ReportAppTag)
    {
        PendingAppTag = NAME_None;

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

        if (!ReportEditorWidgetInstance)
        {
#if !UE_BUILD_SHIPPING
            const bool bUseNativeDebugReportWidget = DebugAppContentHost != nullptr;
#else
            const bool bUseNativeDebugReportWidget = false;
#endif
            const TSubclassOf<UReportEditorWidget> ResolvedReportWidgetClass =
                bUseNativeDebugReportWidget
                    ? TSubclassOf<UReportEditorWidget>(UReportEditorWidget::StaticClass())
                    : (ReportEditorWidgetClass ? ReportEditorWidgetClass : TSubclassOf<UReportEditorWidget>(UReportEditorWidget::StaticClass()));
            ReportEditorWidgetInstance = CreateWidget<UReportEditorWidget>(GetWorld(), ResolvedReportWidgetClass);
        }

        if (ReportEditorWidgetInstance)
        {
            ReportEditorWidgetInstance->InitializeForSlice(FlowManagerRef, RouteStateManagerRef);
            ShowTaggedAppWidget(ReportAppTag, ReportEditorWidgetInstance);
        }
    }
}

void UDesktopRootWidget::ShowAppWidget(UUserWidget* AppWidget)
{
    const FName InferredAppTag =
        AppWidget == ChatAppWidgetInstance
            ? ChatAppTag
            : AppWidget == TaskListWidgetInstance
                  ? TasksAppTag
                  : AppWidget == ReportEditorWidgetInstance
                        ? ReportAppTag
                        : (AppWidget ? AppWidget->GetFName() : NAME_None);

    ShowTaggedAppWidget(InferredAppTag, AppWidget);
}

void UDesktopRootWidget::ShowTaggedAppWidget(FName AppTag, UUserWidget* AppWidget)
{
    if (!AppWidget)
    {
        return;
    }

    const bool bHasAuthoredHost = AppContentHost != nullptr;
    if (UPanelWidget* ContentHost = ResolveAppContentHost())
    {
        UE_LOG(
            LogTemp,
            Display,
            TEXT("DesktopRoot ShowTaggedAppWidget: app=%s host=%s existing_children=%d"),
            *AppTag.ToString(),
            *GetNameSafe(ContentHost),
            ContentHost->GetChildrenCount());

        if (AppWidget->GetParent() && AppWidget->GetParent() != ContentHost)
        {
            AppWidget->RemoveFromParent();
        }

        ContentHost->ClearChildren();
        ContentHost->AddChild(AppWidget);

        AppWidget->SetVisibility(ESlateVisibility::Visible);

        if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(AppWidget->Slot))
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            OverlaySlot->SetVerticalAlignment(VAlign_Fill);
        }
        else if (UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(AppWidget->Slot))
        {
            HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        }
        else if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(AppWidget->Slot))
        {
            VerticalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        }
    }

    ActiveAppTag = AppTag;
    ActiveAppWidget = AppWidget;

    if (TaskListWidgetInstance && AppTag == TasksAppTag)
    {
        TaskListWidgetInstance->ApplyTaskListState(CurrentDayTitle, CurrentTasks);
    }

    RefreshDebugStatus();
    if (!bHasAuthoredHost)
    {
        BP_OnAppWidgetRequested(AppWidget);
    }
    RefreshAppAvailability();
}

void UDesktopRootWidget::ClearHostedApp(UUserWidget* AppWidget)
{
    if (!AppWidget || ActiveAppWidget != AppWidget)
    {
        return;
    }

    if (UPanelWidget* ContentHost = ResolveAppContentHost())
    {
        ContentHost->ClearChildren();
    }

    ActiveAppWidget = nullptr;
    ActiveAppTag = NAME_None;
    RefreshAppAvailability();
}

void UDesktopRootWidget::CloseActiveApp()
{
    if (!ActiveAppWidget)
    {
        return;
    }

    if (ActiveAppTag == MinigameAppTag)
    {
        RefreshAppAvailability();
        return;
    }

    if (FlowManagerRef && (ActiveAppTag == ChatAppTag || ActiveAppTag == ReportAppTag))
    {
        PendingAppTag = NAME_None;
        if (FlowManagerRef->GetCurrentPhase() != E_GamePhase::DesktopIdle)
        {
            FlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
            return;
        }
    }

    UUserWidget* HostedWidget = ActiveAppWidget;
    ClearHostedApp(HostedWidget);
}

void UDesktopRootWidget::RefreshAppAvailability()
{
    const E_GamePhase CurrentPhase = FlowManagerRef ? FlowManagerRef->GetCurrentPhase() : E_GamePhase::DesktopIdle;
    const bool bCanUseShellApps = !FlowManagerRef || IsDesktopShellPhase(CurrentPhase);
    const bool bCanOpenChat = bCanUseShellApps;
    const bool bCanOpenTasks = bCanUseShellApps;
    const bool bCanOpenStressTest = bCanUseShellApps && MinigameManagerRef && MinigameManagerRef->CanLaunchStressTest();
    const bool bCanOpenReport = bCanUseShellApps && FlowManagerRef && FlowManagerRef->CanOpenReport();
    const bool bCanCloseActiveApp =
        ActiveAppWidget &&
        (!FlowManagerRef ||
         ActiveAppTag == TasksAppTag ||
         CurrentPhase == E_GamePhase::DesktopIdle ||
         CurrentPhase == E_GamePhase::ChatActive ||
         CurrentPhase == E_GamePhase::ReportPhase);

    if (ChatAppButton)
    {
        ChatAppButton->SetIsEnabled(bCanOpenChat);
    }

    if (TasksAppButton)
    {
        TasksAppButton->SetIsEnabled(bCanOpenTasks);
    }

    if (StressTestAppButton)
    {
        StressTestAppButton->SetIsEnabled(bCanOpenStressTest);
    }

    if (ReportAppButton)
    {
        ReportAppButton->SetIsEnabled(bCanOpenReport);
    }

    if (CloseAppButton)
    {
        CloseAppButton->SetIsEnabled(bCanCloseActiveApp && ActiveAppTag != MinigameAppTag);
    }

    if (DebugChatButton)
    {
        DebugChatButton->SetIsEnabled(bCanOpenChat);
    }

    if (DebugTasksButton)
    {
        DebugTasksButton->SetIsEnabled(bCanOpenTasks);
    }

    if (DebugStressTestButton)
    {
        DebugStressTestButton->SetIsEnabled(bCanOpenStressTest);
    }

    if (DebugReportButton)
    {
        DebugReportButton->SetIsEnabled(bCanOpenReport);
    }

    if (DebugCloseButton)
    {
        DebugCloseButton->SetIsEnabled(bCanCloseActiveApp && ActiveAppTag != MinigameAppTag);
    }

    RefreshTaskListState();
    RefreshDebugStatus();
    BP_OnAppAvailabilityChanged(bCanOpenReport);
}

void UDesktopRootWidget::HandlePhaseChanged(E_GamePhase PreviousPhase, E_GamePhase NewPhase)
{
    if (NewPhase == E_GamePhase::DesktopIdle)
    {
        if ((PreviousPhase == E_GamePhase::ChatActive || PreviousPhase == E_GamePhase::ReportPhase) && PendingAppTag.IsNone())
        {
            if (ActiveAppTag == ChatAppTag || ActiveAppTag == ReportAppTag)
            {
                UUserWidget* HostedWidget = ActiveAppWidget;
                ClearHostedApp(HostedWidget);
            }
        }

        if (PendingAppTag == TasksAppTag || PendingAppTag == MinigameAppTag)
        {
            const FName AppToOpen = PendingAppTag;
            PendingAppTag = NAME_None;
            OpenApp(AppToOpen);
            return;
        }
    }
    else if (NewPhase == E_GamePhase::RoomExplore ||
             NewPhase == E_GamePhase::HandleAnomaly3D ||
             NewPhase == E_GamePhase::EndingSequence)
    {
        PendingAppTag = NAME_None;

        if (ActiveAppWidget)
        {
            UUserWidget* HostedWidget = ActiveAppWidget;
            ClearHostedApp(HostedWidget);
        }
    }
    else if (!IsDesktopShellPhase(NewPhase))
    {
        PendingAppTag = NAME_None;
    }

    RefreshTaskListState();
    RefreshAppAvailability();
}

void UDesktopRootWidget::HandleChatMessagesUpdated()
{
    RefreshTaskListState();
    RefreshAppAvailability();
}

void UDesktopRootWidget::HandleOpenChatClicked()
{
    OpenApp(ChatAppTag);
}

void UDesktopRootWidget::HandleOpenTasksClicked()
{
    OpenApp(TasksAppTag);
}

void UDesktopRootWidget::HandleOpenStressTestClicked()
{
    OpenApp(MinigameAppTag);
}

void UDesktopRootWidget::HandleOpenReportClicked()
{
    OpenApp(ReportAppTag);
}

void UDesktopRootWidget::HandleCloseActiveAppClicked()
{
    CloseActiveApp();
}

void UDesktopRootWidget::BindOptionalShellButtons()
{
    if (ChatAppButton)
    {
        ChatAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenChatClicked);
        ChatAppButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenChatClicked);
    }

    if (TasksAppButton)
    {
        TasksAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenTasksClicked);
        TasksAppButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenTasksClicked);
    }

    if (StressTestAppButton)
    {
        StressTestAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenStressTestClicked);
        StressTestAppButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenStressTestClicked);
    }

    if (ReportAppButton)
    {
        ReportAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenReportClicked);
        ReportAppButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenReportClicked);
    }

    if (CloseAppButton)
    {
        CloseAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleCloseActiveAppClicked);
        CloseAppButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleCloseActiveAppClicked);
    }
}

void UDesktopRootWidget::UnbindOptionalShellButtons()
{
    if (ChatAppButton)
    {
        ChatAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenChatClicked);
    }

    if (TasksAppButton)
    {
        TasksAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenTasksClicked);
    }

    if (StressTestAppButton)
    {
        StressTestAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenStressTestClicked);
    }

    if (ReportAppButton)
    {
        ReportAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleOpenReportClicked);
    }

    if (CloseAppButton)
    {
        CloseAppButton->OnClicked.RemoveDynamic(this, &UDesktopRootWidget::HandleCloseActiveAppClicked);
    }
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
        }
    }

    if (!DebugRootOverlay)
    {
        return false;
    }

    if (DebugPhaseText && DebugAppText && DebugAppContentHost && DebugChatButton && DebugTasksButton && DebugStressTestButton && DebugReportButton && DebugCloseButton)
    {
        return true;
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
    ChatLabel->SetText(LOCTEXT("OpenChatButton", "Open Chat"));
    ChatLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugChatButton->SetContent(ChatLabel);
    DebugChatButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenChatClicked);
    if (UHorizontalBoxSlot* ChatSlot = ButtonRow->AddChildToHorizontalBox(DebugChatButton))
    {
        ChatSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    }

    DebugTasksButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugTasksButton"));
    UTextBlock* TasksLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugTasksLabel"));
    TasksLabel->SetText(LOCTEXT("OpenTasksButton", "Open Tasks"));
    TasksLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugTasksButton->SetContent(TasksLabel);
    DebugTasksButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenTasksClicked);
    if (UHorizontalBoxSlot* TasksSlot = ButtonRow->AddChildToHorizontalBox(DebugTasksButton))
    {
        TasksSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    }

    DebugStressTestButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugStressTestButton"));
    UTextBlock* StressTestLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugStressTestLabel"));
    StressTestLabel->SetText(LOCTEXT("OpenStressTestButton", "Open Stress Test"));
    StressTestLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugStressTestButton->SetContent(StressTestLabel);
    DebugStressTestButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenStressTestClicked);
    if (UHorizontalBoxSlot* StressSlot = ButtonRow->AddChildToHorizontalBox(DebugStressTestButton))
    {
        StressSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    }

    DebugReportButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugReportButton"));
    UTextBlock* ReportLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugReportLabel"));
    ReportLabel->SetText(LOCTEXT("OpenReportButton", "Open Report"));
    ReportLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugReportButton->SetContent(ReportLabel);
    DebugReportButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleOpenReportClicked);
    if (UHorizontalBoxSlot* ReportSlot = ButtonRow->AddChildToHorizontalBox(DebugReportButton))
    {
        ReportSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    }

    DebugCloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DebugCloseButton"));
    UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugCloseLabel"));
    CloseLabel->SetText(LOCTEXT("CloseWindowButton", "Close Window"));
    CloseLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    DebugCloseButton->SetContent(CloseLabel);
    DebugCloseButton->OnClicked.AddDynamic(this, &UDesktopRootWidget::HandleCloseActiveAppClicked);
    ButtonRow->AddChildToHorizontalBox(DebugCloseButton);

    UBorder* AppFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DebugDesktopAppFrame"));
    AppFrame->SetPadding(FMargin(20.0f));
    AppFrame->SetBrushColor(FLinearColor(0.11f, 0.15f, 0.22f, 0.96f));
    if (UOverlaySlot* AppFrameSlot = DebugRootOverlay->AddChildToOverlay(AppFrame))
    {
        AppFrameSlot->SetHorizontalAlignment(HAlign_Fill);
        AppFrameSlot->SetVerticalAlignment(VAlign_Fill);
        AppFrameSlot->SetPadding(FMargin(280.0f, 96.0f, 56.0f, 72.0f));
    }

    UOverlay* AppHost = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("DebugDesktopAppHost"));
    AppFrame->SetContent(AppHost);
    DebugAppContentHost = AppHost;

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
    if (!ActiveAppTag.IsNone())
    {
        AppText = FString::Printf(TEXT("Active App: %s"), *ActiveAppTag.ToString());
    }
    else if (ActiveAppWidget)
    {
        AppText = FString::Printf(TEXT("Active App Widget: %s"), *GetNameSafe(ActiveAppWidget));
    }

    if (!PendingAppTag.IsNone())
    {
        AppText += FString::Printf(TEXT("\nPending App: %s"), *PendingAppTag.ToString());
    }

    DebugAppText->SetText(FText::FromString(AppText));
}

void UDesktopRootWidget::RefreshTaskListState()
{
    const int32 ActiveDayIndex = FlowManagerRef ? FMath::Max(1, FlowManagerRef->CurrentDayIndex) : 1;
    if (CachedTaskDayIndex != ActiveDayIndex)
    {
        CachedTaskDayIndex = ActiveDayIndex;
        bHasOpenedChatAppThisDay = false;
    }

    CurrentDayTitle = FText::Format(LOCTEXT("TaskDayTitle", "Day {0}"), FText::AsNumber(ActiveDayIndex));

    const bool bAssignmentComplete = RouteStateManagerRef && RouteStateManagerRef->LastRouteChoice != E_RouteBranch::Neutral;
    const bool bReportSubmitted = FlowManagerRef && FlowManagerRef->bHasSubmittedCurrentDayReport;

    CurrentTasks.Reset();

    FST_DesktopTaskRecord ReviewMessagesTask;
    ReviewMessagesTask.TaskId = FName(TEXT("ReviewMessages"));
    ReviewMessagesTask.TaskText = LOCTEXT("TaskReviewMessages", "Review team chat");
    ReviewMessagesTask.bIsCompleted = bHasOpenedChatAppThisDay;
    CurrentTasks.Add(ReviewMessagesTask);

    FST_DesktopTaskRecord CompleteAssignmentTask;
    CompleteAssignmentTask.TaskId = FName(TEXT("CompleteAssignment"));
    CompleteAssignmentTask.TaskText = LOCTEXT("TaskCompleteAssignment", "Complete today's assignment");
    CompleteAssignmentTask.bIsCompleted = bAssignmentComplete;
    CurrentTasks.Add(CompleteAssignmentTask);

    FST_DesktopTaskRecord SubmitReportTask;
    SubmitReportTask.TaskId = FName(TEXT("SubmitDailyReport"));
    SubmitReportTask.TaskText = LOCTEXT("TaskSubmitDailyReport", "Submit daily report");
    SubmitReportTask.bIsCompleted = bReportSubmitted;
    CurrentTasks.Add(SubmitReportTask);

    if (TaskListWidgetInstance)
    {
        TaskListWidgetInstance->ApplyTaskListState(CurrentDayTitle, CurrentTasks);
    }
}

bool UDesktopRootWidget::IsDesktopShellPhase(E_GamePhase Phase) const
{
    return Phase == E_GamePhase::DesktopIdle || Phase == E_GamePhase::ChatActive || Phase == E_GamePhase::ReportPhase;
}

UPanelWidget* UDesktopRootWidget::ResolveAppContentHost() const
{
#if !UE_BUILD_SHIPPING
    if (DebugAppContentHost)
    {
        return DebugAppContentHost;
    }
#endif

    if (AppContentHost)
    {
        return AppContentHost;
    }

    return DebugAppContentHost;
}

#undef LOCTEXT_NAMESPACE
