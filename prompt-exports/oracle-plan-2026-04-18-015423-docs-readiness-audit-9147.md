## Final Prompt
<taskname="Docs readiness audit"/>
<task>Audit the full `docs/` set for implementation readiness of the Unreal vertical slice. Identify gaps, inconsistencies, missing implementation detail, sequencing risks, and unclear handoff points. If docs are sufficient to start building, output a project kickoff plan; if not, output the minimum high-value doc remediation plan that should happen before implementation (target 2-4 work items).</task>

<architecture>The documentation corpus is organized into: (1) scope and roadmap (`vertical-slice-scope`, `day-by-day-content-sheet`), (2) Unreal architecture/class inventory (`project-structure-and-blueprint-list`, `unreal-project-bootstrap-and-blueprint-build-order`, `vertical-slice-18-blueprints-implementation-spec`), (3) implementation mechanics (`vertical-slice-blueprint-wiring-order`, `blueprint-pseudonode-flowcharts`, `vertical-slice-blueprint-variable-panel-checklist`, `blueprint-variables-events-and-data-fields`), (4) flow and UX contracts (`game-state-machine`, `interaction-spec`, `ui-wireframes-and-screen-flow`, `ui-widget-tree-structure`), (5) world build docs (`room-layout-and-player-routing`, `lv-apartment-main-build-steps`), and (6) narrative/data content (`narrative-script-and-system-copy`, `day1-day3-datatable-ready-script`, plus asset planning docs).</architecture>

<selected_context>
docs/vertical-slice-scope.md: 48h target scope and P0/P1/P2; declares Day1-3 slice and FREEZE-first strategy.
docs/unreal-project-bootstrap-and-blueprint-build-order.md: UE bootstrap + full build phases + separate 48h 18-object creation sequence.
docs/vertical-slice-18-blueprints-implementation-spec.md: per-object responsibilities/variables/functions/completion criteria for 18 slice objects.
docs/vertical-slice-blueprint-wiring-order.md: stepwise wiring for first playable chain and per-step acceptance checks.
docs/vertical-slice-blueprint-variable-panel-checklist.md: variable-level configuration matrix (type/default/editability/expose-on-spawn).
docs/blueprint-pseudonode-flowcharts.md: pseudonode event flows approximating UE Blueprint graph logic.
docs/blueprint-variables-events-and-data-fields.md: enums/structs/data table schema and manager/widget event contracts.
docs/project-structure-and-blueprint-list.md: full project architecture and complete class inventory beyond slice.
docs/blueprint-responsibility-map.md: responsibilities and communication boundaries across managers/widgets/world actors.
docs/game-state-machine.md: canonical state set, transitions, input ownership, subsystem pause rules.
docs/interaction-spec.md: 3D/2D interaction rules and trigger criteria.
docs/ui-wireframes-and-screen-flow.md: page wireframes and screen flow.
docs/ui-widget-tree-structure.md: widget-tree hierarchy and UI layering.
docs/lv-apartment-main-build-steps.md + docs/room-layout-and-player-routing.md: level build and player routing for LV_ApartmentMain.
docs/day1-day3-datatable-ready-script.md + docs/narrative-script-and-system-copy.md + docs/day-by-day-content-sheet.md: narrative/system copy and import-ready data scaffolding.
docs/art-asset-checklist.md: full-version art asset scope (not slice-bound).
docs/foundation-docs-needed.md: historical “missing docs” list; many listed docs now exist.
</selected_context>

<relationships>
- Scope docs (`vertical-slice-scope`) should constrain build-order docs (`unreal-project-bootstrap...`, `vertical-slice-18...`, `...wiring-order`).
- State machine (`game-state-machine`) is consumed by flow manager contracts in wiring/pseudonode docs.
- Data schema (`blueprint-variables-events-and-data-fields`) underpins conversation/anomaly/report systems and should align with DataTable script (`day1-day3-datatable-ready-script`).
- UI wireframes (`ui-wireframes...`) map to concrete widget-tree structures (`ui-widget-tree-structure`) and runtime app-hosting behavior in `WBP_DesktopRoot` docs.
- World layout/build docs (`room-layout...`, `lv-apartment-main-build-steps`) support anomaly handling loops specified in interaction and wiring docs.
- Responsibility map should match per-class implementation specs and manager ownership in wiring/pseudonode docs.
</relationships>

<ambiguities>
- Naming drift exists across docs: `DISK_CLEAN` vs `DISKCLEAN`; `E_RouteType` vs `E_RouteBranch`; `DT_EndingSequences` vs `DT_EndingSubtitles`; and enum sets differ between docs.
- Slice-vs-full overlap is inconsistent: some docs enforce FREEZE-only/Day1-3 for slice, while others include BLACKOUT/DISKCLEAN and Day4-7 logic in “core” flows.
- Ownership drift: report flow sometimes uses `BP_ReportManager`, elsewhere `WBP_ReportEditor` drives ending directly; anomaly-choice “report” branch target differs (resume minigame vs jump to report).
- `foundation-docs-needed.md` is partially stale relative to current repository state and may confuse planning priorities.
</ambiguities>

Focus output on a readiness verdict plus either:
1. a concrete kickoff plan (if sufficient), or
2. a prioritized pre-implementation doc remediation plan (2-4 items) with exact docs to update and why.

## Selection
- Files: 20 total (20 full)
- Total tokens: 55065 (Auto view)
- Token breakdown: full 55065

### Files
### Selected Files
/Users/ruirui/Downloads/GitHub/signalproject/
└── docs/
    ├── art-asset-checklist.md — 3,543 tokens (full)
    ├── blueprint-pseudonode-flowcharts.md — 5,179 tokens (full)
    ├── blueprint-responsibility-map.md — 1,576 tokens (full)
    ├── blueprint-variables-events-and-data-fields.md — 2,930 tokens (full)
    ├── day-by-day-content-sheet.md — 2,232 tokens (full)
    ├── day1-day3-datatable-ready-script.md — 2,906 tokens (full)
    ├── foundation-docs-needed.md — 1,869 tokens (full)
    ├── game-state-machine.md — 1,925 tokens (full)
    ├── interaction-spec.md — 1,409 tokens (full)
    ├── lv-apartment-main-build-steps.md — 1,439 tokens (full)
    ├── narrative-script-and-system-copy.md — 3,124 tokens (full)
    ├── project-structure-and-blueprint-list.md — 4,469 tokens (full)
    ├── room-layout-and-player-routing.md — 2,014 tokens (full)
    ├── ui-widget-tree-structure.md — 3,130 tokens (full)
    ├── ui-wireframes-and-screen-flow.md — 3,206 tokens (full)
    ├── unreal-project-bootstrap-and-blueprint-build-order.md — 2,324 tokens (full)
    ├── vertical-slice-18-blueprints-implementation-spec.md — 2,276 tokens (full)
    ├── vertical-slice-blueprint-variable-panel-checklist.md — 4,283 tokens (full)
    ├── vertical-slice-blueprint-wiring-order.md — 3,523 tokens (full)
    └── vertical-slice-scope.md — 1,708 tokens (full)


---

## Generated Plan

## Chat Send ✅
- **Chat**: `docs-readiness-audit-743DE5` | **Mode**: plan

### Response
## 1. Summary

The `docs/` set is **close, but not yet implementation-ready** for a no-questions Unreal vertical-slice build. The core slice loop, room plan, widget structure, and Blueprint subset are already well documented, but the corpus has several **authoritative-contract conflicts**: naming drift, slice-vs-full overlap, ownership drift between managers/widgets, and unresolved flow differences around anomaly reporting and report/end-state ownership. This should be solved with a **targeted doc harmonization**, not a rewrite: freeze one canonical slice contract, align state/ownership/data docs to it, and mark full-version docs as reference-only for the jam slice.

## 2. Current-state analysis

The current documentation already defines almost the full implementation surface:

- **Scope/roadmap**
  - `docs/vertical-slice-scope.md` defines the jam target: Day 1-3, FREEZE-first, one playable anomaly loop, simplified endings.
  - `docs/day-by-day-content-sheet.md` expands to full-game day content, useful as future reference but not slice-authoritative.

- **Architecture/class inventory**
  - `docs/project-structure-and-blueprint-list.md` defines the full Unreal architecture and complete Blueprint inventory.
  - `docs/unreal-project-bootstrap-and-blueprint-build-order.md` gives both full-project phases and a slice-specific 18-object creation order.
  - `docs/vertical-slice-18-blueprints-implementation-spec.md` is the clearest per-object slice reference.

- **Implementation mechanics**
  - `docs/vertical-slice-blueprint-wiring-order.md` gives the shortest playable chain.
  - `docs/blueprint-pseudonode-flowcharts.md` approximates Blueprint graph logic.
  - `docs/vertical-slice-blueprint-variable-panel-checklist.md` defines concrete Blueprint variables.
  - `docs/blueprint-variables-events-and-data-fields.md` defines enums, structs, dispatcher contracts, and table shapes.

- **Flow/UI/world contracts**
  - `docs/game-state-machine.md`, `docs/interaction-spec.md`, `docs/ui-wireframes-and-screen-flow.md`, and `docs/ui-widget-tree-structure.md` together define state ownership, player actions, screen flow, and widget composition.
  - `docs/room-layout-and-player-routing.md` and `docs/lv-apartment-main-build-steps.md` are strong enough to build `LV_ApartmentMain` now.

- **Narrative/data**
  - `docs/narrative-script-and-system-copy.md` gives prose/script baseline.
  - `docs/day1-day3-datatable-ready-script.md` is the most implementation-friendly text source.

### End-to-end current flow as documented

The intended slice call chain is already visible across the docs:

1. `LV_ApartmentMain` loads with `BP_SignalGameMode`.
2. `BP_SignalGameMode` spawns `BP_SignalPlayerCharacter` and `BP_SignalPlayerController`.
3. `BP_SignalPlayerCharacter` performs interaction trace against `BPI_Interactable`.
4. `BP_ComputerTerminal.Interact` asks `BP_SignalPlayerController` to show `WBP_DesktopRoot` and switch input mode.
5. `WBP_DesktopRoot` hosts `WBP_ChatApp`.
6. `WBP_ChatApp` sends input to `BP_ChatConversationManager`.
7. `BP_ChatConversationManager` mutates `ConversationHistory` and calls `BP_AnomalyManager.AccumulateSideEffect`.
8. `BP_MinigameManager` starts `WBP_MG_DependencyMatch`.
9. `BP_AnomalyManager` triggers `FREEZE`.
10. `WBP_AnomalyChoicePopup` records route choice through `BP_RouteStateManager`.
11. If “Handle Myself,” `BP_SignalGameFlowManager` returns control to 3D.
12. `BP_AirConditionerUnit.Interact` resolves the anomaly.
13. `BP_HiddenDialogueUnlocker` injects a hidden option into `BP_ChatConversationManager`.
14. `WBP_ReportEditor` collects the final selection and the flow triggers `WBP_EndingTitleCard`.

### What is already reusable

These docs are already strong and should mostly be reused as-is:

- `docs/lv-apartment-main-build-steps.md`
- `docs/room-layout-and-player-routing.md`
- `docs/ui-widget-tree-structure.md`
- `docs/ui-wireframes-and-screen-flow.md`
- `docs/vertical-slice-blueprint-variable-panel-checklist.md`

### What is blocking implementation readiness

#### 1. Naming drift breaks asset/type creation
Examples already present in the corpus:

- `DISK_CLEAN` vs `DISKCLEAN`
- `E_RouteType` vs `E_RouteBranch`
- `DT_EndingSequences` vs `DT_EndingSubtitles`
- DataTable columns using `Text` in one doc and `ReplyText` / `DialogueText` / `SentenceText` in another

This is not cosmetic in Unreal; it affects enum assets, struct fields, CSV import, Blueprint pins, and table row structs.

#### 2. Slice-vs-full overlap is unresolved
Some docs enforce a tight jam slice:

- FREEZE only
- Day 1-3 only
- 18 objects only

Others still describe:

- BLACKOUT / DISKCLEAN as live systems
- Day 4-7 as active runtime content
- Managers like `BP_ReportManager`, `BP_AnomalyChoiceDirector`, `BP_SignalHUDManager` as core

That leaves the implementer to decide what to actually instantiate.

#### 3. Runtime ownership drifts across docs
The biggest unresolved ownership questions are:

- Does `BP_ReportManager` exist in the slice, or does `WBP_ReportEditor` drive ending logic directly?
- Does anomaly “report supervisor” return to `MinigameActive` or jump to `ReportPhase`?
- Who owns modal popup lifecycle: `BP_MinigameManager`, `PlayerController`, or a HUD layer manager?
- Who is the authoritative state owner for app switching inside desktop: `WBP_DesktopRoot` or `BP_SignalGameFlowManager`?

The docs imply answers, but they are not frozen consistently.

#### 4. Data-driven scope is not frozen
The corpus references:

- `DT_DayConfigs`
- `DT_AnomalyConfigs`
- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

But only some are import-ready. The implementer still has to decide whether slice gameplay is:
- fully data-driven,
- partially hardcoded,
- or hardcoded first and migrated later.

That is a design decision that should be resolved in docs first.

#### 5. Handoff boundaries are implied, not explicit
The material is enough for a lead to interpret, but not yet enough for independent parallel work without Slack/Discord clarification:
- which widget names are frozen for UI art handoff,
- which DataTables narrative can populate immediately,
- which managers must be hand-placed in the level,
- which assets can safely start before runtime contract freeze.

## 3. Design

### Decision: targeted doc remediation, not a broader rewrite

A broader rewrite is unnecessary because the corpus already contains the right design. The problem is **authority fragmentation**, not missing concepts. The right fix is to **freeze the vertical-slice contract in a small number of canonical docs and downgrade the rest to reference/supporting docs**.

---

### Work item 1 — Freeze the canonical vertical-slice contract

**Goal:** make the jam slice unambiguous before any Blueprint assets are named or created.

**Primary docs to update**
- `docs/vertical-slice-scope.md`
- `docs/game-state-machine.md`
- `docs/interaction-spec.md`

**Decisions to resolve explicitly**
- The slice runtime is **Day 1 onboarding + Day 2 full FREEZE loop + Day 3 simplified ending wrapper**.
- Executed anomaly set is:
  - `None`
  - `FREEZE`
- Enum may still include `BLACKOUT` and `DISKCLEAN` for future expansion, but those variants are **non-executing placeholders** in the slice.
- Canonical names:
  - `E_RouteBranch`
  - `DISKCLEAN`
  - `DT_EndingSubtitles`
- Exact anomaly branch behavior:
  - **Report Supervisor**: resolve anomaly, resume/preserve day progression, then enter `ReportPhase` only after minigame completion.
  - **Handle Myself**: enter `HandleAnomaly3D`, require AC resolution, then return to desktop and expose the hidden dialogue path.
- Computer re-entry rule:
  - while `CurrentPhase == HandleAnomaly3D` and anomaly unresolved, computer interaction is disabled.
- `Esc`/leave-desktop rule:
  - enabled in normal desktop states,
  - disabled during `AnomalyChoice`,
  - disabled while unresolved in `HandleAnomaly3D`.

**Why this is first**
This is the naming and flow freeze. If implementation starts before this, Unreal asset naming and Blueprint graph topology will fork immediately.

---

### Work item 2 — Lock runtime ownership and phase-transition authority

**Goal:** remove ambiguity about who owns state, widgets, and flow transitions.

**Primary docs to update**
- `docs/blueprint-responsibility-map.md`
- `docs/vertical-slice-18-blueprints-implementation-spec.md`
- `docs/vertical-slice-blueprint-wiring-order.md`
- `docs/blueprint-pseudonode-flowcharts.md`

**Canonical ownership to document**
- **Level-owned actors**
  - `BP_SignalGameFlowManager`
  - `BP_RouteStateManager`
  - `BP_AnomalyManager`
  - `BP_ChatConversationManager`
  - `BP_MinigameManager`
  - `BP_HiddenDialogueUnlocker`
  - plus room/day managers already placed in `LV_ApartmentMain`
- **PlayerController-owned widgets**
  - `WBP_DesktopRoot`
  - `WBP_EndingTitleCard`
- **DesktopRoot-owned app children**
  - `WBP_ChatApp`
  - `WBP_ReportEditor`
  - current minigame widget in `AppContentHost`
- **State ownership**
  - `BP_SignalGameFlowManager` is the only owner of `CurrentPhase`
  - `BP_RouteStateManager` owns branch counters and unlock state
  - `BP_AnomalyManager` owns `CurrentAnomalyType` and anomaly-active status
  - `BP_ChatConversationManager` owns `ConversationHistory` and `HiddenOptions`

**Reference-acquisition contract**
For the slice, choose one strategy and document it uniformly:
- manager actors are **placed in the level**
- cross-manager refs used by the slice are **instance-editable and manually assigned in the level**
- widgets receive refs from their creator on spawn
- `GetAllActorsOfClass` remains fallback/debug only, not the normative implementation path

**Behavioral edge rules to add**
- repeated `RequestPhaseChange` to the current phase is a no-op
- `TriggerAnomaly` is ignored if `bAnomalyActive == true`
- `InjectHiddenOption` dedupes by `OptionId`
- `WBP_ReportEditor` cannot submit without a selected sentence
- report UI is unavailable until minigame/anomaly chain is closed

**Why this is second**
Without this, two engineers can both “follow the docs” and still build incompatible flow ownership.

---

### Work item 3 — Freeze the slice data contract and table ownership

**Goal:** make CSV/Struct/DataTable work implementable without guessing field names or deciding what is hardcoded.

**Primary docs to update**
- `docs/blueprint-variables-events-and-data-fields.md`
- `docs/day1-day3-datatable-ready-script.md`
- `docs/narrative-script-and-system-copy.md`

**Decisions to resolve explicitly**
Choose the slice data strategy and state it in docs:

#### Recommended slice strategy
**Data-driven now**
- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

**Deferred/hardcoded for slice unless import rows are added in the same work item**
- `DT_DayConfigs`
- `DT_AnomalyConfigs`

If the team wants those too, add minimum import-ready rows for:
- Day 1, Day 2, Day 3
- FREEZE only

**Row schema freeze**
`docs/blueprint-variables-events-and-data-fields.md` must match the actual CSV columns in `docs/day1-day3-datatable-ready-script.md` exactly. Pick one canonical field naming convention and remove aliases.

Example of the type of mismatch to eliminate:
- `ReplyText` vs `Text`
- `DialogueText` vs `Text`
- `SentenceText` vs `Text`

**Consumer mapping to document**
- `BP_ChatConversationManager` reads:
  - `DT_NormalReplies`
  - `DT_HiddenDialogues`
  - optionally `DT_SupervisorLines`
- anomaly popup/system prompts read:
  - `DT_SystemCopy`
- `WBP_ReportEditor` reads:
  - `DT_ReportSentencePools`
- `WBP_EndingTitleCard` reads:
  - `DT_EndingSubtitles`

**Authority note**
- `day1-day3-datatable-ready-script.md` becomes the **implementation-authoritative import source**
- `narrative-script-and-system-copy.md` becomes **reference prose/source text**, not the schema authority

**Why this is third**
It prevents the common Unreal failure mode where UI/Blueprint work finishes first, then CSV import breaks because structs and columns drifted.

---

### Work item 4 — Mark full-version docs as non-authoritative for the slice and add kickoff gates

**Goal:** reduce planning noise and make parallel work safe.

**Primary docs to update**
- `docs/unreal-project-bootstrap-and-blueprint-build-order.md`
- `docs/project-structure-and-blueprint-list.md`
- `docs/foundation-docs-needed.md`

**Required updates**
- Add a top-of-file note in the full-project docs:
  - “Full-version reference; for the 48h vertical slice, follow `vertical-slice-scope`, `vertical-slice-18-blueprints-implementation-spec`, `vertical-slice-blueprint-wiring-order`, and the frozen data/state contracts.”
- In `unreal-project-bootstrap-and-blueprint-build-order.md`, separate:
  - **safe-to-start immediately**
  - **blocked pending contract freeze**
- Mark `foundation-docs-needed.md` as historical/stale so nobody treats it as an active gap list.

**Parallel work gates to define**
- **Can start immediately after Work Item 1**
  - project bootstrap
  - folder creation
  - enum/struct asset creation using frozen names
  - `LV_ApartmentMain` whitebox
  - widget shell creation using frozen widget-tree names
- **Must wait until Work Items 2-3**
  - state transitions
  - inter-manager wiring
  - DataTable import and consumer hookup
  - anomaly/report branching logic

**Why this is fourth**
It is not a design blocker, but it removes avoidable coordination churn once implementation begins.

## 4. File-by-file impact

| File | What changes | Why | Dependencies |
|---|---|---|---|
| `docs/vertical-slice-scope.md` | Add canonical slice boundary, authoritative naming table, exact in-scope anomaly/day/object list, exact branch outcomes | This becomes the top-level slice contract | First |
| `docs/game-state-machine.md` | Reduce slice runtime to authoritative states/transitions; clarify report vs handle paths; add no-op/blocked input rules | Needed so phase ownership and input rules are implementable | After scope freeze |
| `docs/interaction-spec.md` | Align anomaly naming, computer lockout during unresolved anomaly, leave-desktop restrictions | Prevents contradictory 3D/2D interaction behavior | After scope freeze |
| `docs/blueprint-responsibility-map.md` | Freeze slice ownership boundaries; mark full-version managers as out-of-scope for jam path | Prevents manager/widget/world role drift | After scope freeze |
| `docs/vertical-slice-18-blueprints-implementation-spec.md` | Remove or explicitly exclude `BP_ReportManager`-style ownership assumptions; align responsibilities with slice authority | This is the per-object build spec | After responsibility freeze |
| `docs/vertical-slice-blueprint-wiring-order.md` | Align step order to canonical branch behavior and reference acquisition strategy | This is the build-time call-chain doc | After responsibility freeze |
| `docs/blueprint-pseudonode-flowcharts.md` | Update pseudonodes to match authoritative phase/ownership decisions | Prevents graph-level contradictions | After wiring decisions |
| `docs/blueprint-variables-events-and-data-fields.md` | Freeze enum names, struct field names, DataTable row schemas, consumer mapping | This is the data/type authority | Parallel with DataTable doc |
| `docs/day1-day3-datatable-ready-script.md` | Align CSV columns and add/omit tables explicitly per slice strategy | Makes import implementation deterministic | Parallel with data contract doc |
| `docs/narrative-script-and-system-copy.md` | Add note that this is narrative reference, not schema authority; align any conflicting copy IDs/names | Avoids dual-source confusion | After DataTable authority is chosen |
| `docs/unreal-project-bootstrap-and-blueprint-build-order.md` | Split full-project vs slice-authoritative sections; add “safe now / blocked until freeze” gates | Makes kickoff executable | After scope + ownership freeze |
| `docs/project-structure-and-blueprint-list.md` | Add top-level note that this is full-version architecture reference, not jam slice authority | Avoids accidental scope creep | Anytime |
| `docs/foundation-docs-needed.md` | Mark as historical/stale | Avoids false “missing docs” work | Anytime |

## 5. Risks and migration

The main risk is **starting implementation under conflicting names/contracts** and then paying Unreal rename costs:

- renamed enums/structs break Blueprint pins
- renamed DataTable assets/columns break CSV import
- differing widget ownership produces duplicate UI roots or bad input-mode control
- differing anomaly branch behavior causes incompatible graphs between UI and flow managers

**Practical mitigation:** do not create final enum assets, DataTable row structs, or cross-manager Blueprint interfaces until Work Items 1-3 are done.  
Low-risk work that can begin now: whitebox room, folder structure, placeholder widget shells, placeholder meshes/anchors.

## 6. Implementation order

1. **Freeze the slice contract** in `vertical-slice-scope.md`, `game-state-machine.md`, and `interaction-spec.md`.  
   - Must land together.

2. **Freeze runtime ownership and flow authority** in `blueprint-responsibility-map.md`, `vertical-slice-18-blueprints-implementation-spec.md`, `vertical-slice-blueprint-wiring-order.md`, and `blueprint-pseudonode-flowcharts.md`.  
   - Must land together.

3. **Freeze data/type/schema contracts** in `blueprint-variables-events-and-data-fields.md`, `day1-day3-datatable-ready-script.md`, and `narrative-script-and-system-copy.md`.  
   - Must land together before DataTable assets are created.

4. **Add kickoff gating and deconflict full-version docs** in `unreal-project-bootstrap-and-blueprint-build-order.md`, `project-structure-and-blueprint-list.md`, and `foundation-docs-needed.md`.  
   - Can happen immediately after steps 1-3.

5. **Only then start the actual slice build**, in this order:
   1. project/bootstrap + enums/structs
   2. `LV_ApartmentMain` whitebox + placed managers
   3. player/controller + `BPI_Interactable`
   4. computer -> desktop root
   5. chat system
   6. minigame
   7. FREEZE anomaly + choice popup
   8. air conditioner resolution
   9. hidden dialogue injection
   10. report + ending

**Verdict:** do **not** treat the current docs as fully implementation-ready yet; do the 4 focused doc remediations above first. After that, the corpus is strong enough for a clean project kickoff.


> 💡 Continue this plan conversation with ask_oracle(chat_id: "docs-readiness-audit-743DE5", new_chat: false)