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
