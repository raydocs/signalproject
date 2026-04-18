"""
Validate deterministic level placement + binding state for SignalProject slice.
Scope: LV_ApartmentMain actor presence, instance references, and class-default prerequisites.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"

ACTOR_LABELS = {
    "flow": "SP_SignalGameFlowManager",
    "route": "SP_RouteStateManager",
    "anomaly": "SP_AnomalyManager",
    "chat": "SP_ChatConversationManager",
    "minigame": "SP_MinigameManager",
    "hidden": "SP_HiddenDialogueUnlocker",
    "computer": "SP_ComputerTerminal",
    "air": "SP_AirConditionerUnit",
    "player_start": "SP_PlayerStart",
}

EXPECTED_TABLES = {
    "system": "/Game/Signal/Data/DataTables/DT_SystemCopy.DT_SystemCopy",
    "supervisor": "/Game/Signal/Data/DataTables/DT_SupervisorLines.DT_SupervisorLines",
    "normal": "/Game/Signal/Data/DataTables/DT_NormalReplies.DT_NormalReplies",
    "hidden": "/Game/Signal/Data/DataTables/DT_HiddenDialogues.DT_HiddenDialogues",
}

CLASS_DEFAULT_CHECKS = [
    (
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerController",
        "DesktopRootWidgetClass",
    ),
    (
        "/Game/Signal/Blueprints/Player/BP_SignalPlayerController",
        "EndingTitleCardWidgetClass",
    ),
    (
        "/Game/Signal/Blueprints/Minigames/BP_MinigameManager",
        "DependencyMatchWidgetClass",
    ),
    (
        "/Game/Signal/Blueprints/Minigames/BP_MinigameManager",
        "AnomalyChoicePopupClass",
    ),
]


def _all_actors() -> list[unreal.Actor]:
    return list(unreal.EditorLevelLibrary.get_all_level_actors())


def _find_by_label(label: str):
    matches = [a for a in _all_actors() if a.get_actor_label() == label]
    if len(matches) != 1:
        raise RuntimeError(f"Expected exactly one actor labeled '{label}', found {len(matches)}")
    return matches[0]


def _path_or_none(obj) -> str:
    if not obj:
        return "None"
    return obj.get_path_name()


def _expect_ref(owner, prop: str, expected_obj) -> None:
    actual = owner.get_editor_property(prop)
    if actual != expected_obj:
        raise RuntimeError(
            f"Ref mismatch: {owner.get_actor_label()}.{prop} -> "
            f"{_path_or_none(actual)} (expected {_path_or_none(expected_obj)})"
        )


def _expect_int(owner, prop: str, expected_value: int) -> None:
    actual = owner.get_editor_property(prop)
    if int(actual) != int(expected_value):
        raise RuntimeError(
            f"Value mismatch: {owner.get_actor_label()}.{prop}={actual} (expected {expected_value})"
        )


def _expect_asset_path(owner, prop: str, expected_path: str) -> None:
    actual = owner.get_editor_property(prop)
    actual_path = _path_or_none(actual)
    if actual_path != expected_path:
        raise RuntimeError(
            f"Asset mismatch: {owner.get_actor_label()}.{prop} -> {actual_path} "
            f"(expected {expected_path})"
        )


def _load_level() -> None:
    level_obj_path = f"{LEVEL_PATH}.{LEVEL_PATH.rsplit('/', 1)[-1]}"
    if not unreal.EditorAssetLibrary.does_asset_exist(level_obj_path):
        raise RuntimeError(f"Missing map asset: {LEVEL_PATH}")

    world = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not world:
        raise RuntimeError(f"Failed to load map: {LEVEL_PATH}")


def _verify_class_defaults() -> None:
    for bp_path, prop in CLASS_DEFAULT_CHECKS:
        bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if not bp_class:
            raise RuntimeError(f"Missing blueprint class: {bp_path}")
        cdo = unreal.get_default_object(bp_class)
        value = cdo.get_editor_property(prop)
        if not value:
            raise RuntimeError(f"Class default missing: {bp_path}.{prop}")
        unreal.log(f"Class default OK: {bp_path}.{prop} -> {_path_or_none(value)}")


def run() -> None:
    unreal.log("=== SignalProject level-binding validation start ===")
    _load_level()

    actors = {key: _find_by_label(label) for key, label in ACTOR_LABELS.items()}

    # Flow manager and routing
    _expect_int(actors["flow"], "CurrentDayIndex", 2)
    _expect_ref(actors["flow"], "RouteStateManagerRef", actors["route"])
    _expect_ref(actors["flow"], "AnomalyManagerRef", actors["anomaly"])
    _expect_ref(actors["flow"], "ChatConversationManagerRef", actors["chat"])
    _expect_ref(actors["flow"], "MinigameManagerRef", actors["minigame"])
    _expect_ref(actors["flow"], "HiddenDialogueUnlockerRef", actors["hidden"])

    # Anomaly + chat + minigame core refs
    _expect_ref(actors["anomaly"], "GameFlowManagerRef", actors["flow"])
    _expect_ref(actors["anomaly"], "MinigameManagerRef", actors["minigame"])

    _expect_ref(actors["chat"], "AnomalyManagerRef", actors["anomaly"])
    _expect_ref(actors["chat"], "RouteStateManagerRef", actors["route"])
    _expect_asset_path(actors["chat"], "SupervisorLinesTable", EXPECTED_TABLES["supervisor"])
    _expect_asset_path(actors["chat"], "NormalRepliesTable", EXPECTED_TABLES["normal"])
    _expect_asset_path(actors["chat"], "HiddenDialoguesTable", EXPECTED_TABLES["hidden"])

    _expect_ref(actors["minigame"], "GameFlowManagerRef", actors["flow"])
    _expect_ref(actors["minigame"], "AnomalyManagerRef", actors["anomaly"])
    _expect_ref(actors["minigame"], "RouteStateManagerRef", actors["route"])

    # Hidden unlocker + world interactables
    _expect_ref(actors["hidden"], "ChatConversationManagerRef", actors["chat"])
    _expect_ref(actors["hidden"], "RouteStateManagerRef", actors["route"])
    _expect_asset_path(actors["hidden"], "SystemCopyTable", EXPECTED_TABLES["system"])

    _expect_ref(actors["computer"], "GameFlowManagerRef", actors["flow"])
    _expect_asset_path(actors["computer"], "SystemCopyTable", EXPECTED_TABLES["system"])

    _expect_ref(actors["air"], "AnomalyManagerRef", actors["anomaly"])
    _expect_ref(actors["air"], "HiddenDialogueUnlockerRef", actors["hidden"])
    _expect_ref(actors["air"], "GameFlowManagerRef", actors["flow"])
    _expect_asset_path(actors["air"], "SystemCopyTable", EXPECTED_TABLES["system"])

    _verify_class_defaults()

    unreal.log("Level actor labels found exactly once and deterministic refs validated.")
    unreal.log("=== SignalProject level-binding validation complete ===")


run()
