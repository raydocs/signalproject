"""
Inspect Unreal Python property names for PostProcessVolume and FPostProcessSettings.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"


def _load_level():
    world = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not world:
        raise RuntimeError(f"Failed to load map: {LEVEL_PATH}")
    return world


def _find_post_process_volume():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor.get_class().get_name() == "PostProcessVolume":
            return actor
    return None


def run() -> None:
    unreal.log("=== inspect_postprocess_python_api start ===")
    _load_level()

    actor = _find_post_process_volume()
    if not actor:
        actor_class = unreal.load_class(None, "/Script/Engine.PostProcessVolume")
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            actor_class, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0)
        )
        actor.set_actor_label("SP_Debug_PostProcessProbe")
        unreal.log("Spawned temporary PostProcessVolume probe actor")

    actor_names = sorted(
        name for name in dir(actor) if any(token in name.lower() for token in ("bound", "weight", "priority", "blend"))
    )
    unreal.log(f"ACTOR names :: {actor_names}")

    settings = actor.get_editor_property("settings")
    settings_names = sorted(
        name
        for name in dir(settings)
        if any(token in name.lower() for token in ("exposure", "bloom", "override", "histogram"))
    )
    unreal.log(f"SETTINGS names :: {settings_names}")
    unreal.log("=== inspect_postprocess_python_api complete ===")


run()
