#include "Player/SignalPlayerController.h"

#include "Anomalies/AnomalyManager.h"
#include "Components/InputComponent.h"
#include "Core/RouteStateManager.h"
#include "Core/SignalGameFlowManager.h"
#include "Dialogue/ChatConversationManager.h"
#include "Endings/EndingTitleCardWidget.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "Minigames/MinigameManager.h"
#include "Player/SignalPlayerCharacter.h"
#include "SignalSliceTypes.h"
#include "UI/DesktopRootWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/Anchors.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SWeakWidget.h"
#include "Widgets/Text/STextBlock.h"

class SSignalNativeDesktopOverlay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSignalNativeDesktopOverlay) {}
        SLATE_ARGUMENT(TWeakObjectPtr<ASignalPlayerController>, OwningController)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        OwningController = InArgs._OwningController;

        ChildSlot
        [
            SNew(SBox)
            .WidthOverride(420.0f)
            [
                SNew(SBorder)
                .Padding(FMargin(16.0f))
                .BorderBackgroundColor(FLinearColor(0.05f, 0.08f, 0.12f, 0.92f))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SAssignNew(PhaseTextBlock, STextBlock)
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 8.0f, 0.0f, 12.0f)
                    [
                        SAssignNew(AppTextBlock, STextBlock)
                        .ColorAndOpacity(FLinearColor(0.75f, 0.85f, 1.0f, 1.0f))
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(0.0f, 0.0f, 12.0f, 0.0f)
                        [
                            SNew(SButton)
                            .Text(FText::FromString(TEXT("Open Chat")))
                            .OnClicked(this, &SSignalNativeDesktopOverlay::HandleOpenChatClicked)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(0.0f, 0.0f, 12.0f, 0.0f)
                        [
                            SAssignNew(ReportButton, SButton)
                            .Text(FText::FromString(TEXT("Open Report")))
                            .OnClicked(this, &SSignalNativeDesktopOverlay::HandleOpenReportClicked)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .Text(FText::FromString(TEXT("Return Room")))
                            .OnClicked(this, &SSignalNativeDesktopOverlay::HandleReturnToRoomClicked)
                        ]
                    ]
                ]
            ]
        ];
    }

    void Refresh(const FText& PhaseText, const FText& AppText, bool bCanOpenReport)
    {
        if (PhaseTextBlock.IsValid())
        {
            PhaseTextBlock->SetText(PhaseText);
        }

        if (AppTextBlock.IsValid())
        {
            AppTextBlock->SetText(AppText);
        }

        if (ReportButton.IsValid())
        {
            ReportButton->SetEnabled(bCanOpenReport);
        }
    }

private:
    FReply HandleOpenChatClicked()
    {
        if (OwningController.IsValid())
        {
            OwningController->HandleNativeOverlayOpenChat();
        }
        return FReply::Handled();
    }

    FReply HandleOpenReportClicked()
    {
        if (OwningController.IsValid())
        {
            OwningController->HandleNativeOverlayOpenReport();
        }
        return FReply::Handled();
    }

    FReply HandleReturnToRoomClicked()
    {
        if (OwningController.IsValid())
        {
            OwningController->HandleNativeOverlayReturnToRoom();
        }
        return FReply::Handled();
    }

    TWeakObjectPtr<ASignalPlayerController> OwningController;
    TSharedPtr<STextBlock> PhaseTextBlock;
    TSharedPtr<STextBlock> AppTextBlock;
    TSharedPtr<SButton> ReportButton;
};

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
        InputComponent->BindKey(EKeys::F6, IE_Pressed, this, &ASignalPlayerController::HandleDebugOpenTasksPressed);
        InputComponent->BindKey(EKeys::F7, IE_Pressed, this, &ASignalPlayerController::HandleDebugOpenStressTestPressed);
        InputComponent->BindKey(EKeys::F8, IE_Pressed, this, &ASignalPlayerController::HandleDebugOpenReportPressed);
        InputComponent->BindKey(EKeys::F9, IE_Pressed, this, &ASignalPlayerController::HandleDebugShowRoutineEndingPressed);
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
    EnsureNativeDebugDesktopOverlay();

    if (!CachedDesktopRoot)
    {
#if !UE_BUILD_SHIPPING
        const bool bUseNativeDebugDesktopRoot = true;
#else
        const bool bUseNativeDebugDesktopRoot = false;
#endif
        const TSubclassOf<UDesktopRootWidget> ResolvedDesktopRootClass =
            bUseNativeDebugDesktopRoot
                ? TSubclassOf<UDesktopRootWidget>(UDesktopRootWidget::StaticClass())
                : (DesktopRootWidgetClass ? DesktopRootWidgetClass : TSubclassOf<UDesktopRootWidget>(UDesktopRootWidget::StaticClass()));

        UE_LOG(
            LogTemp,
            Display,
            TEXT("ShowDesktopRoot using class: configured=%s resolved=%s native_debug=%s"),
            *GetNameSafe(DesktopRootWidgetClass),
            *GetNameSafe(ResolvedDesktopRootClass),
            bUseNativeDebugDesktopRoot ? TEXT("true") : TEXT("false"));

        if (!DesktopRootWidgetClass && !bUseNativeDebugDesktopRoot)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShowDesktopRoot: DesktopRootWidgetClass is null, falling back to native UDesktopRootWidget."));
        }

        CachedDesktopRoot = CreateWidget<UDesktopRootWidget>(this, ResolvedDesktopRootClass);
        if (!CachedDesktopRoot)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShowDesktopRoot failed: CreateWidget returned null."));
        }

        if (CachedDesktopRoot)
        {
            CachedDesktopRoot->InitializeForSlice(FlowManagerRef, RouteStateManagerRef, ChatConversationManagerRef, MinigameManagerRef);
            UE_LOG(LogTemp, Display, TEXT("Desktop root widget created: %s"), *GetNameSafe(CachedDesktopRoot));
        }
    }

    if (CachedDesktopRoot && !CachedDesktopRoot->IsInViewport())
    {
        CachedDesktopRoot->AddToViewport();
        UE_LOG(LogTemp, Display, TEXT("Desktop root widget added to viewport."));
    }

    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->SetVisibility(ESlateVisibility::Visible);
        CachedDesktopRoot->SetIsEnabled(true);
        CachedDesktopRoot->SetRenderOpacity(1.0f);
        CachedDesktopRoot->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        CachedDesktopRoot->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
        CachedDesktopRoot->SetPositionInViewport(FVector2D::ZeroVector, false);
        CachedDesktopRoot->SetDesiredSizeInViewport(FVector2D(1280.0f, 720.0f));
    }

    NativeDebugActiveAppTag = NAME_None;
    RefreshNativeDebugDesktopOverlay();

    return CachedDesktopRoot != nullptr || NativeDebugDesktopOverlay.IsValid();
}

void ASignalPlayerController::HideDesktopRoot()
{
    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->RemoveFromParent();
    }

    NativeDebugActiveAppTag = NAME_None;
    RemoveNativeDebugDesktopOverlay();
    bIsInDesktopMode = false;
}

bool ASignalPlayerController::OpenDesktopApp(FName AppTag)
{
    if (!ShowDesktopRoot())
    {
        return false;
    }

    NativeDebugActiveAppTag = AppTag;
    RefreshNativeDebugDesktopOverlay();
    UE_LOG(LogTemp, Display, TEXT("Opening desktop app: %s"), *AppTag.ToString());

    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->OpenApp(AppTag);
        return true;
    }

    return NativeDebugDesktopOverlay.IsValid();
}

void ASignalPlayerController::RefreshDesktopRootAvailability()
{
    if (CachedDesktopRoot)
    {
        CachedDesktopRoot->RefreshAppAvailability();
    }

    RefreshNativeDebugDesktopOverlay();
}

bool ASignalPlayerController::ShowEndingTitleCard(FName EndingId)
{
    if (!CachedEndingTitleCard)
    {
#if !UE_BUILD_SHIPPING
        const bool bUseNativeDebugEndingCard = true;
#else
        const bool bUseNativeDebugEndingCard = false;
#endif
        const TSubclassOf<UEndingTitleCardWidget> ResolvedEndingTitleCardClass =
            bUseNativeDebugEndingCard
                ? TSubclassOf<UEndingTitleCardWidget>(UEndingTitleCardWidget::StaticClass())
                : EndingTitleCardWidgetClass;

        if (!ResolvedEndingTitleCardClass)
        {
            return false;
        }

        CachedEndingTitleCard = CreateWidget<UEndingTitleCardWidget>(this, ResolvedEndingTitleCardClass);
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

void ASignalPlayerController::HandleDebugOpenTasksPressed()
{
#if !UE_BUILD_SHIPPING
    OpenDesktopApp(FName(TEXT("Tasks")));
#endif
}

void ASignalPlayerController::HandleDebugOpenStressTestPressed()
{
#if !UE_BUILD_SHIPPING
    OpenDesktopApp(FName(TEXT("StressTest")));
#endif
}

void ASignalPlayerController::HandleDebugOpenReportPressed()
{
#if !UE_BUILD_SHIPPING
    HandleNativeOverlayOpenReport();
#endif
}

void ASignalPlayerController::HandleDebugShowRoutineEndingPressed()
{
#if !UE_BUILD_SHIPPING
    ShowEndingTitleCard(FName(TEXT("RoutineEnding")));
#endif
}

void ASignalPlayerController::EnsureNativeDebugDesktopOverlay()
{
    if (NativeDebugDesktopOverlay.IsValid())
    {
        RefreshNativeDebugDesktopOverlay();
        return;
    }

    UGameViewportClient* GameViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr;
    if (!GameViewportClient)
    {
        return;
    }

    SAssignNew(NativeDebugDesktopOverlay, SSignalNativeDesktopOverlay)
        .OwningController(this);

    NativeDebugDesktopOverlayHost =
        SNew(SWeakWidget)
        .PossiblyNullContent(NativeDebugDesktopOverlay.ToSharedRef());

    GameViewportClient->AddViewportWidgetContent(NativeDebugDesktopOverlayHost.ToSharedRef(), 2000);
    UE_LOG(LogTemp, Display, TEXT("Native debug desktop overlay added to viewport."));
    RefreshNativeDebugDesktopOverlay();
}

void ASignalPlayerController::RemoveNativeDebugDesktopOverlay()
{
    UGameViewportClient* GameViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr;
    if (GameViewportClient && NativeDebugDesktopOverlayHost.IsValid())
    {
        GameViewportClient->RemoveViewportWidgetContent(NativeDebugDesktopOverlayHost.ToSharedRef());
        UE_LOG(LogTemp, Display, TEXT("Native debug desktop overlay removed from viewport."));
    }

    NativeDebugDesktopOverlay.Reset();
    NativeDebugDesktopOverlayHost.Reset();
}

void ASignalPlayerController::RefreshNativeDebugDesktopOverlay()
{
    if (!NativeDebugDesktopOverlay.IsValid())
    {
        return;
    }

    const UEnum* PhaseEnum = StaticEnum<E_GamePhase>();
    const FString PhaseName =
        FlowManagerRef && PhaseEnum
            ? PhaseEnum->GetNameStringByValue(static_cast<int64>(FlowManagerRef->GetCurrentPhase()))
            : TEXT("Unknown");

    FString AppLabel = TEXT("Active App: None");
    if (!NativeDebugActiveAppTag.IsNone())
    {
        AppLabel = FString::Printf(TEXT("Active App: %s"), *NativeDebugActiveAppTag.ToString());
    }
    else if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() == E_GamePhase::ChatActive)
    {
        AppLabel = TEXT("Active App: Chat requested");
    }
    else if (FlowManagerRef && FlowManagerRef->GetCurrentPhase() == E_GamePhase::ReportPhase)
    {
        AppLabel = TEXT("Active App: Report requested");
    }

    NativeDebugDesktopOverlay->Refresh(
        FText::FromString(FString::Printf(TEXT("Native Desktop Debug\nPhase: %s"), *PhaseName)),
        FText::FromString(AppLabel),
        FlowManagerRef && FlowManagerRef->CanOpenReport());
}

void ASignalPlayerController::HandleNativeOverlayOpenChat()
{
    if (!FlowManagerRef)
    {
        return;
    }

    if (FlowManagerRef->GetCurrentPhase() != E_GamePhase::ChatActive)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::ChatActive);
        return;
    }

    OpenDesktopApp(FName(TEXT("Chat")));
}

void ASignalPlayerController::HandleNativeOverlayOpenReport()
{
    if (!FlowManagerRef || !FlowManagerRef->CanOpenReport())
    {
        RefreshNativeDebugDesktopOverlay();
        return;
    }

    if (FlowManagerRef->GetCurrentPhase() != E_GamePhase::ReportPhase)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::ReportPhase);
        return;
    }

    OpenDesktopApp(FName(TEXT("Report")));
}

void ASignalPlayerController::HandleNativeOverlayReturnToRoom()
{
    if (FlowManagerRef)
    {
        FlowManagerRef->RequestPhaseChange(E_GamePhase::RoomExplore);
    }
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
