#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/Interactable.h"
#include "SignalSliceTypes.h"
#include "AirConditionerUnit.generated.h"

class AAnomalyManager;
class AHiddenDialogueUnlocker;
class ASignalGameFlowManager;
class UDataTable;
class USceneComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class SIGNALPROJECT_API AAirConditionerUnit : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    AAirConditionerUnit();

    virtual bool CanInteract_Implementation() const override;
    virtual FText GetInteractionText_Implementation() const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    bool bIsCurrentlyAvailable = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    E_AnomalyType HandledAnomalyType = E_AnomalyType::FREEZE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Anomaly")
    FText InteractionText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Interactable")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Interactable")
    TObjectPtr<UStaticMeshComponent> AirConditionerMesh;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AAnomalyManager> AnomalyManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<AHiddenDialogueUnlocker> HiddenDialogueUnlockerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ASignalGameFlowManager> GameFlowManagerRef;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> SystemCopyTable;
};
