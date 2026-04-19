#include "Anomalies/AnomalyManager.h"

#define LOCTEXT_NAMESPACE "AnomalyManager"

AAnomalyManager::AAnomalyManager()
{
    PrimaryActorTick.bCanEverTick = false;

    FST_AnomalyRouteConfig BlackoutConfig;
    BlackoutConfig.AnomalyType = E_AnomalyType::BLACKOUT;
    BlackoutConfig.ReportColleague = E_ColleagueId::ColleagueA;
    BlackoutConfig.ReportOptionId = FName(TEXT("REPORT_BLACKOUT"));
    BlackoutConfig.ReportOptionLabel = LOCTEXT("BlackoutReportLabel", "The reward preview turned into a purple-black checkerboard block.");
    BlackoutConfig.ReportReplyText = LOCTEXT("BlackoutReportReply", "I see the material link broke. Let me rebuild it from my side.");
    BlackoutConfig.FollowupOptionId = FName(TEXT("FOLLOWUP_BLACKOUT"));
    BlackoutConfig.FollowupOptionLabel = LOCTEXT("BlackoutFollowupLabel", "Did you cut the room lights while fixing the material pass?");
    BlackoutConfig.FollowupReplyText = LOCTEXT("BlackoutFollowupReply", "The room was too bright to read the edges. I killed the lights to see what the asset was hiding.");
    RouteConfigs.Add(BlackoutConfig);

    FST_AnomalyRouteConfig DiskcleanConfig;
    DiskcleanConfig.AnomalyType = E_AnomalyType::DISKCLEAN;
    DiskcleanConfig.ReportColleague = E_ColleagueId::ColleagueB;
    DiskcleanConfig.ReportOptionId = FName(TEXT("REPORT_DISKCLEAN"));
    DiskcleanConfig.ReportOptionLabel = LOCTEXT("DiskcleanReportLabel", "Ten draws stacked too many effects and the client is choking on cache cleanup.");
    DiskcleanConfig.ReportReplyText = LOCTEXT("DiskcleanReportReply", "The render cache is on fire. I am force-clearing it before the whole client seizes up.");
    DiskcleanConfig.FollowupOptionId = FName(TEXT("FOLLOWUP_DISKCLEAN"));
    DiskcleanConfig.FollowupOptionLabel = LOCTEXT("DiskcleanFollowupLabel", "Did you slam the cache purge hard enough to make the computer case scream?");
    DiskcleanConfig.FollowupReplyText = LOCTEXT("DiskcleanFollowupReply", "I dumped every stale effect buffer I could find. The fans should calm down once the purge finishes.");
    RouteConfigs.Add(DiskcleanConfig);

    FST_AnomalyRouteConfig FreezeConfig;
    FreezeConfig.AnomalyType = E_AnomalyType::FREEZE;
    FreezeConfig.ReportColleague = E_ColleagueId::ColleagueC;
    FreezeConfig.ReportOptionId = FName(TEXT("REPORT_FREEZE"));
    FreezeConfig.ReportOptionLabel = LOCTEXT("FreezeReportLabel", "A single draw rolled back the counters and threw a 504 timeout.");
    FreezeConfig.ReportReplyText = LOCTEXT("FreezeReportReply", "The backend sync is stalling. I am resetting the environment before the service freezes solid.");
    FreezeConfig.FollowupOptionId = FName(TEXT("FOLLOWUP_FREEZE"));
    FreezeConfig.FollowupOptionLabel = LOCTEXT("FreezeFollowupLabel", "Did your emergency reset drag the whole room down to sixteen degrees?");
    FreezeConfig.FollowupReplyText = LOCTEXT("FreezeFollowupReply", "The environment only stabilizes when everything gets colder. I pushed the temperature lower to keep the service alive.");
    RouteConfigs.Add(FreezeConfig);
}

void AAnomalyManager::AccumulateSideEffect(E_ColleagueId /*SourceColleague*/, float Amount)
{
    if (bAnomalyActive)
    {
        return;
    }

    AccumulatedFreezeWeight += FMath::Max(0.0f, Amount);
}

E_AnomalyType AAnomalyManager::ResolveDominantAnomaly() const
{
    return AccumulatedFreezeWeight >= FreezeThreshold ? E_AnomalyType::FREEZE : E_AnomalyType::None;
}

void AAnomalyManager::TriggerAnomaly(E_AnomalyType NewAnomaly)
{
    if (bAnomalyActive || NewAnomaly == E_AnomalyType::None)
    {
        return;
    }

    CurrentAnomalyType = NewAnomaly;
    bAnomalyActive = true;
    bAwaitingIssueReport = true;
    bHasAcceptedIssueReport = false;

    OnAnomalyTriggered.Broadcast(CurrentAnomalyType);
}

bool AAnomalyManager::TrySubmitIssueReport(E_ColleagueId TargetColleague)
{
    if (!bAnomalyActive || !bAwaitingIssueReport || bHasAcceptedIssueReport)
    {
        return false;
    }

    const FST_AnomalyRouteConfig* RouteConfig = FindRouteConfig(CurrentAnomalyType);
    if (!RouteConfig || RouteConfig->ReportColleague != TargetColleague)
    {
        return false;
    }

    bAwaitingIssueReport = false;
    bHasAcceptedIssueReport = true;
    OnAnomalyReportAccepted.Broadcast(CurrentAnomalyType);
    return true;
}

void AAnomalyManager::ResolveCurrentAnomaly()
{
    if (!bAnomalyActive)
    {
        return;
    }

    bAnomalyActive = false;
    bAwaitingIssueReport = false;
    bHasAcceptedIssueReport = false;
    CurrentAnomalyType = E_AnomalyType::None;
    AccumulatedFreezeWeight = 0.0f;

    OnAnomalyResolved.Broadcast();
}

void AAnomalyManager::ResetForDay(int32 /*DayIndex*/)
{
    AccumulatedFreezeWeight = 0.0f;
    CurrentAnomalyType = E_AnomalyType::None;
    bAnomalyActive = false;
    bAwaitingIssueReport = false;
    bHasAcceptedIssueReport = false;
}

FST_HiddenOptionRecord AAnomalyManager::BuildCurrentIssueReportOption() const
{
    FST_HiddenOptionRecord Result;

    if (!bAnomalyActive || !bAwaitingIssueReport)
    {
        return Result;
    }

    if (const FST_AnomalyRouteConfig* RouteConfig = FindRouteConfig(CurrentAnomalyType))
    {
        Result.OptionId = RouteConfig->ReportOptionId;
        Result.TargetColleague = RouteConfig->ReportColleague;
        Result.Label = RouteConfig->ReportOptionLabel;
        Result.RequiredAnomaly = CurrentAnomalyType;
        Result.OptionKind = E_ChatOptionKind::AnomalyReport;
    }

    return Result;
}

FST_HiddenOptionRecord AAnomalyManager::BuildCurrentFollowupOption() const
{
    FST_HiddenOptionRecord Result;

    if (!bAnomalyActive)
    {
        return Result;
    }

    if (const FST_AnomalyRouteConfig* RouteConfig = FindRouteConfig(CurrentAnomalyType))
    {
        Result.OptionId = RouteConfig->FollowupOptionId;
        Result.TargetColleague = RouteConfig->ReportColleague;
        Result.Label = RouteConfig->FollowupOptionLabel;
        Result.RequiredAnomaly = CurrentAnomalyType;
        Result.OptionKind = E_ChatOptionKind::SelfHandleFollowup;
    }

    return Result;
}

FText AAnomalyManager::GetCurrentReportReplyText() const
{
    if (const FST_AnomalyRouteConfig* RouteConfig = FindRouteConfig(CurrentAnomalyType))
    {
        return RouteConfig->ReportReplyText;
    }

    return FText::GetEmpty();
}

FText AAnomalyManager::GetFollowupReplyText(E_AnomalyType ForAnomaly) const
{
    if (const FST_AnomalyRouteConfig* RouteConfig = FindRouteConfig(ForAnomaly))
    {
        return RouteConfig->FollowupReplyText;
    }

    return FText::GetEmpty();
}

FText AAnomalyManager::GetStressStatusText(E_AnomalyType ForAnomaly) const
{
    switch (ForAnomaly)
    {
    case E_AnomalyType::BLACKOUT:
        return LOCTEXT("BlackoutStressStatus", "Material corruption reproduced: reward output is rendering as a purple-black block.");
    case E_AnomalyType::DISKCLEAN:
        return LOCTEXT("DiskcleanStressStatus", "Perf-cache meltdown reproduced: ten-draw effects are shaking the whole render surface.");
    case E_AnomalyType::FREEZE:
        return LOCTEXT("FreezeStressStatus", "Backend rollback reproduced: counters jumped backward after a timeout.");
    default:
        return LOCTEXT("GenericStressStatus", "Pressure test is stable. Keep pushing the simulator.");
    }
}

const FST_AnomalyRouteConfig* AAnomalyManager::FindRouteConfig(E_AnomalyType ForAnomaly) const
{
    for (const FST_AnomalyRouteConfig& RouteConfig : RouteConfigs)
    {
        if (RouteConfig.AnomalyType == ForAnomaly)
        {
            return &RouteConfig;
        }
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
