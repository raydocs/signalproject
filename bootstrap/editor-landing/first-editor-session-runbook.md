# First Editor Session Runbook (Hybrid Native Landing)

Goal: make the first Unreal Editor session deterministic and low-touch.

Scope: setup only. No speculative asset generation.

---

## 0) Preflight (outside Editor)

1. Ensure native compile is green for `SignalProjectEditor` in UE5.7.
2. If Mac build tools fail with Xcode plugin/load errors, resolve host Xcode state first (machine-level issue, not project code).

---

## 1) Open project and verify native surface

1. Open `SignalProject.uproject` in UE5.7.
2. Confirm module `SignalProject` loads.
3. In Class Viewer, verify key native classes exist:
   - `ASignalGameFlowManager`, `ARouteStateManager`, `AAnomalyManager`, `AChatConversationManager`, `AMinigameManager`, `AHiddenDialogueUnlocker`
   - `AComputerTerminal`, `AAirConditionerUnit`
   - `ASignalPlayerController`, `ASignalPlayerCharacter`, `ASignalGameMode`
   - Widget bases: `UDesktopRootWidget`, `UChatAppWidget`, `UDependencyMatchWidget`, `UAnomalyChoicePopupWidget`, `UReportEditorWidget`, `UEndingTitleCardWidget`

Optional script: run `bootstrap/editor-tools/unreal_python/validate_slice_editor_setup.py`.

---

## 2) Import DataTables (frozen order)

Option A (recommended): run `bootstrap/editor-tools/unreal_python/import_slice_datatables.py`.

Option B (manual): import in this exact order:

1. `DT_SystemCopy` (`FST_SystemCopyRow`)
2. `DT_SupervisorLines` (`FST_SupervisorLineRow`)
3. `DT_NormalReplies` (`FST_NormalReplyRow`)
4. `DT_HiddenDialogues` (`FST_HiddenDialogueRow`)
5. `DT_ReportSentencePools` (`FST_ReportSentenceRow`)
6. `DT_EndingSubtitles` (`FST_EndingSubtitleRow`)

CSV source folder: `bootstrap/import-data/`.

---

## 3) Create derived assets

Follow `bootstrap/editor-landing/native-class-to-derived-asset-matrix.md`.

Minimum first-session outputs:

- `WBP_DesktopRoot`, `WBP_ChatApp`, `WBP_MG_DependencyMatch`, `WBP_AnomalyChoicePopup`, `WBP_ReportEditor`, `WBP_EndingTitleCard`
- Recommended BP children for runtime actors/managers and controller

Do not attempt to auto-generate widget trees via script.

---

## 4) Create level and bind refs

1. Create `LV_ApartmentMain` under `/Game/Signal/Levels/Gameplay/`.
2. Place required runtime actor instances.
3. Apply bindings exactly from `bootstrap/editor-landing/level-manual-ref-binding-sheet.md`.
4. Set `CurrentDayIndex=2` on flow manager for deterministic Day2 FREEZE validation (or your chosen test day explicitly).

---

## 5) Finalize startup map config

After `LV_ApartmentMain` exists and saves cleanly:

- update `Config/DefaultEngine.ini`:
  - `GameDefaultMap=/Game/Signal/Levels/Gameplay/LV_ApartmentMain`
  - `EditorStartupMap=/Game/Signal/Levels/Gameplay/LV_ApartmentMain`

---

## 6) First validation gate

Run `bootstrap/first-playable-smoke-test.md`.

If any gate fails:

- log failure using the template in the smoke test doc
- fix ownership/binding issues first (before content polish)

---

## Exit criteria for first session

- [ ] DataTables imported with correct row structs
- [ ] Required derived assets created
- [ ] `LV_ApartmentMain` created with required runtime actors placed
- [ ] Manual refs assigned per binding sheet
- [ ] Startup map config updated
- [ ] Smoke test started with clear pass/fail record
