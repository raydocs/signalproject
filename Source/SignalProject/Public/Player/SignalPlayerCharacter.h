#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SignalPlayerCharacter.generated.h"

class IInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSignalInteractionPromptChangedSignature, const FText&, PromptText);

UCLASS(Blueprintable)
class SIGNALPROJECT_API ASignalPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASignalPlayerCharacter();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Interaction")
    void PerformInteractionTrace();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Interaction")
    void TryInteract();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Interaction")
    void SetMovementEnabled(bool bEnabled);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Interaction", meta = (ClampMin = "50.0"))
    float InteractionTraceDistance = 350.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Interaction")
    bool bCanInteract = false;

    UPROPERTY(BlueprintAssignable, Category = "Signal Slice|Interaction")
    FSignalInteractionPromptChangedSignature OnInteractionPromptChanged;

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> CurrentFocusedInteractable;

    UPROPERTY(Transient)
    FText CurrentPromptText;
};
