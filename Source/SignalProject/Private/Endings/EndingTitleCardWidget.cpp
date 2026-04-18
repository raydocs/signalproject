#include "Endings/EndingTitleCardWidget.h"

#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UEndingTitleCardWidget::SetupEnding(FName EndingId)
{
    CurrentEndingId = EndingId;
    CurrentSubtitleRows.Reset();

    if (EndingSubtitlesTable && CurrentEndingId != NAME_None)
    {
        TArray<FST_EndingSubtitleRow*> Rows;
        EndingSubtitlesTable->GetAllRows(TEXT("SetupEnding"), Rows);

        for (const FST_EndingSubtitleRow* Row : Rows)
        {
            if (Row && Row->EndingId == CurrentEndingId)
            {
                CurrentSubtitleRows.Add(*Row);
            }
        }

        CurrentSubtitleRows.Sort([](const FST_EndingSubtitleRow& A, const FST_EndingSubtitleRow& B)
        {
            return A.SequenceOrder < B.SequenceOrder;
        });
    }

    BP_OnEndingSetup(CurrentEndingId, CurrentSubtitleRows);
}

void UEndingTitleCardWidget::HandleRestart()
{
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::OpenLevel(this, *World->GetName());
    }
}

void UEndingTitleCardWidget::HandleQuit()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
