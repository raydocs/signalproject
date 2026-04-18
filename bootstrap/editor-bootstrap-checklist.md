# Editor Bootstrap Checklist (Hybrid Flow After Native Work Items 1-2)

Use this checklist during the first Unreal Editor session after native compile.

Authority references:

- Runtime ownership/spec: `docs/vertical-slice-18-blueprints-implementation-spec.md`
- Wiring order: `docs/vertical-slice-blueprint-wiring-order.md`
- Schema/types: `docs/blueprint-variables-events-and-data-fields.md`
- Import rows: `docs/day1-day3-datatable-ready-script.md`
- Level/layout: `docs/lv-apartment-main-build-steps.md`, `docs/room-layout-and-player-routing.md`
- UI shell: `docs/ui-widget-tree-structure.md`

Companion landing docs:

- `bootstrap/editor-landing/native-class-to-derived-asset-matrix.md`
- `bootstrap/editor-landing/level-manual-ref-binding-sheet.md`
- `bootstrap/editor-landing/first-editor-session-runbook.md`

---

## A. Native bring-up sanity

- [ ] Open `SignalProject.uproject` in UE5.7 and let Unreal finish startup.
- [ ] Confirm module `SignalProject` is loaded.
- [ ] Confirm native classes are visible (FlowManager, RouteStateManager, AnomalyManager, ChatConversationManager, MinigameManager, HiddenDialogueUnlocker, ComputerTerminal, AirConditionerUnit).
- [ ] Confirm the playable path uses `BP_SignalGameMode` (global default and/or map override).
- [ ] Keep `DefaultInput` mappings unchanged (`Interact`, `LeaveDesktop`).

## B. DataTable import (frozen order)

- [ ] Create/import DataTables in this exact order using native row structs:
  1. [ ] `DT_SystemCopy` using `FST_SystemCopyRow`
  2. [ ] `DT_SupervisorLines` using `FST_SupervisorLineRow`
  3. [ ] `DT_NormalReplies` using `FST_NormalReplyRow`
  4. [ ] `DT_HiddenDialogues` using `FST_HiddenDialogueRow`
  5. [ ] `DT_ReportSentencePools` using `FST_ReportSentenceRow`
  6. [ ] `DT_EndingSubtitles` using `FST_EndingSubtitleRow`
- [ ] Source CSVs: `bootstrap/import-data/*.csv`.
- [ ] Do not rename CSV columns.

## C. Derived asset creation

- [ ] Follow `native-class-to-derived-asset-matrix.md` for native class -> asset creation.
- [ ] Create UMG child blueprints for native widget bases.
- [ ] Create optional BP children of native actor/controller classes where defaults are needed.
- [ ] Do not introduce full-version-only systems (`ReportManager`, `AnomalyChoiceDirector`, `HUDManager`).

## D. Level bring-up and manual refs

- [ ] Create `LV_ApartmentMain`.
- [ ] Place required runtime actors.
- [ ] Apply manual ref bindings exactly per `level-manual-ref-binding-sheet.md`.
- [ ] Keep cross-actor refs manual (`EditInstanceOnly`), map-assigned.

## E. Startup map finalization

- [ ] After level exists, update `Config/DefaultEngine.ini` maps from `/Engine/Maps/Entry` to `/Game/Signal/Levels/Gameplay/LV_ApartmentMain`.

## F. Validation gate

- [ ] Run `bootstrap/first-playable-smoke-test.md`.
- [ ] Treat any failed smoke gate as blocker.
