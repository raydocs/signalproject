#include "Dialogue/HiddenDialogueUnlocker.h"

#include "Core/RouteStateManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "SignalSliceTypes.h"

AHiddenDialogueUnlocker::AHiddenDialogueUnlocker()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHiddenDialogueUnlocker::UnlockColleagueAHiddenOption()
{
    if (!ChatConversationManagerRef)
    {
        return;
    }

    FST_HiddenOptionRecord HiddenOption;
    HiddenOption.OptionId = StableOptionId;
    HiddenOption.TargetColleague = E_ColleagueId::ColleagueA;
    HiddenOption.Label = FText::FromString(TEXT("Ask about the glitch"));
    HiddenOption.RequiredAnomaly = E_AnomalyType::FREEZE;
    HiddenOption.UnlockDay = 2;

    ChatConversationManagerRef->InjectHiddenOption(HiddenOption);

    if (RouteStateManagerRef)
    {
        RouteStateManagerRef->SetColleagueUnlockState(E_ColleagueId::ColleagueA, E_SkillUnlockState::AnomalySeen);
    }
}
