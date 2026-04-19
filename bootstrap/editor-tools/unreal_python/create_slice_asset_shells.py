"""
Deterministic asset-shell creator for SignalProject slice.
Creates BP/UMG derived asset shells and an empty gameplay level.
Run inside Unreal Editor Python commandlet mode.
"""

from __future__ import annotations

import unreal

CREATE_LEVEL = True
STOP_ON_ERROR = True

BP_SPECS = [
    ("/Script/SignalProject.SignalGameMode", "BP_SignalGameMode", "/Game/Signal/Blueprints/Framework"),
    ("/Script/SignalProject.SignalPlayerController", "BP_SignalPlayerController", "/Game/Signal/Blueprints/Player"),
    ("/Script/SignalProject.SignalPlayerCharacter", "BP_SignalPlayerCharacter", "/Game/Signal/Blueprints/Player"),
    ("/Script/SignalProject.SignalGameFlowManager", "BP_SignalGameFlowManager", "/Game/Signal/Blueprints/Core"),
    ("/Script/SignalProject.RouteStateManager", "BP_RouteStateManager", "/Game/Signal/Blueprints/Core"),
    ("/Script/SignalProject.AnomalyManager", "BP_AnomalyManager", "/Game/Signal/Blueprints/Anomalies"),
    ("/Script/SignalProject.ChatConversationManager", "BP_ChatConversationManager", "/Game/Signal/Blueprints/Dialogue"),
    ("/Script/SignalProject.MinigameManager", "BP_MinigameManager", "/Game/Signal/Blueprints/Minigames"),
    ("/Script/SignalProject.HiddenDialogueUnlocker", "BP_HiddenDialogueUnlocker", "/Game/Signal/Blueprints/Dialogue"),
    ("/Script/SignalProject.ComputerTerminal", "BP_ComputerTerminal", "/Game/Signal/Blueprints/Interactables"),
    ("/Script/SignalProject.AirConditionerUnit", "BP_AirConditionerUnit", "/Game/Signal/Blueprints/Interactables"),
]

WIDGET_SPECS = [
    ("/Script/SignalProject.DesktopRootWidget", "WBP_DesktopRoot", "/Game/Signal/Blueprints/UI"),
    ("/Script/SignalProject.ChatAppWidget", "WBP_ChatApp", "/Game/Signal/Blueprints/UI"),
    ("/Script/SignalProject.TaskListWidget", "WBP_TaskList", "/Game/Signal/Blueprints/UI"),
    ("/Script/SignalProject.DependencyMatchWidget", "WBP_MG_DependencyMatch", "/Game/Signal/Blueprints/Minigames"),
    ("/Script/SignalProject.AnomalyChoicePopupWidget", "WBP_AnomalyChoicePopup", "/Game/Signal/Blueprints/UI"),
    ("/Script/SignalProject.ReportEditorWidget", "WBP_ReportEditor", "/Game/Signal/Blueprints/Report"),
    ("/Script/SignalProject.EndingTitleCardWidget", "WBP_EndingTitleCard", "/Game/Signal/Blueprints/Endings"),
]

LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"


def _asset_obj_path(asset_dir: str, asset_name: str) -> str:
    return f"{asset_dir}/{asset_name}.{asset_name}"


def _ensure_dir(asset_dir: str) -> None:
    unreal.EditorAssetLibrary.make_directory(asset_dir)


def _load_native_class(class_path: str):
    loaded = unreal.load_class(None, class_path)
    if not loaded:
        raise RuntimeError(f"Missing native class: {class_path}")
    return loaded


def _create_blueprint(parent_class_path: str, asset_name: str, asset_dir: str) -> None:
    asset_obj_path = _asset_obj_path(asset_dir, asset_name)
    if unreal.EditorAssetLibrary.does_asset_exist(asset_obj_path):
        unreal.log(f"Exists, skip BP create: {asset_obj_path}")
        return

    parent_class = _load_native_class(parent_class_path)
    _ensure_dir(asset_dir)

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    created = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        asset_dir,
        unreal.Blueprint,
        factory,
    )

    if not created:
        raise RuntimeError(f"Failed to create BP asset: {asset_obj_path}")

    unreal.EditorAssetLibrary.save_loaded_asset(created)
    unreal.log(f"Created BP asset: {asset_obj_path} (parent={parent_class_path})")


def _create_widget_blueprint(parent_class_path: str, asset_name: str, asset_dir: str) -> None:
    asset_obj_path = _asset_obj_path(asset_dir, asset_name)
    if unreal.EditorAssetLibrary.does_asset_exist(asset_obj_path):
        unreal.log(f"Exists, skip UMG create: {asset_obj_path}")
        return

    parent_class = _load_native_class(parent_class_path)
    _ensure_dir(asset_dir)

    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    created = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        asset_dir,
        unreal.WidgetBlueprint,
        factory,
    )

    if not created:
        raise RuntimeError(f"Failed to create UMG asset: {asset_obj_path}")

    unreal.EditorAssetLibrary.save_loaded_asset(created)
    unreal.log(f"Created UMG asset: {asset_obj_path} (parent={parent_class_path})")


def _create_level(level_path: str) -> None:
    level_obj_path = f"{level_path}.{level_path.rsplit('/', 1)[-1]}"
    if unreal.EditorAssetLibrary.does_asset_exist(level_obj_path):
        unreal.log(f"Exists, skip level create: {level_path}")
        return

    level_dir = level_path.rsplit("/", 1)[0]
    _ensure_dir(level_dir)

    created = unreal.EditorLevelLibrary.new_level(level_path)
    if not created:
        raise RuntimeError(
            "Failed to create level with EditorLevelLibrary.new_level. "
            f"Requested: {level_path}"
        )

    unreal.log(f"Created empty level: {level_path}")


def _verify_assets_exist() -> None:
    missing = []

    for _, asset_name, asset_dir in BP_SPECS:
        obj_path = _asset_obj_path(asset_dir, asset_name)
        if not unreal.EditorAssetLibrary.does_asset_exist(obj_path):
            missing.append(obj_path)

    for _, asset_name, asset_dir in WIDGET_SPECS:
        obj_path = _asset_obj_path(asset_dir, asset_name)
        if not unreal.EditorAssetLibrary.does_asset_exist(obj_path):
            missing.append(obj_path)

    if CREATE_LEVEL:
        level_obj_path = f"{LEVEL_PATH}.{LEVEL_PATH.rsplit('/', 1)[-1]}"
        if not unreal.EditorAssetLibrary.does_asset_exist(level_obj_path):
            missing.append(level_obj_path)

    if missing:
        raise RuntimeError(
            "Post-create verification failed; missing assets: " + ", ".join(missing)
        )

    unreal.log("All requested shell assets and level exist.")


def run() -> None:
    unreal.log("=== SignalProject asset-shell creation start ===")
    errors = []

    for parent_class_path, asset_name, asset_dir in BP_SPECS:
        try:
            _create_blueprint(parent_class_path, asset_name, asset_dir)
        except Exception as exc:
            msg = f"[ERROR] BP {asset_name}: {exc}"
            errors.append(msg)
            unreal.log_error(msg)
            if STOP_ON_ERROR:
                break

    if not errors:
        for parent_class_path, asset_name, asset_dir in WIDGET_SPECS:
            try:
                _create_widget_blueprint(parent_class_path, asset_name, asset_dir)
            except Exception as exc:
                msg = f"[ERROR] UMG {asset_name}: {exc}"
                errors.append(msg)
                unreal.log_error(msg)
                if STOP_ON_ERROR:
                    break

    if not errors and CREATE_LEVEL:
        try:
            _create_level(LEVEL_PATH)
        except Exception as exc:
            msg = f"[ERROR] LEVEL {LEVEL_PATH}: {exc}"
            errors.append(msg)
            unreal.log_error(msg)

    if not errors:
        _verify_assets_exist()

    if errors:
        raise RuntimeError("Asset-shell creation stopped due to errors. Check Output Log.")

    unreal.log("=== SignalProject asset-shell creation complete ===")


run()
