#include "Core/SignalGameFlowManager.h"

#include "Anomalies/AnomalyManager.h"
#include "Core/RouteStateManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Minigames/MinigameManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Player/SignalPlayerController.h"

ASignalGameFlowManager::ASignalGameFlowManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

namespace
{
    constexpr uint64 SignalPhaseDebugMessageKey = 420043;
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

    RequestPhaseChange(ResolveStartupPhase());
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

    const UEnum* PhaseEnum = StaticEnum<E_GamePhase>();
    const FString PhaseName = PhaseEnum ? PhaseEnum->GetNameStringByValue(static_cast<int64>(NewPhase)) : TEXT("UnknownPhase");
    UE_LOG(LogTemp, Display, TEXT("Entered phase: %s"), *PhaseName);
#if !UE_BUILD_SHIPPING
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            SignalPhaseDebugMessageKey,
            5.0f,
            FColor::Green,
            FString::Printf(TEXT("Phase: %s"), *PhaseName),
            false,
            FVector2D(1.1f, 1.1f));
    }
#endif

    switch (NewPhase)
    {
    case E_GamePhase::RoomExplore:
    case E_GamePhase::HandleAnomaly3D:
        CachedPlayerController->HideDesktopRoot();
        CachedPlayerController->SetRoomInputMode();
        break;
    case E_GamePhase::DesktopIdle:
        CachedPlayerController->ShowDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        CachedPlayerController->RefreshDesktopRootAvailability();
        break;
    case E_GamePhase::ChatActive:
        CachedPlayerController->ShowDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        CachedPlayerController->RefreshDesktopRootAvailability();
        CachedPlayerController->OpenDesktopApp(FName(TEXT("Chat")));
        break;
    case E_GamePhase::MinigameActive:
    case E_GamePhase::AnomalyChoice:
        CachedPlayerController->ShowDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        CachedPlayerController->RefreshDesktopRootAvailability();
        break;
    case E_GamePhase::ReportPhase:
        CachedPlayerController->ShowDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        CachedPlayerController->RefreshDesktopRootAvailability();
        CachedPlayerController->OpenDesktopApp(FName(TEXT("Report")));
        break;
    case E_GamePhase::EndingSequence:
        CachedPlayerController->HideDesktopRoot();
        CachedPlayerController->SetDesktopInputMode();
        CachedPlayerController->ShowEndingTitleCard(PendingEndingId);
        break;
    default:
        break;
    }
}

bool ASignalGameFlowManager::CanEnterDesktop() const
{
    return CurrentPhase == E_GamePhase::RoomExplore;
}

bool ASignalGameFlowManager::CanOpenReport() const
{
    if (CurrentPhase == E_GamePhase::MinigameActive ||
        CurrentPhase == E_GamePhase::AnomalyChoice ||
        CurrentPhase == E_GamePhase::HandleAnomaly3D)
    {
        return false;
    }

    if (!RouteStateManagerRef)
    {
        return false;
    }

    if (RouteStateManagerRef->LastRouteChoice == E_RouteBranch::HandleMyself)
    {
        return RouteStateManagerRef->bHasConsumedColleagueAHiddenOption;
    }

    return RouteStateManagerRef->LastRouteChoice == E_RouteBranch::ReportSupervisor;
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

E_GamePhase ASignalGameFlowManager::ResolveStartupPhase() const
{
    FString RequestedPhaseName;
    if (!FParse::Value(FCommandLine::Get(), TEXT("SignalDebugPhase="), RequestedPhaseName))
    {
        return E_GamePhase::RoomExplore;
    }

    RequestedPhaseName.TrimStartAndEndInline();

    const UEnum* PhaseEnum = StaticEnum<E_GamePhase>();
    if (!PhaseEnum)
    {
        return E_GamePhase::RoomExplore;
    }

    const int64 EnumValue = PhaseEnum->GetValueByNameString(RequestedPhaseName);
    if (EnumValue == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unknown -SignalDebugPhase value '%s'; defaulting to RoomExplore."), *RequestedPhaseName);
        return E_GamePhase::RoomExplore;
    }

    const E_GamePhase RequestedPhase = static_cast<E_GamePhase>(EnumValue);
    UE_LOG(LogTemp, Display, TEXT("Applying debug startup phase: %s"), *RequestedPhaseName);
    return RequestedPhase;
}
