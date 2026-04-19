#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "MinigameManager.generated.h"

class AAnomalyManager;
class ARouteStateManager;
class ASignalGameFlowManager;
class UAnomalyChoicePopupWidget;
class UDependencyMatchWidget;
class UDesktopRootWidget;

UCLASS(Blueprintable)
class SIGNALPROJECT_API AMinigameManager : public AActor
{
    GENERATED_BODY()

public:
    AMinigameManager();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void RegisterDesktopRoot(UDesktopRootWidget* InDesktopRoot);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void BindAnomalyCallbacks();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void ResetForDay(int32 DayIndex);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void StartMinigame(E_MinigameType MinigameType);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    bool CanLaunchStressTest() const;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void HandleSingleDraw();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void HandleTenDraw();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void PauseMinigame();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void ResumeMinigame();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void CompleteMinigame();

    UFUNCTION()
    void HandleAnomalyTriggered(E_AnomalyType TriggeredAnomaly);

    UFUNCTION()
    void HandleAnomalyReportAccepted(E_AnomalyType AcceptedAnomaly);

    UFUNCTION()
    void HandleAnomalyResolved();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Minigame")
    void HandleAnomalyChoice(E_RouteBranch SelectedBranch);

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    E_MinigameType CurrentMinigameType = E_MinigameType::DependencyMatch;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    TObjectPtr<UDependencyMatchWidget> CurrentMinigameWidget;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    TObjectPtr<UAnomalyChoicePopupWidget> ChoicePopupRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    bool bIsMinigameRunning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    int32 CompletedStressLoops = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    FST_StressTestViewState CurrentStressState;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Signal Slice|Minigame")
    TObjectPtr<UDesktopRootWidget> DesktopRootRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ASignalGameFlowManager> GameFlowManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AAnomalyManager> AnomalyManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UDependencyMatchWidget> DependencyMatchWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UAnomalyChoicePopupWidget> AnomalyChoicePopupClass;

private:
    void ApplyDraw(int32 DrawAmount);
    void RefreshStressWidgetState();
    void EndStressSession();
    E_AnomalyType ResolveTriggeredAnomalyForDraw(int32 DrawAmount) const;
    void ResetStressSessionViewState();

private:
    UPROPERTY(Transient)
    int32 CurrentDayIndex = 1;

    UPROPERTY(Transient)
    bool bAwaitingWorldResolutionCompletion = false;
};
