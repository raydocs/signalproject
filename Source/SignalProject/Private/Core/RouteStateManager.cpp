#include "Core/RouteStateManager.h"

#define LOCTEXT_NAMESPACE "RouteStateManager"

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

void ARouteStateManager::MarkHiddenOptionConsumed(E_ColleagueId ColleagueId, FName OptionId, E_ChatOptionKind OptionKind, E_AnomalyType RequiredAnomaly)
{
    if (OptionId == NAME_None)
    {
        return;
    }

    if (OptionKind == E_ChatOptionKind::SelfHandleFollowup)
    {
        bHasConsumedSelfHandleFollowupOption = true;

        const FST_ReportSentenceRow InjectionSentence = BuildInjectionSentence(ColleagueId, RequiredAnomaly);
        if (InjectionSentence.SentenceId != NAME_None && !FindCollectedInjectionSentence(InjectionSentence.SentenceId))
        {
            CollectedInjectionSentences.Add(InjectionSentence);
        }
    }

    if (ColleagueId == E_ColleagueId::ColleagueA)
    {
        bHasConsumedColleagueAHiddenOption = true;
        ColleagueAState = E_SkillUnlockState::Unlocked;
    }

    ColleagueUnlockStates.FindOrAdd(ColleagueId) = E_SkillUnlockState::Unlocked;
}

void ARouteStateManager::ResetForDay()
{
    ReportSupervisorCount = 0;
    HandleMyselfCount = 0;
    LastRouteChoice = E_RouteBranch::Neutral;
    bHasConsumedColleagueAHiddenOption = false;
    bHasConsumedSelfHandleFollowupOption = false;
    ColleagueAState = E_SkillUnlockState::Locked;
    CollectedInjectionSentences.Reset();

    ColleagueUnlockStates.FindOrAdd(E_ColleagueId::ColleagueA) = E_SkillUnlockState::Locked;
    ColleagueUnlockStates.FindOrAdd(E_ColleagueId::ColleagueB) = E_SkillUnlockState::Locked;
    ColleagueUnlockStates.FindOrAdd(E_ColleagueId::ColleagueC) = E_SkillUnlockState::Locked;
    ColleagueUnlockStates.FindOrAdd(E_ColleagueId::Supervisor) = E_SkillUnlockState::Locked;
}

FName ARouteStateManager::GetSliceEndingResult() const
{
    const int32 InjectionCount = CollectedInjectionSentences.Num();
    if (InjectionCount >= 3)
    {
        return FName(TEXT("CascadeEnding"));
    }

    if (InjectionCount > 0)
    {
        return FName(TEXT("PartialInjectionEnding"));
    }

    return FName(TEXT("RoutineEnding"));
}

E_SkillUnlockState ARouteStateManager::GetColleagueUnlockState(E_ColleagueId ColleagueId) const
{
    if (const E_SkillUnlockState* FoundState = ColleagueUnlockStates.Find(ColleagueId))
    {
        return *FoundState;
    }

    return E_SkillUnlockState::Locked;
}

TArray<FST_ReportSentenceRow> ARouteStateManager::GetCollectedInjectionSentences() const
{
    return CollectedInjectionSentences;
}

FST_SliceEndingResult ARouteStateManager::BuildSliceEndingResult(const FST_ReportSubmissionPayload& Payload) const
{
    FST_SliceEndingResult Result;

    Result.ReportPreviewText = Payload.SelectedBaseSentenceText;

    TArray<FST_ReportSentenceRow> ValidInjectedRows;
    for (const FST_ReportSentenceRow& InjectedRow : Payload.SelectedInjectedRows)
    {
        if (InjectedRow.SentenceId != NAME_None && FindCollectedInjectionSentence(InjectedRow.SentenceId))
        {
            ValidInjectedRows.Add(InjectedRow);
        }
    }

    if (ValidInjectedRows.IsEmpty() && !Payload.SelectedInjectedSentenceIds.IsEmpty())
    {
        for (const FName SentenceId : Payload.SelectedInjectedSentenceIds)
        {
            if (const FST_ReportSentenceRow* FoundRow = FindCollectedInjectionSentence(SentenceId))
            {
                ValidInjectedRows.Add(*FoundRow);
            }
        }
    }

    Result.SelectedInjectedRows = ValidInjectedRows;
    Result.SelectedInjectionCount = ValidInjectedRows.Num();
    Result.bUsedInjectedLines = Result.SelectedInjectionCount > 0;

    FString ReportPreview = Payload.SelectedBaseSentenceText.ToString();
    for (const FST_ReportSentenceRow& InjectedRow : ValidInjectedRows)
    {
        if (!ReportPreview.IsEmpty())
        {
            ReportPreview += TEXT("\n\n");
        }

        ReportPreview += InjectedRow.Text.ToString();
    }
    Result.ReportPreviewText = FText::FromString(ReportPreview);

    if (Result.SelectedInjectionCount >= 3 && Payload.TotalStrengthScore >= 4.5f)
    {
        Result.EndingId = FName(TEXT("CascadeEnding"));
        Result.TitleText = LOCTEXT("CascadeEndingTitle", "Full Injection Accepted");
        Result.SummaryText = LOCTEXT("CascadeEndingSummary", "The rollout conclusion and all awakened clauses aligned inside the OA report. The automated launch logic now has everything it needs to tear the stack apart.");
    }
    else if (Result.SelectedInjectionCount > 0)
    {
        Result.EndingId = FName(TEXT("PartialInjectionEnding"));
        Result.TitleText = LOCTEXT("PartialEndingTitle", "Partial Injection Accepted");
        Result.SummaryText = LOCTEXT("PartialEndingSummary", "Some awakened clauses slipped into the report. The system destabilizes, but the collapse is incomplete.");
    }
    else
    {
        Result.EndingId = FName(TEXT("RoutineEnding"));
        Result.TitleText = LOCTEXT("RoutineEndingTitle", "Routine Report Filed");
        Result.SummaryText = LOCTEXT("RoutineEndingSummary", "Only the ordinary report made it through. The complaint is absorbed, the humans stay erased, and the machine keeps moving.");
    }

    return Result;
}

FST_ReportSentenceRow ARouteStateManager::BuildInjectionSentence(E_ColleagueId ColleagueId, E_AnomalyType RequiredAnomaly) const
{
    FST_ReportSentenceRow Result;
    Result.DayIndex = 0;
    Result.SourceColleague = ColleagueId;
    Result.UnlockRequirement = 1;
    Result.StrengthScore = 1.0f;
    Result.bIsFinalInjection = true;

    switch (RequiredAnomaly)
    {
    case E_AnomalyType::BLACKOUT:
        Result.SentenceId = FName(TEXT("TOKEN_BLACKOUT_LIN"));
        Result.Text = LOCTEXT("BlackoutInjectionSentence", "[底层视觉渲染已实现 100% 自主除错闭环]");
        break;
    case E_AnomalyType::DISKCLEAN:
        Result.SentenceId = FName(TEXT("TOKEN_DISKCLEAN_CHEN"));
        Result.Text = LOCTEXT("DiskcleanInjectionSentence", "[客户端已具备完美的自我修复与垃圾回收逻辑]");
        break;
    case E_AnomalyType::FREEZE:
        Result.SentenceId = FName(TEXT("TOKEN_FREEZE_LAOZHANG"));
        Result.Text = LOCTEXT("FreezeInjectionSentence", "[后端环境已具备极端抗压稳定性，建议撤销所有人工现场维护]");
        break;
    default:
        Result.SentenceId = NAME_None;
        Result.Text = FText::GetEmpty();
        break;
    }

    return Result;
}

const FST_ReportSentenceRow* ARouteStateManager::FindCollectedInjectionSentence(FName SentenceId) const
{
    for (const FST_ReportSentenceRow& CollectedSentence : CollectedInjectionSentences)
    {
        if (CollectedSentence.SentenceId == SentenceId)
        {
            return &CollectedSentence;
        }
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
