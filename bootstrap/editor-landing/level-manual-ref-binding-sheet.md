# Level + Manual Ref Binding Sheet (`LV_ApartmentMain`)

Use this sheet after placing actors in `LV_ApartmentMain`.

Rule: all cross-actor refs exposed as `EditInstanceOnly` must be assigned manually in map instances.

---

## Required level actor instances

Place one instance each:

- `ASignalGameFlowManager`
- `ARouteStateManager`
- `AAnomalyManager`
- `AChatConversationManager`
- `AMinigameManager`
- `AHiddenDialogueUnlocker`
- `AComputerTerminal` (or BP child)
- `AAirConditionerUnit` (or BP child)

---

## Binding table

| Owning instance | Property | Assign to | Required | Notes |
|---|---|---|---|---|
| `SignalGameFlowManager` | `RouteStateManagerRef` | placed `RouteStateManager` | Yes | |
| `SignalGameFlowManager` | `AnomalyManagerRef` | placed `AnomalyManager` | Yes | |
| `SignalGameFlowManager` | `ChatConversationManagerRef` | placed `ChatConversationManager` | Yes | |
| `SignalGameFlowManager` | `MinigameManagerRef` | placed `MinigameManager` | Yes | |
| `SignalGameFlowManager` | `HiddenDialogueUnlockerRef` | placed `HiddenDialogueUnlocker` | Yes | |
| `SignalGameFlowManager` | `CurrentDayIndex` | `2` for Day2 FREEZE smoke pass | Recommended | Keep explicit for deterministic testing. |
| `AnomalyManager` | `GameFlowManagerRef` | placed `SignalGameFlowManager` | Yes | |
| `AnomalyManager` | `MinigameManagerRef` | placed `MinigameManager` (or BP child) | Recommended | Current type is `AActor`; assign minigame actor instance. |
| `ChatConversationManager` | `AnomalyManagerRef` | placed `AnomalyManager` | Yes | |
| `ChatConversationManager` | `RouteStateManagerRef` | placed `RouteStateManager` | Yes | |
| `ChatConversationManager` | `SupervisorLinesTable` | `DT_SupervisorLines` | Yes | |
| `ChatConversationManager` | `NormalRepliesTable` | `DT_NormalReplies` | Yes | |
| `ChatConversationManager` | `HiddenDialoguesTable` | `DT_HiddenDialogues` | Yes | |
| `MinigameManager` | `GameFlowManagerRef` | placed `SignalGameFlowManager` | Yes | |
| `MinigameManager` | `AnomalyManagerRef` | placed `AnomalyManager` | Yes | |
| `MinigameManager` | `RouteStateManagerRef` | placed `RouteStateManager` | Yes | |
| `MinigameManager` | `DependencyMatchWidgetClass` | `WBP_MG_DependencyMatch` | Yes | Popup/loop chain depends on it. |
| `MinigameManager` | `AnomalyChoicePopupClass` | `WBP_AnomalyChoicePopup` | Yes | Minigame manager owns popup lifecycle. |
| `HiddenDialogueUnlocker` | `ChatConversationManagerRef` | placed `ChatConversationManager` | Yes | |
| `HiddenDialogueUnlocker` | `RouteStateManagerRef` | placed `RouteStateManager` | Yes | |
| `ComputerTerminal` | `GameFlowManagerRef` | placed `SignalGameFlowManager` | Yes | |
| `ComputerTerminal` | `SystemCopyTable` | `DT_SystemCopy` | Optional | Prompt/system copy support. |
| `AirConditionerUnit` | `AnomalyManagerRef` | placed `AnomalyManager` | Yes | |
| `AirConditionerUnit` | `HiddenDialogueUnlockerRef` | placed `HiddenDialogueUnlocker` | Yes | |
| `AirConditionerUnit` | `GameFlowManagerRef` | placed `SignalGameFlowManager` | Yes | |
| `AirConditionerUnit` | `SystemCopyTable` | `DT_SystemCopy` | Optional | Prompt/system copy support. |

---

## Derived asset class bindings (CDO / defaults)

| Owning class/asset | Property | Assign to | Required |
|---|---|---|---|
| `BP_SignalPlayerController` (recommended) | `DesktopRootWidgetClass` | `WBP_DesktopRoot` | Yes |
| `BP_SignalPlayerController` (recommended) | `EndingTitleCardWidgetClass` | `WBP_EndingTitleCard` | Yes |
| `WBP_DesktopRoot` | `ChatAppWidgetClass` | `WBP_ChatApp` | Yes |
| `WBP_DesktopRoot` | `ReportEditorWidgetClass` | `WBP_ReportEditor` | Yes |
| `WBP_ReportEditor` | `ReportSentencesTable` | `DT_ReportSentencePools` | Yes |
| `WBP_EndingTitleCard` | `EndingSubtitlesTable` | `DT_EndingSubtitles` | Yes |

---

## Completion check

- [ ] Every required row above assigned.
- [ ] No missing `EditInstanceOnly` refs on placed runtime actors.
- [ ] Controller and desktop/report/ending widget class fields assigned.
- [ ] Proceed to smoke test.
