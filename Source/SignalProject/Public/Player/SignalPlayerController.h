#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SignalPlayerController.generated.h"

class AAnomalyManager;
class AChatConversationManager;
class AMinigameManager;
class ARouteStateManager;
class ASignalGameFlowManager;
class UDesktopRootWidget;
class UEndingTitleCardWidget;

UCLASS(Blueprintable)
class SIGNALPROJECT_API ASignalPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASignalPlayerController();

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
    bool ShowEndingTitleCard(FName EndingId);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UDesktopRootWidget> DesktopRootWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal Slice|UI")
    TSubclassOf<UEndingTitleCardWidget> EndingTitleCardWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|UI")
    bool bIsInDesktopMode = false;

private:
    UFUNCTION()
    void HandleLeaveDesktopPressed();

private:
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
};
