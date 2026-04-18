# Editor Tools (Optional, Unreal-Python)

These helpers are safe to author in-repo now and run later inside Unreal Editor.

They do **not** create speculative widget trees or map layouts.

## Scripts

- `unreal_python/import_slice_datatables.py`
  - Imports/updates the six slice DataTables from `bootstrap/import-data/*.csv`
  - Uses native row structs (`FST_*`) from `SignalSliceTypes.h`
  - Deterministic fixed import order

- `unreal_python/validate_slice_editor_setup.py`
  - Checks key native classes/structs load in Editor
  - Checks expected CSV files exist
  - Reports DataTable asset presence and row-struct alignment if assets already exist

## How to run

Inside Unreal Editor Python console (or Python script runner), execute script by path.

Example:

```python
exec(open(r"/absolute/path/to/signalproject/bootstrap/editor-tools/unreal_python/validate_slice_editor_setup.py").read())
```

Then:

```python
exec(open(r"/absolute/path/to/signalproject/bootstrap/editor-tools/unreal_python/import_slice_datatables.py").read())
```

## Notes

- Scripts assume project root layout matches this repository.
- Scripts are intentionally strict and will emit clear errors for missing structs/CSVs.
- Asset internals (widget tree design, level composition) remain manual by design.
