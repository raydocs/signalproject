# Work Item 3 Editor Pending Items

This file tracks Editor-only follow-up after the code-side item 3 implementation.

UE target: **UE5.7**
Project target: **Hybrid native runtime + Editor-authored assets**

## Pending (must be done in Unreal Editor)

- Update `WBP_ReportEditor` to present two distinct areas:
  - normal/base report clause selection
  - hidden/final injection area for collected colleague lines
- Wire `WBP_ReportEditor` to the additive native events and methods:
  - `BP_OnReportOptionsBuilt`
  - `BP_OnInjectedClausesBuilt`
  - `HandleSentenceSelected`
  - `HandleInjectionSentenceToggled`
  - `HandleSubmitClicked`
- If `DT_ReportSentencePools` is used for authored report clauses, verify the base/day rows still match the current slice and do not conflict with the native fallback clause.
- Optionally add authored final-injection rows to `DT_ReportSentencePools` / CSV if you want data-driven text instead of the native fallback token clauses.
- Update `WBP_EndingTitleCard` to consume the richer runtime ending state from `BP_OnEndingResultResolved` if you want custom visuals beyond the native fallback text layout.
- If `DT_EndingSubtitles` does not yet contain rows for the new ids, add authored rows for:
  - `RoutineEnding`
  - `PartialInjectionEnding`
  - `CascadeEnding`
- Verify greying/offline presentation for awakened colleagues in the chat UI still matches the route/token state.
- Run the end-to-end slice path in PIE:
  - trigger anomaly
  - self-handle
  - consume follow-up probe
  - collect injection line
  - open OA report
  - submit with/without injections
  - verify ending card output

## Explicitly not created in repo

- No fake `.uasset` files
- No fake `.umap` files
- No speculative editor automation for widget trees or authored ending sequences
