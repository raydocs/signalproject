#include "Dialogue/ChatConversationManager.h"

#include "Anomalies/AnomalyManager.h"
#include "Core/RouteStateManager.h"
#include "Engine/DataTable.h"

namespace
{
FName BuildChatMessageId()
{
    return FName(*FGuid::NewGuid().ToString(EGuidFormats::Digits));
}

FText BuildDisplayName(E_ColleagueId SpeakerId)
{
    switch (SpeakerId)
    {
    case E_ColleagueId::ColleagueA:
        return FText::FromString(TEXT("Lin"));
    case E_ColleagueId::ColleagueB:
        return FText::FromString(TEXT("Chen"));
    case E_ColleagueId::ColleagueC:
        return FText::FromString(TEXT("Lao Zhang"));
    case E_ColleagueId::Supervisor:
        return FText::FromString(TEXT("Supervisor"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}
}

AChatConversationManager::AChatConversationManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AChatConversationManager::InitializeChatSystem(int32 DayIndex)
{
    CurrentDayIndex = FMath::Max(1, DayIndex);
    CurrentConversationTarget = E_ColleagueId::ColleagueA;
    HiddenOptions.Reset();
    ConversationHistory.Reset();
    ThreadHistories.Reset();
    NormalReplyCursorByColleague.Reset();

    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueA);
    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueB);
    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueC);
    ThreadHistories.FindOrAdd(E_ColleagueId::Supervisor);

    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueA) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueB) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueC) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::Supervisor) = 0;

    SyncCurrentConversationFromThreadCache();
    OnConversationChanged.Broadcast(CurrentConversationTarget);
    OnMessagesUpdated.Broadcast();
}

void AChatConversationManager::SwitchConversation(E_ColleagueId Target)
{
    CurrentConversationTarget = Target;
    SyncCurrentConversationFromThreadCache();
    OnConversationChanged.Broadcast(CurrentConversationTarget);
    OnMessagesUpdated.Broadcast();
}

void AChatConversationManager::SendPlayerMessage(const FText& MessageText)
{
    if (MessageText.IsEmpty())
    {
        return;
    }

    FST_ChatMessageRecord PlayerMessage;
    PlayerMessage.MessageId = BuildChatMessageId();
    PlayerMessage.SpeakerId = CurrentConversationTarget;
    PlayerMessage.DisplayName = FText::FromString(TEXT("You"));
    PlayerMessage.MessageText = MessageText;
    PlayerMessage.bIsPlayer = true;

    ThreadHistories.FindOrAdd(CurrentConversationTarget).Add(PlayerMessage);
    SyncCurrentConversationFromThreadCache();

    const FText Reply = ResolveDeterministicReply(CurrentConversationTarget);
    if (!Reply.IsEmpty())
    {
        AppendReply(CurrentConversationTarget, Reply);
    }
    else
    {
        OnMessagesUpdated.Broadcast();
    }
}

void AChatConversationManager::AppendReply(E_ColleagueId FromId, const FText& ReplyText)
{
    if (ReplyText.IsEmpty())
    {
        return;
    }

    FST_ChatMessageRecord ReplyMessage;
    ReplyMessage.MessageId = BuildChatMessageId();
    ReplyMessage.SpeakerId = FromId;
    ReplyMessage.DisplayName = BuildDisplayName(FromId);
    ReplyMessage.MessageText = ReplyText;
    ReplyMessage.bIsPlayer = false;

    ThreadHistories.FindOrAdd(FromId).Add(ReplyMessage);

    if (FromId == CurrentConversationTarget)
    {
        SyncCurrentConversationFromThreadCache();
        OnMessagesUpdated.Broadcast();
    }
}

void AChatConversationManager::InjectHiddenOption(const FST_HiddenOptionRecord& HiddenOption)
{
    if (HiddenOption.OptionId == NAME_None)
    {
        return;
    }

    for (const FST_HiddenOptionRecord& ExistingOption : HiddenOptions)
    {
        if (ExistingOption.OptionId == HiddenOption.OptionId)
        {
            return;
        }
    }

    HiddenOptions.Add(HiddenOption);
    OnHiddenOptionInjected.Broadcast(HiddenOption);
    OnMessagesUpdated.Broadcast();
}

void AChatConversationManager::PlayHiddenDialogue(E_ColleagueId Target)
{
    if (!HiddenDialoguesTable)
    {
        return;
    }

    TArray<FST_HiddenDialogueRow*> MatchingRows;
    HiddenDialoguesTable->GetAllRows(TEXT("PlayHiddenDialogue"), MatchingRows);

    MatchingRows.RemoveAll([Target, this](const FST_HiddenDialogueRow* Row)
    {
        return !Row || Row->ColleagueId != Target || Row->DayIndex != CurrentDayIndex;
    });

    MatchingRows.Sort([](const FST_HiddenDialogueRow& A, const FST_HiddenDialogueRow& B)
    {
        return A.SequenceOrder < B.SequenceOrder;
    });

    for (const FST_HiddenDialogueRow* Row : MatchingRows)
    {
        if (!Row)
        {
            continue;
        }

        FST_ChatMessageRecord HiddenMessage;
        HiddenMessage.MessageId = Row->DialogueId;
        HiddenMessage.SpeakerId = Target;
        HiddenMessage.DisplayName = BuildDisplayName(Target);
        HiddenMessage.MessageText = Row->Text;
        HiddenMessage.bIsHiddenFragment = true;
        ThreadHistories.FindOrAdd(Target).Add(HiddenMessage);
    }

    if (Target == CurrentConversationTarget)
    {
        SyncCurrentConversationFromThreadCache();
        OnMessagesUpdated.Broadcast();
    }
}

bool AChatConversationManager::ConsumeHiddenOption(FName OptionId)
{
    if (OptionId == NAME_None)
    {
        return false;
    }

    const FST_HiddenOptionRecord ReportOption = AnomalyManagerRef ? AnomalyManagerRef->BuildCurrentIssueReportOption() : FST_HiddenOptionRecord();
    if (ReportOption.OptionId == OptionId && ReportOption.TargetColleague == CurrentConversationTarget)
    {
        FST_ChatMessageRecord PlayerMessage;
        PlayerMessage.MessageId = BuildChatMessageId();
        PlayerMessage.SpeakerId = CurrentConversationTarget;
        PlayerMessage.DisplayName = FText::FromString(TEXT("You"));
        PlayerMessage.MessageText = ReportOption.Label;
        PlayerMessage.bIsPlayer = true;
        ThreadHistories.FindOrAdd(CurrentConversationTarget).Add(PlayerMessage);

        if (AnomalyManagerRef && AnomalyManagerRef->TrySubmitIssueReport(CurrentConversationTarget))
        {
            AppendReply(CurrentConversationTarget, AnomalyManagerRef->GetCurrentReportReplyText());
            SyncCurrentConversationFromThreadCache();
            OnMessagesUpdated.Broadcast();
            return true;
        }

        SyncCurrentConversationFromThreadCache();
        OnMessagesUpdated.Broadcast();
        return false;
    }

    for (FST_HiddenOptionRecord& HiddenOption : HiddenOptions)
    {
        if (HiddenOption.OptionId != OptionId)
        {
            continue;
        }

        if (HiddenOption.bConsumed)
        {
            return false;
        }

        FST_ChatMessageRecord PlayerMessage;
        PlayerMessage.MessageId = BuildChatMessageId();
        PlayerMessage.SpeakerId = HiddenOption.TargetColleague;
        PlayerMessage.DisplayName = FText::FromString(TEXT("You"));
        PlayerMessage.MessageText = HiddenOption.Label;
        PlayerMessage.bIsPlayer = true;
        ThreadHistories.FindOrAdd(HiddenOption.TargetColleague).Add(PlayerMessage);

        HiddenOption.bConsumed = true;

        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->MarkHiddenOptionConsumed(HiddenOption.TargetColleague, HiddenOption.OptionId, HiddenOption.OptionKind, HiddenOption.RequiredAnomaly);
        }

        const int32 PreviousMessageCount = ThreadHistories.FindOrAdd(HiddenOption.TargetColleague).Num();
        PlayHiddenDialogue(HiddenOption.TargetColleague);
        const int32 NewMessageCount = ThreadHistories.FindOrAdd(HiddenOption.TargetColleague).Num();

        if (NewMessageCount == PreviousMessageCount &&
            HiddenOption.OptionKind == E_ChatOptionKind::SelfHandleFollowup &&
            AnomalyManagerRef)
        {
            AppendReply(HiddenOption.TargetColleague, AnomalyManagerRef->GetFollowupReplyText(HiddenOption.RequiredAnomaly));
        }

        if (HiddenOption.TargetColleague == CurrentConversationTarget)
        {
            SyncCurrentConversationFromThreadCache();
        }

        OnMessagesUpdated.Broadcast();
        return true;
    }

    return false;
}

TArray<FST_HiddenOptionRecord> AChatConversationManager::GetVisibleOptionsForCurrentConversation() const
{
    TArray<FST_HiddenOptionRecord> Result;

    if (AnomalyManagerRef)
    {
        const FST_HiddenOptionRecord ReportOption = AnomalyManagerRef->BuildCurrentIssueReportOption();
        if (ReportOption.OptionId != NAME_None && ReportOption.TargetColleague == CurrentConversationTarget)
        {
            Result.Add(ReportOption);
        }
    }

    for (const FST_HiddenOptionRecord& HiddenOption : HiddenOptions)
    {
        if (!HiddenOption.bConsumed && HiddenOption.TargetColleague == CurrentConversationTarget)
        {
            Result.Add(HiddenOption);
        }
    }

    return Result;
}

void AChatConversationManager::SyncCurrentConversationFromThreadCache()
{
    ConversationHistory = ThreadHistories.FindOrAdd(CurrentConversationTarget);
}

FText AChatConversationManager::ResolveDeterministicReply(E_ColleagueId TargetColleague)
{
    if (TargetColleague == E_ColleagueId::Supervisor)
    {
        if (!SupervisorLinesTable)
        {
            return FText::GetEmpty();
        }

        TArray<FST_SupervisorLineRow*> SupervisorRows;
        SupervisorLinesTable->GetAllRows(TEXT("ResolveDeterministicReplySupervisor"), SupervisorRows);

        for (const FST_SupervisorLineRow* Row : SupervisorRows)
        {
            if (Row && Row->DayIndex == CurrentDayIndex)
            {
                return Row->Text;
            }
        }

        for (const FST_SupervisorLineRow* Row : SupervisorRows)
        {
            if (Row)
            {
                return Row->Text;
            }
        }

        return FText::GetEmpty();
    }

    if (!NormalRepliesTable)
    {
        return FText::GetEmpty();
    }

    TArray<FST_NormalReplyRow*> AllRows;
    NormalRepliesTable->GetAllRows(TEXT("ResolveDeterministicReplyNormal"), AllRows);

    TArray<FST_NormalReplyRow*> MatchingRows;
    for (FST_NormalReplyRow* Row : AllRows)
    {
        if (Row && Row->DayIndex == CurrentDayIndex && Row->ColleagueId == TargetColleague)
        {
            MatchingRows.Add(Row);
        }
    }

    if (MatchingRows.IsEmpty())
    {
        for (FST_NormalReplyRow* Row : AllRows)
        {
            if (Row && Row->ColleagueId == TargetColleague)
            {
                MatchingRows.Add(Row);
            }
        }
    }

    if (MatchingRows.IsEmpty())
    {
        return FText::GetEmpty();
    }

    int32& Cursor = NormalReplyCursorByColleague.FindOrAdd(TargetColleague);
    const int32 SafeIndex = Cursor % MatchingRows.Num();
    Cursor = (SafeIndex + 1) % MatchingRows.Num();

    return MatchingRows[SafeIndex]->Text;
}
