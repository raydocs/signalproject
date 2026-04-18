#include "Core/SignalGameFlowManager.h"

#include "Anomalies/AnomalyManager.h"
#include "Core/RouteStateManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Kismet/GameplayStatics.h"
#include "Minigames/MinigameManager.h"
#include "Player/SignalPlayerController.h"

ASignalGameFlowManager::ASignalGameFlowManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASignalGameFlowManager::BeginPlay()
{
    Super::BeginPlay();
    InitializeGameFlow();
}

void ASignalGameFlowManager::InitializeGameFlow()
{
    CachedPlayerController = Cast<ASignalPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (CachedPlayerController)
    {
        CachedPlayerController->RegisterSliceRefs(this, RouteStateManagerRef, AnomalyManagerRef, ChatConversationManagerRef, MinigameManagerRef);
    }

    if (MinigameManagerRef)
    {
        MinigameManagerRef->BindAnomalyCallbacks();
    }

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->InitializeChatSystem(CurrentDayIndex);
    }

    StartDay(CurrentDayIndex);
}

void ASignalGameFlowManager::StartDay(int32 DayIndex)
{
    CurrentDayIndex = FMath::Max(1, DayIndex);

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->InitializeChatSystem(CurrentDayIndex);
    }

    RequestPhaseChange(E_GamePhase::RoomExplore);
}

bool ASignalGameFlowManager::RequestPhaseChange(E_GamePhase NewPhase)
{
    if (CurrentPhase == NewPhase)
    {
        return false;
    }

    PreviousPhase = CurrentPhase;
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(PreviousPhase, CurrentPhase);
    HandlePhaseEntered(NewPhase);
    return true;
}

void ASignalGameFlowManager::HandlePhaseEntered(E_GamePhase NewPhase)
{
    if (!CachedPlayerController)
    {
        CachedPlayerController = Cast<ASignalPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    }

    if (!CachedPlayerController)
    {
        return;
    }

    switch (NewPhase)
    {
    case E_GamePhase::RoomExplore:
    case E_GamePhase::HandleAnomaly3D:
        CachedPlayerController->HideDesktopRoot();
        CachedPlayerController->SetRoomInputMode();
        break;
    case E_GamePhase::DesktopIdle:
    case E_GamePhase::ChatActive:
    case E_GamePhase::MinigameActive:
    case E_GamePhase::AnomalyChoice:
    case E_GamePhase::ReportPhase:
        CachedPlayerController->ShowDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        break;
    case E_GamePhase::EndingSequence:
        CachedPlayerController->HideDesktopRoot();
        CachedPlayerController->SetRoomInputMode();
        CachedPlayerController->ShowEndingTitleCard(PendingEndingId);
        break;
    default:
        break;
    }
}

bool ASignalGameFlowManager::CanEnterDesktop() const
{
    return CurrentPhase == E_GamePhase::RoomExplore ||
           CurrentPhase == E_GamePhase::DesktopIdle ||
           CurrentPhase == E_GamePhase::ChatActive;
}

bool ASignalGameFlowManager::CanOpenReport() const
{
    return CurrentPhase == E_GamePhase::DesktopIdle ||
           CurrentPhase == E_GamePhase::ChatActive ||
           CurrentPhase == E_GamePhase::MinigameActive;
}

void ASignalGameFlowManager::SubmitSliceReport(FName /*SelectedSentenceId*/)
{
    const FName EndingId = RouteStateManagerRef ? RouteStateManagerRef->GetSliceEndingResult() : FName(TEXT("BadEnding"));
    TriggerEnding(EndingId);
}

void ASignalGameFlowManager::TriggerEnding(FName EndingId)
{
    PendingEndingId = EndingId;
    RequestPhaseChange(E_GamePhase::EndingSequence);
}
