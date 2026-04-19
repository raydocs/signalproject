"""
Deterministic level actor placement + instance binding for SignalProject slice.
Scope: LV_ApartmentMain only, no geometry/layout/widget-tree work.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

STOP_ON_ERROR = True
LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"
GAME_MODE_OVERRIDE_BP = "/Game/Signal/Blueprints/Framework/BP_SignalGameMode"
GAME_MODE_OVERRIDE_FALLBACK = "/Script/SignalProject.SignalGameMode"

ACTOR_SPECS = {
    "flow": {
        "label": "SP_SignalGameFlowManager",
        "bp": "/Game/Signal/Blueprints/Core/BP_SignalGameFlowManager",
        "fallback": "/Script/SignalProject.SignalGameFlowManager",
        "location": unreal.Vector(0.0, 0.0, 120.0),
    },
    "route": {
        "label": "SP_RouteStateManager",
        "bp": "/Game/Signal/Blueprints/Core/BP_RouteStateManager",
        "fallback": "/Script/SignalProject.RouteStateManager",
        "location": unreal.Vector(220.0, 0.0, 120.0),
    },
    "anomaly": {
        "label": "SP_AnomalyManager",
        "bp": "/Game/Signal/Blueprints/Anomalies/BP_AnomalyManager",
        "fallback": "/Script/SignalProject.AnomalyManager",
        "location": unreal.Vector(440.0, 0.0, 120.0),
    },
    "chat": {
        "label": "SP_ChatConversationManager",
        "bp": "/Game/Signal/Blueprints/Dialogue/BP_ChatConversationManager",
        "fallback": "/Script/SignalProject.ChatConversationManager",
        "location": unreal.Vector(660.0, 0.0, 120.0),
    },
    "minigame": {
        "label": "SP_MinigameManager",
        "bp": "/Game/Signal/Blueprints/Minigames/BP_MinigameManager",
        "fallback": "/Script/SignalProject.MinigameManager",
        "location": unreal.Vector(880.0, 0.0, 120.0),
    },
    "hidden": {
        "label": "SP_HiddenDialogueUnlocker",
        "bp": "/Game/Signal/Blueprints/Dialogue/BP_HiddenDialogueUnlocker",
        "fallback": "/Script/SignalProject.HiddenDialogueUnlocker",
        "location": unreal.Vector(1100.0, 0.0, 120.0),
    },
    "computer": {
        "label": "SP_ComputerTerminal",
        "bp": "/Game/Signal/Blueprints/Interactables/BP_ComputerTerminal",
        "fallback": "/Script/SignalProject.ComputerTerminal",
        "location": unreal.Vector(0.0, 300.0, 120.0),
    },
    "resolver_blackout": {
        "label": "SP_AirResolver_BLACKOUT",
        "bp": "/Game/Signal/Blueprints/Interactables/BP_AirConditionerUnit",
        "fallback": "/Script/SignalProject.AirConditionerUnit",
        "location": unreal.Vector(240.0, 300.0, 120.0),
    },
    "resolver_diskclean": {
        "label": "SP_AirResolver_DISKCLEAN",
        "bp": "/Game/Signal/Blueprints/Interactables/BP_AirConditionerUnit",
        "fallback": "/Script/SignalProject.AirConditionerUnit",
        "location": unreal.Vector(420.0, 300.0, 120.0),
    },
    "resolver_freeze": {
        "label": "SP_AirResolver_FREEZE",
        "bp": "/Game/Signal/Blueprints/Interactables/BP_AirConditionerUnit",
        "fallback": "/Script/SignalProject.AirConditionerUnit",
        "location": unreal.Vector(600.0, 300.0, 120.0),
    },
}

RESOLVER_CONFIGS = {
    "resolver_blackout": {
        "anomaly_type": unreal.E_AnomalyType.BLACKOUT,
        "interaction_text": "Toggle light-switch bypass (BLACKOUT placeholder)",
    },
    "resolver_diskclean": {
        "anomaly_type": unreal.E_AnomalyType.DISKCLEAN,
        "interaction_text": "Inspect computer-case heavy-load vents (DISKCLEAN placeholder)",
    },
    "resolver_freeze": {
        "anomaly_type": unreal.E_AnomalyType.FREEZE,
        "interaction_text": "Adjust AC cooling controls (FREEZE placeholder)",
    },
}

LEGACY_ACTOR_LABELS = [
    "SP_AirConditionerUnit",
]

PLAYER_START_SPEC = {
    "label": "SP_PlayerStart",
    "class_path": "/Script/Engine.PlayerStart",
    "location": unreal.Vector(-320.0, -120.0, 120.0),
}

DATA_TABLES = {
    "system_copy": "/Game/Signal/Data/DataTables/DT_SystemCopy",
    "supervisor": "/Game/Signal/Data/DataTables/DT_SupervisorLines",
    "normal": "/Game/Signal/Data/DataTables/DT_NormalReplies",
    "hidden": "/Game/Signal/Data/DataTables/DT_HiddenDialogues",
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
    (
        "/Game/Signal/Blueprints/UI/WBP_DesktopRoot",
        "TaskListWidgetClass",
    ),
]


def _ensure_map_loaded() -> None:
    level_obj_path = f"{LEVEL_PATH}.{LEVEL_PATH.rsplit('/', 1)[-1]}"
    if not unreal.EditorAssetLibrary.does_asset_exist(level_obj_path):
        raise RuntimeError(f"Missing map asset: {LEVEL_PATH}")

    world = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not world:
        raise RuntimeError(f"Failed to load map: {LEVEL_PATH}")


def _actor_subsystem() -> unreal.EditorActorSubsystem:
    return unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


def _all_actors() -> list[unreal.Actor]:
    return list(_actor_subsystem().get_all_level_actors())


def _find_actor_by_label(label: str):
    for actor in _all_actors():
        if actor.get_actor_label() == label:
            return actor
    return None


def _load_spawn_class(bp_path: str, fallback_class_path: str):
    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if bp_class:
        return bp_class

    native = unreal.load_class(None, fallback_class_path)
    if native:
        unreal.log_warning(f"Blueprint class missing ({bp_path}); using native class {fallback_class_path}")
        return native

    raise RuntimeError(f"Unable to load class from {bp_path} or fallback {fallback_class_path}")


def _spawn_or_reuse(spec: dict, summary: dict):
    existing = _find_actor_by_label(spec["label"])
    if existing:
        summary["reused"].append(spec["label"])
        return existing

    spawn_class = _load_spawn_class(spec["bp"], spec["fallback"])
    actor = _actor_subsystem().spawn_actor_from_class(spawn_class, spec["location"], unreal.Rotator(0.0, 0.0, 0.0))
    if not actor:
        raise RuntimeError(f"Failed to spawn actor for label {spec['label']}")

    actor.set_actor_label(spec["label"])
    summary["created"].append(spec["label"])
    return actor


def _remove_legacy_actors(summary: dict):
    for legacy_label in LEGACY_ACTOR_LABELS:
        legacy_actor = _find_actor_by_label(legacy_label)
        if legacy_actor:
            _actor_subsystem().destroy_actor(legacy_actor)
            summary.setdefault("removed", []).append(legacy_label)


def _ensure_player_start(summary: dict):
    existing = _find_actor_by_label(PLAYER_START_SPEC["label"])
    if existing:
        summary["reused"].append(PLAYER_START_SPEC["label"])
        return existing

    for actor in _all_actors():
        if actor.get_class().get_name() == "PlayerStart":
            summary["reused"].append(f"{PLAYER_START_SPEC['label']}(existing PlayerStart:{actor.get_actor_label()})")
            return actor

    cls = unreal.load_class(None, PLAYER_START_SPEC["class_path"])
    if not cls:
        raise RuntimeError("Failed to load /Script/Engine.PlayerStart")

    actor = _actor_subsystem().spawn_actor_from_class(cls, PLAYER_START_SPEC["location"], unreal.Rotator(0.0, 0.0, 0.0))
    if not actor:
        raise RuntimeError("Failed to spawn PlayerStart")

    actor.set_actor_label(PLAYER_START_SPEC["label"])
    summary["created"].append(PLAYER_START_SPEC["label"])
    return actor


def _load_table(asset_path: str):
    table = unreal.load_asset(asset_path)
    if not table:
        raise RuntimeError(f"Missing DataTable asset: {asset_path}")
    return table


def _safe_set(owner, prop: str, value, summary: dict):
    try:
        owner.set_editor_property(prop, value)
    except Exception as exc:
        raise RuntimeError(f"{owner.get_actor_label()}.{prop} set failed: {exc}")

    actual = owner.get_editor_property(prop)
    if actual != value:
        raise RuntimeError(f"{owner.get_actor_label()}.{prop} verification mismatch")

    summary["bindings"].append(f"{owner.get_actor_label()}.{prop}")


def _safe_set_text(owner, prop: str, value: str, summary: dict):
    try:
        owner.set_editor_property(prop, value)
    except Exception as exc:
        raise RuntimeError(f"{owner.get_actor_label()}.{prop} text set failed: {exc}")

    actual = owner.get_editor_property(prop)
    actual_str = actual.to_string() if hasattr(actual, "to_string") else str(actual)
    if actual_str != value:
        raise RuntimeError(
            f"{owner.get_actor_label()}.{prop} text verification mismatch: {actual_str} != {value}"
        )

    summary["bindings"].append(f"{owner.get_actor_label()}.{prop}")


def _obj_path_or_none(obj) -> str:
    if not obj:
        return "None"
    return obj.get_path_name()


def _verify_class_defaults() -> None:
    for bp_path, prop in CLASS_DEFAULT_CHECKS:
        cls = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if not cls:
            raise RuntimeError(f"Missing class-default target blueprint: {bp_path}")
        cdo = unreal.get_default_object(cls)
        value = cdo.get_editor_property(prop)
        if not value:
            raise RuntimeError(f"Class default missing: {bp_path}.{prop}")
        unreal.log(f"Class default OK: {bp_path}.{prop} -> {_obj_path_or_none(value)}")


def _apply_world_settings(summary: dict) -> None:
    editor_world = unreal.EditorLevelLibrary.get_editor_world()
    if not editor_world:
        raise RuntimeError("Failed to access editor world")

    world_settings = editor_world.get_world_settings()
    desired_game_mode = _load_spawn_class(GAME_MODE_OVERRIDE_BP, GAME_MODE_OVERRIDE_FALLBACK)

    world_settings.set_editor_property("default_game_mode", desired_game_mode)
    actual = world_settings.get_editor_property("default_game_mode")
    if actual != desired_game_mode:
        raise RuntimeError(
            "WorldSettings.default_game_mode verification mismatch: "
            f"{_obj_path_or_none(actual)} != {_obj_path_or_none(desired_game_mode)}"
        )

    summary["bindings"].append("WorldSettings.default_game_mode")
    unreal.log(f"World settings OK: default_game_mode -> {_obj_path_or_none(actual)}")


def _save_level() -> None:
    if not unreal.EditorLevelLibrary.save_current_level():
        raise RuntimeError("Failed to save current level")


def run() -> None:
    unreal.log("=== SignalProject deterministic level placement start ===")
    _ensure_map_loaded()

    summary = {
        "created": [],
        "reused": [],
        "removed": [],
        "bindings": [],
    }

    errors = []
    actors = {}

    try:
        _remove_legacy_actors(summary)

        for key, spec in ACTOR_SPECS.items():
            actors[key] = _spawn_or_reuse(spec, summary)

        _ensure_player_start(summary)

        dt_system = _load_table(DATA_TABLES["system_copy"])
        dt_supervisor = _load_table(DATA_TABLES["supervisor"])
        dt_normal = _load_table(DATA_TABLES["normal"])
        dt_hidden = _load_table(DATA_TABLES["hidden"])

        # Flow manager
        _safe_set(actors["flow"], "CurrentDayIndex", 2, summary)
        _safe_set(actors["flow"], "RouteStateManagerRef", actors["route"], summary)
        _safe_set(actors["flow"], "AnomalyManagerRef", actors["anomaly"], summary)
        _safe_set(actors["flow"], "ChatConversationManagerRef", actors["chat"], summary)
        _safe_set(actors["flow"], "MinigameManagerRef", actors["minigame"], summary)
        _safe_set(actors["flow"], "HiddenDialogueUnlockerRef", actors["hidden"], summary)

        # Anomaly manager
        _safe_set(actors["anomaly"], "GameFlowManagerRef", actors["flow"], summary)
        _safe_set(actors["anomaly"], "MinigameManagerRef", actors["minigame"], summary)

        # Chat conversation manager
        _safe_set(actors["chat"], "AnomalyManagerRef", actors["anomaly"], summary)
        _safe_set(actors["chat"], "RouteStateManagerRef", actors["route"], summary)
        _safe_set(actors["chat"], "SupervisorLinesTable", dt_supervisor, summary)
        _safe_set(actors["chat"], "NormalRepliesTable", dt_normal, summary)
        _safe_set(actors["chat"], "HiddenDialoguesTable", dt_hidden, summary)

        # Minigame manager
        _safe_set(actors["minigame"], "GameFlowManagerRef", actors["flow"], summary)
        _safe_set(actors["minigame"], "AnomalyManagerRef", actors["anomaly"], summary)
        _safe_set(actors["minigame"], "RouteStateManagerRef", actors["route"], summary)

        # Hidden dialogue unlocker
        _safe_set(actors["hidden"], "ChatConversationManagerRef", actors["chat"], summary)
        _safe_set(actors["hidden"], "RouteStateManagerRef", actors["route"], summary)
        _safe_set(actors["hidden"], "SystemCopyTable", dt_system, summary)

        # Interactables
        _safe_set(actors["computer"], "GameFlowManagerRef", actors["flow"], summary)
        _safe_set(actors["computer"], "SystemCopyTable", dt_system, summary)

        for resolver_key, resolver_config in RESOLVER_CONFIGS.items():
            resolver_actor = actors[resolver_key]
            _safe_set(resolver_actor, "AnomalyManagerRef", actors["anomaly"], summary)
            _safe_set(resolver_actor, "HiddenDialogueUnlockerRef", actors["hidden"], summary)
            _safe_set(resolver_actor, "GameFlowManagerRef", actors["flow"], summary)
            _safe_set(resolver_actor, "SystemCopyTable", dt_system, summary)
            _safe_set(resolver_actor, "bIsCurrentlyAvailable", True, summary)
            _safe_set(resolver_actor, "HandledAnomalyType", resolver_config["anomaly_type"], summary)
            _safe_set_text(resolver_actor, "InteractionText", resolver_config["interaction_text"], summary)

        _apply_world_settings(summary)
        _verify_class_defaults()
        _save_level()

    except Exception as exc:
        errors.append(str(exc))
        unreal.log_error(f"[ERROR] {exc}")
        if STOP_ON_ERROR:
            raise RuntimeError("Deterministic level placement pass failed. Check Output Log.")

    unreal.log(f"Created actors: {summary['created']}")
    unreal.log(f"Reused actors: {summary['reused']}")
    unreal.log(f"Removed legacy actors: {summary['removed']}")
    unreal.log(f"Applied bindings ({len(summary['bindings'])}): {summary['bindings']}")

    if errors:
        raise RuntimeError("Deterministic level placement pass completed with errors.")

    unreal.log("=== SignalProject deterministic level placement complete ===")


run()
