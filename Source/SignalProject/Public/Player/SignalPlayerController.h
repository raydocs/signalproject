#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SignalPlayerController.generated.h"

class AAnomalyManager;
class AChatConversationManager;
class AMinigameManager;
class ARouteStateManager;
class SSignalNativeDesktopOverlay;
class ASignalGameFlowManager;
class UDesktopRootWidget;
class UEndingTitleCardWidget;

UCLASS(Blueprintable)
class SIGNALPROJECT_API ASignalPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASignalPlayerController();

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    void RegisterSliceRefs(
        ASignalGameFlowManager* InFlowManager,
        ARouteStateManager* InRouteStateManager,
        AAnomalyManager* InAnomalyManager,
        AChatConversationManager* InChatConversationManager,
        AMinigameManager* InMinigameManager);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    void SetRoomInputMode();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    void SetDesktopInputMode();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    bool ShowDesktopRoot();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    void HideDesktopRoot();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    bool OpenDesktopApp(FName AppTag);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    void RefreshDesktopRootAvailability();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Controller")
    bool ShowEndingTitleCard(FName EndingId);

    void HandleNativeOverlayOpenChat();
    void HandleNativeOverlayOpenReport();
    void HandleNativeOverlayReturnToRoom();

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UDesktopRootWidget> DesktopRootWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UEndingTitleCardWidget> EndingTitleCardWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|UI")
    bool bIsInDesktopMode = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Debug")
    bool bApplyStartupViewRotation = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|Debug")
    FRotator StartupViewRotation = FRotator(-6.0f, 35.0f, 0.0f);

private:
    UFUNCTION()
    void HandleLeaveDesktopPressed();

    UFUNCTION()
    void HandleDebugOpenDesktopPressed();

    UFUNCTION()
    void HandleDebugOpenChatPressed();

    UFUNCTION()
    void HandleDebugOpenTasksPressed();

    UFUNCTION()
    void HandleDebugOpenStressTestPressed();

    UFUNCTION()
    void HandleDebugOpenReportPressed();

    UFUNCTION()
    void HandleDebugShowRoutineEndingPressed();

    void EnsureNativeDebugDesktopOverlay();
    void RemoveNativeDebugDesktopOverlay();
    void RefreshNativeDebugDesktopOverlay();
    void ApplyStartupViewRotation();
    void SetPawnMovementEnabled(bool bEnabled);

private:
    TSharedPtr<SSignalNativeDesktopOverlay> NativeDebugDesktopOverlay;
    TSharedPtr<class SWidget> NativeDebugDesktopOverlayHost;

    UPROPERTY(Transient)
    TObjectPtr<UDesktopRootWidget> CachedDesktopRoot;

    UPROPERTY(Transient)
    TObjectPtr<UEndingTitleCardWidget> CachedEndingTitleCard;

    UPROPERTY(Transient)
    TObjectPtr<ASignalGameFlowManager> FlowManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<ARouteStateManager> RouteStateManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<AAnomalyManager> AnomalyManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<AChatConversationManager> ChatConversationManagerRef;

    UPROPERTY(Transient)
    TObjectPtr<AMinigameManager> MinigameManagerRef;

    UPROPERTY(Transient)
    FName NativeDebugActiveAppTag = NAME_None;
};
