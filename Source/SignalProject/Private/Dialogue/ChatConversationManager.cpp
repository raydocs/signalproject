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
}

AChatConversationManager::AChatConversationManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AChatConversationManager::InitializeChatSystem(int32 DayIndex)
{
    CurrentDayIndex = FMath::Max(1, DayIndex);

    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueA);
    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueB);
    ThreadHistories.FindOrAdd(E_ColleagueId::ColleagueC);
    ThreadHistories.FindOrAdd(E_ColleagueId::Supervisor);

    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueA) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueB) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::ColleagueC) = 0;
    NormalReplyCursorByColleague.FindOrAdd(E_ColleagueId::Supervisor) = 0;

    SyncCurrentConversationFromThreadCache();
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

    if (AnomalyManagerRef)
    {
        AnomalyManagerRef->AccumulateSideEffect(CurrentConversationTarget, 1.0f);
    }

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
    ReplyMessage.DisplayName = FText::FromString(UEnum::GetValueAsString(FromId));
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
        HiddenMessage.DisplayName = FText::FromString(UEnum::GetValueAsString(Target));
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

        HiddenOption.bConsumed = true;

        if (RouteStateManagerRef)
        {
            RouteStateManagerRef->MarkHiddenOptionConsumed(HiddenOption.TargetColleague, HiddenOption.OptionId);
        }

        PlayHiddenDialogue(HiddenOption.TargetColleague);
        return true;
    }

    return false;
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
