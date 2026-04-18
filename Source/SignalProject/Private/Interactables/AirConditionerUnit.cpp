#include "Interactables/AirConditionerUnit.h"

#include "Anomalies/AnomalyManager.h"
#include "Components/SceneComponent.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/HiddenDialogueUnlocker.h"

AAirConditionerUnit::AAirConditionerUnit()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
}

bool AAirConditionerUnit::CanInteract_Implementation() const
{
    if (!bIsCurrentlyAvailable || !GameFlowManagerRef || !AnomalyManagerRef)
    {
        return false;
    }

    const bool bPhaseAllows = GameFlowManagerRef->GetCurrentPhase() == E_GamePhase::HandleAnomaly3D;
    const bool bMatchingAnomaly = AnomalyManagerRef->bAnomalyActive && AnomalyManagerRef->CurrentAnomalyType == HandledAnomalyType;
    return bPhaseAllows && bMatchingAnomaly;
}

FText AAirConditionerUnit::GetInteractionText_Implementation() const
{
    return FText::FromString(TEXT("Repair Air Conditioner"));
}

void AAirConditionerUnit::Interact_Implementation(AActor* /*InteractingActor*/)
{
    if (!CanInteract_Implementation())
    {
        return;
    }

    if (AnomalyManagerRef)
    {
        AnomalyManagerRef->ResolveCurrentAnomaly();
    }

    if (HiddenDialogueUnlockerRef)
    {
        HiddenDialogueUnlockerRef->UnlockColleagueAHiddenOption();
    }

    bIsCurrentlyAvailable = false;

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::RoomExplore);
    }
}
