#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "HiddenDialogueUnlocker.generated.h"

class AChatConversationManager;
class ARouteStateManager;
class UDataTable;

UCLASS(Blueprintable)
class SIGNALPROJECT_API AHiddenDialogueUnlocker : public AActor
{
    GENERATED_BODY()

public:
    AHiddenDialogueUnlocker();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|HiddenDialogue")
    void UnlockHiddenOption(const FST_HiddenOptionRecord& HiddenOption);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|HiddenDialogue")
    void UnlockColleagueAHiddenOption();

public:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> SystemCopyTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|HiddenDialogue")
    FName StableOptionId = FName(TEXT("HIDDEN_LABEL_A_01"));
};
