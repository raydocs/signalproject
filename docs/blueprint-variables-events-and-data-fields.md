# Signal Project 蓝图变量 / 事件 / 数据字段表

> 目标：把核心 Blueprint 在 Unreal 中真正需要建的变量、事件、结构体字段、DataTable 字段提前定义出来。
> 原则：先覆盖 48 小时切片所需，再兼容完整版扩展。

---

## 1. 使用方式

这份文档不是“理论设计”，而是用于：

- 创建 Blueprint 变量
- 定义自定义事件
- 创建 Struct
- 设计 DataTable 列
- 统一 Widget 和 Manager 的输入输出

---

## 2. 核心枚举

## 2.1 E_GamePhase

建议值：

- `Boot`
- `RoomExplore`
- `DesktopIdle`
- `ChatActive`
- `MinigameActive`
- `AnomalyChoice`
- `HandleAnomaly3D`
- `ReportPhase`
- `RevealSequence`
- `EndingSequence`
- `Paused`

## 2.2 E_AnomalyType

- `None`
- `FREEZE`
- `BLACKOUT`
- `DISKCLEAN`

## 2.3 E_ColleagueId

- `ColleagueA`
- `ColleagueB`
- `ColleagueC`
- `Supervisor`

## 2.4 E_SkillUnlockState

- `Locked`
- `AnomalySeen`
- `Unlocked`

## 2.5 E_RouteBranch

- `Neutral`
- `ReportSupervisor`
- `HandleMyself`

## 2.6 E_MinigameType

- `DependencyMatch`
- `WhackABug`
- `BugfixFinder`

## 2.7 E_ReportResult

- `Normal`
- `Detected`
- `Success`

---

## 3. 核心 Struct 设计

## 3.1 ST_ChatMessageRecord

### 字段

- `MessageId` : `Name`
- `SpeakerId` : `E_ColleagueId`
- `DisplayName` : `Text`
- `MessageText` : `Text`
- `TimestampText` : `Text`
- `bIsPlayer` : `Bool`
- `bIsHiddenFragment` : `Bool`
- `bIsSystemMessage` : `Bool`
- `bIsVoiceMessage` : `Bool`
- `VoiceAssetId` : `Name`
- `VisualStyleTag` : `Name`

### 用途

- Widget 渲染单条消息
- 支持正常消息和隐藏消息共用一套数据结构

## 3.2 ST_HiddenOptionRecord

### 字段

- `OptionId` : `Name`
- `TargetColleague` : `E_ColleagueId`
- `Label` : `Text`
- `RequiredAnomaly` : `E_AnomalyType`
- `bConsumed` : `Bool`
- `UnlockDay` : `Int`

## 3.3 ST_ColleagueDefinition

### 字段

- `ColleagueId` : `E_ColleagueId`
- `DisplayName` : `Text`
- `AnomalyType` : `E_AnomalyType`
- `AvatarNormal` : `Texture2D`
- `AvatarCorrupted` : `Texture2D`
- `NormalReplyPoolId` : `Name`
- `HiddenDialogueId` : `Name`
- `DistractionSentencePoolId` : `Name`

## 3.4 ST_DayConfig

### 字段

- `DayIndex` : `Int`
- `DayTitle` : `Text`
- `MorningSupervisorIntroId` : `Name`
- `PrimaryColleague` : `E_ColleagueId`
- `MinigameType` : `E_MinigameType`
- `PrimaryAnomaly` : `E_AnomalyType`
- `bHasBranchChoice` : `Bool`
- `bHasReportPhase` : `Bool`
- `bHasReveal` : `Bool`
- `NextDayIndex` : `Int`

## 3.5 ST_AnomalyConfig

### 字段

- `AnomalyType` : `E_AnomalyType`
- `DisplayName` : `Text`
- `Description` : `Text`
- `ChoicePopupTitle` : `Text`
- `ChoicePopupBody` : `Text`
- `WorldTargetTag` : `Name`
- `OverlayWidgetClass` : `WidgetClass`
- `WorldEffectTag` : `Name`

## 3.6 ST_ReportSentenceOption

### 字段

- `SentenceId` : `Name`
- `SentenceText` : `Text`
- `UnlockRequirement` : `Int`
- `SourceColleague` : `E_ColleagueId`
- `StrengthScore` : `Float`
- `bIsFinalInjection` : `Bool`

## 3.7 ST_EndingBeat

### 字段

- `EndingId` : `Name`
- `TitleText` : `Text`
- `SubtitleText` : `Text`
- `RouteType` : `E_RouteBranch`
- `WidgetStyleTag` : `Name`

---

## 4. 核心 Blueprint 变量表

## 4.1 BP_SignalGameFlowManager

### 变量

- `CurrentDayIndex` : `Int`
- `CurrentPhase` : `E_GamePhase`
- `PreviousPhase` : `E_GamePhase`
- `CurrentMinigameType` : `E_MinigameType`
- `CurrentAnomalyType` : `E_AnomalyType`
- `bIsEndingTriggered` : `Bool`
- `bIsRevealTriggered` : `Bool`
- `DayConfigTable` : `DataTable`
- `CachedHUDManager` : `BP_SignalHUDManager Ref`
- `CachedRouteStateManager` : `BP_RouteStateManager Ref`
- `CachedAnomalyManager` : `BP_AnomalyManager Ref`
- `CachedReportManager` : `BP_ReportManager Ref`

### 关键事件

- `InitializeGameFlow`
- `StartDay(Int DayIndex)`
- `RequestPhaseChange(E_GamePhase NewPhase)`
- `HandlePhaseEntered(E_GamePhase NewPhase)`
- `CompleteCurrentDay`
- `CheckRevealTrigger`
- `CheckEndingTrigger`
- `TriggerEnding(Name EndingId)`

### Dispatcher 建议

- `OnPhaseChanged(E_GamePhase NewPhase)`
- `OnDayChanged(Int NewDay)`
- `OnEndingTriggered(Name EndingId)`

---

## 4.2 BP_RouteStateManager

### 变量

- `ReportSupervisorCount` : `Int`
- `HandleMyselfCount` : `Int`
- `PressureScore` : `Float`
- `ColleagueAState` : `E_SkillUnlockState`
- `ColleagueBState` : `E_SkillUnlockState`
- `ColleagueCState` : `E_SkillUnlockState`
- `UnlockedSkillCount` : `Int`

### 关键事件

- `RecordRouteChoice(E_RouteBranch Branch)`
- `SetColleagueUnlockState(E_ColleagueId ColleagueId, E_SkillUnlockState NewState)`
- `RecalculateUnlockedSkillCount`
- `IsGoodEndingEligible -> Bool`
- `IsBadEndingEligible -> Bool`

### Dispatcher

- `OnRouteStateUpdated`
- `OnSkillUnlocked(E_ColleagueId ColleagueId)`

---

## 4.3 BP_AnomalyManager

### 变量

- `CurrentAnomalyType` : `E_AnomalyType`
- `AccumulatedFreezeWeight` : `Float`
- `AccumulatedBlackoutWeight` : `Float`
- `AccumulatedDiskCleanWeight` : `Float`
- `bAnomalyActive` : `Bool`
- `AnomalyConfigTable` : `DataTable`
- `CachedCurrentAnomalyConfig` : `ST_AnomalyConfig`

### 关键事件

- `ResetAccumulation`
- `AccumulateSideEffect(E_ColleagueId ColleagueId, Float Amount)`
- `ResolveDominantAnomaly`
- `TriggerAnomaly(E_AnomalyType Type)`
- `ResolveCurrentAnomaly`
- `CancelCurrentAnomaly`

### Dispatcher

- `OnAnomalyTriggered(E_AnomalyType Type)`
- `OnAnomalyResolved(E_AnomalyType Type)`

---

## 4.4 BP_ChatConversationManager

### 变量

- `CurrentConversationTarget` : `E_ColleagueId`
- `ConversationHistory` : `Array<ST_ChatMessageRecord>`
- `HiddenOptions` : `Array<ST_HiddenOptionRecord>`
- `ColleagueDefinitionsTable` : `DataTable`
- `NormalRepliesTable` : `DataTable`
- `HiddenDialogueTable` : `DataTable`
- `CachedAnomalyManager` : `BP_AnomalyManager Ref`

### 关键事件

- `InitializeChatSystem`
- `SwitchConversation(E_ColleagueId Target)`
- `SendPlayerMessage(Text MessageText)`
- `AppendReply(E_ColleagueId FromId, Text ReplyText)`
- `InjectHiddenOption(ST_HiddenOptionRecord OptionRecord)`
- `PlayHiddenDialogue(E_ColleagueId Target)`
- `GetConversationHistory(E_ColleagueId Target)`

### Dispatcher

- `OnConversationChanged(E_ColleagueId Target)`
- `OnMessagesUpdated`
- `OnHiddenOptionInjected(E_ColleagueId Target)`

---

## 4.5 BP_MinigameManager

### 变量

- `CurrentMinigameType` : `E_MinigameType`
- `CurrentMinigameWidget` : `UserWidget Ref`
- `bIsMinigameRunning` : `Bool`
- `CurrentScore` : `Int`
- `RemainingTime` : `Float`

### 关键事件

- `StartMinigame(E_MinigameType Type)`
- `PauseMinigame`
- `ResumeMinigame`
- `CompleteMinigame(Bool bSuccess)`
- `AbortMinigameForAnomaly`

### Dispatcher

- `OnMinigameStarted(E_MinigameType Type)`
- `OnMinigameCompleted(Bool bSuccess)`
- `OnMinigameInterrupted`

---

## 4.6 BP_ReportManager

### 变量

- `AvailableSentenceOptions` : `Array<ST_ReportSentenceOption>`
- `SelectedSentenceId` : `Name`
- `SentencePoolTable` : `DataTable`
- `CachedRouteStateManager` : `BP_RouteStateManager Ref`

### 关键事件

- `BuildSentenceOptionsForCurrentDay`
- `SelectSentence(Name SentenceId)`
- `SubmitReport`
- `EvaluateReportResult`

### Dispatcher

- `OnSentenceOptionsBuilt`
- `OnReportSubmitted(E_ReportResult Result)`

---

## 4.7 BP_SignalPlayerController

### 变量

- `CurrentInputModeTag` : `Name`
- `bShowMouseCursorOverride` : `Bool`
- `CachedDesktopRoot` : `WBP_DesktopRoot Ref`
- `CachedInteractionWidget` : `UserWidget Ref`

### 关键事件

- `SetRoomInputMode`
- `SetDesktopInputMode`
- `SetModalInputMode`
- `SetCinematicInputMode`
- `ShowDesktopRoot`
- `HideDesktopRoot`

---

## 4.8 BP_SignalPlayerCharacter

### 变量

- `InteractionTraceDistance` : `Float`
- `CurrentFocusedInteractable` : `Actor Ref`
- `bCanMove` : `Bool`
- `bCanInteract` : `Bool`

### 关键事件

- `PerformInteractionTrace`
- `TryInteract`
- `SetMovementEnabled(Bool bEnabled)`

---

## 4.9 BP_ComputerTerminal

### 变量

- `InteractionText` : `Text`
- `DeskViewAnchor` : `Actor Ref`
- `bIsUsable` : `Bool`

### 关键事件

- `Interact`
- `EnterDesktopMode`
- `ExitDesktopMode`

---

## 4.10 BP_AirConditionerUnit

### 变量

- `InteractionText` : `Text`
- `HandledAnomalyType` : `E_AnomalyType`
- `bIsCurrentlyAvailable` : `Bool`

### 关键事件

- `Interact`
- `PlayRepairFeedback`
- `SetAvailability(Bool bEnabled)`

---

## 5. 核心 Widget 变量与事件

## 5.1 WBP_DesktopRoot

### 变量

- `CurrentOpenAppTag` : `Name`
- `ChatAppWidget` : `WBP_ChatApp Ref`
- `ReportWidget` : `WBP_ReportEditor Ref`
- `NotificationContainer` : `PanelWidget Ref`

### 事件

- `OpenApp(Name AppTag)`
- `CloseCurrentApp`
- `ShowNotification(Text Message)`

## 5.2 WBP_ChatApp

### 变量

- `CurrentConversationTarget` : `E_ColleagueId`
- `MessageListItems` : `Array<ST_ChatMessageRecord>`
- `HiddenOptionItems` : `Array<ST_HiddenOptionRecord>`
- `ConversationManagerRef` : `BP_ChatConversationManager Ref`

### 事件

- `RefreshConversation`
- `HandleSendClicked`
- `HandleContactSelected`
- `HandleHiddenOptionClicked(Name OptionId)`

## 5.3 WBP_AnomalyChoicePopup

### 变量

- `CurrentAnomalyType` : `E_AnomalyType`
- `PopupTitle` : `Text`
- `PopupBody` : `Text`

### 事件

- `SetupForAnomaly(E_AnomalyType Type)`
- `HandleReportSupervisor`
- `HandleHandleMyself`

## 5.4 WBP_ReportEditor

### 变量

- `SentenceOptions` : `Array<ST_ReportSentenceOption>`
- `SelectedSentenceId` : `Name`
- `ReportManagerRef` : `BP_ReportManager Ref`

### 事件

- `BuildOptionList`
- `HandleSentenceSelected(Name SentenceId)`
- `HandleSubmitClicked`

---

## 6. DataTable 建议列

## 6.1 DT_DayConfigs

列建议：

- `DayIndex`
- `DayTitle`
- `MorningSupervisorIntroId`
- `PrimaryColleague`
- `MinigameType`
- `PrimaryAnomaly`
- `bHasBranchChoice`
- `bHasReportPhase`
- `bHasReveal`
- `NextDayIndex`

## 6.2 DT_NormalReplies

列建议：

- `ReplyId`
- `ColleagueId`
- `DayIndex`
- `ReplyText`
- `Weight`

## 6.3 DT_HiddenDialogues

列建议：

- `DialogueId`
- `ColleagueId`
- `SequenceOrder`
- `DialogueText`
- `bGlitchEnter`
- `bGlitchExit`

## 6.4 DT_AnomalyConfigs

列建议：

- `AnomalyType`
- `DisplayName`
- `Description`
- `ChoicePopupTitle`
- `ChoicePopupBody`
- `WorldTargetTag`
- `OverlayWidgetClass`
- `WorldEffectTag`

## 6.5 DT_ReportSentencePools

列建议：

- `SentenceId`
- `SentenceText`
- `UnlockRequirement`
- `SourceColleague`
- `StrengthScore`
- `bIsFinalInjection`

---

## 7. 切片版最小必建字段

如果只做 48 小时版本，先保证这些字段存在：

- `CurrentPhase`
- `CurrentDayIndex`
- `CurrentAnomalyType`
- `ReportSupervisorCount`
- `ColleagueAState`
- `ConversationHistory`
- `HiddenOptions`
- `CurrentMinigameType`
- `SelectedSentenceId`

Struct 至少先建：

- `ST_ChatMessageRecord`
- `ST_HiddenOptionRecord`
- `ST_ReportSentenceOption`
- `ST_AnomalyConfig`

---

## 8. 最终建议

建工程时，不要一开始把所有变量都塞进一个蓝图。

正确拆法是：

- 流程变量放 `BP_SignalGameFlowManager`
- 路线变量放 `BP_RouteStateManager`
- 异常变量放 `BP_AnomalyManager`
- 对话数据放 `BP_ChatConversationManager`
- Widget 只保留显示所需缓存和当前选择

这样后面扩 Day 4-Day 7 的时候才不会炸。
