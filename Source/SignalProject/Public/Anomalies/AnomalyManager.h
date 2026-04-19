#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "AnomalyManager.generated.h"

class ASignalGameFlowManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalAnomalyTriggeredSignature, E_AnomalyType, TriggeredAnomaly);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalAnomalyReportAcceptedSignature, E_AnomalyType, AcceptedAnomaly);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSignalAnomalyResolvedSignature);

UCLASS(Blueprintable)
class SIGNALPROJECT_API AAnomalyManager : public AActor
{
    GENERATED_BODY()

public:
    AAnomalyManager();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    void AccumulateSideEffect(E_ColleagueId SourceColleague, float Amount);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    E_AnomalyType ResolveDominantAnomaly() const;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    void TriggerAnomaly(E_AnomalyType NewAnomaly);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    bool TrySubmitIssueReport(E_ColleagueId TargetColleague);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    void ResolveCurrentAnomaly();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Anomaly")
    void ResetForDay(int32 DayIndex);

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Anomaly")
    FST_HiddenOptionRecord BuildCurrentIssueReportOption() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Anomaly")
    FST_HiddenOptionRecord BuildCurrentFollowupOption() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Anomaly")
    FText GetCurrentReportReplyText() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Anomaly")
    FText GetFollowupReplyText(E_AnomalyType ForAnomaly) const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Anomaly")
    FText GetStressStatusText(E_AnomalyType ForAnomaly) const;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    float AccumulatedFreezeWeight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    E_AnomalyType CurrentAnomalyType = E_AnomalyType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    bool bAnomalyActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    bool bAwaitingIssueReport = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    bool bHasAcceptedIssueReport = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly", meta = (ClampMin = "0.1"))
    float FreezeThreshold = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    TArray<FST_AnomalyRouteConfig> RouteConfigs;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ASignalGameFlowManager> GameFlowManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AActor> MinigameManagerRef;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Anomaly")
    FSignalAnomalyTriggeredSignature OnAnomalyTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Anomaly")
    FSignalAnomalyReportAcceptedSignature OnAnomalyReportAccepted;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Anomaly")
    FSignalAnomalyResolvedSignature OnAnomalyResolved;

private:
    const FST_AnomalyRouteConfig* FindRouteConfig(E_AnomalyType ForAnomaly) const;
};
