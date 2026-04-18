#include "Core/RouteStateManager.h"

ARouteStateManager::ARouteStateManager()
{
    PrimaryActorTick.bCanEverTick = false;

    ColleagueUnlockStates.Add(E_ColleagueId::ColleagueA, E_SkillUnlockState::Locked);
    ColleagueUnlockStates.Add(E_ColleagueId::ColleagueB, E_SkillUnlockState::Locked);
    ColleagueUnlockStates.Add(E_ColleagueId::ColleagueC, E_SkillUnlockState::Locked);
    ColleagueUnlockStates.Add(E_ColleagueId::Supervisor, E_SkillUnlockState::Locked);
}

void ARouteStateManager::RecordRouteChoice(E_RouteBranch RouteChoice)
{
    LastRouteChoice = RouteChoice;

    if (RouteChoice == E_RouteBranch::ReportSupervisor)
    {
        ++ReportSupervisorCount;
    }
    else if (RouteChoice == E_RouteBranch::HandleMyself)
    {
        ++HandleMyselfCount;
    }
}

void ARouteStateManager::SetColleagueUnlockState(E_ColleagueId ColleagueId, E_SkillUnlockState NewState)
{
    ColleagueUnlockStates.FindOrAdd(ColleagueId) = NewState;

    if (ColleagueId == E_ColleagueId::ColleagueA)
    {
        ColleagueAState = NewState;
    }
}

void ARouteStateManager::MarkHiddenOptionConsumed(E_ColleagueId ColleagueId, FName OptionId)
{
    if (ColleagueId == E_ColleagueId::ColleagueA && OptionId != NAME_None)
    {
        bHasConsumedColleagueAHiddenOption = true;
        ColleagueAState = E_SkillUnlockState::Unlocked;
        ColleagueUnlockStates.FindOrAdd(E_ColleagueId::ColleagueA) = E_SkillUnlockState::Unlocked;
    }
}

FName ARouteStateManager::GetSliceEndingResult() const
{
    const bool bGoodPath = (HandleMyselfCount > 0) && bHasConsumedColleagueAHiddenOption;
    return bGoodPath ? FName(TEXT("GoodEnding")) : FName(TEXT("BadEnding"));
}

E_SkillUnlockState ARouteStateManager::GetColleagueUnlockState(E_ColleagueId ColleagueId) const
{
    if (const E_SkillUnlockState* FoundState = ColleagueUnlockStates.Find(ColleagueId))
    {
        return *FoundState;
    }

    return E_SkillUnlockState::Locked;
}
