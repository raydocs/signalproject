# Native Class -> Derived Asset Matrix (Editor Landing)

Purpose: make the first Editor session deterministic now that native classes compile.

Rules:

- Prefer creating **derived assets** for defaults/visuals; keep runtime authority in native C++.
- Do not create fake assets in-repo.
- Do not duplicate native enums/structs as separate BP schema assets.

---

## Runtime actor/controller/pawn classes

| Native class | Native path | Suggested derived asset | Suggested Content path | Required in first session? | Notes |
|---|---|---|---|---|---|
| `ASignalGameMode` | `/Script/SignalProject.SignalGameMode` | `BP_SignalGameMode` (optional) | `/Game/Signal/Blueprints/Framework/` | Optional | Native class already valid as project default. |
| `ASignalPlayerController` | `/Script/SignalProject.SignalPlayerController` | `BP_SignalPlayerController` | `/Game/Signal/Blueprints/Player/` | Recommended | Easiest place to set `DesktopRootWidgetClass` + `EndingTitleCardWidgetClass`. |
| `ASignalPlayerCharacter` | `/Script/SignalProject.SignalPlayerCharacter` | `BP_SignalPlayerCharacter` (optional) | `/Game/Signal/Blueprints/Player/` | Optional | For tuning defaults only. |
| `ASignalGameFlowManager` | `/Script/SignalProject.SignalGameFlowManager` | `BP_SignalGameFlowManager` (optional) | `/Game/Signal/Blueprints/Core/` | Recommended | Level-placed authority actor; defaults in BP child can reduce repetitive setup. |
| `ARouteStateManager` | `/Script/SignalProject.RouteStateManager` | `BP_RouteStateManager` (optional) | `/Game/Signal/Blueprints/Core/` | Recommended | Level-placed state actor. |
| `AAnomalyManager` | `/Script/SignalProject.AnomalyManager` | `BP_AnomalyManager` (optional) | `/Game/Signal/Blueprints/Anomalies/` | Recommended | Set threshold defaults here if needed. |
| `AChatConversationManager` | `/Script/SignalProject.ChatConversationManager` | `BP_ChatConversationManager` (optional) | `/Game/Signal/Blueprints/Dialogue/` | Recommended | Assign DataTables and refs in level instance. |
| `AMinigameManager` | `/Script/SignalProject.MinigameManager` | `BP_MinigameManager` (optional) | `/Game/Signal/Blueprints/Minigames/` | Recommended | Set widget class defaults + map refs. |
| `AHiddenDialogueUnlocker` | `/Script/SignalProject.HiddenDialogueUnlocker` | `BP_HiddenDialogueUnlocker` (optional) | `/Game/Signal/Blueprints/Dialogue/` | Recommended | Keeps stable option defaults centralized. |
| `AComputerTerminal` | `/Script/SignalProject.ComputerTerminal` | `BP_ComputerTerminal` | `/Game/Signal/Blueprints/Interactables/` | Recommended | Use BP child for meshes/collision visuals; keep interact logic native. |
| `AAirConditionerUnit` | `/Script/SignalProject.AirConditionerUnit` | `BP_AirConditionerUnit` | `/Game/Signal/Blueprints/Interactables/` | Recommended | Use BP child for meshes/collision visuals; keep resolve logic native. |

## Native widget bases -> UMG children

| Native widget base | Native path | Required UMG child | Suggested Content path | Required in first session? | Notes |
|---|---|---|---|---|---|
| `UDesktopRootWidget` | `/Script/SignalProject.DesktopRootWidget` | `WBP_DesktopRoot` | `/Game/Signal/Blueprints/UI/` | Yes | Controller-owned root host. |
| `UChatAppWidget` | `/Script/SignalProject.ChatAppWidget` | `WBP_ChatApp` | `/Game/Signal/Blueprints/UI/` | Yes | Desktop child app. |
| `UDependencyMatchWidget` | `/Script/SignalProject.DependencyMatchWidget` | `WBP_MG_DependencyMatch` | `/Game/Signal/Blueprints/Minigames/` | Yes | Minigame widget class used by minigame manager. |
| `UAnomalyChoicePopupWidget` | `/Script/SignalProject.AnomalyChoicePopupWidget` | `WBP_AnomalyChoicePopup` | `/Game/Signal/Blueprints/UI/` | Yes | Popup lifecycle owned by minigame manager. |
| `UReportEditorWidget` | `/Script/SignalProject.ReportEditorWidget` | `WBP_ReportEditor` | `/Game/Signal/Blueprints/Report/` | Yes | Report gating still enforced through flow/route refs. |
| `UEndingTitleCardWidget` | `/Script/SignalProject.EndingTitleCardWidget` | `WBP_EndingTitleCard` | `/Game/Signal/Blueprints/Endings/` | Yes | Controller-owned ending widget. |

## DataTable assets

| DataTable asset | Suggested Content path | Row struct (native) | CSV source |
|---|---|---|---|
| `DT_SystemCopy` | `/Game/Signal/Data/DataTables/` | `FST_SystemCopyRow` | `bootstrap/import-data/DT_SystemCopy.csv` |
| `DT_SupervisorLines` | `/Game/Signal/Data/DataTables/` | `FST_SupervisorLineRow` | `bootstrap/import-data/DT_SupervisorLines.csv` |
| `DT_NormalReplies` | `/Game/Signal/Data/DataTables/` | `FST_NormalReplyRow` | `bootstrap/import-data/DT_NormalReplies.csv` |
| `DT_HiddenDialogues` | `/Game/Signal/Data/DataTables/` | `FST_HiddenDialogueRow` | `bootstrap/import-data/DT_HiddenDialogues.csv` |
| `DT_ReportSentencePools` | `/Game/Signal/Data/DataTables/` | `FST_ReportSentenceRow` | `bootstrap/import-data/DT_ReportSentencePools.csv` |
| `DT_EndingSubtitles` | `/Game/Signal/Data/DataTables/` | `FST_EndingSubtitleRow` | `bootstrap/import-data/DT_EndingSubtitles.csv` |

Next step after asset creation: apply the binding sheet in `bootstrap/editor-landing/level-manual-ref-binding-sheet.md`.
