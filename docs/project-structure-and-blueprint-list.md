# Signal Project Unreal Structure And Blueprint List

> Target: full-content version of `Signal`
> Engine: Unreal Engine 5
> Format: 3D explorable room + 2D desktop gameplay in UMG
> Goal: define a production-ready project layout and the full Blueprint class list before implementation starts

---

## 1. Project Architecture Overview

The game is best split into four major layers:

1. `3D World Layer`
- player movement in apartment/home office
- interactable props such as computer, AC, lights, disk-clean target
- room state changes and environmental anomaly presentation

2. `Desktop UI Layer`
- full-screen desktop OS experience in UMG
- chat, minigames, anomaly choice popup, report editor
- hidden dialogue, supervisor reveal, ending cards

3. `Game Flow Layer`
- day progression
- route tracking
- anomaly scheduling
- unlock states, endings, soft-lock pressure

4. `Data Layer`
- colleague definitions
- day configuration
- dialogue lines
- report sentence pools
- anomaly metadata

---

## 2. Recommended Unreal Project Top-Level Structure

```text
SignalProject/
  Config/
  Content/
    Signal/
      Art/
      Audio/
      Blueprints/
      Data/
      Levels/
      Materials/
      Meshes/
      Niagara/
      Sequences/
      Textures/
      UI/
    StarterContent/           # optional during prototyping
  Source/                     # optional, only if later adding C++
```

For the first phase, the game can be fully Blueprint-driven.

---

## 3. Production Folder Layout Inside Content/Signal

```text
Content/Signal/
  Art/
    Concepts/
    Moodboards/
    UIExports/
  Audio/
    BGM/
    SFX/
    Voice/
    MixStates/
  Blueprints/
    Core/
    Framework/
    Player/
    Interactables/
    World/
    UI/
    Minigames/
    Dialogue/
    Anomalies/
    Report/
    Endings/
    Debug/
  Data/
    DataAssets/
    DataTables/
    Curves/
    Enums/
    Structs/
  Levels/
    Persistent/
    Gameplay/
    Lighting/
    Test/
  Materials/
    Master/
    Instances/
    Decals/
    PostProcess/
    UI/
  Meshes/
    Environment/
    Props/
    Hero/
  Niagara/
    Ambient/
    Anomalies/
    UI/
  Sequences/
    Intro/
    DayTransitions/
    Reveal/
    Endings/
  Textures/
    UI/
    Environment/
    Decals/
    VFX/
  UI/
    Widgets/
    Styles/
    Icons/
    Fonts/
    RenderTargets/
```

---

## 4. Naming Convention

Use consistent Unreal prefixes.

### Blueprint Classes

- `BP_` gameplay actor or object blueprint
- `WBP_` widget blueprint
- `BPI_` blueprint interface
- `E_` enum
- `ST_` struct
- `DA_` primary data asset
- `DT_` data table
- `GM_` game mode if separated
- `GI_` game instance if separated
- `PC_` player controller if separated
- `HUD_` HUD manager if separated

### Examples

- `BP_SignalGameFlowManager`
- `BP_ComputerTerminal`
- `WBP_DesktopRoot`
- `DA_Colleague_A`
- `DT_DayConfigs`
- `E_AnomalyType`

---

## 5. Full Blueprint Class List

## 5.1 Core Framework Blueprints

These are the classes that keep the whole project alive.

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_SignalGameInstance` | Blueprint Game Instance | Persistent runtime data, global save/session state |
| `BP_SignalGameMode` | Game Mode | High-level mode setup for playable levels |
| `BP_SignalPlayerController` | Player Controller | Input mode switching, UI ownership, cursor state |
| `BP_SignalPlayerCharacter` | Character | 3D room movement, interaction tracing |
| `BP_SignalHUDManager` | Actor or HUD | Spawns root widgets, manages overlay stack |
| `BP_SignalGameFlowManager` | Actor | Day loop, route state, anomaly progression, ending checks |
| `BP_SignalSaveManager` | Actor or UObject-style BP | Save/load wrapper if persistence is needed |
| `BP_SignalStateRouter` | Actor | Central dispatcher for desktop/world/report/reveal states |

## 5.2 Blueprint Interfaces

| Class | Type | Purpose |
| --- | --- | --- |
| `BPI_Interactable` | Interface | Generic `CanInteract`, `Interact`, prompt text |
| `BPI_ComputerScreenTarget` | Interface | Objects that can open desktop mode |
| `BPI_AnomalyReactable` | Interface | World actors that respond to anomaly state changes |
| `BPI_RoomStateListener` | Interface | World actors reacting to day/route/escalation changes |
| `BPI_ReportSentenceSource` | Interface | Optional provider for sentence unlocks |

## 5.3 Player And Camera Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_SignalPlayerCharacter` | Character | First-person or close third-person player |
| `BP_SignalPlayerCameraRig` | Actor | Camera transitions between room mode and desk mode |
| `BP_DeskViewAnchor` | Actor | Fixed viewpoint when using the computer |
| `BP_InteractionTraceComponent` | Actor Component BP | Reusable line trace interaction logic |
| `BP_PlayerModeComponent` | Actor Component BP | Switches between Room, Desktop, Choice, Report, Ending |

## 5.4 World And Room Management Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_RoomStateManager` | Actor | Controls global room lighting and environmental states |
| `BP_DayStateDirector` | Actor | Applies day-specific dressing, post-process, ambient state |
| `BP_EnvironmentVariantController` | Actor | Swaps materials/props as pressure rises |
| `BP_PostProcessStateController` | Actor | Handles world-space visual distortion |
| `BP_AmbientAudioStateController` | Actor | Links room states to audio moods |

## 5.5 Interactable World Props

### Core Room Props

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_ComputerTerminal` | Actor | Opens desktop UI, receives alert state |
| `BP_AirConditionerUnit` | Actor | Freeze anomaly handling target |
| `BP_LightControlPanel` | Actor | Blackout anomaly handling target |
| `BP_DiskCleanTerminal` | Actor | Disk-clean anomaly handling target |
| `BP_DeskChairInteractable` | Actor | Optional sit/stand transition support |
| `BP_RoomDoorBlocker` | Actor | Keeps scope contained and supports locked-state flavor |

### Flavor Or Secondary Props

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_FlickerLamp` | Actor | Reacts to anomaly states |
| `BP_WindowLightCard` | Actor | Controls outside mood and time-of-day color |
| `BP_PersonalPropSet` | Actor | Bundled clutter set for progression swaps |
| `BP_MonitorGlowProxy` | Actor | Extra monitor/world-space emissive control |

## 5.6 Desktop Root And Major UMG Blueprints

These form the playable 2D core.

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_DesktopRoot` | Widget | Main full-screen desktop experience |
| `WBP_DesktopWallpaper` | Widget | Background and corrupted variations |
| `WBP_TopHUD` | Widget | Day, environment indicators, hidden route cues |
| `WBP_AppWindowFrame` | Widget | Shared app window frame and drag visuals |
| `WBP_SystemNotification` | Widget | Toasts and unlock notices |
| `WBP_SystemModal` | Widget | Generic modal/warning shell |
| `WBP_BootSequence` | Widget | Opening boot/login style screen |
| `WBP_ShutdownSequence` | Widget | Ending shutdown or takeover screen |

## 5.7 Chat System Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_ChatApp` | Widget | Chat application root |
| `WBP_ChatContactList` | Widget | Contact/thread selector |
| `WBP_ChatThreadView` | Widget | Message list panel |
| `WBP_ChatInputBar` | Widget | Text input and send action |
| `WBP_MessageBubble_Player` | Widget | Player text bubble |
| `WBP_MessageBubble_Colleague` | Widget | Standard colleague bubble |
| `WBP_MessageBubble_Supervisor` | Widget | Supervisor bubble |
| `WBP_MessageBubble_System` | Widget | System or alert message |
| `WBP_MessageBubble_HiddenFragment` | Widget | Hidden dialogue distorted bubble |
| `WBP_HiddenOptionCard` | Widget | Clickable special unlock card |
| `WBP_TypingIndicator` | Widget | Normal and glitched typing animation |
| `BP_ChatConversationManager` | Actor or Object BP | Loads threads, inserts replies, unlocks hidden options |
| `BP_ChatMessageFactory` | Actor or Object BP | Builds message widget data from content records |

## 5.8 Dialogue And Narrative Flow Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_DialogueDirector` | Actor | Plays normal and hidden dialogue sequences |
| `BP_HiddenDialogueUnlocker` | Actor | Checks anomaly handling success and injects hidden options |
| `BP_SupervisorRevealDirector` | Actor | Runs Day 6 reveal flow |
| `BP_EndingNarrativeDirector` | Actor | Plays good/bad ending narrative beats |
| `WBP_DialogueOverlay` | Widget | Short overlay for scripted sequences |
| `WBP_RevealTagOverlay` | Widget | Compute tag reveal and UI corruption |
| `WBP_DayTransitionCard` | Widget | Day start/end title card |
| `WBP_FlashbackCard` | Widget | Memory fragment card or narrative insert |

## 5.9 Anomaly System Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_AnomalyManager` | Actor | Central anomaly state machine |
| `BP_AnomalyAccumulator` | Actor Component BP | Accumulates side effects from chat usage |
| `BP_AnomalyChoiceDirector` | Actor | Presents report-vs-handle choice |
| `WBP_AnomalyChoicePopup` | Widget | The decision popup |
| `WBP_AnomalyAlertBanner` | Widget | In-UI anomaly warning |
| `BP_AnomalyEffectRouter` | Actor | Routes anomaly type to UI/world/minigame effect layers |

### Per-Anomaly Implementations

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_Anomaly_FREEZE` | Actor | Freeze logic wrapper |
| `BP_Anomaly_BLACKOUT` | Actor | Blackout logic wrapper |
| `BP_Anomaly_DISKCLEAN` | Actor | Disk-clean logic wrapper |
| `BP_FREEZE_WorldEffect` | Actor | AC, room cold visuals, frosted state |
| `BP_BLACKOUT_WorldEffect` | Actor | Room darkness, selective emissives |
| `BP_DISKCLEAN_WorldEffect` | Actor | System purge pulses, monitor corruption |
| `WBP_FREEZE_Overlay` | Widget | Frost edges and lag feedback |
| `WBP_BLACKOUT_Overlay` | Widget | Black mask and glow emphasis |
| `WBP_DISKCLEAN_Overlay` | Widget | Wipe, deletion, scramble effect |

## 5.10 Minigame Blueprints

Each minigame should have a root controller widget plus supporting widgets and logic blueprints.

### Shared Minigame Layer

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_MinigameManager` | Actor | Chooses current minigame and handles completion |
| `WBP_MinigameShell` | Widget | Shared frame with timer, score, pause state |
| `WBP_MinigameResultPanel` | Widget | Success/failure/performance feedback |
| `BP_MinigameAnomalyBridge` | Actor | Injects anomaly modifications into current minigame |

### Lianliankan / Dependency Match

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_MG_DependencyMatch` | Widget | Root gameplay widget |
| `WBP_MG_DependencyTile` | Widget | Single tile entry |
| `BP_MG_DependencyMatchController` | Actor or Object BP | Grid generation, match rules, timer |
| `BP_MG_DependencyLinkFX` | Actor or Object BP | Visual line and clear feedback |

### Whack-a-Bug

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_MG_WhackABug` | Widget | Root gameplay widget |
| `WBP_MG_BugTarget` | Widget | Single bug target |
| `BP_MG_WhackABugController` | Actor or Object BP | Spawn timing, score, streak logic |
| `BP_MG_BugSpawnPattern` | Object BP | Optional pattern provider |

### Bugfix Line Finder

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_MG_BugfixFinder` | Widget | Root gameplay widget |
| `WBP_MG_CodeLineEntry` | Widget | Clickable code line |
| `BP_MG_BugfixFinderController` | Actor or Object BP | Correct-line logic, reset timing |
| `BP_MG_CodeSnippetProvider` | Object BP | Supplies pseudo-code sets |

## 5.11 Report And Prompt Injection Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `WBP_ReportEditor` | Widget | Main report editor screen |
| `WBP_ReportSentenceOption` | Widget | Selectable sentence chip |
| `WBP_ReportSubmitPanel` | Widget | Submit button and result status |
| `BP_ReportManager` | Actor | Unlocks sentence pools, validates result |
| `BP_SupervisorDetectionModel` | Actor or Object BP | Detection logic for final phase |
| `BP_PromptInjectionDirector` | Actor | Runs final attack phase and transition to ending |
| `WBP_DetectionWarningOverlay` | Widget | Detected/failed state |
| `WBP_FinalInjectionHighlight` | Widget | Shows the final unlocked option |

## 5.12 Ending And Route Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_RouteStateManager` | Actor | Tracks report route vs ally route |
| `BP_BadEndingDirector` | Actor | Distillation ending sequence |
| `BP_GoodEndingDirector` | Actor | Prompt-injection success ending |
| `WBP_EndingTitleCard` | Widget | Ending title presentation |
| `WBP_EndingSubtitleCard` | Widget | Text montage overlays |
| `WBP_PostEndingSummary` | Widget | Optional final summary / restart screen |

## 5.13 Effects, Presentation, And Utility Blueprints

| Class | Type | Purpose |
| --- | --- | --- |
| `BP_GlitchFXDirector` | Actor | Coordinates UI and world glitch bursts |
| `BP_ScreenDistortionController` | Actor | Monitor distortion and render target effects |
| `BP_LightingCueController` | Actor | Flicker, pulse, intensity swaps |
| `BP_WidgetLayerManager` | Actor or Object BP | Keeps modal stack and priority sane |
| `BP_DebugSignalCheats` | Actor | Dev shortcuts for day skip, unlocks, endings |
| `WBP_DebugPanel` | Widget | In-editor testing tools |

---

## 6. Data Assets And Non-Blueprint Data Classes

These are not all Blueprints, but the project will need them.

## 6.1 Enums

| Name | Purpose |
| --- | --- |
| `E_GamePhase` | Boot, Room, Desktop, Minigame, Choice, Report, Reveal, Ending |
| `E_AnomalyType` | None, Freeze, Blackout, DiskClean |
| `E_RouteType` | Neutral, Supervisor, HiddenAlly |
| `E_ColleagueId` | A, B, C, Supervisor |
| `E_DayId` | Day1-Day7 |
| `E_MinigameType` | DependencyMatch, WhackABug, BugfixFinder |
| `E_ReportResult` | Normal, Detected, Success |

## 6.2 Structs

| Name | Purpose |
| --- | --- |
| `ST_ChatMessageRecord` | Speaker, type, text, flags, timestamps |
| `ST_HiddenOptionRecord` | Label, unlock condition, dialogue target |
| `ST_ColleagueDefinition` | Name, anomaly, normal replies, hidden dialogue refs |
| `ST_DayConfig` | Day intro, minigame, anomaly pool, content beats |
| `ST_AnomalyConfig` | Type, UI text, world target, overlay ref |
| `ST_ReportSentenceOption` | Text, unlock source, strength, state |
| `ST_EndingBeat` | Title, subtitle, visual event ref |

## 6.3 Data Assets / Data Tables

| Name | Type | Purpose |
| --- | --- | --- |
| `DA_Colleague_A` | Primary Data Asset | Full colleague A definition |
| `DA_Colleague_B` | Primary Data Asset | Full colleague B definition |
| `DA_Colleague_C` | Primary Data Asset | Full colleague C definition |
| `DA_SupervisorProfile` | Primary Data Asset | Supervisor profile and reveal data |
| `DT_DayConfigs` | Data Table | Day 1-7 configuration |
| `DT_NormalReplies` | Data Table | Normal chat rotation lines |
| `DT_HiddenDialogues` | Data Table | Hidden fragment sequences |
| `DT_ReportSentencePools` | Data Table | Final report sentence options and unlock conditions |
| `DT_AnomalyConfigs` | Data Table | UI text, overlays, world targets |
| `DT_EndingSequences` | Data Table | Final sequence text cards |

---

## 7. Suggested Level Layout

```text
Levels/
  Persistent/
    LV_PersistentRoot
  Gameplay/
    LV_ApartmentMain
    LV_ApartmentLighting_Day
    LV_ApartmentLighting_Night
    LV_ApartmentVariant_EndingBad
    LV_ApartmentVariant_EndingGood
  Test/
    LV_Test_DesktopUI
    LV_Test_Chat
    LV_Test_Anomalies
    LV_Test_Minigame_DependencyMatch
    LV_Test_Minigame_WhackABug
    LV_Test_Minigame_BugfixFinder
```

If you want to keep scope sane, most of the game can live in `LV_ApartmentMain`, with managers handling state swaps instead of building many separate maps.

---

## 8. Build Order Recommendation

If the team uses this structure, the safest implementation order is:

1. framework blueprints
2. player controller + room/desktop mode switching
3. `BP_ComputerTerminal` + `WBP_DesktopRoot`
4. chat system core
5. one anomaly pipeline end-to-end
6. one minigame end-to-end
7. hidden dialogue unlock flow
8. report editor skeleton
9. route state tracking
10. reveal and ending directors
11. remaining anomalies and minigames
12. polish, VFX, debug tools

---

## 9. Minimum Full-Project Class Core

If you want to identify the absolute backbone classes before any expansion, these are the non-negotiable ones:

- `BP_SignalPlayerController`
- `BP_SignalPlayerCharacter`
- `BP_SignalGameFlowManager`
- `BP_RoomStateManager`
- `BP_ComputerTerminal`
- `BP_AnomalyManager`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `BP_ReportManager`
- `BP_RouteStateManager`
- `WBP_DesktopRoot`
- `WBP_ChatApp`
- `WBP_AnomalyChoicePopup`
- `WBP_ReportEditor`

---

## 10. Suggested Next Documents

After this doc, the most useful planning docs are:

1. a `48-hour vertical slice blueprint subset`
2. a `Day 1-Day 7 gameplay content sheet`
3. a `widget wireframe and screen flow doc`
4. a `state machine diagram for Room/Desktop/Choice/Report/Ending`
