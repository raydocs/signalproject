#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "EndingTitleCardWidget.generated.h"

class UDataTable;

UCLASS(Abstract, Blueprintable)
class SIGNALPROJECT_API UEndingTitleCardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void SetupEnding(FName EndingId);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void HandleRestart();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Ending")
    void HandleQuit();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Ending")
    void BP_OnEndingSetup(FName EndingId, const TArray<FST_EndingSubtitleRow>& SubtitleRows);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Data")
    TObjectPtr<UDataTable> EndingSubtitlesTable;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Ending", meta = (AllowPrivateAccess = "true"))
    FName CurrentEndingId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Ending", meta = (AllowPrivateAccess = "true"))
    TArray<FST_EndingSubtitleRow> CurrentSubtitleRows;
};
