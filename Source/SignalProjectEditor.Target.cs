using UnrealBuildTool;
using System.Collections.Generic;

public class SignalProjectEditorTarget : TargetRules
{
    public SignalProjectEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.Add("SignalProject");
    }
}
