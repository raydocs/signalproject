# Unreal Vertical Slice Bootstrap Handoff Pack

This folder is the **Work Item 1 in-repo bootstrap package** for the Signal Unreal vertical slice.

It is intentionally derived from authority docs and does **not** replace them.

## Authority-first rule

If anything here appears to conflict with authority docs, follow authority docs.

Primary authority for this slice:

- `docs/vertical-slice-scope.md`
- `docs/game-state-machine.md`
- `docs/interaction-spec.md`
- `docs/vertical-slice-18-blueprints-implementation-spec.md`
- `docs/vertical-slice-blueprint-wiring-order.md`
- `docs/blueprint-variables-events-and-data-fields.md`
- `docs/day1-day3-datatable-ready-script.md`

## What this bootstrap folder contains

- `asset-manifest.csv`  
  Required Unreal bootstrap asset inventory (project/config files, enums, structs, datatables, level, 18 slice assets).
- `import-data/DT_SystemCopy.csv`
- `import-data/DT_SupervisorLines.csv`
- `import-data/DT_NormalReplies.csv`
- `import-data/DT_HiddenDialogues.csv`
- `import-data/DT_ReportSentencePools.csv`
- `import-data/DT_EndingSubtitles.csv`  
  Import CSVs extracted from frozen `docs/day1-day3-datatable-ready-script.md`.
- `editor-bootstrap-checklist.md`  
  Ordered Unreal Editor bootstrap checklist for Work Items 2-4.
- `first-playable-smoke-test.md`  
  First-playable validation sheet for the Day2 FREEZE slice loop and Day3 ending gate.

## Repo-only vs Unreal Editor split

### Repo-only (completed in Work Item 1)

- Build this bootstrap handoff folder
- Freeze import CSVs from authority doc
- Provide manifest and execution checklists

### Requires Unreal Editor (Work Items 2-4)

- Create `.uproject`, `Config/`, `Content/` assets
- Create enum/struct/datatable `.uasset` assets and import CSVs
- Create `LV_ApartmentMain` and 18 slice object shells
- Place level actors, bind manual refs, wire runtime flow
- Run first-playable smoke test in-editor

## Execution order after this handoff

1. Use `asset-manifest.csv` to create Unreal project/config/data/slice assets in order.
2. Import DataTables from `import-data/` in frozen order:
   1. `DT_SystemCopy`
   2. `DT_SupervisorLines`
   3. `DT_NormalReplies`
   4. `DT_HiddenDialogues`
   5. `DT_ReportSentencePools`
   6. `DT_EndingSubtitles`
3. Follow `editor-bootstrap-checklist.md` for project setup and slice shell creation.
4. Validate runtime behavior using `first-playable-smoke-test.md`.

## Notes

- This pack does not add or modify slice behavior.
- This pack does not edit authority docs.
