# Work Item 2 Pending Items (Require Unreal Editor)

This file records Work Item 2 items that are intentionally **not** created in-repo as text files.

UE version target: **UE5.7**
Project type target: **Blueprint-only**

## Pending (must be done in Unreal Editor)

- Create and resave Unreal-generated project metadata as needed when opening `SignalProject.uproject` in UE5.7.
- Create all enum `.uasset` assets:
  - `E_GamePhase`, `E_AnomalyType`, `E_ColleagueId`, `E_SkillUnlockState`, `E_RouteBranch`, `E_MinigameType`
- Create all struct `.uasset` assets:
  - `ST_ChatMessageRecord`, `ST_HiddenOptionRecord`, `ST_SupervisorLineRow`, `ST_NormalReplyRow`, `ST_HiddenDialogueRow`, `ST_SystemCopyRow`, `ST_ReportSentenceRow`, `ST_EndingSubtitleRow`
- Create DataTable `.uasset` assets and import CSVs from `bootstrap/import-data/` in frozen order.
- Create `LV_ApartmentMain.umap` and all Blueprint/Widget assets for later Work Items.

## Explicitly not created here

- No fake `.uasset` files
- No fake `.umap` files
- No runtime Blueprint wiring in text guesses

Authority remains unchanged in `docs/`.
