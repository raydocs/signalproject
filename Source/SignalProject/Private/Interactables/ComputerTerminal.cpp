#include "Interactables/ComputerTerminal.h"

#include "Components/SceneComponent.h"
#include "Core/SignalGameFlowManager.h"
#include "SignalSliceTypes.h"

AComputerTerminal::AComputerTerminal()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    DeskViewAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("DeskViewAnchor"));
    DeskViewAnchor->SetupAttachment(SceneRoot);
}

bool AComputerTerminal::CanInteract_Implementation() const
{
    return GameFlowManagerRef && GameFlowManagerRef->CanEnterDesktop();
}

FText AComputerTerminal::GetInteractionText_Implementation() const
{
    return FText::FromString(TEXT("Use Terminal"));
}

void AComputerTerminal::Interact_Implementation(AActor* /*InteractingActor*/)
{
    if (!CanInteract_Implementation() || !GameFlowManagerRef)
    {
        return;
    }

    GameFlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
}
