#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DependencyMatchWidget.generated.h"

class AMinigameManager;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UDependencyMatchWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void InitializeForSlice(AMinigameManager* InMinigameManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void BuildGrid();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void HandleTileClicked(int32 TileId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void SetPaused(bool bInPaused);

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|MinigameUI")
    void BP_OnGridBuildRequested();

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|MinigameUI")
    void BP_OnPauseChanged(bool bInPaused);

private:
    UPROPERTY(Transient)
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|MinigameUI", meta = (AllowPrivateAccess = "true"))
    bool bIsPaused = false;
};
