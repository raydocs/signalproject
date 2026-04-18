#include "UI/AnomalyChoicePopupWidget.h"

#include "Minigames/MinigameManager.h"

void UAnomalyChoicePopupWidget::InitializeForSlice(AMinigameManager* InMinigameManager)
{
    MinigameManagerRef = InMinigameManager;
}

void UAnomalyChoicePopupWidget::SetupForAnomaly(E_AnomalyType InAnomalyType)
{
    CurrentAnomalyType = InAnomalyType;
    BP_OnAnomalySetup(CurrentAnomalyType);
}

void UAnomalyChoicePopupWidget::HandleReportSupervisor()
{
    if (MinigameManagerRef)
    {
        MinigameManagerRef->HandleAnomalyChoice(E_RouteBranch::ReportSupervisor);
    }
}

void UAnomalyChoicePopupWidget::HandleHandleMyself()
{
    if (MinigameManagerRef)
    {
        MinigameManagerRef->HandleAnomalyChoice(E_RouteBranch::HandleMyself);
    }
}
