#include "Interactables/ComputerTerminal.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/SignalGameFlowManager.h"
#include "Engine/StaticMesh.h"
#include "SignalSliceTypes.h"
#include "UObject/ConstructorHelpers.h"

AComputerTerminal::AComputerTerminal()
{
    PrimaryActorTick.bCanEverTick = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    TerminalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TerminalMesh"));
    TerminalMesh->SetupAttachment(SceneRoot);
    TerminalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    TerminalMesh->SetRelativeScale3D(FVector(0.8f, 0.35f, 0.6f));
    TerminalMesh->SetMobility(EComponentMobility::Static);
    TerminalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TerminalMesh->SetCollisionResponseToAllChannels(ECR_Block);
    TerminalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    TerminalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
    if (CubeMesh.Succeeded())
    {
        TerminalMesh->SetStaticMesh(CubeMesh.Object);
    }

    DeskViewAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("DeskViewAnchor"));
    DeskViewAnchor->SetupAttachment(SceneRoot);
    DeskViewAnchor->SetRelativeLocation(FVector(-70.0f, 0.0f, 78.0f));
    DeskViewAnchor->SetRelativeRotation(FRotator(-8.0f, 0.0f, 0.0f));
}

bool AComputerTerminal::CanInteract_Implementation() const
{
    return GameFlowManagerRef && GameFlowManagerRef->CanEnterDesktop();
}

FText AComputerTerminal::GetInteractionText_Implementation() const
{
    return FText::FromString(TEXT("Use Computer"));
}

void AComputerTerminal::Interact_Implementation(AActor* /*InteractingActor*/)
{
    if (!CanInteract_Implementation() || !GameFlowManagerRef)
    {
        return;
    }

    GameFlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
}
