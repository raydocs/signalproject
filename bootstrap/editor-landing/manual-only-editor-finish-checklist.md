# Manual-Only Editor Finish Checklist

Purpose: list the remaining Unreal Editor work that is **not worth automating further** after the hybrid runtime, DataTables, shell assets, level creation, class defaults, actor placement, and deterministic instance bindings have already been scripted.

---

## 0. Already automated — do **not** redo by hand unless something is broken

These items are already in place:

- Native UE5.7 project/module scaffold
- Runtime C++ classes under `Source/`
- Slice DataTables imported under `/Game/Signal/Data/DataTables/`
- Derived BP / UMG shell assets created under `/Game/Signal/Blueprints/`
- `LV_ApartmentMain` created under `/Game/Signal/Levels/Gameplay/`
- Startup map config points to `LV_ApartmentMain`
- Deterministic class-default bindings:
  - controller widget classes
  - minigame widget classes
  - desktop child widget classes
  - report / ending / chat DataTable defaults
- Deterministic level actor placement:
  - flow manager
  - route state manager
  - anomaly manager
  - chat conversation manager
  - minigame manager
  - hidden dialogue unlocker
  - computer terminal
  - air conditioner
  - player start
- Deterministic instance bindings (validated)
- `CurrentDayIndex = 2` on the placed flow manager for Day 2 FREEZE testing

Reference scripts already used successfully:

- `bootstrap/editor-tools/unreal_python/import_slice_datatables.py`
- `bootstrap/editor-tools/unreal_python/create_slice_asset_shells.py`
- `bootstrap/editor-tools/unreal_python/apply_slice_class_defaults.py`
- `bootstrap/editor-tools/unreal_python/place_slice_level_actors.py`
- `bootstrap/editor-tools/unreal_python/validate_slice_editor_setup.py`
- `bootstrap/editor-tools/unreal_python/validate_slice_level_bindings.py`

---

## 1. Required manual decisions before first playable

- [ ] **Choose runtime entry path for playtesting**
  - Recommended: use a BP child game mode / controller path for easy editor-time defaults.
  - Verify the actual playable path uses the intended controller defaults (`DesktopRootWidgetClass`, `EndingTitleCardWidgetClass`).
  - If needed, set `World Settings -> GameMode Override` to `BP_SignalGameMode` and confirm it uses the intended controller / pawn classes.

- [ ] **Confirm the placed actors in `LV_ApartmentMain` are the BP children you want to keep**
  - Keep the labeled actors created by automation unless you are intentionally replacing them.
  - If you replace any actor, reassign the same label and restore the expected references.

---

## 2. Manual-only level / spatial setup

These are manual because they depend on visual judgment, pacing, and interaction feel.

- [ ] Build the apartment whitebox / room geometry in `LV_ApartmentMain`
- [ ] Position the desk/computer area so the player can clearly approach and use it
- [ ] Position the air conditioner so the player can physically reach it during `HandleAnomaly3D`
- [ ] Tune the **desk ↔ AC walk path** to roughly the expected `3–5 second` travel window from the docs
- [ ] Adjust `PlayerStart` transform so the initial spawn faces the intended room flow
- [ ] Add any required static meshes / props / simple blocking volumes
- [ ] Ensure computer and AC are not clipping or unreachable
- [ ] Add / position any desk-view anchor or camera helper actors if your interaction presentation needs them

References:

- `docs/lv-apartment-main-build-steps.md`
- `docs/room-layout-and-player-routing.md`

---

## 3. Manual-only interactable presentation setup

These are manual because they depend on art assets, collision tuning, and UX feel.

- [ ] Give `BP_ComputerTerminal` a visible mesh / presentation setup suitable for line-trace interaction
- [ ] Give `BP_AirConditionerUnit` a visible mesh / presentation setup suitable for line-trace interaction
- [ ] Verify collision / traceability for both interactables in-editor
- [ ] Verify prompt visibility distances feel correct in actual play
- [ ] If needed, add simple helper components for facing / interaction readability

---

## 4. Manual-only UMG visual tree and button hookup

The UMG shell assets exist, but their **designer trees, layout, and visual hookup** still need manual authoring.

### `WBP_DesktopRoot`
- [ ] Create the visual root layout
- [ ] Add a desktop app host/content area
- [ ] Add app-switch affordances for Chat / Report
- [ ] Hook button clicks to the native widget behavior / app switching flow

### `WBP_ChatApp`
- [ ] Build the conversation UI (contacts, history, send area)
- [ ] Add a visible message list region
- [ ] Add send interaction UI
- [ ] Add hidden-option presentation region
- [ ] Hook UI actions to native chat widget methods

### `WBP_MG_DependencyMatch`
- [ ] Build the minigame layout/grid
- [ ] Add any timer / score / pause visuals you want
- [ ] Hook tile/button interactions to the native minigame methods

### `WBP_AnomalyChoicePopup`
- [ ] Add title/body copy presentation
- [ ] Add `Report Supervisor` button
- [ ] Add `Handle Myself` button
- [ ] Hook both buttons to the native popup handlers

### `WBP_ReportEditor`
- [ ] Build the report sentence list UI
- [ ] Add selected-state visual feedback
- [ ] Add submit button
- [ ] Hook selection + submit to native report widget methods

### `WBP_EndingTitleCard`
- [ ] Build title/subtitle presentation
- [ ] Add restart/quit controls if desired
- [ ] Hook restart/quit widgets if you intend to use them in first playable

Reference:

- `docs/ui-widget-tree-structure.md`

---

## 5. Manual-only blueprint tuning / defaults pass

These items are not complex to script, but are usually easier and safer to finalize by hand while looking at the actual assets.

- [ ] Inspect each BP shell for sensible transform / visual defaults
- [ ] Inspect minigame / anomaly thresholds and tune if the current defaults feel off
- [ ] Confirm any optional BP child defaults you want to use instead of native defaults
- [ ] Ensure DataTable-backed widgets display sensible copy in-editor

---

## 6. Manual playthrough / smoke test

This is the main remaining non-automatable milestone.

Run:

- `bootstrap/first-playable-smoke-test.md`

Manual pass criteria:

- [ ] Player can spawn, move, and look in room
- [ ] Computer interaction works in 3D
- [ ] Desktop root actually appears and is usable
- [ ] Chat UI is usable enough to send a message
- [ ] FREEZE can be triggered through the intended loop
- [ ] Popup appears and both branches can be chosen
- [ ] `Report Supervisor` path reaches report without entering room handling
- [ ] `Handle Myself` path exits to room, requires AC interaction, then allows hidden dialogue path
- [ ] Report submit reaches ending
- [ ] Ending subtitles display from `DT_EndingSubtitles`

---

## 7. Optional polish after first playable

These should wait until the first playable chain works.

- [ ] Art pass / lighting / dressing
- [ ] UI polish / animations / transitions
- [ ] Better interaction prompts / sounds / FX
- [ ] Camera polish
- [ ] Additional readability tweaks for anomaly state

---

## Done when

You are at a trustworthy first-playable manual finish line when all of these are true:

- [ ] `LV_ApartmentMain` is spatially navigable and readable
- [ ] Computer and AC are visually present and interactable
- [ ] All six UMG assets have usable visual trees and hooked controls
- [ ] Project enters the intended desktop / chat / minigame / anomaly / report / ending loop
- [ ] Both anomaly branches behave according to `bootstrap/first-playable-smoke-test.md`
- [ ] No automated placement/binding work needs to be rerun unless assets are intentionally replaced
