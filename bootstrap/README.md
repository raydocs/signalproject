# Unreal Vertical Slice Bootstrap Handoff Pack

This folder is the bootstrap handoff pack for the Signal UE5.7 vertical slice after native Work Items 1-2 are in place and compiling.

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

## Current repo reality (April 18, 2026)

- Native hybrid module exists (`Source/SignalProject/*`).
- Runtime authority classes + widget bases exist in C++ and compile in UE5.7.
- Editor-only assets are still pending by design (`.uasset`, `.umap`, widget trees, actor placement).

## What this bootstrap folder now contains

- `asset-manifest.csv`  
  Hybrid inventory for native files, DataTable imports, and Editor-later assets.
- `import-data/*.csv`
  Frozen CSV sources for six DataTables.
- `editor-bootstrap-checklist.md`  
  Updated checklist for first Editor bring-up after native compile.
- `first-playable-smoke-test.md`  
  Behavioral validation sheet.
- `work-item-2-editor-pending.md`  
  Editor-only pending scope after native in-repo work.
- `editor-landing/native-class-to-derived-asset-matrix.md`
  Native class -> derived asset matrix for deterministic Editor creation.
- `editor-landing/level-manual-ref-binding-sheet.md`
  Explicit per-actor manual ref binding sheet for `LV_ApartmentMain`.
- `editor-landing/first-editor-session-runbook.md`
  Step-by-step first Editor session runbook.
- `editor-tools/README.md` and `editor-tools/unreal_python/*.py`
  Optional Unreal-Python helpers for DataTable import and setup sanity checks.

## Repo-only vs Unreal Editor split

### Repo-only (already done)

- Native targets, module, and build rules under `Source/`
- Native runtime authority classes and widget bases
- Frozen native schema in `Source/SignalProject/Public/SignalSliceTypes.h`
- Hybrid project/config wiring (`.uproject`, `Config/*`)

### Still requires Unreal Editor

- DataTable `.uasset` creation/import from `bootstrap/import-data/`
- UMG widget blueprints derived from native widget bases
- Optional BP children of native classes for designer defaults
- `LV_ApartmentMain` creation and actor placement
- Manual cross-actor ref assignment in level instances
- Map startup path update after level exists

## First Editor session entry point

Start with:

1. `bootstrap/editor-landing/first-editor-session-runbook.md`
2. `bootstrap/editor-landing/native-class-to-derived-asset-matrix.md`
3. `bootstrap/editor-landing/level-manual-ref-binding-sheet.md`

Optional automation:

- `bootstrap/editor-tools/unreal_python/import_slice_datatables.py`
- `bootstrap/editor-tools/unreal_python/validate_slice_editor_setup.py`

## Notes

- No fake `.uasset` or `.umap` files are authored in-repo.
- Do not duplicate native schema as separate BP enum/struct assets.
- Keep phase authority intact: only `ASignalGameFlowManager` mutates `CurrentPhase`.
