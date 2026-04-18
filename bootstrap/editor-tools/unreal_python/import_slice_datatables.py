"""
Deterministic DataTable importer for SignalProject slice.
Run inside Unreal Editor Python.
"""

from __future__ import annotations

import csv
import os
import tempfile

import unreal

DESTINATION_PATH = "/Game/Signal/Data/DataTables"
IMPORT_INTO_EXISTING = True
STOP_ON_ERROR = True

IMPORT_PLAN = [
    ("DT_SystemCopy", "DT_SystemCopy.csv", "/Script/SignalProject.ST_SystemCopyRow"),
    ("DT_SupervisorLines", "DT_SupervisorLines.csv", "/Script/SignalProject.ST_SupervisorLineRow"),
    ("DT_NormalReplies", "DT_NormalReplies.csv", "/Script/SignalProject.ST_NormalReplyRow"),
    ("DT_HiddenDialogues", "DT_HiddenDialogues.csv", "/Script/SignalProject.ST_HiddenDialogueRow"),
    ("DT_ReportSentencePools", "DT_ReportSentencePools.csv", "/Script/SignalProject.ST_ReportSentenceRow"),
    ("DT_EndingSubtitles", "DT_EndingSubtitles.csv", "/Script/SignalProject.ST_EndingSubtitleRow"),
]


def _project_root() -> str:
    return os.path.abspath(unreal.Paths.project_dir())


def _csv_dir() -> str:
    return os.path.join(_project_root(), "bootstrap", "import-data")


def _load_script_struct(struct_path: str) -> unreal.ScriptStruct:
    candidates = [
        struct_path,
        f"ScriptStruct'{struct_path}'",
    ]

    for candidate in candidates:
        struct_obj = unreal.load_object(None, candidate)
        if struct_obj:
            return struct_obj

    raise RuntimeError(f"Missing row struct: {struct_path}")


def _ensure_data_table(asset_name: str, row_struct: unreal.ScriptStruct) -> unreal.DataTable:
    asset_path = f"{DESTINATION_PATH}/{asset_name}.{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        existing = unreal.load_asset(asset_path)
        if not isinstance(existing, unreal.DataTable):
            raise RuntimeError(f"Existing asset is not DataTable: {asset_path}")
        return existing

    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", row_struct)

    created = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        DESTINATION_PATH,
        unreal.DataTable,
        factory,
    )

    if not created:
        raise RuntimeError(f"Failed to create DataTable: {asset_name}")

    return created


def _build_row_key_csv_for_ending_subtitles(source_csv_path: str) -> str:
    with open(source_csv_path, "r", encoding="utf-8", newline="") as src_fp:
        reader = csv.DictReader(src_fp)
        source_fields = list(reader.fieldnames or [])

        if source_fields != ["EndingId", "SequenceOrder", "Text"]:
            raise RuntimeError(
                f"Unexpected DT_EndingSubtitles header: {source_fields}. "
                "Expected ['EndingId', 'SequenceOrder', 'Text']."
            )

        tmp_fp = tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            newline="",
            prefix="dt_endingsubtitles_with_rowkeys_",
            suffix=".csv",
            delete=False,
        )

        transformed_path = tmp_fp.name
        with tmp_fp:
            writer = csv.DictWriter(tmp_fp, fieldnames=["Name", *source_fields])
            writer.writeheader()

            for row in reader:
                ending_id = (row.get("EndingId") or "").strip()
                sequence_raw = (row.get("SequenceOrder") or "").strip()

                try:
                    sequence_number = int(sequence_raw)
                    sequence_suffix = f"{sequence_number:02d}"
                except ValueError:
                    sequence_suffix = sequence_raw.zfill(2)

                row_name = f"{ending_id}_{sequence_suffix}"
                writer.writerow(
                    {
                        "Name": row_name,
                        "EndingId": row.get("EndingId", ""),
                        "SequenceOrder": row.get("SequenceOrder", ""),
                        "Text": row.get("Text", ""),
                    }
                )

    unreal.log(
        "Generated derived CSV for DT_EndingSubtitles with deterministic row keys "
        f"at: {transformed_path}"
    )
    return transformed_path


def _import_one(asset_name: str, csv_name: str, struct_path: str) -> None:
    source_csv_path = os.path.join(_csv_dir(), csv_name)
    if not os.path.isfile(source_csv_path):
        raise RuntimeError(f"CSV missing: {source_csv_path}")

    row_struct = _load_script_struct(struct_path)
    data_table = _ensure_data_table(asset_name, row_struct)

    if unreal.EditorAssetLibrary.does_asset_exist(f"{DESTINATION_PATH}/{asset_name}.{asset_name}") and not IMPORT_INTO_EXISTING:
        unreal.log_warning(f"Skip existing DataTable (IMPORT_INTO_EXISTING=False): {asset_name}")
        return

    import_csv_path = source_csv_path
    should_cleanup_import_csv = False

    if asset_name == "DT_EndingSubtitles":
        import_csv_path = _build_row_key_csv_for_ending_subtitles(source_csv_path)
        should_cleanup_import_csv = True

    try:
        ok = unreal.DataTableFunctionLibrary.fill_data_table_from_csv_file(data_table, import_csv_path)
        if not ok:
            raise RuntimeError(f"CSV import failed for {asset_name} from {import_csv_path}")
    finally:
        if should_cleanup_import_csv and os.path.isfile(import_csv_path):
            os.remove(import_csv_path)

    unreal.EditorAssetLibrary.save_loaded_asset(data_table)
    unreal.log(f"Imported DataTable: {asset_name} <- {csv_name} ({struct_path})")


def run() -> None:
    unreal.log("=== SignalProject DataTable import start ===")
    unreal.EditorAssetLibrary.make_directory(DESTINATION_PATH)

    errors = []
    for asset_name, csv_name, struct_path in IMPORT_PLAN:
        try:
            _import_one(asset_name, csv_name, struct_path)
        except Exception as exc:
            msg = f"[ERROR] {asset_name}: {exc}"
            errors.append(msg)
            unreal.log_error(msg)
            if STOP_ON_ERROR:
                break

    if errors:
        raise RuntimeError("DataTable import completed with errors. Check Output Log.")

    unreal.log("=== SignalProject DataTable import complete ===")


run()
