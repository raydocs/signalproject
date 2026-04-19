#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalSliceTypes.h"
#include "TaskListWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS(Blueprintable)
class SIGNALPROJECT_API UTaskListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Tasks")
    void ApplyTaskListState(const FText& InDayTitle, const TArray<FST_DesktopTaskRecord>& InTasks);

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Signal Slice|Tasks")
    void BP_OnTaskListChanged(const FText& DayTitle, const TArray<FST_DesktopTaskRecord>& Tasks);

private:
    void EnsureDefaultLayout();
    void RefreshDefaultLayout();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Tasks", meta = (AllowPrivateAccess = "true"))
    FText CurrentDayTitle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Tasks", meta = (AllowPrivateAccess = "true"))
    TArray<FST_DesktopTaskRecord> CurrentTasks;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> DayTitleText;

    UPROPERTY(Transient)
    TObjectPtr<UVerticalBox> TaskListBox;
};
