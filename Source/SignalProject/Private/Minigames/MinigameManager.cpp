#include "Minigames/MinigameManager.h"

#include "Anomalies/AnomalyManager.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Minigames/DependencyMatchWidget.h"
#include "UI/AnomalyChoicePopupWidget.h"
#include "UI/DesktopRootWidget.h"

#define LOCTEXT_NAMESPACE "MinigameManager"

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
    AnomalyManagerRef->OnAnomalyReportAccepted.RemoveDynamic(this, &AMinigameManager::HandleAnomalyReportAccepted);
    AnomalyManagerRef->OnAnomalyReportAccepted.AddDynamic(this, &AMinigameManager::HandleAnomalyReportAccepted);
    AnomalyManagerRef->OnAnomalyResolved.RemoveDynamic(this, &AMinigameManager::HandleAnomalyResolved);
    AnomalyManagerRef->OnAnomalyResolved.AddDynamic(this, &AMinigameManager::HandleAnomalyResolved);
}

void AMinigameManager::ResetForDay(int32 DayIndex)
{
    CurrentDayIndex = FMath::Max(1, DayIndex);
    CompletedStressLoops = 0;
    bAwaitingWorldResolutionCompletion = false;

    if (ChoicePopupRef)
    {
        ChoicePopupRef->RemoveFromParent();
    }

    EndStressSession();
}

void AMinigameManager::StartMinigame(E_MinigameType MinigameType)
{
    if (!CanLaunchStressTest())
    {
        return;
    }

    CurrentMinigameType = MinigameType;

    if (!CurrentMinigameWidget)
    {
        const TSubclassOf<UDependencyMatchWidget> ResolvedWidgetClass =
            DependencyMatchWidgetClass ? DependencyMatchWidgetClass : TSubclassOf<UDependencyMatchWidget>(UDependencyMatchWidget::StaticClass());

        if (UWorld* World = GetWorld())
        {
            CurrentMinigameWidget = CreateWidget<UDependencyMatchWidget>(World, ResolvedWidgetClass);
        }
    }

    if (!CurrentMinigameWidget)
    {
        return;
    }

    CurrentMinigameWidget->InitializeForSlice(this);
    ResetStressSessionViewState();
    bIsMinigameRunning = true;
    RefreshStressWidgetState();

    if (DesktopRootRef)
    {
        DesktopRootRef->ShowTaggedAppWidget(FName(TEXT("Minigame")), CurrentMinigameWidget);
    }

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::MinigameActive);
    }
}

bool AMinigameManager::CanLaunchStressTest() const
{
    return !bIsMinigameRunning && CompletedStressLoops < 3 && (!AnomalyManagerRef || !AnomalyManagerRef->bAnomalyActive);
}

void AMinigameManager::HandleSingleDraw()
{
    ApplyDraw(1);
}

void AMinigameManager::HandleTenDraw()
{
    ApplyDraw(10);
}

void AMinigameManager::PauseMinigame()
{
    bIsMinigameRunning = false;
    CurrentStressState.bIsPaused = true;
    RefreshStressWidgetState();
}

void AMinigameManager::ResumeMinigame()
{
    bIsMinigameRunning = true;
    CurrentStressState.bIsPaused = false;
    RefreshStressWidgetState();
}

void AMinigameManager::CompleteMinigame()
{
    EndStressSession();
}

void AMinigameManager::HandleAnomalyTriggered(E_AnomalyType TriggeredAnomaly)
{
    PauseMinigame();
    CurrentStressState.bAwaitingIssueReport = true;
    CurrentStressState.ActiveAnomalyType = TriggeredAnomaly;
    CurrentStressState.StatusText =
        AnomalyManagerRef ? AnomalyManagerRef->GetStressStatusText(TriggeredAnomaly) : LOCTEXT("StressBugDetected", "A critical issue appeared during the pressure test.");
    RefreshStressWidgetState();

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::ChatActive);
    }
}

void AMinigameManager::HandleAnomalyReportAccepted(E_AnomalyType AcceptedAnomaly)
{
    CurrentStressState.bAwaitingIssueReport = false;
    CurrentStressState.StatusText =
        AnomalyManagerRef ? AnomalyManagerRef->GetStressStatusText(AcceptedAnomaly) : LOCTEXT("StressBugAcknowledged", "Issue routed. Decide whether to escalate or fix it yourself.");
    RefreshStressWidgetState();

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

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::AnomalyChoice);
    }

    if (ChoicePopupRef)
    {
        ChoicePopupRef->SetupForAnomaly(AcceptedAnomaly);
        ChoicePopupRef->AddToViewport(90);
    }
}

void AMinigameManager::HandleAnomalyResolved()
{
    if (bAwaitingWorldResolutionCompletion)
    {
        ++CompletedStressLoops;
        bAwaitingWorldResolutionCompletion = false;
    }

    CurrentStressState.ActiveAnomalyType = E_AnomalyType::None;
    CurrentStressState.bAwaitingIssueReport = false;
    CurrentStressState.StatusText = LOCTEXT("StressResolvedStatus", "Issue cleared. Reopen the stress-test app when you are ready for the next repro.");
    RefreshStressWidgetState();
}

void AMinigameManager::HandleAnomalyChoice(E_RouteBranch SelectedBranch)
{
    if (!GameFlowManagerRef)
    {
        return;
    }

    if (ChoicePopupRef)
    {
        ChoicePopupRef->RemoveFromParent();
    }

    if (SelectedBranch == E_RouteBranch::ReportSupervisor)
    {
        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->RecordRouteChoice(E_RouteBranch::ReportSupervisor);
        }

        ++CompletedStressLoops;
        bAwaitingWorldResolutionCompletion = false;
        EndStressSession();

        if (AnomalyManagerRef)
        {
            AnomalyManagerRef->ResolveCurrentAnomaly();
        }

        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::ReportPhase);
        return;
    }

    if (SelectedBranch == E_RouteBranch::HandleMyself)
    {
        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->RecordRouteChoice(E_RouteBranch::HandleMyself);
        }

        bAwaitingWorldResolutionCompletion = true;
        EndStressSession();
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::HandleAnomaly3D);
    }
}

void AMinigameManager::ApplyDraw(int32 DrawAmount)
{
    if (!bIsMinigameRunning || CurrentStressState.bIsPaused || DrawAmount <= 0)
    {
        return;
    }

    if (AnomalyManagerRef && AnomalyManagerRef->bAnomalyActive)
    {
        return;
    }

    CurrentStressState.TotalDraws += DrawAmount;
    CurrentStressState.LastDrawAmount = DrawAmount;
    CurrentStressState.RemainingCurrency = FMath::Max(0, CurrentStressState.RemainingCurrency - DrawAmount);
    CurrentStressState.StressPoints += DrawAmount * 10;
    CurrentStressState.OutputText =
        DrawAmount >= 10
            ? LOCTEXT("TenDrawOutput", "Ten-draw burst complete. Effect stack and counters updated.")
            : LOCTEXT("SingleDrawOutput", "Single draw complete. Reward preview refreshed.");

    const E_AnomalyType TriggeredAnomaly = ResolveTriggeredAnomalyForDraw(DrawAmount);
    if (TriggeredAnomaly != E_AnomalyType::None)
    {
        CurrentStressState.ActiveAnomalyType = TriggeredAnomaly;
        CurrentStressState.StatusText =
            AnomalyManagerRef ? AnomalyManagerRef->GetStressStatusText(TriggeredAnomaly) : LOCTEXT("StressBugTriggeredFallback", "A critical issue surfaced during the latest repro.");
        RefreshStressWidgetState();

        if (AnomalyManagerRef)
        {
            AnomalyManagerRef->TriggerAnomaly(TriggeredAnomaly);
        }
        return;
    }

    switch (CompletedStressLoops)
    {
    case 0:
        CurrentStressState.StatusText = LOCTEXT("NeedSingleDrawStatus", "No critical issue yet. Use Single Draw to test the reward material path.");
        break;
    case 1:
        CurrentStressState.StatusText = LOCTEXT("NeedTenDrawStatus", "No critical issue yet. Use Ten Draw to hammer the effects cache.");
        break;
    case 2:
        CurrentStressState.StatusText = LOCTEXT("NeedSingleDrawAgainStatus", "No critical issue yet. Use Single Draw again to probe rollback and timeout behavior.");
        break;
    default:
        CurrentStressState.StatusText = LOCTEXT("AllStressIssuesComplete", "All planned bug cases have been reproduced for this runtime slice.");
        break;
    }

    RefreshStressWidgetState();
}

void AMinigameManager::RefreshStressWidgetState()
{
    const bool bCanInteract = !(AnomalyManagerRef && AnomalyManagerRef->bAnomalyActive) && !CurrentStressState.bIsPaused && CompletedStressLoops < 3;

    switch (CompletedStressLoops)
    {
    case 0:
    case 2:
        CurrentStressState.bCanSingleDraw = bCanInteract;
        CurrentStressState.bCanTenDraw = false;
        break;
    case 1:
        CurrentStressState.bCanSingleDraw = false;
        CurrentStressState.bCanTenDraw = bCanInteract;
        break;
    default:
        CurrentStressState.bCanSingleDraw = false;
        CurrentStressState.bCanTenDraw = false;
        break;
    }

    if (CurrentMinigameWidget)
    {
        CurrentMinigameWidget->ApplyStressState(CurrentStressState);
        CurrentMinigameWidget->SetPaused(CurrentStressState.bIsPaused);
    }
}

void AMinigameManager::EndStressSession()
{
    bIsMinigameRunning = false;

    if (DesktopRootRef && CurrentMinigameWidget)
    {
        DesktopRootRef->ClearHostedApp(CurrentMinigameWidget);
    }

    ResetStressSessionViewState();
    RefreshStressWidgetState();
}

E_AnomalyType AMinigameManager::ResolveTriggeredAnomalyForDraw(int32 DrawAmount) const
{
    switch (CompletedStressLoops)
    {
    case 0:
        return DrawAmount == 1 ? E_AnomalyType::BLACKOUT : E_AnomalyType::None;
    case 1:
        return DrawAmount >= 10 ? E_AnomalyType::DISKCLEAN : E_AnomalyType::None;
    case 2:
        return DrawAmount == 1 ? E_AnomalyType::FREEZE : E_AnomalyType::None;
    default:
        return E_AnomalyType::None;
    }
}

void AMinigameManager::ResetStressSessionViewState()
{
    CurrentStressState = FST_StressTestViewState();
    CurrentStressState.RemainingCurrency = 100;

    switch (CompletedStressLoops)
    {
    case 0:
        CurrentStressState.StatusText = LOCTEXT("StressIntroBlackout", "Pressure test ready. Use Single Draw to validate reward material output.");
        break;
    case 1:
        CurrentStressState.StatusText = LOCTEXT("StressIntroDiskclean", "Pressure test ready. Use Ten Draw to push the FX stack and cache path.");
        break;
    case 2:
        CurrentStressState.StatusText = LOCTEXT("StressIntroFreeze", "Pressure test ready. Use Single Draw again to probe rollback and timeout behavior.");
        break;
    default:
        CurrentStressState.StatusText = LOCTEXT("StressAllDone", "All planned pressure-test issues have already been reproduced.");
        break;
    }

    CurrentStressState.OutputText = LOCTEXT("StressOutputPlaceholder", "Latest draw output will appear here.");
}

#undef LOCTEXT_NAMESPACE
