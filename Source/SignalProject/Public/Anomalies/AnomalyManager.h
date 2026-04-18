#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "AnomalyManager.generated.h"

class ASignalGameFlowManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalAnomalyTriggeredSignature, E_AnomalyType, TriggeredAnomaly);
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
    void ResolveCurrentAnomaly();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    float AccumulatedFreezeWeight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    E_AnomalyType CurrentAnomalyType = E_AnomalyType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    bool bAnomalyActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly", meta = (ClampMin = "0.1"))
    float FreezeThreshold = 3.0f;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ASignalGameFlowManager> GameFlowManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AActor> MinigameManagerRef;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Anomaly")
    FSignalAnomalyTriggeredSignature OnAnomalyTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Anomaly")
    FSignalAnomalyResolvedSignature OnAnomalyResolved;
};
