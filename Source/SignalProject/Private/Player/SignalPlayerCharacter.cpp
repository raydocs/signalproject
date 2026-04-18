#include "Player/SignalPlayerCharacter.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactables/Interactable.h"

ASignalPlayerCharacter::ASignalPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASignalPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    PerformInteractionTrace();
}

void ASignalPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ASignalPlayerCharacter::TryInteract);
    }
}

void ASignalPlayerCharacter::PerformInteractionTrace()
{
    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;

    if (Controller)
    {
        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else
    {
        ViewLocation = GetActorLocation();
        ViewRotation = GetActorRotation();
    }

    const FVector TraceEnd = ViewLocation + (ViewRotation.Vector() * InteractionTraceDistance);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(SignalInteractionTrace), false, this);
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, Params);

    AActor* NewFocusedActor = bHit ? Hit.GetActor() : nullptr;
    bool bNewCanInteract = false;
    FText NewPromptText;

    if (NewFocusedActor && NewFocusedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        bNewCanInteract = IInteractable::Execute_CanInteract(NewFocusedActor);
        NewPromptText = IInteractable::Execute_GetInteractionText(NewFocusedActor);
    }

    const bool bActorChanged = (CurrentFocusedInteractable != NewFocusedActor);
    const bool bPromptChanged = !CurrentPromptText.EqualTo(NewPromptText);

    CurrentFocusedInteractable = NewFocusedActor;
    bCanInteract = bNewCanInteract;

    if (bActorChanged || bPromptChanged)
    {
        CurrentPromptText = bCanInteract ? NewPromptText : FText::GetEmpty();
        OnInteractionPromptChanged.Broadcast(CurrentPromptText);
    }
}

void ASignalPlayerCharacter::TryInteract()
{
    if (!bCanInteract || !CurrentFocusedInteractable)
    {
        return;
    }

    if (!CurrentFocusedInteractable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        return;
    }

    if (!IInteractable::Execute_CanInteract(CurrentFocusedInteractable))
    {
        return;
    }

    IInteractable::Execute_Interact(CurrentFocusedInteractable, this);
}

void ASignalPlayerCharacter::SetMovementEnabled(bool bEnabled)
{
    GetCharacterMovement()->DisableMovement();

    if (bEnabled)
    {
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    }
}
