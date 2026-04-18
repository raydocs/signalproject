# First-Playable Smoke Test (Day2 FREEZE Slice)

Run after Work Items 2-4 wiring is complete.

Authority references:

- `docs/vertical-slice-scope.md`
- `docs/game-state-machine.md`
- `docs/interaction-spec.md`
- `docs/vertical-slice-blueprint-wiring-order.md`
- `docs/vertical-slice-18-blueprints-implementation-spec.md`

---

## Test setup

- Map: `LV_ApartmentMain`
- Expected playable chain:
  - `RoomExplore -> DesktopIdle -> ChatActive -> MinigameActive -> AnomalyChoice`
  - Branch A: `ReportSupervisor -> ReportPhase -> EndingSequence`
  - Branch B: `HandleMyself -> HandleAnomaly3D -> RoomExplore -> DesktopIdle -> ChatActive(hidden) -> ReportPhase -> EndingSequence`

## Pass/fail checklist

### 1) Room movement and world interaction

- [ ] Player spawns and can move/look in room.
- [ ] Computer shows `[E] Use Computer` when in range.
- [ ] AC shows interaction prompt only when `HandleAnomaly3D + FREEZE active`.

### 2) Computer enter/exit and desktop ownership

- [ ] Interacting with computer requests `RequestPhaseChange(DesktopIdle)`.
- [ ] `WBP_DesktopRoot` is created/owned by `BP_SignalPlayerController`.
- [ ] Desktop input mode enables cursor and disables room movement.
- [ ] Leave desktop (`LeaveDesktop`) works only in `DesktopIdle` / `ChatActive`.

### 3) Chat and minigame start

- [ ] Chat app opens through desktop root app host.
- [ ] Sending message updates chat history.
- [ ] Sending Colleague A message increases FREEZE weight.
- [ ] Minigame starts via `BP_MinigameManager.StartMinigame(DependencyMatch)` and renders in `AppContentHost`.

### 4) FREEZE trigger and popup

- [ ] `FREEZE` popup appears once when threshold is met.
- [ ] Duplicate trigger is ignored while `bAnomalyActive == true`.
- [ ] Popup is created/owned by `BP_MinigameManager` and routes choice back to it.

### 5) Branch A: Report Supervisor

- [ ] Choosing `Report Supervisor` records route as `ReportSupervisor`.
- [ ] Anomaly resolves without entering `HandleAnomaly3D`.
- [ ] Minigame closes/returns per wiring order, then enters `ReportPhase`.

### 6) Branch B: Handle Myself -> AC resolve

- [ ] Choosing `Handle Myself` enters `HandleAnomaly3D` and exits desktop.
- [ ] Computer cannot be used while anomaly unresolved in 3D handling phase.
- [ ] AC interaction resolves FREEZE and calls hidden unlock for ColleagueA.
- [ ] After AC resolve, phase returns to `RoomExplore` (not auto `DesktopIdle`).

### 7) Hidden option gate and report gate

- [ ] Hidden option does not appear until player manually returns to computer.
- [ ] Hidden option appears for Colleague A after manual return.
- [ ] Hidden option is deduped by `OptionId` (no duplicates).
- [ ] Report remains locked on `HandleMyself` route until hidden dialogue is consumed.

### 8) Report submit and ending

- [ ] Report app opens only when `CanOpenReport()` passes.
- [ ] Report submit requires selected sentence.
- [ ] Submit calls `SubmitSliceReport` on flow manager.
- [ ] Ending is triggered by flow manager and displayed by player controller.
- [ ] Ending subtitles are loaded from `DT_EndingSubtitles` rows.

---

## Failure logging template

Use one row per failure.

| Step | Expected | Actual | Suspected Owner Boundary | Blocking? |
|---|---|---|---|---|
| example | Enter ReportPhase only after gate opens | Report opened early | `WBP_DesktopRoot` bypassed `CanOpenReport` | Yes |
