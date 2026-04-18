#include "Player/SignalPlayerController.h"

#include "Anomalies/AnomalyManager.h"
#include "Components/InputComponent.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Endings/EndingTitleCardWidget.h"
#include "Minigames/MinigameManager.h"
#include "SignalSliceTypes.h"
#include "UI/DesktopRootWidget.h"

ASignalPlayerController::ASignalPlayerController()
{
    bShowMouseCursor = false;
}

void ASignalPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction(TEXT("LeaveDesktop"), IE_Pressed, this, &ASignalPlayerController::HandleLeaveDesktopPressed);
    }
}

void ASignalPlayerController::RegisterSliceRefs(
    ASignalGameFlowManager* InFlowManager,
    ARouteStateManager* InRouteStateManager,
    AAnomalyManager* InAnomalyManager,
    AChatConversationManager* InChatConversationManager,
    AMinigameManager* InMinigameManager)
{
    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;
    AnomalyManagerRef = InAnomalyManager;
    ChatConversationManagerRef = InChatConversationManager;
    MinigameManagerRef = InMinigameManager;

    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->InitializeForSlice(FlowManagerRef, RouteStateManagerRef, ChatConversationManagerRef, MinigameManagerRef);
    }
}

void ASignalPlayerController::SetRoomInputMode()
{
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
    bIsInDesktopMode = false;
}

void ASignalPlayerController::SetDesktopInputMode()
{
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    bIsInDesktopMode = true;
}

bool ASignalPlayerController::ShowDesktopRoot()
{
    if (!CachedDesktopRoot)
    {
        if (!DesktopRootWidgetClass)
        {
            return false;
        }

        CachedDesktopRoot = CreateWidget<UDesktopRootWidget>(this, DesktopRootWidgetClass);
        if (!CachedDesktopRoot)
        {
            return false;
        }

        CachedDesktopRoot->InitializeForSlice(FlowManagerRef, RouteStateManagerRef, ChatConversationManagerRef, MinigameManagerRef);
    }

    if (!CachedDesktopRoot->IsInViewport())
    {
        CachedDesktopRoot->AddToViewport();
    }

    return true;
}

void ASignalPlayerController::HideDesktopRoot()
{
    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->RemoveFromParent();
    }

    bIsInDesktopMode = false;
}

bool ASignalPlayerController::ShowEndingTitleCard(FName EndingId)
{
    if (!CachedEndingTitleCard)
    {
        if (!EndingTitleCardWidgetClass)
        {
            return false;
        }

        CachedEndingTitleCard = CreateWidget<UEndingTitleCardWidget>(this, EndingTitleCardWidgetClass);
        if (!CachedEndingTitleCard)
        {
            return false;
        }
    }

    CachedEndingTitleCard->SetupEnding(EndingId);

    if (!CachedEndingTitleCard->IsInViewport())
    {
        CachedEndingTitleCard->AddToViewport(100);
    }

    return true;
}

void ASignalPlayerController::HandleLeaveDesktopPressed()
{
    if (!FlowManagerRef)
    {
        return;
    }

    const E_GamePhase Phase = FlowManagerRef->GetCurrentPhase();
    if (Phase == E_GamePhase::DesktopIdle || Phase == E_GamePhase::ChatActive)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::RoomExplore);
    }
}
