#include "UI/ChatAppWidget.h"

#include "Dialogue/ChatConversationManager.h"

void UChatAppWidget::InitializeForSlice(
    AChatConversationManager* InChatConversationManager,
    ARouteStateManager* InRouteStateManager)
{
    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->OnMessagesUpdated.RemoveDynamic(this, &UChatAppWidget::HandleMessagesUpdated);
        ChatConversationManagerRef->OnConversationChanged.RemoveDynamic(this, &UChatAppWidget::HandleConversationChanged);
    }

    ChatConversationManagerRef = InChatConversationManager;
    RouteStateManagerRef = InRouteStateManager;

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->OnMessagesUpdated.AddDynamic(this, &UChatAppWidget::HandleMessagesUpdated);
        ChatConversationManagerRef->OnConversationChanged.AddDynamic(this, &UChatAppWidget::HandleConversationChanged);
        CurrentContact = ChatConversationManagerRef->CurrentConversationTarget;
    }

    RefreshConversation();
}

void UChatAppWidget::RefreshConversation()
{
    if (!ChatConversationManagerRef)
    {
        BP_OnConversationRefreshed(TArray<FST_ChatMessageRecord>(), TArray<FST_HiddenOptionRecord>());
        return;
    }

    BP_OnConversationRefreshed(ChatConversationManagerRef->ConversationHistory, ChatConversationManagerRef->HiddenOptions);
}

void UChatAppWidget::HandleSendClicked(const FText& MessageText)
{
    if (!ChatConversationManagerRef)
    {
        return;
    }

    ChatConversationManagerRef->SendPlayerMessage(MessageText);
}

void UChatAppWidget::HandleHiddenOptionClicked(FName OptionId)
{
    if (!ChatConversationManagerRef)
    {
        return;
    }

    ChatConversationManagerRef->ConsumeHiddenOption(OptionId);
}

void UChatAppWidget::HandleContactSelected(E_ColleagueId Contact)
{
    CurrentContact = Contact;

    if (ChatConversationManagerRef)
    {
        ChatConversationManagerRef->SwitchConversation(Contact);
    }
    else
    {
        RefreshConversation();
    }
}

void UChatAppWidget::HandleMessagesUpdated()
{
    RefreshConversation();
}

void UChatAppWidget::HandleConversationChanged(E_ColleagueId NewTarget)
{
    CurrentContact = NewTarget;
    RefreshConversation();
}
