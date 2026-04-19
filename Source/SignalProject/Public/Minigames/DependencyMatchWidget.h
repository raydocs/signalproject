#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "DependencyMatchWidget.generated.h"

class AMinigameManager;
class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UDependencyMatchWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void InitializeForSlice(AMinigameManager* InMinigameManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void BuildGrid();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void HandleTileClicked(int32 TileId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void SetPaused(bool bInPaused);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void ApplyStressState(const FST_StressTestViewState& InState);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void HandleSingleDrawClicked();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|MinigameUI")
    void HandleTenDrawClicked();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|MinigameUI")
    void BP_OnGridBuildRequested();

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|MinigameUI")
    void BP_OnPauseChanged(bool bInPaused);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|MinigameUI")
    void BP_OnStressStateChanged(const FST_StressTestViewState& State);

private:
    UFUNCTION()
    void HandleSingleDrawButtonPressed();

    UFUNCTION()
    void HandleTenDrawButtonPressed();

    void EnsureDefaultLayout();
    void RefreshDefaultLayout();

private:
    UPROPERTY(Transient)
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|MinigameUI", meta = (AllowPrivateAccess = "true"))
    bool bIsPaused = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|MinigameUI", meta = (AllowPrivateAccess = "true"))
    FST_StressTestViewState CurrentStressState;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> HeaderText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> OutputTextBlock;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> StatusTextBlock;

    UPROPERTY(Transient)
    TObjectPtr<UButton> SingleDrawButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> TenDrawButton;
};
