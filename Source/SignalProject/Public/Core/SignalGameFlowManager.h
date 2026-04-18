#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "SignalGameFlowManager.generated.h"

class AAnomalyManager;
class AChatConversationManager;
class AHiddenDialogueUnlocker;
class AMinigameManager;
class ARouteStateManager;
class ASignalPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSignalPhaseChangedSignature, E_GamePhase, PreviousPhase, E_GamePhase, NewPhase);

UCLASS(Blueprintable)
class SIGNALPROJECT_API ASignalGameFlowManager : public AActor
{
    GENERATED_BODY()

public:
    ASignalGameFlowManager();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    void InitializeGameFlow();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    void StartDay(int32 DayIndex);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    bool RequestPhaseChange(E_GamePhase NewPhase);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    void HandlePhaseEntered(E_GamePhase NewPhase);

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Flow")
    bool CanEnterDesktop() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Flow")
    bool CanOpenReport() const;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    void SubmitSliceReport(FName SelectedSentenceId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Flow")
    void TriggerEnding(FName EndingId);

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Flow")
    E_GamePhase GetCurrentPhase() const { return CurrentPhase; }

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Flow")
    int32 CurrentDayIndex = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Flow")
    E_GamePhase CurrentPhase = E_GamePhase::Boot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Flow")
    E_GamePhase PreviousPhase = E_GamePhase::Boot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Flow")
    FName PendingEndingId = NAME_None;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AAnomalyManager> AnomalyManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AHiddenDialogueUnlocker> HiddenDialogueUnlockerRef;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Flow")
    FSignalPhaseChangedSignature OnPhaseChanged;

private:
    E_GamePhase ResolveStartupPhase() const;

    UPROPERTY(Transient)
    TObjectPtr<ASignalPlayerController> CachedPlayerController;
};
