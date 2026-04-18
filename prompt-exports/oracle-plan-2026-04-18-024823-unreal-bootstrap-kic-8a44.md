## Final Prompt
<taskname="Unreal bootstrap kickoff"/>
<task>Create a practical project-bootstrap implementation plan to start this Unreal vertical slice immediately in a docs-only repository. The docs are already harmonized and slice-authoritative. Output 2-4 work items max, with item 1 being actionable right now by a delegated agent in-repo. Distinguish clearly between work that can be done now in the repository (text/scaffold/process artifacts) and work that requires Unreal Editor-generated assets (.uproject, Content/, Config/, Source/, .uasset creation/wiring). Include verification criteria and likely blockers per work item.</task>

<architecture>
The selected docs define a frozen Day1-Day3 slice contract and the required runtime ownership model:
- Scope/state/interaction authority: `vertical-slice-scope`, `game-state-machine`, `interaction-spec`.
- Runtime object ownership and wiring: `vertical-slice-18-blueprints-implementation-spec`, `vertical-slice-blueprint-wiring-order`.
- Data/schema/import authority: `blueprint-variables-events-and-data-fields`, `day1-day3-datatable-ready-script`.
- Bootstrap and repo structure references: `unreal-project-bootstrap-and-blueprint-build-order`, `project-structure-and-blueprint-list`.
- Level/UI implementation shape: `lv-apartment-main-build-steps`, `room-layout-and-player-routing`, `ui-widget-tree-structure`.
The repository currently has docs only and no Unreal skeleton files.
</architecture>

<selected_context>
docs/vertical-slice-scope.md: Slice execution boundary is frozen to Day1 onboarding + Day2 FREEZE full loop + Day3 simplified ending wrapper; only FREEZE executes.
docs/game-state-machine.md: Canonical phases, transition guards, input ownership, and lock rules (`CanEnterDesktop`, anomaly-choice behavior, report gating).
docs/interaction-spec.md: Authoritative 3D/2D interaction contracts and forbidden bypasses (especially HandleMyself -> AC resolve -> manual return to computer).
docs/vertical-slice-18-blueprints-implementation-spec.md: Exact 18-object slice set, per-object ownership, required vars/functions, and implementation completion criteria.
docs/vertical-slice-blueprint-wiring-order.md: Ordered wiring plan from level-placed manager refs through branch handling, hidden dialogue unlock, report, and ending.
docs/blueprint-variables-events-and-data-fields.md: Frozen enum/struct/table names, field schema, consumer mapping, and minimal must-build data assets for slice.
docs/day1-day3-datatable-ready-script.md: Import-ready Day1-Day3 DataTable rows and required import order.
docs/unreal-project-bootstrap-and-blueprint-build-order.md: Bootstrap gates, immediate safe-start tasks, folder setup guidance, and 48h slice object creation order.
docs/project-structure-and-blueprint-list.md: Full-version structure and naming conventions useful for stable repository skeleton planning (slice docs still override behavior).
docs/lv-apartment-main-build-steps.md: Practical level setup sequence for LV_ApartmentMain and first playable FREEZE routing validation.
docs/room-layout-and-player-routing.md: Spatial layout and movement-time constraints for computer<->AC anomaly loop.
docs/ui-widget-tree-structure.md: Concrete widget-tree shells and host/container expectations for DesktopRoot/Chat/Minigame/Report/Ending.
</selected_context>

<relationships>
- `vertical-slice-scope` constrains all runtime build decisions in `vertical-slice-18...` and `...wiring-order`.
- `game-state-machine` + `interaction-spec` define guardrails that wiring and object ownership must implement.
- `blueprint-variables-events-and-data-fields` defines schema consumed by systems implemented in `vertical-slice-18...`; `day1-day3-datatable-ready-script` supplies row content for those schemas.
- `unreal-project-bootstrap-and-blueprint-build-order` and `project-structure-and-blueprint-list` inform bootstrap sequencing and folder conventions, but must not override slice-authoritative behavior.
- `lv-apartment-main-build-steps` + `room-layout-and-player-routing` provide validation geometry/flow for the Day2 FREEZE branch.
- `ui-widget-tree-structure` aligns with ownership constraints in `vertical-slice-18...` and app-host rules in wiring order.
</relationships>

<ambiguities>
None. The provided context states docs are harmonized and slice-authoritative; treat naming/ownership/phase contracts as frozen for kickoff planning.
</ambiguities>

<planning_constraints>
- Keep decomposition to 2-4 work items max.
- Make work item 1 immediately executable in-repo (without Unreal Editor).
- Explicitly tag each work item as `in-repo now` or `requires Unreal Editor`.
- For each item include: goal, concrete outputs, verification criteria, blockers/dependencies.
</planning_constraints>

## Selection
- Files: 12 total (12 full)
- Total tokens: 37592 (Auto view)
- Token breakdown: full 37592

### Files
### Selected Files
/Users/ruirui/Downloads/GitHub/signalproject/
└── docs/
    ├── blueprint-variables-events-and-data-fields.md — 3,114 tokens (full)
    ├── day1-day3-datatable-ready-script.md — 2,590 tokens (full)
    ├── game-state-machine.md — 2,866 tokens (full)
    ├── interaction-spec.md — 2,397 tokens (full)
    ├── lv-apartment-main-build-steps.md — 1,439 tokens (full)
    ├── project-structure-and-blueprint-list.md — 5,208 tokens (full)
    ├── room-layout-and-player-routing.md — 2,014 tokens (full)
    ├── ui-widget-tree-structure.md — 3,130 tokens (full)
    ├── unreal-project-bootstrap-and-blueprint-build-order.md — 3,417 tokens (full)
    ├── vertical-slice-18-blueprints-implementation-spec.md — 4,450 tokens (full)
    ├── vertical-slice-blueprint-wiring-order.md — 4,537 tokens (full)
    └── vertical-slice-scope.md — 2,430 tokens (full)


---

## Generated Plan

## Chat Send ✅
- **Chat**: `unreal-bootstrap-kickoff-4FF0C5` | **Mode**: plan

### Response
## 1. Summary

This is a **targeted bootstrap enablement** task, not a doc refactor: the repository already contains the frozen slice contract, so the right approach is to **derive execution artifacts from the existing docs without changing their authority**, then let Unreal Editor generate the actual project/assets from that package. The practical plan is four work items: **(1) in-repo bootstrap pack now**, **(2) Unreal project/data scaffold**, **(3) slice asset shells + whitebox level**, and **(4) runtime wiring + smoke validation**.

## 2. Current-state analysis

### What exists today

The repo is **docs-only**. The selected docs already define the runtime architecture end-to-end:

- **Scope/behavior authority**
  - `docs/vertical-slice-scope.md`
  - `docs/game-state-machine.md`
  - `docs/interaction-spec.md`

- **Runtime ownership / object model**
  - `docs/vertical-slice-18-blueprints-implementation-spec.md`
  - `docs/vertical-slice-blueprint-wiring-order.md`

- **Schema / data import authority**
  - `docs/blueprint-variables-events-and-data-fields.md`
  - `docs/day1-day3-datatable-ready-script.md`

- **Bootstrap / folder / level / UI shape references**
  - `docs/unreal-project-bootstrap-and-blueprint-build-order.md`
  - `docs/project-structure-and-blueprint-list.md`
  - `docs/lv-apartment-main-build-steps.md`
  - `docs/room-layout-and-player-routing.md`
  - `docs/ui-widget-tree-structure.md`

### Runtime ownership and call chain already frozen in docs

The authoritative runtime chain is:

`BP_SignalPlayerCharacter`
→ `BPI_Interactable`
→ `BP_ComputerTerminal`
→ `BP_SignalGameFlowManager.RequestPhaseChange(DesktopIdle)`
→ `BP_SignalPlayerController.ShowDesktopRoot / SetDesktopInputMode`
→ `WBP_DesktopRoot`
→ `WBP_ChatApp`
→ `BP_ChatConversationManager.SendPlayerMessage`
→ `BP_AnomalyManager.AccumulateSideEffect`
→ `BP_MinigameManager.StartMinigame`
→ `BP_AnomalyManager.TriggerAnomaly(FREEZE)`
→ `BP_MinigameManager.HandleAnomalyTriggered`
→ `WBP_AnomalyChoicePopup`
→ branch:
- `ReportSupervisor` → resolve anomaly → close minigame → `ReportPhase`
- `HandleMyself` → `HandleAnomaly3D` → `BP_AirConditionerUnit.Interact`
→ `BP_HiddenDialogueUnlocker.UnlockForColleague(ColleagueA)`
→ `BP_ChatConversationManager.InjectHiddenOption`
→ player manually returns to computer
→ hidden dialogue consumed
→ `WBP_ReportEditor`
→ `BP_SignalGameFlowManager.SubmitSliceReport`
→ `BP_SignalPlayerController.ShowEndingTitleCard`

### Hard constraints that must not drift during bootstrap

- `BP_SignalGameFlowManager` is the **only owner** of `CurrentPhase`.
- No widget/world actor directly sets phase; all transitions go through `RequestPhaseChange`.
- Core manager refs are **level-placed + manually assigned**; no normative `GetAllActorsOfClass`.
- Widget dependencies are passed via **Expose on Spawn**.
- Only `FREEZE` executes in the slice.
- Only the frozen **18-object slice set** is required; do not add full-version managers as bootstrap dependencies.
- DataTables are fixed to the six slice tables, with exact enum/struct/field names from `docs/blueprint-variables-events-and-data-fields.md`.

### What is reusable vs. what is blocking

**Reusable immediately**
- All naming, ownership, variable/function checklists, import schemas, and row content.
- Build order, level layout constraints, widget host/container expectations.

**Blocking today**
- No `.uproject`
- No `Config/`
- No `Content/Signal/`
- No `.uasset` enums/structs/DataTables/Blueprints/widgets/level
- No extracted CSV import files
- No single bootstrap entrypoint for a delegated agent

### Why this should be a targeted bootstrap pass, not a broader refactor

The docs are already harmonized and authoritative. Refactoring or rewriting them would create duplication risk and move authority. The missing piece is **operationalization**: turning frozen docs into a repo-side handoff pack, then into Unreal-generated assets.

## 3. Design

## Work Item 1 — Bootstrap handoff pack  
**Tag:** `in-repo now`

### Goal
Create the repo artifacts that let an in-repo delegated agent prepare the Unreal handoff **without Unreal Editor** and without reinterpreting the docs.

### Concrete outputs
Create a new top-level bootstrap package, keeping existing authority docs unchanged:

1. `bootstrap/README.md`  
   Single entrypoint that states:
   - authoritative docs
   - derived artifacts in this folder
   - execution order across all work items
   - explicit “repo-only vs Unreal Editor” split

2. `bootstrap/asset-manifest.csv`  
   One row per required bootstrap asset with fields like:
   ```text
   AssetName,AssetKind,TargetPath,CreationMode,AuthorityDoc,BlockingFor,Notes
   ```
   Include:
   - Unreal project root files
   - 6 enums
   - 8 structs
   - 6 DataTables
   - `LV_ApartmentMain`
   - the 18 slice assets

3. `bootstrap/import-data/DT_SystemCopy.csv`
4. `bootstrap/import-data/DT_SupervisorLines.csv`
5. `bootstrap/import-data/DT_NormalReplies.csv`
6. `bootstrap/import-data/DT_HiddenDialogues.csv`
7. `bootstrap/import-data/DT_ReportSentencePools.csv`
8. `bootstrap/import-data/DT_EndingSubtitles.csv`  
   These are extracted directly from `docs/day1-day3-datatable-ready-script.md`, with no field renaming.

9. `bootstrap/editor-bootstrap-checklist.md`  
   Checklist for Work Items 2-4:
   - project creation
   - folder setup
   - enum/struct/data table creation
   - 18-asset shell creation
   - level placement/manual refs
   - smoke test gates

10. `bootstrap/first-playable-smoke-test.md`  
    Scenario-based validation sheet:
    - room movement
    - computer enter/exit
    - chat send
    - minigame start
    - FREEZE popup
    - both anomaly branches
    - AC resolve
    - hidden option gate
    - report gate
    - ending display

### State/data flow
This work item creates the transformation boundary:

`authoritative docs`  
→ `repo-side manifest + raw CSV imports + checklists`  
→ consumed manually in Unreal Editor during Work Items 2-4

No runtime logic changes occur here.

### Verification criteria
- No existing authority doc is edited.
- `bootstrap/asset-manifest.csv` lists every required slice bootstrap asset exactly once.
- The six CSV files match the frozen headers and import order.
- Row counts match the current slice data:
  - `DT_SystemCopy`: 33
  - `DT_SupervisorLines`: 13
  - `DT_NormalReplies`: 22
  - `DT_HiddenDialogues`: 4
  - `DT_ReportSentencePools`: 8
  - `DT_EndingSubtitles`: 8
- The bootstrap README clearly labels which steps require Unreal Editor.

### Likely blockers / dependencies
- None external; this is immediately executable in-repo.
- Main implementation risk is accidental duplication of behavioral rules. Mitigation: derived files must reference source docs instead of restating full logic.

---

## Work Item 2 — Unreal project and data foundation  
**Tag:** `requires Unreal Editor`

### Goal
Generate the Unreal project skeleton and the frozen data/schema assets so the slice can be built against actual `.uasset` types.

### Concrete outputs

#### Project root
- `SignalProject.uproject`
- `Config/DefaultEngine.ini`
- `Config/DefaultGame.ini`
- `Config/DefaultInput.ini`

Assumption: **Blueprint-only UE5 project**, no `Source/` at bootstrap stage.

#### Content folder scaffold
Create:
- `Content/Signal/Blueprints/...`
- `Content/Signal/Data/Enums`
- `Content/Signal/Data/Structs`
- `Content/Signal/Data/DataTables`
- `Content/Signal/Levels/Gameplay`

#### Data assets
Create exact enum assets:
- `E_GamePhase`
- `E_AnomalyType`
- `E_ColleagueId`
- `E_SkillUnlockState`
- `E_RouteBranch`
- `E_MinigameType`

Create exact struct assets:
- `ST_ChatMessageRecord`
- `ST_HiddenOptionRecord`
- `ST_SupervisorLineRow`
- `ST_NormalReplyRow`
- `ST_HiddenDialogueRow`
- `ST_SystemCopyRow`
- `ST_ReportSentenceRow`
- `ST_EndingSubtitleRow`

Create/import exact DataTables:
- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

Import source must be the six CSV files from Work Item 1, in the frozen import order.

#### Input/config
Add action mappings needed by slice bootstrap:
- `Interact`
- `LeaveDesktop`

Retain template movement/look mappings.

### State/data flow
`bootstrap/import-data/*.csv`
→ Unreal DataTable import
→ consumed later by:
- `BP_ChatConversationManager`
- `WBP_ReportEditor`
- `WBP_EndingTitleCard`
- interaction/system-copy consumers listed in schema doc

No runtime save/persistence is introduced; DataTables are the only structured content boundary.

### Verification criteria
- Project opens cleanly.
- No `Source/` folder is introduced.
- All enum names, struct names, and DataTable names match docs exactly.
- No out-of-scope tables (`DT_DayConfigs`, `DT_AnomalyConfigs`) are created for slice bootstrap.
- All six DataTables import with correct row counts and correct struct bindings.
- `DefaultInput.ini` includes `Interact` and `LeaveDesktop`.

### Likely blockers / dependencies
- Depends on Work Item 1 CSV extraction.
- Unreal version must be chosen before creation; validate against UE5.x availability.
- CSV encoding/import issues can break `Text` fields; validate with one Chinese text row from `DT_SystemCopy` before bulk import.

---

## Work Item 3 — Slice asset shells and whitebox level  
**Tag:** `requires Unreal Editor`

### Goal
Create the minimum Unreal assets and level geometry needed to start real slice implementation without inventing new ownership patterns.

### Concrete outputs

#### Level
- `LV_ApartmentMain`
- Whitebox room per `docs/lv-apartment-main-build-steps.md`
- Computer and AC positions satisfying the `3–5 second` walk constraint from `docs/room-layout-and-player-routing.md`
- A placed `DeskViewAnchor` as an Empty Actor or equivalent level actor; do **not** add a new slice dependency Blueprint just for bootstrap

#### 18 slice assets
Create shells for exactly:
1. `BP_SignalGameMode`
2. `BP_SignalPlayerController`
3. `BP_SignalPlayerCharacter`
4. `BP_SignalGameFlowManager`
5. `BP_RouteStateManager`
6. `BP_AnomalyManager`
7. `BP_ComputerTerminal`
8. `BP_AirConditionerUnit`
9. `WBP_DesktopRoot`
10. `WBP_ChatApp`
11. `BP_ChatConversationManager`
12. `BP_MinigameManager`
13. `WBP_MG_DependencyMatch`
14. `WBP_AnomalyChoicePopup`
15. `BP_HiddenDialogueUnlocker`
16. `WBP_ReportEditor`
17. `WBP_EndingTitleCard`
18. `BPI_Interactable`

Each asset should contain the required variables/functions from `docs/vertical-slice-18-blueprints-implementation-spec.md`, but remain slice-scoped:
- no `BP_ReportManager`
- no `BP_AnomalyChoiceDirector`
- no `BP_SignalHUDManager`

#### Widget shells
Build only the containers and named widgets needed for host ownership:
- `WBP_DesktopRoot`: `AppContentHost`, `ChatAppButton`, `ReportAppButton`
- `WBP_ChatApp`: contacts, message list host, send button, hidden option region
- `WBP_MG_DependencyMatch`: tile grid, timer/score, pause state
- `WBP_AnomalyChoicePopup`: title/body/report/handle buttons
- `WBP_ReportEditor`: options host, selected state, submit button
- `WBP_EndingTitleCard`: ending title, subtitle host, restart/quit

#### Level-placed actors
Place and manually assign:
- `BP_SignalGameFlowManager`
- `BP_RouteStateManager`
- `BP_AnomalyManager`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `BP_HiddenDialogueUnlocker`
- `BP_ComputerTerminal`
- `BP_AirConditionerUnit`

And bind the Step 0 refs from `docs/vertical-slice-blueprint-wiring-order.md` in Details panels.

### State/data flow
All assets remain within the frozen ownership model:
- level actors own runtime manager state
- controller owns `WBP_DesktopRoot` and `WBP_EndingTitleCard`
- desktop root hosts child app widgets
- minigame manager owns popup lifecycle

All Blueprint runtime remains on the Unreal game thread; there is no concurrent subsystem design to add here.

### Verification criteria
- All 18 assets exist and compile/save.
- `LV_ApartmentMain` loads with `GameMode Override = BP_SignalGameMode`.
- All eight required level-placed actors are present.
- Every required manual reference in Step 0 is assigned.
- Widget shells expose the required container/button variables.
- No shell introduces direct phase mutation or `GetAllActorsOfClass` as a core dependency.

### Likely blockers / dependencies
- Depends on Work Item 2 type assets.
- Whitebox placement can drift from routing docs if done without the movement-time constraint.
- Most common blocker is missing manual refs on placed managers; treat this as a hard gate before wiring.

---

## Work Item 4 — Runtime wiring and first-playable validation  
**Tag:** `requires Unreal Editor`

### Goal
Wire the slice in the exact frozen order and prove the Day 2 `FREEZE` loop plus Day 3 ending gate is playable.

### Concrete outputs

Implement wiring in three passes, following `docs/vertical-slice-blueprint-wiring-order.md`:

#### Pass A — room ↔ desktop
- GameMode → PlayerController → Character
- `InitializeGameFlow`
- `RegisterSliceRefs`
- `BPI_Interactable` computer path
- `ShowDesktopRoot` / `SetDesktopInputMode`
- `ChatActive` entry
- `Leave Desktop` only in `DesktopIdle` / `ChatActive`

#### Pass B — chat → minigame → FREEZE → branch
- `SendPlayerMessage` accumulates Colleague A side effect
- `StartMinigame(DependencyMatch)`
- anomaly callback binding
- `ResolveDominantAnomaly`
- popup creation via `BP_MinigameManager`
- `ReportSupervisor` and `HandleMyself` branch handling

#### Pass C — 3D resolve → hidden dialogue → report → ending
- `HandleAnomaly3D` exits desktop and restores room input
- AC interaction resolves anomaly and unlocks hidden option
- player must manually return to computer
- hidden option injection and consumption
- `CanOpenReport` gating
- `SubmitSliceReport`
- `TriggerEnding`
- `ShowEndingTitleCard`

### Required guard behavior
These must be implemented and validated explicitly:
- `RequestPhaseChange(CurrentPhase)` = no-op
- `TriggerAnomaly` ignored when `bAnomalyActive == true`
- computer locked during unresolved `HandleAnomaly3D`
- hidden option deduped by `OptionId`
- `HandleMyself` cannot open report before hidden dialogue consumption
- `ReportSupervisor` never enters `HandleAnomaly3D`
- AC resolve returns to `RoomExplore`, not `DesktopIdle`

### Verification criteria
Use `bootstrap/first-playable-smoke-test.md` and require all of the following:
1. Player can move in room.
2. Computer shows `[E] Use Computer`.
3. Interacting enters `DesktopIdle`.
4. `WBP_DesktopRoot` is controller-owned and visible.
5. Chat send updates history and increases FREEZE weight.
6. Minigame starts inside `AppContentHost`.
7. FREEZE popup appears only once.
8. `Report Supervisor` resolves anomaly and proceeds to report path without 3D return.
9. `Handle Myself` exits to room, locks computer, enables AC.
10. AC resolve unlocks hidden option and returns to `RoomExplore`.
11. Hidden option appears only after manual return to computer.
12. Hidden dialogue consumption gates report access.
13. Report submit triggers ending through flow manager.
14. Ending card is controller-owned and displays subtitle rows from `DT_EndingSubtitles`.

### Likely blockers / dependencies
- Depends on Work Item 3 shells and manual refs.
- Highest-risk failures:
  - phase changes bypassing flow manager
  - missing Expose-on-Spawn refs to widgets
  - popup lifecycle split between widget and manager
  - report gate implemented in widget instead of flow manager

## 4. File-by-file impact

### Existing files to keep unchanged
Do **not** modify these authority docs during bootstrap:
- `docs/vertical-slice-scope.md`
- `docs/game-state-machine.md`
- `docs/interaction-spec.md`
- `docs/vertical-slice-18-blueprints-implementation-spec.md`
- `docs/vertical-slice-blueprint-wiring-order.md`
- `docs/blueprint-variables-events-and-data-fields.md`
- `docs/day1-day3-datatable-ready-script.md`

### New repo-authored files

| Path | What changes | Why | Dependencies |
|---|---|---|---|
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/README.md` | New bootstrap entrypoint doc | Gives delegated agents one starting doc | None |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/asset-manifest.csv` | New asset inventory | Prevents naming/folder drift in Editor work | None |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_SystemCopy.csv` | Extracted CSV | Direct Unreal import source | `day1-day3-datatable-ready-script.md` |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_SupervisorLines.csv` | Extracted CSV | Direct Unreal import source | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_NormalReplies.csv` | Extracted CSV | Direct Unreal import source | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_HiddenDialogues.csv` | Extracted CSV | Direct Unreal import source | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_ReportSentencePools.csv` | Extracted CSV | Direct Unreal import source | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/import-data/DT_EndingSubtitles.csv` | Extracted CSV | Direct Unreal import source | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/editor-bootstrap-checklist.md` | New execution checklist | Converts docs into ordered editor tasks | asset manifest |
| `/Users/ruirui/Downloads/GitHub/signalproject/bootstrap/first-playable-smoke-test.md` | New validation checklist | Defines pass/fail for first playable slice | asset manifest |

### Unreal root/config files

| Path | What changes | Why | Dependencies |
|---|---|---|---|
| `/Users/ruirui/Downloads/GitHub/signalproject/SignalProject.uproject` | New UE project file | Required to create all assets | Work Item 1 |
| `/Users/ruirui/Downloads/GitHub/signalproject/Config/DefaultEngine.ini` | Generated/updated | Project maps/settings | project creation |
| `/Users/ruirui/Downloads/GitHub/signalproject/Config/DefaultGame.ini` | Generated/updated | Project defaults | project creation |
| `/Users/ruirui/Downloads/GitHub/signalproject/Config/DefaultInput.ini` | Generated/updated | `Interact` / `LeaveDesktop` mappings | project creation |

### Unreal data assets

| Path | What changes | Why | Dependencies |
|---|---|---|---|
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_GamePhase.uasset` | New enum | Frozen phase set | Work Item 1 |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_AnomalyType.uasset` | New enum | Frozen anomaly set | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_ColleagueId.uasset` | New enum | Chat/data identity | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_SkillUnlockState.uasset` | New enum | Hidden-line state | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_RouteBranch.uasset` | New enum | Branch results | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Enums/E_MinigameType.uasset` | New enum | Minigame type | same |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_ChatMessageRecord.uasset` | New struct | Runtime chat state | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_HiddenOptionRecord.uasset` | New struct | Hidden option runtime state | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_SupervisorLineRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_NormalReplyRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_HiddenDialogueRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_SystemCopyRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_ReportSentenceRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/Structs/ST_EndingSubtitleRow.uasset` | New struct | DataTable row schema | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_SystemCopy.uasset` | New DataTable | System copy consumption | structs + CSV |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_SupervisorLines.uasset` | New DataTable | Supervisor text | structs + CSV |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_NormalReplies.uasset` | New DataTable | Colleague replies | structs + CSV |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_HiddenDialogues.uasset` | New DataTable | Hidden dialogue lines | structs + CSV |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_ReportSentencePools.uasset` | New DataTable | Report options | structs + CSV |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Data/DataTables/DT_EndingSubtitles.uasset` | New DataTable | Ending subtitle display | structs + CSV |

### Unreal level and runtime assets

| Path | What changes | Why | Dependencies |
|---|---|---|---|
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Levels/Gameplay/LV_ApartmentMain.umap` | New main level | Whitebox + level-placed actors | project scaffold |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Framework/BP_SignalGameMode.uasset` | New Blueprint | GameMode setup | data scaffold |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Player/BP_SignalPlayerController.uasset` | New Blueprint | UI ownership/input mode | data scaffold |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Player/BP_SignalPlayerCharacter.uasset` | New Blueprint | movement/interaction trace | input config |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Core/BP_SignalGameFlowManager.uasset` | New Blueprint | phase authority | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Core/BP_RouteStateManager.uasset` | New Blueprint | route state owner | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Anomalies/BP_AnomalyManager.uasset` | New Blueprint | anomaly owner | enums |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Interactables/BP_ComputerTerminal.uasset` | New Blueprint | room→desktop interaction | BPI + flow manager |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Interactables/BP_AirConditionerUnit.uasset` | New Blueprint | FREEZE resolve point | anomaly + unlocker |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Interactables/BPI_Interactable.uasset` | New Blueprint Interface | unified world interactions | none |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/UI/WBP_DesktopRoot.uasset` | New Widget | desktop host container | controller refs |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/UI/WBP_ChatApp.uasset` | New Widget | chat UI | desktop root + conversation manager |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Dialogue/BP_ChatConversationManager.uasset` | New Blueprint | chat data owner | DataTables |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Minigames/BP_MinigameManager.uasset` | New Blueprint | minigame lifecycle owner | flow/anomaly refs |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Minigames/WBP_MG_DependencyMatch.uasset` | New Widget | slice minigame | minigame manager |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/UI/WBP_AnomalyChoicePopup.uasset` | New Widget | branch popup | minigame manager |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Dialogue/BP_HiddenDialogueUnlocker.uasset` | New Blueprint | unlock injection coordinator | route/chat refs |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Report/WBP_ReportEditor.uasset` | New Widget | report submission UI | DataTable + flow manager |
| `/Users/ruirui/Downloads/GitHub/signalproject/Content/Signal/Blueprints/Endings/WBP_EndingTitleCard.uasset` | New Widget | ending display | DataTable + controller |

## 6. Implementation order

1. **Create Work Item 1 artifacts in-repo first**: bootstrap README, asset manifest, six CSVs, editor checklist, smoke-test checklist.
2. **Review the manifest against the frozen docs** and approve it as the only bootstrap execution pack; do not edit authority docs.
3. **Create the UE5 Blueprint-only project at repo root** and commit/generated-save `.uproject` + minimal config files.
4. **Create `Content/Signal` folder structure**, then create enums, then structs, then import the six DataTables from `bootstrap/import-data`.
5. **Create `LV_ApartmentMain` whitebox** and set movement/input baseline; place `DeskViewAnchor` as a level actor.
6. **Create the 18 slice assets as shells**, with required variables/functions only; keep all non-slice/full-version classes out.
7. **Place the eight required level actors and bind all Step 0 refs** before any gameplay wiring; this step is atomic because later blueprint logic depends on those refs.
8. **Wire gameplay in the documented order**: room/desktop → chat/minigame/FREEZE → branch handling → AC resolve/hidden dialogue/report/ending.
9. **Run the smoke-test checklist** and record failures by ownership boundary: refs, phase authority, widget ownership, popup lifecycle, report gating.
10. **Only after smoke test passes**, allow polish tasks (art replacement, overlays, extra UI child widgets, debug tools).


> 💡 Continue this plan conversation with ask_oracle(chat_id: "unreal-bootstrap-kickoff-4FF0C5", new_chat: false)