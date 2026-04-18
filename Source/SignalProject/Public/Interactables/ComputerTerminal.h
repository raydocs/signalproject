#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/Interactable.h"
#include "ComputerTerminal.generated.h"

class ASignalGameFlowManager;
class UDataTable;
class USceneComponent;

UCLASS(Blueprintable)
class SIGNALPROJECT_API AComputerTerminal : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    AComputerTerminal();

    virtual bool CanInteract_Implementation() const override;
    virtual FText GetInteractionText_Implementation() const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Interactable")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Interactable")
    TObjectPtr<USceneComponent> DeskViewAnchor;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Signal Slice|Refs")
    TObjectPtr<ASignalGameFlowManager> GameFlowManagerRef;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> SystemCopyTable;
};
