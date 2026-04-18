#include "Minigames/DependencyMatchWidget.h"

#include "Minigames/MinigameManager.h"

void UDependencyMatchWidget::InitializeForSlice(AMinigameManager* InMinigameManager)
{
    MinigameManagerRef = InMinigameManager;
}

void UDependencyMatchWidget::BuildGrid()
{
    BP_OnGridBuildRequested();
}

void UDependencyMatchWidget::HandleTileClicked(int32 TileId)
{
    if (bIsPaused || TileId < 0 || !MinigameManagerRef)
    {
        return;
    }

    MinigameManagerRef->CompleteMinigame();
}

void UDependencyMatchWidget::SetPaused(bool bInPaused)
{
    bIsPaused = bInPaused;
    BP_OnPauseChanged(bIsPaused);
}
