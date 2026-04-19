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
    void MarkHiddenOptionConsumed(E_ColleagueId ColleagueId, FName OptionId, E_ChatOptionKind OptionKind, E_AnomalyType RequiredAnomaly);

    UFUNCTION(BlueprintCallable, Category = "Signal Slice|Route")
    void ResetForDay();

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    FName GetSliceEndingResult() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    E_SkillUnlockState GetColleagueUnlockState(E_ColleagueId ColleagueId) const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    TArray<FST_ReportSentenceRow> GetCollectedInjectionSentences() const;

    UFUNCTION(BlueprintPure, Category = "Signal Slice|Route")
    FST_SliceEndingResult BuildSliceEndingResult(const FST_ReportSubmissionPayload& Payload) const;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    bool bHasConsumedSelfHandleFollowupOption = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route")
    TArray<FST_ReportSentenceRow> CollectedInjectionSentences;

private:
    FST_ReportSentenceRow BuildInjectionSentence(E_ColleagueId ColleagueId, E_AnomalyType RequiredAnomaly) const;
    const FST_ReportSentenceRow* FindCollectedInjectionSentence(FName SentenceId) const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal Slice|Route", meta = (AllowPrivateAccess = "true"))
    TMap<E_ColleagueId, E_SkillUnlockState> ColleagueUnlockStates;
};
