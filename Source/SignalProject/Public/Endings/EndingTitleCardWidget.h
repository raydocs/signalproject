#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "EndingTitleCardWidget.generated.h"

class UButton;
class UDataTable;
class UTextBlock;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UEndingTitleCardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void SetupEnding(FName EndingId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void HandleRestart();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void HandleQuit();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Ending")
    void BP_OnEndingSetup(FName EndingId, const TArray<FST_EndingSubtitleRow>& SubtitleRows);

    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Ending")
    void BP_OnEndingResultResolved(const FST_SliceEndingResult& EndingResult);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> EndingSubtitlesTable;

private:
    void EnsureDefaultLayout();
    void RefreshDefaultLayout();
    void BuildFallbackSubtitleRows(FName EndingId, TArray<FST_EndingSubtitleRow>& OutRows) const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Ending", meta = (AllowPrivateAccess = "true"))
    FName CurrentEndingId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Ending", meta = (AllowPrivateAccess = "true"))
    TArray<FST_EndingSubtitleRow> CurrentSubtitleRows;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Ending", meta = (AllowPrivateAccess = "true"))
    FST_SliceEndingResult CurrentEndingResult;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> SummaryText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> DetailText;

    UPROPERTY(Transient)
    TObjectPtr<UButton> RestartButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> QuitButton;
};
