#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "ChatAppWidget.generated.h"

class AChatConversationManager;
class ARouteStateManager;
class UButton;
class UComboBoxString;
class UEditableTextBox;
class UOverlay;
class UScrollBox;
class UTextBlock;
class UVerticalBox;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UChatAppWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|ChatUI")
    void InitializeForSlice(AChatConversationManager* InChatConversationManager, ARouteStateManager* InRouteStateManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|ChatUI")
    void RefreshConversation();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|ChatUI")
    void HandleSendClicked(const FText& MessageText);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|ChatUI")
    void HandleHiddenOptionClicked(FName OptionId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|ChatUI")
    void HandleContactSelected(E_ColleagueId Contact);

protected:
    UFUNCTION()
    void HandleMessagesUpdated();

    UFUNCTION()
    void HandleConversationChanged(E_ColleagueId NewTarget);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|ChatUI")
    void BP_OnConversationRefreshed(const TArray<FST_ChatMessageRecord>& Messages, const TArray<FST_HiddenOptionRecord>& Options);

private:
    UFUNCTION()
    void HandleFallbackSendPressed();

    UFUNCTION()
    void HandleFallbackUseOptionPressed();

    UFUNCTION()
    void HandleSelectColleagueA();

    UFUNCTION()
    void HandleSelectColleagueB();

    UFUNCTION()
    void HandleSelectColleagueC();

    UFUNCTION()
    void HandleSelectSupervisor();

    void EnsureDefaultLayout();
    void RefreshDefaultLayout(const TArray<FST_ChatMessageRecord>& Messages, const TArray<FST_HiddenOptionRecord>& Options);
    FText BuildContactDisplayName(E_ColleagueId Contact) const;

    UPROPERTY(Transient)
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|ChatUI", meta = (AllowPrivateAccess = "true"))
    E_ColleagueId CurrentContact = E_ColleagueId::ColleagueA;

    UPROPERTY(Transient)
    TObjectPtr<UOverlay> FallbackRootOverlay;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> HeaderTextBlock;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> FooterHintTextBlock;

    UPROPERTY(Transient)
    TObjectPtr<UButton> ContactAButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> ContactBButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> ContactCButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> SupervisorButton;

    UPROPERTY(Transient)
    TObjectPtr<UScrollBox> MessageScrollBox;

    UPROPERTY(Transient)
    TObjectPtr<UVerticalBox> MessageListPanel;

    UPROPERTY(Transient)
    TObjectPtr<UEditableTextBox> MessageInputTextBox;

    UPROPERTY(Transient)
    TObjectPtr<UButton> SendButton;

    UPROPERTY(Transient)
    TObjectPtr<UComboBoxString> HiddenOptionsComboBox;

    UPROPERTY(Transient)
    TObjectPtr<UButton> UseHiddenOptionButton;

    TMap<FString, FName> FallbackOptionIdsByLabel;
};
