#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "ChatAppWidget.generated.h"

class AChatConversationManager;
class ARouteStateManager;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UChatAppWidget : public UUserWidget
{
    GENERATED_BODY()

public:
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
    UPROPERTY(Transient)
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|ChatUI", meta = (AllowPrivateAccess = "true"))
    E_ColleagueId CurrentContact = E_ColleagueId::ColleagueA;
};
