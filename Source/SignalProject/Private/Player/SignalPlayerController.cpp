#include "Player/SignalPlayerController.h"

#include "Anomalies/AnomalyManager.h"
#include "Components/InputComponent.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Endings/EndingTitleCardWidget.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "Minigames/MinigameManager.h"
#include "Player/SignalPlayerCharacter.h"
#include "SignalSliceTypes.h"
#include "UI/DesktopRootWidget.h"
#include "Widgets/Layout/Anchors.h"

ASignalPlayerController::ASignalPlayerController()
{
    bShowMouseCursor = false;
}

void ASignalPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ApplyStartupViewRotation();
}

void ASignalPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ApplyStartupViewRotation();
}

void ASignalPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction(TEXT("LeaveDesktop"), IE_Pressed, this, &ASignalPlayerController::HandleLeaveDesktopPressed);
#if !UE_BUILD_SHIPPING
        InputComponent->BindKey(EKeys::Home, IE_Pressed, this, &ASignalPlayerController::HandleDebugOpenDesktopPressed);
        InputComponent->BindKey(EKeys::PageUp, IE_Pressed, this, &ASignalPlayerController::HandleDebugOpenChatPressed);
#endif
    }
}

void ASignalPlayerController::RegisterSliceRefs(
    ASignalGameFlowManager* InFlowManager,
    ARouteStateManager* InRouteStateManager,
    AAnomalyManager* InAnomalyManager,
    AChatConversationManager* InChatConversationManager,
    AMinigameManager* InMinigameManager)
{
    FlowManagerRef = InFlowManager;
    RouteStateManagerRef = InRouteStateManager;
    AnomalyManagerRef = InAnomalyManager;
    ChatConversationManagerRef = InChatConversationManager;
    MinigameManagerRef = InMinigameManager;

    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->InitializeForSlice(FlowManagerRef, RouteStateManagerRef, ChatConversationManagerRef, MinigameManagerRef);
    }

    if (!FlowManagerRef)
    {
        return;
    }

    switch (FlowManagerRef->GetCurrentPhase())
    {
    case E_GamePhase::DesktopIdle:
        ShowDesktopRoot();
        break;
    case E_GamePhase::ChatActive:
        OpenDesktopApp(FName(TEXT("Chat")));
        break;
    case E_GamePhase::ReportPhase:
        OpenDesktopApp(FName(TEXT("Report")));
        break;
    default:
        break;
    }
}

void ASignalPlayerController::SetRoomInputMode()
{
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
    bIsInDesktopMode = false;
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    SetPawnMovementEnabled(true);
}

void ASignalPlayerController::SetDesktopInputMode()
{
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    bIsInDesktopMode = true;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    SetPawnMovementEnabled(false);
}

bool ASignalPlayerController::ShowDesktopRoot()
{
    if (!CachedDesktopRoot)
    {
        if (!DesktopRootWidgetClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShowDesktopRoot failed: DesktopRootWidgetClass is null."));
            return false;
        }

        CachedDesktopRoot = CreateWidget<UDesktopRootWidget>(this, DesktopRootWidgetClass);
        if (!CachedDesktopRoot)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShowDesktopRoot failed: CreateWidget returned null."));
            return false;
        }

        CachedDesktopRoot->InitializeForSlice(FlowManagerRef, RouteStateManagerRef, ChatConversationManagerRef, MinigameManagerRef);
        UE_LOG(LogTemp, Display, TEXT("Desktop root widget created: %s"), *GetNameSafe(CachedDesktopRoot));
    }

    if (!CachedDesktopRoot->IsInViewport())
    {
        CachedDesktopRoot->AddToViewport();
        UE_LOG(LogTemp, Display, TEXT("Desktop root widget added to viewport."));
    }

    CachedDesktopRoot->SetVisibility(ESlateVisibility::Visible);
    CachedDesktopRoot->SetIsEnabled(true);
    CachedDesktopRoot->SetRenderOpacity(1.0f);
    CachedDesktopRoot->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
    CachedDesktopRoot->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
    CachedDesktopRoot->SetPositionInViewport(FVector2D::ZeroVector, false);
    CachedDesktopRoot->SetDesiredSizeInViewport(FVector2D(1280.0f, 720.0f));

    return true;
}

void ASignalPlayerController::HideDesktopRoot()
{
    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->RemoveFromParent();
    }

    bIsInDesktopMode = false;
}

bool ASignalPlayerController::OpenDesktopApp(FName AppTag)
{
    if (!ShowDesktopRoot() || !CachedDesktopRoot)
    {
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("Opening desktop app: %s"), *AppTag.ToString());
    CachedDesktopRoot->OpenApp(AppTag);
    return true;
}

void ASignalPlayerController::RefreshDesktopRootAvailability()
{
    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->RefreshAppAvailability();
    }
}

bool ASignalPlayerController::ShowEndingTitleCard(FName EndingId)
{
    if (!CachedEndingTitleCard)
    {
        if (!EndingTitleCardWidgetClass)
        {
            return false;
        }

        CachedEndingTitleCard = CreateWidget<UEndingTitleCardWidget>(this, EndingTitleCardWidgetClass);
        if (!CachedEndingTitleCard)
        {
            return false;
        }
    }

    CachedEndingTitleCard->SetupEnding(EndingId);

    if (!CachedEndingTitleCard->IsInViewport())
    {
        CachedEndingTitleCard->AddToViewport(100);
    }

    return true;
}

void ASignalPlayerController::HandleLeaveDesktopPressed()
{
    if (!FlowManagerRef)
    {
        return;
    }

    const E_GamePhase Phase = FlowManagerRef->GetCurrentPhase();
    if (Phase == E_GamePhase::DesktopIdle || Phase == E_GamePhase::ChatActive)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::RoomExplore);
    }
}

void ASignalPlayerController::HandleDebugOpenDesktopPressed()
{
#if !UE_BUILD_SHIPPING
    if (FlowManagerRef)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::DesktopIdle);
    }
#endif
}

void ASignalPlayerController::HandleDebugOpenChatPressed()
{
#if !UE_BUILD_SHIPPING
    if (FlowManagerRef)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::ChatActive);
    }
#endif
}

void ASignalPlayerController::ApplyStartupViewRotation()
{
    if (!bApplyStartupViewRotation)
    {
        return;
    }

    SetControlRotation(StartupViewRotation);

    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->FaceRotation(FRotator(0.0f, StartupViewRotation.Yaw, 0.0f), 0.0f);
    }

    UE_LOG(
        LogTemp,
        Display,
        TEXT("Applied startup view rotation: Pitch=%.2f Yaw=%.2f Roll=%.2f"),
        StartupViewRotation.Pitch,
        StartupViewRotation.Yaw,
        StartupViewRotation.Roll);
}

void ASignalPlayerController::SetPawnMovementEnabled(bool bEnabled)
{
    if (ASignalPlayerCharacter* SignalCharacter = Cast<ASignalPlayerCharacter>(GetPawn()))
    {
        SignalCharacter->SetMovementEnabled(bEnabled);
    }
}
