"""
Apply deterministic class-default bindings for SignalProject slice shell assets.
Scope: class defaults + startup maps only; no instance-level placement/ref wiring.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

STOP_ON_ERROR = True
TARGET_MAP = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"

# (Blueprint asset path, property name, kind, value asset path)
BINDINGS = [
    # Game mode playable class chain
    (
        "/Game/Signal/Blueprints/Framework/BP_SignalGameMode",
        "player_controller_class",
        "class",
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerController",
    ),
    (
        "/Game/Signal/Blueprints/Framework/BP_SignalGameMode",
        "default_pawn_class",
        "class",
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerCharacter",
    ),
    # Controller widget classes
    (
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerController",
        "DesktopRootWidgetClass",
        "class",
        "/Game/Signal/Blueprints/UI/WBP_DesktopRoot",
    ),
    (
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerController",
        "EndingTitleCardWidgetClass",
        "class",
        "/Game/Signal/Blueprints/Endings/WBP_EndingTitleCard",
    ),
    # Minigame manager widget classes
    (
        "/Game/Signal/Blueprints/Minigames/BP_MinigameManager",
        "DependencyMatchWidgetClass",
        "class",
        "/Game/Signal/Blueprints/Minigames/WBP_MG_DependencyMatch",
    ),
    (
        "/Game/Signal/Blueprints/Minigames/BP_MinigameManager",
        "AnomalyChoicePopupClass",
        "class",
        "/Game/Signal/Blueprints/UI/WBP_AnomalyChoicePopup",
    ),
    # Desktop root app child widget classes
    (
        "/Game/Signal/Blueprints/UI/WBP_DesktopRoot",
        "ChatAppWidgetClass",
        "class",
        "/Game/Signal/Blueprints/UI/WBP_ChatApp",
    ),
    (
        "/Game/Signal/Blueprints/UI/WBP_DesktopRoot",
        "ReportEditorWidgetClass",
        "class",
        "/Game/Signal/Blueprints/Report/WBP_ReportEditor",
    ),
    # Deterministic widget DataTable defaults
    (
        "/Game/Signal/Blueprints/Report/WBP_ReportEditor",
        "ReportSentencesTable",
        "asset",
        "/Game/Signal/Data/DataTables/DT_ReportSentencePools",
    ),
    (
        "/Game/Signal/Blueprints/Endings/WBP_EndingTitleCard",
        "EndingSubtitlesTable",
        "asset",
        "/Game/Signal/Data/DataTables/DT_EndingSubtitles",
    ),
    # Deterministic chat manager DataTable defaults
    (
        "/Game/Signal/Blueprints/Dialogue/BP_ChatConversationManager",
        "SupervisorLinesTable",
        "asset",
        "/Game/Signal/Data/DataTables/DT_SupervisorLines",
    ),
    (
        "/Game/Signal/Blueprints/Dialogue/BP_ChatConversationManager",
        "NormalRepliesTable",
        "asset",
        "/Game/Signal/Data/DataTables/DT_NormalReplies",
    ),
    (
        "/Game/Signal/Blueprints/Dialogue/BP_ChatConversationManager",
        "HiddenDialoguesTable",
        "asset",
        "/Game/Signal/Data/DataTables/DT_HiddenDialogues",
    ),
]


def _load_blueprint_asset(bp_path: str) -> unreal.Blueprint:
    bp = unreal.load_asset(bp_path)
    if not bp:
        raise RuntimeError(f"Missing blueprint asset: {bp_path}")
    return bp


def _load_bp_generated_class(bp_path: str):
    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if bp_class:
        return bp_class

    leaf = bp_path.rsplit("/", 1)[-1]
    fallback = unreal.load_class(None, f"{bp_path}.{leaf}_C")
    if fallback:
        return fallback

    raise RuntimeError(f"Unable to load generated class for blueprint: {bp_path}")


def _resolve_value(kind: str, value_path: str):
    if kind == "class":
        return _load_bp_generated_class(value_path)
    if kind == "asset":
        value = unreal.load_asset(value_path)
        if not value:
            raise RuntimeError(f"Missing asset: {value_path}")
        return value

    raise RuntimeError(f"Unsupported binding kind: {kind}")


def _obj_path_or_none(obj) -> str:
    if not obj:
        return "None"
    return obj.get_path_name()


def _apply_binding(bp_path: str, prop: str, kind: str, value_path: str) -> None:
    bp_asset = _load_blueprint_asset(bp_path)
    bp_class = _load_bp_generated_class(bp_path)
    cdo = unreal.get_default_object(bp_class)

    value = _resolve_value(kind, value_path)

    # Fail early if property does not exist on this CDO.
    try:
        current = cdo.get_editor_property(prop)
    except Exception as exc:
        raise RuntimeError(f"Property '{prop}' missing on {bp_path}: {exc}")

    if current != value:
        cdo.set_editor_property(prop, value)

    # Some commandlet Python runtimes do not expose KismetEditorUtilities.
    # For this deterministic pass, setting CDO defaults and saving the asset is sufficient.
    unreal.EditorAssetLibrary.save_loaded_asset(bp_asset)

    applied = cdo.get_editor_property(prop)
    unreal.log(
        f"Bound {bp_path}.{prop} -> {_obj_path_or_none(applied)} "
        f"(target={_obj_path_or_none(value)})"
    )


def _apply_map_settings() -> None:
    # Validate map asset exists before writing config.
    level_obj_path = f"{TARGET_MAP}.{TARGET_MAP.rsplit('/', 1)[-1]}"
    if not unreal.EditorAssetLibrary.does_asset_exist(level_obj_path):
        raise RuntimeError(f"Target map does not exist: {TARGET_MAP}")

    maps = unreal.GameMapsSettings.get_game_maps_settings()
    target_soft_path = unreal.SoftObjectPath(level_obj_path)
    maps.set_editor_property("game_default_map", target_soft_path)
    maps.set_editor_property("editor_startup_map", target_soft_path)
    maps.save_config()

    actual_game_map = maps.get_editor_property("game_default_map")
    actual_editor_map = maps.get_editor_property("editor_startup_map")
    unreal.log(f"Applied map settings: GameDefaultMap={actual_game_map}, EditorStartupMap={actual_editor_map}")


def run() -> None:
    unreal.log("=== SignalProject class-default binding pass start ===")

    errors = []
    for bp_path, prop, kind, value_path in BINDINGS:
        try:
            _apply_binding(bp_path, prop, kind, value_path)
        except Exception as exc:
            msg = f"[ERROR] Binding failed for {bp_path}.{prop}: {exc}"
            errors.append(msg)
            unreal.log_error(msg)
            if STOP_ON_ERROR:
                break

    if not errors:
        unreal.log(
            "Startup map config is managed in Config/DefaultEngine.ini; "
            "skipping GameMapsSettings mutation in commandlet pass."
        )

    if errors:
        raise RuntimeError("Class-default binding pass completed with errors. Check Output Log.")

    unreal.log("=== SignalProject class-default binding pass complete ===")


run()
