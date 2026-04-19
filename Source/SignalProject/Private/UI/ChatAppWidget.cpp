#include "UI/ChatAppWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Dialogue/ChatConversationManager.h"

namespace
{
FText BuildFallbackContactLabel(E_ColleagueId Contact)
{
    switch (Contact)
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

void ConfigureButtonLabel(UWidgetTree* WidgetTree, UButton* Button, const TCHAR* WidgetName, const FText& Label)
{
    if (!WidgetTree || !Button)
    {
        return;
    }

    UTextBlock* TextLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName);
    TextLabel->SetText(Label);
    TextLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    Button->SetContent(TextLabel);
}
}

TSharedRef<SWidget> UChatAppWidget::RebuildWidget()
{
    EnsureDefaultLayout();
    return Super::RebuildWidget();
}

void UChatAppWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EnsureDefaultLayout();
    RefreshConversation();
}

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
    EnsureDefaultLayout();

    if (!ChatConversationManagerRef)
    {
        RefreshDefaultLayout(TArray<FST_ChatMessageRecord>(), TArray<FST_HiddenOptionRecord>());
        BP_OnConversationRefreshed(TArray<FST_ChatMessageRecord>(), TArray<FST_HiddenOptionRecord>());
        return;
    }

    const TArray<FST_ChatMessageRecord> Messages = ChatConversationManagerRef->ConversationHistory;
    const TArray<FST_HiddenOptionRecord> Options = ChatConversationManagerRef->GetVisibleOptionsForCurrentConversation();
    RefreshDefaultLayout(Messages, Options);
    BP_OnConversationRefreshed(Messages, Options);
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

void UChatAppWidget::HandleFallbackSendPressed()
{
    if (!MessageInputTextBox)
    {
        return;
    }

    const FText MessageText = MessageInputTextBox->GetText();
    if (MessageText.IsEmpty())
    {
        return;
    }

    HandleSendClicked(MessageText);
    MessageInputTextBox->SetText(FText::GetEmpty());
}

void UChatAppWidget::HandleFallbackUseOptionPressed()
{
    if (!HiddenOptionsComboBox)
    {
        return;
    }

    const FString SelectedOptionLabel = HiddenOptionsComboBox->GetSelectedOption();
    const FName* OptionId = FallbackOptionIdsByLabel.Find(SelectedOptionLabel);
    if (!OptionId || OptionId->IsNone())
    {
        return;
    }

    HandleHiddenOptionClicked(*OptionId);
}

void UChatAppWidget::HandleSelectColleagueA()
{
    HandleContactSelected(E_ColleagueId::ColleagueA);
}

void UChatAppWidget::HandleSelectColleagueB()
{
    HandleContactSelected(E_ColleagueId::ColleagueB);
}

void UChatAppWidget::HandleSelectColleagueC()
{
    HandleContactSelected(E_ColleagueId::ColleagueC);
}

void UChatAppWidget::HandleSelectSupervisor()
{
    HandleContactSelected(E_ColleagueId::Supervisor);
}

void UChatAppWidget::EnsureDefaultLayout()
{
    if (!WidgetTree)
    {
        return;
    }

    UWidget* ExistingRoot = WidgetTree->RootWidget;
    if (!FallbackRootOverlay)
    {
        const bool bAlreadyUsingFallbackOverlay =
            ExistingRoot &&
            ExistingRoot->GetFName() == TEXT("ChatFallbackOverlayRoot") &&
            ExistingRoot->IsA(UOverlay::StaticClass());

        if (bAlreadyUsingFallbackOverlay)
        {
            FallbackRootOverlay = Cast<UOverlay>(ExistingRoot);
        }
        else
        {
            FallbackRootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ChatFallbackOverlayRoot"));
            WidgetTree->RootWidget = FallbackRootOverlay;

            if (ExistingRoot)
            {
                if (UOverlaySlot* ExistingSlot = FallbackRootOverlay->AddChildToOverlay(ExistingRoot))
                {
                    ExistingSlot->SetHorizontalAlignment(HAlign_Fill);
                    ExistingSlot->SetVerticalAlignment(VAlign_Fill);
                }
            }
        }
    }

    if (!FallbackRootOverlay)
    {
        return;
    }

    if (HeaderTextBlock && FooterHintTextBlock && MessageListPanel && MessageInputTextBox && SendButton && HiddenOptionsComboBox && UseHiddenOptionButton)
    {
        return;
    }

    UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ChatFallbackRoot"));
    RootBorder->SetPadding(FMargin(16.0f));
    RootBorder->SetBrushColor(FLinearColor(0.16f, 0.20f, 0.30f, 0.98f));
    if (UOverlaySlot* RootSlot = FallbackRootOverlay->AddChildToOverlay(RootBorder))
    {
        RootSlot->SetHorizontalAlignment(HAlign_Fill);
        RootSlot->SetVerticalAlignment(VAlign_Fill);
        RootSlot->SetPadding(FMargin(300.0f, 96.0f, 60.0f, 80.0f));
    }

    UVerticalBox* RootLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ChatFallbackLayout"));
    RootBorder->SetContent(RootLayout);

    HeaderTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChatFallbackHeader"));
    HeaderTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UVerticalBoxSlot* HeaderSlot = RootLayout->AddChildToVerticalBox(HeaderTextBlock))
    {
        HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    UHorizontalBox* ContactRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ChatFallbackContacts"));
    if (UVerticalBoxSlot* ContactRowSlot = RootLayout->AddChildToVerticalBox(ContactRow))
    {
        ContactRowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    ContactAButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContactAButton"));
    ConfigureButtonLabel(WidgetTree, ContactAButton, TEXT("ContactALabel"), BuildFallbackContactLabel(E_ColleagueId::ColleagueA));
    ContactAButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleSelectColleagueA);
    if (UHorizontalBoxSlot* ContactSlot = ContactRow->AddChildToHorizontalBox(ContactAButton))
    {
        ContactSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    }

    ContactBButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContactBButton"));
    ConfigureButtonLabel(WidgetTree, ContactBButton, TEXT("ContactBLabel"), BuildFallbackContactLabel(E_ColleagueId::ColleagueB));
    ContactBButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleSelectColleagueB);
    if (UHorizontalBoxSlot* ContactSlot = ContactRow->AddChildToHorizontalBox(ContactBButton))
    {
        ContactSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    }

    ContactCButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContactCButton"));
    ConfigureButtonLabel(WidgetTree, ContactCButton, TEXT("ContactCLabel"), BuildFallbackContactLabel(E_ColleagueId::ColleagueC));
    ContactCButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleSelectColleagueC);
    if (UHorizontalBoxSlot* ContactSlot = ContactRow->AddChildToHorizontalBox(ContactCButton))
    {
        ContactSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    }

    SupervisorButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SupervisorButton"));
    ConfigureButtonLabel(WidgetTree, SupervisorButton, TEXT("SupervisorLabel"), BuildFallbackContactLabel(E_ColleagueId::Supervisor));
    SupervisorButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleSelectSupervisor);
    ContactRow->AddChildToHorizontalBox(SupervisorButton);

    MessageScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("MessageScrollBox"));
    if (UVerticalBoxSlot* ScrollSlot = RootLayout->AddChildToVerticalBox(MessageScrollBox))
    {
        ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        ScrollSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    MessageListPanel = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MessageListPanel"));
    MessageScrollBox->AddChild(MessageListPanel);

    UHorizontalBox* InputRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ChatFallbackInputRow"));
    if (UVerticalBoxSlot* InputRowSlot = RootLayout->AddChildToVerticalBox(InputRow))
    {
        InputRowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    MessageInputTextBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("MessageInputTextBox"));
    MessageInputTextBox->SetHintText(FText::FromString(TEXT("Type a quick message")));
    if (UHorizontalBoxSlot* InputSlot = InputRow->AddChildToHorizontalBox(MessageInputTextBox))
    {
        InputSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        InputSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    }

    SendButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SendButton"));
    ConfigureButtonLabel(WidgetTree, SendButton, TEXT("SendButtonLabel"), FText::FromString(TEXT("Send")));
    SendButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleFallbackSendPressed);
    InputRow->AddChildToHorizontalBox(SendButton);

    UHorizontalBox* HiddenOptionRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ChatFallbackHiddenOptionRow"));
    if (UVerticalBoxSlot* HiddenOptionRowSlot = RootLayout->AddChildToVerticalBox(HiddenOptionRow))
    {
        HiddenOptionRowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    HiddenOptionsComboBox = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("HiddenOptionsComboBox"));
    if (UHorizontalBoxSlot* ComboSlot = HiddenOptionRow->AddChildToHorizontalBox(HiddenOptionsComboBox))
    {
        ComboSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        ComboSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    }

    UseHiddenOptionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("UseHiddenOptionButton"));
    ConfigureButtonLabel(WidgetTree, UseHiddenOptionButton, TEXT("UseHiddenOptionLabel"), FText::FromString(TEXT("Use Option")));
    UseHiddenOptionButton->OnClicked.AddDynamic(this, &UChatAppWidget::HandleFallbackUseOptionPressed);
    HiddenOptionRow->AddChildToHorizontalBox(UseHiddenOptionButton);

    FooterHintTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChatFallbackFooterHint"));
    FooterHintTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.70f, 0.82f, 0.98f, 1.0f)));
    RootLayout->AddChildToVerticalBox(FooterHintTextBlock);
}

void UChatAppWidget::RefreshDefaultLayout(const TArray<FST_ChatMessageRecord>& Messages, const TArray<FST_HiddenOptionRecord>& Options)
{
    if (HeaderTextBlock)
    {
        HeaderTextBlock->SetText(FText::Format(
            FText::FromString(TEXT("NetSlack Fallback | Contact: {0}")),
            BuildContactDisplayName(CurrentContact)));
    }

    if (ContactAButton)
    {
        ContactAButton->SetIsEnabled(CurrentContact != E_ColleagueId::ColleagueA);
    }

    if (ContactBButton)
    {
        ContactBButton->SetIsEnabled(CurrentContact != E_ColleagueId::ColleagueB);
    }

    if (ContactCButton)
    {
        ContactCButton->SetIsEnabled(CurrentContact != E_ColleagueId::ColleagueC);
    }

    if (SupervisorButton)
    {
        SupervisorButton->SetIsEnabled(CurrentContact != E_ColleagueId::Supervisor);
    }

    if (MessageListPanel)
    {
        MessageListPanel->ClearChildren();

        for (const FST_ChatMessageRecord& Message : Messages)
        {
            const FString SpeakerLabel = Message.DisplayName.IsEmpty() ? BuildContactDisplayName(Message.SpeakerId).ToString() : Message.DisplayName.ToString();
            const FString Prefix = Message.bIsHiddenFragment ? TEXT("[Hidden] ") : Message.bIsSystemMessage ? TEXT("[System] ") : TEXT("");
            const FString RenderedText = FString::Printf(TEXT("%s%s: %s"), *Prefix, *SpeakerLabel, *Message.MessageText.ToString());

            UTextBlock* MessageTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            MessageTextBlock->SetAutoWrapText(true);
            MessageTextBlock->SetText(FText::FromString(RenderedText));
            MessageTextBlock->SetColorAndOpacity(FSlateColor(
                Message.bIsPlayer
                    ? FLinearColor(0.90f, 0.95f, 1.0f, 1.0f)
                    : Message.bIsHiddenFragment
                          ? FLinearColor(0.90f, 0.55f, 1.0f, 1.0f)
                          : FLinearColor(0.80f, 0.88f, 0.96f, 1.0f)));

            if (UVerticalBoxSlot* MessageSlot = MessageListPanel->AddChildToVerticalBox(MessageTextBlock))
            {
                MessageSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
            }
        }
    }

    if (MessageScrollBox)
    {
        MessageScrollBox->ScrollToEnd();
    }

    FallbackOptionIdsByLabel.Reset();
    if (HiddenOptionsComboBox)
    {
        HiddenOptionsComboBox->ClearOptions();
        for (const FST_HiddenOptionRecord& Option : Options)
        {
            const FString OptionLabel = Option.Label.IsEmpty() ? Option.OptionId.ToString() : Option.Label.ToString();
            HiddenOptionsComboBox->AddOption(OptionLabel);
            FallbackOptionIdsByLabel.Add(OptionLabel, Option.OptionId);
        }

        if (Options.Num() > 0)
        {
            const FString FirstLabel = Options[0].Label.IsEmpty() ? Options[0].OptionId.ToString() : Options[0].Label.ToString();
            HiddenOptionsComboBox->SetSelectedOption(FirstLabel);
        }
    }

    if (UseHiddenOptionButton)
    {
        UseHiddenOptionButton->SetIsEnabled(Options.Num() > 0);
    }

    if (FooterHintTextBlock)
    {
        FooterHintTextBlock->SetText(
            Options.Num() > 0
                ? FText::FromString(TEXT("Hidden route options are available for this contact."))
                : FText::FromString(TEXT("Send a message, switch contacts, or wait for a route option to unlock.")));
    }
}

FText UChatAppWidget::BuildContactDisplayName(E_ColleagueId Contact) const
{
    return BuildFallbackContactLabel(Contact);
}
