"""
Inspect first-playable readiness for SignalProject.
Scope: report manual-editor gaps that are still likely blockers after automated setup passes.
Run via UnrealEditor-Cmd Python commandlet.
"""

from __future__ import annotations

import unreal

LEVEL_PATH = "/Game/Signal/Levels/Gameplay/LV_ApartmentMain"

ACTOR_LABELS = {
    "computer": "SP_ComputerTerminal",
    "air": "SP_AirConditionerUnit",
    "player_start": "SP_PlayerStart",
}

WIDGET_BLUEPRINTS = {
    "DesktopRoot": "/Game/Signal/Blueprints/UI/WBP_DesktopRoot",
    "ChatApp": "/Game/Signal/Blueprints/UI/WBP_ChatApp",
    "DependencyMatch": "/Game/Signal/Blueprints/Minigames/WBP_MG_DependencyMatch",
    "AnomalyChoicePopup": "/Game/Signal/Blueprints/UI/WBP_AnomalyChoicePopup",
    "ReportEditor": "/Game/Signal/Blueprints/Report/WBP_ReportEditor",
    "EndingTitleCard": "/Game/Signal/Blueprints/Endings/WBP_EndingTitleCard",
}

MESH_COMPONENT_CLASS_NAMES = {
    "StaticMeshComponent",
    "SkeletalMeshComponent",
    "InstancedStaticMeshComponent",
    "HierarchicalInstancedStaticMeshComponent",
}

COLLISION_COMPONENT_CLASS_NAMES = {
    "BoxComponent",
    "SphereComponent",
    "CapsuleComponent",
    "StaticMeshComponent",
    "SkeletalMeshComponent",
}

PANEL_WIDGET_CLASS_NAMES = {
    "CanvasPanel",
    "Overlay",
    "Border",
    "VerticalBox",
    "HorizontalBox",
    "ScrollBox",
    "SizeBox",
    "UniformGridPanel",
}

INTERACTIVE_WIDGET_CLASS_NAMES = {
    "Button",
    "CheckBox",
    "EditableText",
    "EditableTextBox",
}


def _load_level():
    world = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not world:
        raise RuntimeError(f"Failed to load map: {LEVEL_PATH}")
    return world


def _all_actors() -> list[unreal.Actor]:
    return list(unreal.EditorLevelLibrary.get_all_level_actors())


def _find_actor_by_label(label: str):
    for actor in _all_actors():
        if actor.get_actor_label() == label:
            return actor
    return None


def _iter_widget_tree(widget) -> list:
    widgets = []
    if not widget:
        return widgets

    widgets.append(widget)

    try:
        child_count = widget.get_children_count()
    except Exception:
        child_count = 0

    for index in range(child_count):
        child = widget.get_child_at(index)
        widgets.extend(_iter_widget_tree(child))

    if widget.get_class().get_name() == "NamedSlot":
        try:
            child = widget.get_content()
        except Exception:
            child = None
        widgets.extend(_iter_widget_tree(child))

    return widgets


def _widget_tree_report(bp_path: str) -> str:
    widget_bp = unreal.load_asset(bp_path)
    if not widget_bp:
        return f"MISSING asset={bp_path}"

    widget_tree = None
    asset_read_errors = []

    try:
        widget_tree = widget_bp.get_editor_property("widget_tree")
    except Exception as exc:
        asset_read_errors.append(str(exc))

    if not widget_tree:
        try:
            widget_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
            if widget_class:
                widget_cdo = unreal.get_default_object(widget_class)
                widget_tree = widget_cdo.get_editor_property("widget_tree")
        except Exception as exc:
            asset_read_errors.append(str(exc))

    if not widget_tree:
        reason = " | ".join(asset_read_errors) if asset_read_errors else "unknown"
        return f"UNREADABLE asset={bp_path} reason={reason}"

    root_widget = None
    if widget_tree:
        try:
            root_widget = widget_tree.get_editor_property("root_widget")
        except Exception:
            root_widget = None

    if not root_widget:
        return "EMPTY root_widget=None total_widgets=0 panels=0 interactive=0 text=0"

    all_widgets = _iter_widget_tree(root_widget)
    class_names = [widget.get_class().get_name() for widget in all_widgets]
    panel_count = sum(1 for name in class_names if name in PANEL_WIDGET_CLASS_NAMES)
    interactive_count = sum(1 for name in class_names if name in INTERACTIVE_WIDGET_CLASS_NAMES)
    text_count = sum(1 for name in class_names if name in {"TextBlock", "RichTextBlock"})

    status = "READYISH"
    if len(all_widgets) <= 1:
        status = "EMPTYISH"
    elif interactive_count == 0:
        status = "LAYOUT_ONLY"

    return (
        f"{status} root={root_widget.get_name()} total_widgets={len(all_widgets)} "
        f"panels={panel_count} interactive={interactive_count} text={text_count}"
    )


def _component_report(actor) -> str:
    if not actor:
        return "MISSING actor"

    components = actor.get_components_by_class(unreal.ActorComponent)
    component_names = [component.get_class().get_name() for component in components]

    mesh_components = [name for name in component_names if name in MESH_COMPONENT_CLASS_NAMES]
    collision_components = [name for name in component_names if name in COLLISION_COMPONENT_CLASS_NAMES]

    visible_mesh_assets = []
    for component in components:
        class_name = component.get_class().get_name()
        if class_name not in MESH_COMPONENT_CLASS_NAMES:
            continue

        mesh_asset = None
        try:
            mesh_asset = component.get_editor_property("static_mesh")
        except Exception:
            mesh_asset = None

        if not mesh_asset:
            try:
                mesh_asset = component.get_editor_property("skeletal_mesh_asset")
            except Exception:
                mesh_asset = None

        if mesh_asset:
            visible_mesh_assets.append(mesh_asset.get_path_name())

    return (
        f"class={actor.get_class().get_name()} "
        f"mesh_components={len(mesh_components)} "
        f"collision_like_components={len(collision_components)} "
        f"mesh_assets={visible_mesh_assets if visible_mesh_assets else ['None']}"
    )


def _level_geometry_report() -> str:
    actors = _all_actors()
    total = len(actors)
    static_mesh_actors = 0
    actors_with_any_mesh = 0

    for actor in actors:
        has_mesh = False
        for component in actor.get_components_by_class(unreal.ActorComponent):
            class_name = component.get_class().get_name()
            if class_name in MESH_COMPONENT_CLASS_NAMES:
                has_mesh = True
                if class_name == "StaticMeshComponent" and actor.get_class().get_name() == "StaticMeshActor":
                    static_mesh_actors += 1
        if has_mesh:
            actors_with_any_mesh += 1

    return (
        f"total_actors={total} static_mesh_actors={static_mesh_actors} "
        f"actors_with_any_mesh={actors_with_any_mesh}"
    )


def run() -> None:
    unreal.log("=== SignalProject first-playable readiness inspection start ===")
    _load_level()

    unreal.log(f"LEVEL {LEVEL_PATH} :: {_level_geometry_report()}")

    for key, label in ACTOR_LABELS.items():
        actor = _find_actor_by_label(label)
        unreal.log(f"ACTOR {key} {label} :: {_component_report(actor)}")

    for name, bp_path in WIDGET_BLUEPRINTS.items():
        unreal.log(f"WIDGET {name} :: {_widget_tree_report(bp_path)}")

    unreal.log("=== SignalProject first-playable readiness inspection complete ===")


run()
