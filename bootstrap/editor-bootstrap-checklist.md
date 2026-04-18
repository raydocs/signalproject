# Editor Bootstrap Checklist (Work Items 2-4)

Use this checklist after Work Item 1 handoff is complete.

Authority references:

- Runtime ownership/spec: `docs/vertical-slice-18-blueprints-implementation-spec.md`
- Wiring order: `docs/vertical-slice-blueprint-wiring-order.md`
- Schema/types: `docs/blueprint-variables-events-and-data-fields.md`
- Import rows: `docs/day1-day3-datatable-ready-script.md`
- Level/layout: `docs/lv-apartment-main-build-steps.md`, `docs/room-layout-and-player-routing.md`
- UI shell: `docs/ui-widget-tree-structure.md`

---

## A. Project creation and root config

- [ ] Create Blueprint-only UE5 project `SignalProject` at repo root.
- [ ] Confirm generated files exist: `SignalProject.uproject`, `Config/DefaultEngine.ini`, `Config/DefaultGame.ini`, `Config/DefaultInput.ini`.
- [ ] Add/confirm input actions: `Interact`, `LeaveDesktop`.

## B. Folder scaffold in Content

- [ ] Create `Content/Signal/Blueprints/` and slice subfolders used in manifest.
- [ ] Create `Content/Signal/Data/Enums`, `Content/Signal/Data/Structs`, `Content/Signal/Data/DataTables`.
- [ ] Create `Content/Signal/Levels/Gameplay`.

## C. Enum / Struct / DataTable creation

- [ ] Create enums (exact names):
  - [ ] `E_GamePhase`
  - [ ] `E_AnomalyType`
  - [ ] `E_ColleagueId`
  - [ ] `E_SkillUnlockState`
  - [ ] `E_RouteBranch`
  - [ ] `E_MinigameType`
- [ ] Create structs (exact names):
  - [ ] `ST_ChatMessageRecord`
  - [ ] `ST_HiddenOptionRecord`
  - [ ] `ST_SupervisorLineRow`
  - [ ] `ST_NormalReplyRow`
  - [ ] `ST_HiddenDialogueRow`
  - [ ] `ST_SystemCopyRow`
  - [ ] `ST_ReportSentenceRow`
  - [ ] `ST_EndingSubtitleRow`
- [ ] Create/import DataTables in frozen order:
  1. [ ] `DT_SystemCopy` from `bootstrap/import-data/DT_SystemCopy.csv`
  2. [ ] `DT_SupervisorLines` from `bootstrap/import-data/DT_SupervisorLines.csv`
  3. [ ] `DT_NormalReplies` from `bootstrap/import-data/DT_NormalReplies.csv`
  4. [ ] `DT_HiddenDialogues` from `bootstrap/import-data/DT_HiddenDialogues.csv`
  5. [ ] `DT_ReportSentencePools` from `bootstrap/import-data/DT_ReportSentencePools.csv`
  6. [ ] `DT_EndingSubtitles` from `bootstrap/import-data/DT_EndingSubtitles.csv`

## D. Slice asset shell creation (18 only)

- [ ] Create `LV_ApartmentMain` and set `GameMode Override = BP_SignalGameMode`.
- [ ] Create exactly 18 slice assets:
  1. [ ] `BP_SignalGameMode`
  2. [ ] `BP_SignalPlayerController`
  3. [ ] `BP_SignalPlayerCharacter`
  4. [ ] `BP_SignalGameFlowManager`
  5. [ ] `BP_RouteStateManager`
  6. [ ] `BP_AnomalyManager`
  7. [ ] `BP_ComputerTerminal`
  8. [ ] `BP_AirConditionerUnit`
  9. [ ] `WBP_DesktopRoot`
  10. [ ] `WBP_ChatApp`
  11. [ ] `BP_ChatConversationManager`
  12. [ ] `BP_MinigameManager`
  13. [ ] `WBP_MG_DependencyMatch`
  14. [ ] `WBP_AnomalyChoicePopup`
  15. [ ] `BP_HiddenDialogueUnlocker`
  16. [ ] `WBP_ReportEditor`
  17. [ ] `WBP_EndingTitleCard`
  18. [ ] `BPI_Interactable`
- [ ] Do not add full-version-only dependencies (`BP_ReportManager`, `BP_AnomalyChoiceDirector`, `BP_SignalHUDManager`).

## E. Level placement and mandatory manual refs (Step 0)

- [ ] Place these actors in `LV_ApartmentMain`:
  - [ ] `BP_SignalGameFlowManager`
  - [ ] `BP_RouteStateManager`
  - [ ] `BP_AnomalyManager`
  - [ ] `BP_ChatConversationManager`
  - [ ] `BP_MinigameManager`
  - [ ] `BP_HiddenDialogueUnlocker`
  - [ ] `BP_ComputerTerminal`
  - [ ] `BP_AirConditionerUnit`
- [ ] Manually assign all Step 0 refs in Details panel per `docs/vertical-slice-blueprint-wiring-order.md`.

## F. Wiring and first-playable gate

- [ ] Wire in order: room/desktop -> chat/minigame/FREEZE -> branch handling -> AC resolve -> hidden option -> report -> ending.
- [ ] Validate with `bootstrap/first-playable-smoke-test.md`.
- [ ] Treat failures as blockers until all smoke gates pass.
