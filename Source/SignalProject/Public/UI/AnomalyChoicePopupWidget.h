#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "AnomalyChoicePopupWidget.generated.h"

class AMinigameManager;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UAnomalyChoicePopupWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Signal Slice|AnomalyPopup")
    void InitializeForSlice(AMinigameManager* InMinigameManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|AnomalyPopup")
    void SetupForAnomaly(E_AnomalyType InAnomalyType);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|AnomalyPopup")
    void HandleReportSupervisor();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|AnomalyPopup")
    void HandleHandleMyself();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|AnomalyPopup")
    void BP_OnAnomalySetup(E_AnomalyType InAnomalyType);

private:
    UPROPERTY(Transient)
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|AnomalyPopup", meta = (AllowPrivateAccess = "true"))
    E_AnomalyType CurrentAnomalyType = E_AnomalyType::None;
};
