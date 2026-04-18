#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(BlueprintType)
class SIGNALPROJECT_API UInteractable : public UInterface
{
    GENERATED_BODY()
};

class SIGNALPROJECT_API IInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Signal Slice|Interactable")
    bool CanInteract() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Signal Slice|Interactable")
    FText GetInteractionText() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Signal Slice|Interactable")
    void Interact(AActor* InteractingActor);
};
