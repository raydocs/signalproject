#include "Minigames/MinigameManager.h"

#include "Anomalies/AnomalyManager.h"
#include "Blueprint/UserWidget.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Minigames/DependencyMatchWidget.h"
#include "UI/AnomalyChoicePopupWidget.h"
#include "UI/DesktopRootWidget.h"

AMinigameManager::AMinigameManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMinigameManager::RegisterDesktopRoot(UDesktopRootWidget* InDesktopRoot)
{
    DesktopRootRef = InDesktopRoot;
}

void AMinigameManager::BindAnomalyCallbacks()
{
    if (!AnomalyManagerRef)
    {
        return;
    }

    AnomalyManagerRef->OnAnomalyTriggered.RemoveDynamic(this, &AMinigameManager::HandleAnomalyTriggered);
    AnomalyManagerRef->OnAnomalyTriggered.AddDynamic(this, &AMinigameManager::HandleAnomalyTriggered);
}

void AMinigameManager::StartMinigame(E_MinigameType MinigameType)
{
    CurrentMinigameType = MinigameType;

    if (!CurrentMinigameWidget && DependencyMatchWidgetClass)
    {
        if (UWorld* World = GetWorld())
        {
            CurrentMinigameWidget = CreateWidget<UDependencyMatchWidget>(World, DependencyMatchWidgetClass);
            if (CurrentMinigameWidget)
            {
                CurrentMinigameWidget->InitializeForSlice(this);
                CurrentMinigameWidget->BuildGrid();
            }
        }
    }

    if (DesktopRootRef && CurrentMinigameWidget)
    {
        DesktopRootRef->ShowAppWidget(CurrentMinigameWidget);
    }

    bIsMinigameRunning = true;

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::MinigameActive);
    }
}

void AMinigameManager::PauseMinigame()
{
    bIsMinigameRunning = false;

    if (CurrentMinigameWidget)
    {
        CurrentMinigameWidget->SetPaused(true);
    }
}

void AMinigameManager::ResumeMinigame()
{
    bIsMinigameRunning = true;

    if (CurrentMinigameWidget)
    {
        CurrentMinigameWidget->SetPaused(false);
    }
}

void AMinigameManager::CompleteMinigame()
{
    if (CurrentMinigameWidget)
    {
        CurrentMinigameWidget->RemoveFromParent();
    }

    bIsMinigameRunning = false;
}

void AMinigameManager::HandleAnomalyTriggered(E_AnomalyType TriggeredAnomaly)
{
    PauseMinigame();

    if (!ChoicePopupRef && AnomalyChoicePopupClass)
    {
        if (UWorld* World = GetWorld())
        {
            ChoicePopupRef = CreateWidget<UAnomalyChoicePopupWidget>(World, AnomalyChoicePopupClass);
            if (ChoicePopupRef)
            {
                ChoicePopupRef->InitializeForSlice(this);
            }
        }
    }

    if (ChoicePopupRef)
    {
        ChoicePopupRef->SetupForAnomaly(TriggeredAnomaly);
        ChoicePopupRef->AddToViewport(90);
    }
}

void AMinigameManager::HandleAnomalyChoice(E_RouteBranch SelectedBranch)
{
    if (!GameFlowManagerRef)
    {
        return;
    }

    if (SelectedBranch == E_RouteBranch::ReportSupervisor)
    {
        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->RecordRouteChoice(E_RouteBranch::ReportSupervisor);
        }

        if (AnomalyManagerRef)
        {
            AnomalyManagerRef->ResolveCurrentAnomaly();
        }

        if (ChoicePopupRef)
        {
            ChoicePopupRef->RemoveFromParent();
        }

        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::MinigameActive);
        ResumeMinigame();
        CompleteMinigame();
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::ReportPhase);
        return;
    }

    if (SelectedBranch == E_RouteBranch::HandleMyself)
    {
        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->RecordRouteChoice(E_RouteBranch::HandleMyself);
        }

        if (ChoicePopupRef)
        {
            ChoicePopupRef->RemoveFromParent();
        }

        if (CurrentMinigameWidget)
        {
            CurrentMinigameWidget->RemoveFromParent();
        }

        bIsMinigameRunning = false;
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::HandleAnomaly3D);
    }
}
