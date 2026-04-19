#include "Dialogue/HiddenDialogueUnlocker.h"

#include "Core/RouteStateManager.h"
#include "Dialogue/ChatConversationManager.h"

AHiddenDialogueUnlocker::AHiddenDialogueUnlocker()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHiddenDialogueUnlocker::UnlockHiddenOption(const FST_HiddenOptionRecord& HiddenOption)
{
    if (!ChatConversationManagerRef || HiddenOption.OptionId == NAME_None)
    {
        return;
    }

    ChatConversationManagerRef->InjectHiddenOption(HiddenOption);

    if (RouteStateManagerRef)
    {
        RouteStateManagerRef->SetColleagueUnlockState(HiddenOption.TargetColleague, E_SkillUnlockState::AnomalySeen);
    }
}

void AHiddenDialogueUnlocker::UnlockColleagueAHiddenOption()
{
    FST_HiddenOptionRecord HiddenOption;
    HiddenOption.OptionId = StableOptionId;
    HiddenOption.TargetColleague = E_ColleagueId::ColleagueA;
    HiddenOption.Label = FText::FromString(TEXT("Ask about the glitch"));
    HiddenOption.RequiredAnomaly = E_AnomalyType::FREEZE;
    HiddenOption.UnlockDay = 2;
    HiddenOption.OptionKind = E_ChatOptionKind::HiddenDialogue;

    UnlockHiddenOption(HiddenOption);
}
