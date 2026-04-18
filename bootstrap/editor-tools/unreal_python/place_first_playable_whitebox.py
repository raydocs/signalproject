"""
Place a minimal first-playable whitebox room for SignalProject.
Scope: reusable placeholder geometry, basic lighting, and practical actor transforms only.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"

CUBE_MESH_PATH = "/Engine/BasicShapes/Cube.Cube"
WHITEBOX_MATERIAL_PATH = "/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"

WHITEBOX_STATIC_MESH_SPECS = [
    {
        "label": "SP_Whitebox_Floor",
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(6.0, 5.0, 0.2),
    },
    {
        "label": "SP_Whitebox_Ceiling",
        "location": unreal.Vector(0.0, 0.0, 280.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(6.0, 5.0, 0.2),
    },
    {
        "label": "SP_Whitebox_Wall_PosX",
        "location": unreal.Vector(310.0, 0.0, 150.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(0.2, 5.2, 3.0),
    },
    {
        "label": "SP_Whitebox_Wall_NegX",
        "location": unreal.Vector(-310.0, 0.0, 150.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(0.2, 5.2, 3.0),
    },
    {
        "label": "SP_Whitebox_Wall_PosY",
        "location": unreal.Vector(0.0, 260.0, 150.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(6.2, 0.2, 3.0),
    },
    {
        "label": "SP_Whitebox_Wall_NegY",
        "location": unreal.Vector(0.0, -260.0, 150.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(6.2, 0.2, 3.0),
    },
    {
        "label": "SP_Whitebox_Desk",
        "location": unreal.Vector(180.0, 160.0, 40.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(1.4, 0.7, 0.8),
    },
    {
        "label": "SP_Whitebox_Bed",
        "location": unreal.Vector(-180.0, 150.0, 22.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "scale": unreal.Vector(2.0, 0.9, 0.45),
    },
]

LIGHT_SPECS = [
    {
        "label": "SP_Whitebox_PointLight",
        "class_path": "/Script/Engine.PointLight",
        "location": unreal.Vector(150.0, 120.0, 230.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "intensity": 1100.0,
        "attenuation_radius": 500.0,
    },
    {
        "label": "SP_Whitebox_FillLight",
        "class_path": "/Script/Engine.PointLight",
        "location": unreal.Vector(-120.0, -40.0, 210.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "intensity": 320.0,
        "attenuation_radius": 420.0,
    },
]

POST_PROCESS_SPEC = {
    "label": "SP_Whitebox_PostProcess",
    "class_path": "/Script/Engine.PostProcessVolume",
    "location": unreal.Vector(0.0, 0.0, 120.0),
    "rotation": unreal.Rotator(0.0, 0.0, 0.0),
}

RUNTIME_ACTOR_TRANSFORMS = {
    "SP_PlayerStart": {
        "location": unreal.Vector(-220.0, -120.0, 100.0),
        "rotation": unreal.Rotator(-6.0, 35.0, 0.0),
    },
    "SP_ComputerTerminal": {
        "location": unreal.Vector(180.0, 160.0, 50.0),
        "rotation": unreal.Rotator(0.0, -90.0, 0.0),
    },
    "SP_AirConditionerUnit": {
        "location": unreal.Vector(-220.0, -160.0, 210.0),
        "rotation": unreal.Rotator(0.0, 35.0, 0.0),
    },
}


def _load_level() -> None:
    world = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not world:
        raise RuntimeError(f"Failed to load map: {LEVEL_PATH}")


def _all_actors() -> list[unreal.Actor]:
    return list(unreal.EditorLevelLibrary.get_all_level_actors())


def _find_actor_by_label(label: str):
    for actor in _all_actors():
        if actor.get_actor_label() == label:
            return actor
    return None


def _spawn_or_reuse(class_path: str, label: str, location, rotation, summary: dict):
    actor = _find_actor_by_label(label)
    if actor:
        summary["reused"].append(label)
    else:
        actor_class = unreal.load_class(None, class_path)
        if not actor_class:
            raise RuntimeError(f"Failed to load class: {class_path}")

        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, rotation)
        if not actor:
            raise RuntimeError(f"Failed to spawn actor: {label}")

        actor.set_actor_label(label)
        summary["created"].append(label)

    actor.set_actor_location(location, False, False)
    actor.set_actor_rotation(rotation, False)
    return actor


def _configure_static_mesh_actor(actor, scale) -> None:
    mesh_component = actor.get_editor_property("static_mesh_component")
    if not mesh_component:
        raise RuntimeError(f"StaticMeshActor missing component: {actor.get_actor_label()}")

    cube_mesh = unreal.load_asset(CUBE_MESH_PATH)
    whitebox_material = unreal.load_asset(WHITEBOX_MATERIAL_PATH)
    if not cube_mesh:
        raise RuntimeError(f"Failed to load mesh: {CUBE_MESH_PATH}")
    if not whitebox_material:
        raise RuntimeError(f"Failed to load material: {WHITEBOX_MATERIAL_PATH}")

    mesh_component.set_static_mesh(cube_mesh)
    mesh_component.set_mobility(unreal.ComponentMobility.STATIC)
    mesh_component.set_collision_profile_name("BlockAll")
    mesh_component.set_material(0, whitebox_material)
    actor.set_actor_scale3d(scale)


def _configure_light_actor(actor, intensity: float, attenuation_radius: float) -> None:
    light_component = actor.get_component_by_class(unreal.LightComponent)
    if not light_component:
        raise RuntimeError(f"Light actor missing light component: {actor.get_actor_label()}")

    light_component.set_editor_property("intensity", intensity)
    try:
        light_component.set_editor_property("attenuation_radius", attenuation_radius)
    except Exception:
        pass


def _configure_post_process_volume(actor) -> None:
    actor.set_editor_property("unbound", True)
    actor.set_editor_property("blend_weight", 1.0)
    actor.set_editor_property("priority", 50.0)

    settings = actor.get_editor_property("settings")
    settings.override_auto_exposure_method = True
    settings.auto_exposure_method = unreal.AutoExposureMethod.AEM_MANUAL
    settings.override_auto_exposure_bias = True
    settings.auto_exposure_bias = 0.0
    settings.override_auto_exposure_min_brightness = True
    settings.auto_exposure_min_brightness = 1.0
    settings.override_auto_exposure_max_brightness = True
    settings.auto_exposure_max_brightness = 1.0
    settings.override_bloom_intensity = True
    settings.bloom_intensity = 0.0
    actor.set_editor_property("settings", settings)


def _apply_runtime_actor_transform(label: str, spec: dict, summary: dict) -> None:
    actor = _find_actor_by_label(label)
    if not actor:
        raise RuntimeError(f"Missing runtime actor labeled {label}")

    actor.set_actor_location(spec["location"], False, False)
    actor.set_actor_rotation(spec["rotation"], False)
    summary["moved"].append(label)


def _save_level() -> None:
    if not unreal.EditorLevelLibrary.save_current_level():
        raise RuntimeError("Failed to save current level")


def run() -> None:
    unreal.log("=== SignalProject whitebox placement start ===")
    _load_level()

    summary = {
        "created": [],
        "reused": [],
        "moved": [],
    }

    for spec in WHITEBOX_STATIC_MESH_SPECS:
        actor = _spawn_or_reuse(
            "/Script/Engine.StaticMeshActor",
            spec["label"],
            spec["location"],
            spec["rotation"],
            summary,
        )
        _configure_static_mesh_actor(actor, spec["scale"])

    for spec in LIGHT_SPECS:
        actor = _spawn_or_reuse(
            spec["class_path"],
            spec["label"],
            spec["location"],
            spec["rotation"],
            summary,
        )
        _configure_light_actor(actor, spec["intensity"], spec["attenuation_radius"])

    post_process = _spawn_or_reuse(
        POST_PROCESS_SPEC["class_path"],
        POST_PROCESS_SPEC["label"],
        POST_PROCESS_SPEC["location"],
        POST_PROCESS_SPEC["rotation"],
        summary,
    )
    _configure_post_process_volume(post_process)

    for label, spec in RUNTIME_ACTOR_TRANSFORMS.items():
        _apply_runtime_actor_transform(label, spec, summary)

    _save_level()

    unreal.log(f"Whitebox created: {summary['created']}")
    unreal.log(f"Whitebox reused: {summary['reused']}")
    unreal.log(f"Runtime actors moved: {summary['moved']}")
    unreal.log("=== SignalProject whitebox placement complete ===")


run()
