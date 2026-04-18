#include "Anomalies/AnomalyManager.h"

#include "Core/SignalGameFlowManager.h"

AAnomalyManager::AAnomalyManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAnomalyManager::AccumulateSideEffect(E_ColleagueId /*SourceColleague*/, float Amount)
{
    if (bAnomalyActive)
    {
        return;
    }

    AccumulatedFreezeWeight += FMath::Max(0.0f, Amount);

    if (ResolveDominantAnomaly() == E_AnomalyType::FREEZE)
    {
        TriggerAnomaly(E_AnomalyType::FREEZE);
    }
}

E_AnomalyType AAnomalyManager::ResolveDominantAnomaly() const
{
    return AccumulatedFreezeWeight >= FreezeThreshold ? E_AnomalyType::FREEZE : E_AnomalyType::None;
}

void AAnomalyManager::TriggerAnomaly(E_AnomalyType NewAnomaly)
{
    if (bAnomalyActive)
    {
        return;
    }

    if (NewAnomaly != E_AnomalyType::FREEZE)
    {
        return;
    }

    bAnomalyActive = true;
    CurrentAnomalyType = NewAnomaly;
    OnAnomalyTriggered.Broadcast(NewAnomaly);

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::AnomalyChoice);
    }
}

void AAnomalyManager::ResolveCurrentAnomaly()
{
    if (!bAnomalyActive)
    {
        return;
    }

    bAnomalyActive = false;
    CurrentAnomalyType = E_AnomalyType::None;
    AccumulatedFreezeWeight = 0.0f;

    OnAnomalyResolved.Broadcast();
}
