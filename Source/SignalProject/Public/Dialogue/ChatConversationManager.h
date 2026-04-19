#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "ChatConversationManager.generated.h"

class AAnomalyManager;
class ARouteStateManager;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSignalChatMessagesUpdatedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalConversationChangedSignature, E_ColleagueId, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalHiddenOptionInjectedSignature, FST_HiddenOptionRecord, HiddenOption);

UCLASS(Blueprintable)
class SIGNALPROJECT_API AChatConversationManager : public AActor
{
    GENERATED_BODY()

public:
    AChatConversationManager();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void InitializeChatSystem(int32 DayIndex);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void SwitchConversation(E_ColleagueId Target);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void SendPlayerMessage(const FText& MessageText);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void AppendReply(E_ColleagueId FromId, const FText& ReplyText);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void InjectHiddenOption(const FST_HiddenOptionRecord& HiddenOption);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    void PlayHiddenDialogue(E_ColleagueId Target);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    bool ConsumeHiddenOption(FName OptionId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Chat")
    TArray<FST_HiddenOptionRecord> GetVisibleOptionsForCurrentConversation() const;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Chat")
    E_ColleagueId CurrentConversationTarget = E_ColleagueId::ColleagueA;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Chat")
    TArray<FST_ChatMessageRecord> ConversationHistory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Chat")
    TArray<FST_HiddenOptionRecord> HiddenOptions;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> SupervisorLinesTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> NormalRepliesTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> HiddenDialoguesTable;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AAnomalyManager> AnomalyManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Chat")
    FSignalChatMessagesUpdatedSignature OnMessagesUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Chat")
    FSignalConversationChangedSignature OnConversationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Chat")
    FSignalHiddenOptionInjectedSignature OnHiddenOptionInjected;

private:
    int32 CurrentDayIndex = 1;

    TMap<E_ColleagueId, TArray<FST_ChatMessageRecord>> ThreadHistories;

    TMap<E_ColleagueId, int32> NormalReplyCursorByColleague;

    void SyncCurrentConversationFromThreadCache();
    FText ResolveDeterministicReply(E_ColleagueId TargetColleague);
};
