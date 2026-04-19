"""
SignalProject editor setup sanity checker.
Run inside Unreal Editor Python.
"""

from __future__ import annotations

import os
import unreal

REQUIRED_CLASSES = [
    "/Script/SignalProject.SignalGameMode",
    "/Script/SignalProject.SignalPlayerController",
    "/Script/SignalProject.SignalPlayerCharacter",
    "/Script/SignalProject.SignalGameFlowManager",
    "/Script/SignalProject.RouteStateManager",
    "/Script/SignalProject.AnomalyManager",
    "/Script/SignalProject.ChatConversationManager",
    "/Script/SignalProject.MinigameManager",
    "/Script/SignalProject.HiddenDialogueUnlocker",
    "/Script/SignalProject.ComputerTerminal",
    "/Script/SignalProject.AirConditionerUnit",
    "/Script/SignalProject.DesktopRootWidget",
    "/Script/SignalProject.ChatAppWidget",
    "/Script/SignalProject.TaskListWidget",
    "/Script/SignalProject.DependencyMatchWidget",
    "/Script/SignalProject.AnomalyChoicePopupWidget",
    "/Script/SignalProject.ReportEditorWidget",
    "/Script/SignalProject.EndingTitleCardWidget",
]

REQUIRED_STRUCTS = [
    "/Script/SignalProject.ST_SystemCopyRow",
    "/Script/SignalProject.ST_SupervisorLineRow",
    "/Script/SignalProject.ST_NormalReplyRow",
    "/Script/SignalProject.ST_HiddenDialogueRow",
    "/Script/SignalProject.ST_ReportSentenceRow",
    "/Script/SignalProject.ST_EndingSubtitleRow",
]

CSV_FILES = [
    "DT_SystemCopy.csv",
    "DT_SupervisorLines.csv",
    "DT_NormalReplies.csv",
    "DT_HiddenDialogues.csv",
    "DT_ReportSentencePools.csv",
    "DT_EndingSubtitles.csv",
]

EXPECTED_DATA_TABLES = [
    ("/Game/Signal/Data/DataTables/DT_SystemCopy", "/Script/SignalProject.ST_SystemCopyRow"),
    ("/Game/Signal/Data/DataTables/DT_SupervisorLines", "/Script/SignalProject.ST_SupervisorLineRow"),
    ("/Game/Signal/Data/DataTables/DT_NormalReplies", "/Script/SignalProject.ST_NormalReplyRow"),
    ("/Game/Signal/Data/DataTables/DT_HiddenDialogues", "/Script/SignalProject.ST_HiddenDialogueRow"),
    ("/Game/Signal/Data/DataTables/DT_ReportSentencePools", "/Script/SignalProject.ST_ReportSentenceRow"),
    ("/Game/Signal/Data/DataTables/DT_EndingSubtitles", "/Script/SignalProject.ST_EndingSubtitleRow"),
]


def _project_root() -> str:
    return os.path.abspath(unreal.Paths.project_dir())


def _csv_dir() -> str:
    return os.path.join(_project_root(), "bootstrap", "import-data")


def _check_classes() -> list[str]:
    missing = []
    for class_path in REQUIRED_CLASSES:
        cls = unreal.load_class(None, class_path)
        if not cls:
            missing.append(class_path)
    return missing


def _load_struct(struct_path: str):
    candidates = [
        struct_path,
        f"ScriptStruct'{struct_path}'",
    ]
    for candidate in candidates:
        struct_obj = unreal.load_object(None, candidate)
        if struct_obj:
            return struct_obj
    return None


def _check_structs() -> list[str]:
    missing = []
    for struct_path in REQUIRED_STRUCTS:
        struct_obj = _load_struct(struct_path)
        if not struct_obj:
            missing.append(struct_path)
    return missing


def _check_csvs() -> list[str]:
    missing = []
    base = _csv_dir()
    for filename in CSV_FILES:
        full = os.path.join(base, filename)
        if not os.path.isfile(full):
            missing.append(full)
    return missing


def _check_data_tables() -> list[str]:
    issues = []
    for asset_path, expected_struct in EXPECTED_DATA_TABLES:
        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            issues.append(f"Missing DataTable asset: {asset_path}")
            continue

        data_table = unreal.load_asset(asset_path)
        if not isinstance(data_table, unreal.DataTable):
            issues.append(f"Asset is not DataTable: {asset_path}")
            continue

        row_struct = data_table.get_editor_property("row_struct")
        struct_path = row_struct.get_path_name() if row_struct else "<None>"
        if struct_path != expected_struct:
            issues.append(
                f"Row struct mismatch for {asset_path}: expected {expected_struct}, got {struct_path}"
            )
    return issues


def run() -> None:
    unreal.log("=== SignalProject editor setup validation start ===")

    missing_classes = _check_classes()
    missing_structs = _check_structs()
    missing_csvs = _check_csvs()
    data_table_issues = _check_data_tables()

    if missing_classes:
        for item in missing_classes:
            unreal.log_error(f"Missing native class: {item}")
    else:
        unreal.log("All required native classes load successfully.")

    if missing_structs:
        for item in missing_structs:
            unreal.log_error(f"Missing native struct: {item}")
    else:
        unreal.log("All required native row structs load successfully.")

    if missing_csvs:
        for item in missing_csvs:
            unreal.log_error(f"Missing CSV: {item}")
    else:
        unreal.log("All expected bootstrap CSV files found.")

    if data_table_issues:
        for item in data_table_issues:
            unreal.log_warning(item)
    else:
        unreal.log("All expected DataTable assets exist and match row structs.")

    blocking = bool(missing_classes or missing_structs or missing_csvs)
    if blocking:
        raise RuntimeError("Blocking setup issues found. Check Output Log.")

    unreal.log("=== SignalProject editor setup validation complete ===")


run()
