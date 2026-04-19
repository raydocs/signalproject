# Work Item 2 Editor Pending Items

This file tracks Editor-only work that remains intentionally out-of-repo.

UE target: **UE5.7**
Project target: **Hybrid native runtime + Editor-authored assets**

## Pending (must be done in Unreal Editor)

- Create/import DataTable assets from `bootstrap/import-data/` using native row structs (`FST_*`).
- Create UMG widget blueprints derived from native widget bases:
  - `UDesktopRootWidget`
  - `UChatAppWidget`
  - `UTaskListWidget`
  - `UDependencyMatchWidget`
  - `UAnomalyChoicePopupWidget`
  - `UReportEditorWidget`
  - `UEndingTitleCardWidget`
- Create optional BP children of native actor/controller classes where designer defaults are needed.
- Create `LV_ApartmentMain.umap`.
- Place required actor instances and set manual cross-actor refs (`EditInstanceOnly`) per binding sheet.
- Assign widget class properties and DataTable references in derived assets/instances.
- Update `WBP_DesktopRoot` to expose and wire a real panel-type app host/container plus task/window controls (`AppContentHost` should be an `Overlay`/`CanvasPanel`/other `UPanelWidget`, plus `TasksAppButton`, `StressTestAppButton`, optional `CloseAppButton`) and assign `TaskListWidgetClass`.
- Add or update the desktop shell art/button wiring so the pressure-test app is launchable from the desktop without relying on the native debug shell.
- Update `WBP_MG_DependencyMatch` (or derived pressure-test app widget) to show the new stress-test view state cleanly:
  - single draw button
  - ten draw button
  - draw output text
  - status text / active issue hint
  - optional paused / awaiting-report presentation
  - button wiring to `HandleSingleDrawClicked` / `HandleTenDrawClicked` or equivalent native call-through
  - visual refresh from `BP_OnStressStateChanged`
- Ensure `WBP_ChatApp` still renders hidden/synthetic option buttons from `BP_OnConversationRefreshed`, including anomaly-report and self-handle follow-up options.
- Create/configure placed interactable instances or BP children using the generalized `AAirConditionerUnit` native class for all three anomaly cases:
  - `HandledAnomalyType = BLACKOUT`, light-switch style interaction text/visuals
  - `HandledAnomalyType = DISKCLEAN`, computer-case / heavy-load interaction text/visuals
  - `HandledAnomalyType = FREEZE`, AC interaction text/visuals
- Ensure the placed anomaly resolver actors start available on each day/run as intended by the level setup.
- If any old `BP_OnAppWidgetRequested` logic is still manually re-parenting app widgets, remove/bypass it once native hosting is in use.
- Run `bootstrap/first-playable-smoke-test.md` and resolve blockers.

## Explicitly not created in repo

- No fake `.uasset` files
- No fake `.umap` files
- No duplicated BP enum/struct assets for frozen native schema
- No speculative auto-generation scripts for widget trees or map placement

Authority remains in `docs/`.
