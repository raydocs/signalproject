#include "UI/DesktopRootWidget.h"

#include "Blueprint/UserWidget.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Minigames/MinigameManager.h"
#include "Report/ReportEditorWidget.h"
#include "UI/ChatAppWidget.h"

void UDesktopRootWidget::InitializeForSlice(
    ASignalGameFlowManager* InFlowManager,
    ARouteStateManager* InRouteStateManager,
    AChatConversationManager* InChatConversationManager,
    AMinigameManager* InMinigameManager)
{
    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;
    ChatConversationManagerRef = InChatConversationManager;
    MinigameManagerRef = InMinigameManager;

    if (MinigameManagerRef)
    {
        MinigameManagerRef->RegisterDesktopRoot(this);
    }

    RefreshAppAvailability();
}

void UDesktopRootWidget::OpenApp(FName AppTag)
{
    if (AppTag == FName(TEXT("Chat")))
    {
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
    BP_OnAppWidgetRequested(AppWidget);
}

void UDesktopRootWidget::RefreshAppAvailability()
{
    const bool bCanOpenReport = FlowManagerRef && FlowManagerRef->CanOpenReport();
    BP_OnAppAvailabilityChanged(bCanOpenReport);
}
