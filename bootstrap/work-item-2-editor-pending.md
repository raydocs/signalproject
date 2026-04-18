# Work Item 2 Editor Pending Items

This file tracks Editor-only work that remains intentionally out-of-repo.

UE target: **UE5.7**
Project target: **Hybrid native runtime + Editor-authored assets**

## Pending (must be done in Unreal Editor)

- Create/import DataTable assets from `bootstrap/import-data/` using native row structs (`FST_*`).
- Create UMG widget blueprints derived from native widget bases:
  - `UDesktopRootWidget`
  - `UChatAppWidget`
  - `UDependencyMatchWidget`
  - `UAnomalyChoicePopupWidget`
  - `UReportEditorWidget`
  - `UEndingTitleCardWidget`
- Create optional BP children of native actor/controller classes where designer defaults are needed.
- Create `LV_ApartmentMain.umap`.
- Place required actor instances and set manual cross-actor refs (`EditInstanceOnly`) per binding sheet.
- Assign widget class properties and DataTable references in derived assets/instances.
- Run `bootstrap/first-playable-smoke-test.md` and resolve blockers.

## Explicitly not created in repo

- No fake `.uasset` files
- No fake `.umap` files
- No duplicated BP enum/struct assets for frozen native schema
- No speculative auto-generation scripts for widget trees or map placement

Authority remains in `docs/`.
