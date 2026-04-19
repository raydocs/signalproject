#include "Interactables/AirConditionerUnit.h"

#include "Anomalies/AnomalyManager.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/HiddenDialogueUnlocker.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AAirConditionerUnit::AAirConditionerUnit()
{
    PrimaryActorTick.bCanEverTick = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    AirConditionerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AirConditionerMesh"));
    AirConditionerMesh->SetupAttachment(SceneRoot);
    AirConditionerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));
    AirConditionerMesh->SetRelativeScale3D(FVector(0.9f, 0.28f, 0.35f));
    AirConditionerMesh->SetMobility(EComponentMobility::Static);
    AirConditionerMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    AirConditionerMesh->SetCollisionResponseToAllChannels(ECR_Block);
    AirConditionerMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    AirConditionerMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
    if (CubeMesh.Succeeded())
    {
        AirConditionerMesh->SetStaticMesh(CubeMesh.Object);
    }

    InteractionText = FText::FromString(TEXT("Repair Air Conditioner"));
}

bool AAirConditionerUnit::CanInteract_Implementation() const
{
    if (!bIsCurrentlyAvailable || !GameFlowManagerRef || !AnomalyManagerRef)
    {
        return false;
    }

    const bool bPhaseAllows = GameFlowManagerRef->GetCurrentPhase() == E_GamePhase::HandleAnomaly3D;
    const bool bMatchingAnomaly = AnomalyManagerRef->bAnomalyActive && AnomalyManagerRef->CurrentAnomalyType == HandledAnomalyType;
    return bPhaseAllows && bMatchingAnomaly;
}

FText AAirConditionerUnit::GetInteractionText_Implementation() const
{
    return InteractionText.IsEmpty() ? FText::FromString(TEXT("Repair Air Conditioner")) : InteractionText;
}

void AAirConditionerUnit::Interact_Implementation(AActor* /*InteractingActor*/)
{
    if (!CanInteract_Implementation())
    {
        return;
    }

    const FST_HiddenOptionRecord FollowupOption = AnomalyManagerRef ? AnomalyManagerRef->BuildCurrentFollowupOption() : FST_HiddenOptionRecord();

    if (AnomalyManagerRef)
    {
        AnomalyManagerRef->ResolveCurrentAnomaly();
    }

    if (HiddenDialogueUnlockerRef && FollowupOption.OptionId != NAME_None)
    {
        HiddenDialogueUnlockerRef->UnlockHiddenOption(FollowupOption);
    }

    bIsCurrentlyAvailable = false;

    if (GameFlowManagerRef)
    {
        GameFlowManagerRef->RequestPhaseChange(E_GamePhase::RoomExplore);
    }
}
