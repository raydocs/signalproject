#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SignalSliceTypes.generated.h"

/**
 * Frozen slice-native enums and structs.
 * Authority: docs/blueprint-variables-events-and-data-fields.md
 */
UENUM(BlueprintType)
enum class E_GamePhase : uint8
{
    Boot,
    RoomExplore,
    DesktopIdle,
    ChatActive,
    MinigameActive,
    AnomalyChoice,
    HandleAnomaly3D,
    ReportPhase,
    EndingSequence,
    RevealSequence,
    Paused
};

UENUM(BlueprintType)
enum class E_AnomalyType : uint8
{
    None,
    FREEZE,
    BLACKOUT,
    DISKCLEAN
};

UENUM(BlueprintType)
enum class E_ColleagueId : uint8
{
    ColleagueA,
    ColleagueB,
    ColleagueC,
    Supervisor
};

UENUM(BlueprintType)
enum class E_SkillUnlockState : uint8
{
    Locked,
    AnomalySeen,
    Unlocked
};

UENUM(BlueprintType)
enum class E_RouteBranch : uint8
{
    Neutral,
    ReportSupervisor,
    HandleMyself
};

UENUM(BlueprintType)
enum class E_MinigameType : uint8
{
    DependencyMatch,
    WhackABug,
    BugfixFinder
};

UENUM(BlueprintType)
enum class E_ChatOptionKind : uint8
{
    HiddenDialogue,
    SelfHandleFollowup,
    AnomalyReport
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_ChatMessageRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName MessageId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId SpeakerId = E_ColleagueId::ColleagueA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText MessageText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText TimestampText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsPlayer = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsHiddenFragment = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsSystemMessage = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsVoiceMessage = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName VoiceAssetId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName VisualStyleTag = NAME_None;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_HiddenOptionRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName OptionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId TargetColleague = E_ColleagueId::ColleagueA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_AnomalyType RequiredAnomaly = E_AnomalyType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bConsumed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 UnlockDay = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ChatOptionKind OptionKind = E_ChatOptionKind::HiddenDialogue;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_StressTestViewState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 TotalDraws = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 LastDrawAmount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 RemainingCurrency = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 StressPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bCanSingleDraw = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bCanTenDraw = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsPaused = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bAwaitingIssueReport = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_AnomalyType ActiveAnomalyType = E_AnomalyType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText OutputText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText StatusText;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_AnomalyRouteConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_AnomalyType AnomalyType = E_AnomalyType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId ReportColleague = E_ColleagueId::ColleagueA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName ReportOptionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText ReportOptionLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText ReportReplyText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName FollowupOptionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText FollowupOptionLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText FollowupReplyText;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_DesktopTaskRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName TaskId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText TaskText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_SupervisorLineRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName LineId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 DayIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName UseCase = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId SpeakerId = E_ColleagueId::Supervisor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 Weight = 0;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_NormalReplyRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName ReplyId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 DayIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId ColleagueId = E_ColleagueId::ColleagueA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 Weight = 0;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_HiddenDialogueRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName DialogueId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 DayIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId ColleagueId = E_ColleagueId::ColleagueA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 SequenceOrder = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bGlitchEnter = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bGlitchExit = false;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_SystemCopyRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName CopyId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName Category = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName SubType = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_ReportSentenceRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName SentenceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 DayIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    E_ColleagueId SourceColleague = E_ColleagueId::Supervisor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 UnlockRequirement = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    float StrengthScore = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bIsFinalInjection = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_ReportSubmissionPayload
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName SelectedBaseSentenceId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText SelectedBaseSentenceText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    TArray<FName> SelectedInjectedSentenceIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    TArray<FST_ReportSentenceRow> SelectedInjectedRows;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    float TotalStrengthScore = 0.0f;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_SliceEndingResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName EndingId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText TitleText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText SummaryText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText ReportPreviewText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 SelectedInjectionCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    bool bUsedInjectedLines = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    TArray<FST_ReportSentenceRow> SelectedInjectedRows;
};

USTRUCT(BlueprintType)
struct SIGNALPROJECT_API FST_EndingSubtitleRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FName EndingId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    int32 SequenceOrder = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal Slice")
    FText Text;
};
