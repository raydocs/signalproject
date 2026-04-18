#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalSliceTypes.h"
#include "RouteStateManager.generated.h"

UCLASS(Blueprintable)
class SIGNALPROJECT_API ARouteStateManager : public AActor
{
    GENERATED_BODY()

public:
    ARouteStateManager();

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Route")
    void RecordRouteChoice(E_RouteBranch RouteChoice);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Route")
    void SetColleagueUnlockState(E_ColleagueId ColleagueId, E_SkillUnlockState NewState);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Route")
    void MarkHiddenOptionConsumed(E_ColleagueId ColleagueId, FName OptionId);

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    FName GetSliceEndingResult() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    E_SkillUnlockState GetColleagueUnlockState(E_ColleagueId ColleagueId) const;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    int32 ReportSupervisorCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    int32 HandleMyselfCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    E_RouteBranch LastRouteChoice = E_RouteBranch::Neutral;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    E_SkillUnlockState ColleagueAState = E_SkillUnlockState::Locked;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    bool bHasConsumedColleagueAHiddenOption = false;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route", meta = (AllowPrivateAccess = "true"))
    TMap<E_ColleagueId, E_SkillUnlockState> ColleagueUnlockStates;
};
