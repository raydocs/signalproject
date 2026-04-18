# Signal Project 蓝图变量 / 事件 / 数据字段表

> **切片权威说明（Authoritative for Slice Data / Schema）**
> - 本文与 `docs/day1-day3-datatable-ready-script.md` 一起构成 `48 小时竖切片` 的数据契约。
> - **本文负责冻结：** 枚举名、Struct 名、字段名、Blueprint 变量名、DataTable 行结构、消费方映射。
> - **`docs/day1-day3-datatable-ready-script.md` 负责冻结：** 实际导入的 slice 行内容。
> - **`docs/narrative-script-and-system-copy.md` 仅是文案参考 / prose source，不是 schema authority。**
> - 若与完整版或参考性文档冲突，以本文和 `day1-day3-datatable-ready-script.md` 为准。

---

## 1. Slice 数据策略（本次冻结结果）

### 1.1 现在就数据驱动（Data-driven now）

48 小时竖切片中，以下表是 **必须真正创建并消费** 的 DataTable：

- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

### 1.2 本轮延后，不属于 slice DataTable 创建必需项

以下表 **不是** 本轮竖切片必须创建的实现前置：

- `DT_DayConfigs`
- `DT_AnomalyConfigs`

它们可以在完整版阶段再引入；本轮切片不依赖它们完成 Day 1-Day 3 主链。

### 1.3 RowName 规则（必须统一）

为了让 Unreal `DataTable / CSV / JSON` 创建过程可预测，RowName 规则固定为：

- `DT_SupervisorLines`：`RowName = LineId`
- `DT_NormalReplies`：`RowName = ReplyId`
- `DT_HiddenDialogues`：`RowName = DialogueId`
- `DT_SystemCopy`：`RowName = CopyId`
- `DT_ReportSentencePools`：`RowName = SentenceId`
- `DT_EndingSubtitles`：`RowName = <EndingId>_<SequenceOrder as 2-digit zero-padded int>`（例如 `GoodEnding_01`、`GoodEnding_02`）

> `RowName` 是 Unreal 导入层规则；下文列出的字段是 Struct 字段。不要再额外发明别名列。

---

## 2. 核心枚举（Canonical Enum Names）

## 2.1 `E_GamePhase`

切片运行中真正会进入的 phase：

- `Boot`
- `RoomExplore`
- `DesktopIdle`
- `ChatActive`
- `MinigameActive`
- `AnomalyChoice`
- `HandleAnomaly3D`
- `ReportPhase`
- `EndingSequence`

允许为未来保留但 **不是本轮切片运行态**：

- `RevealSequence`
- `Paused`

## 2.2 `E_AnomalyType`

- `None`
- `FREEZE`
- `BLACKOUT`
- `DISKCLEAN`

切片实际执行的只有：

- `None`
- `FREEZE`

`BLACKOUT` / `DISKCLEAN` 仅保留未来占位值，不参与本轮 DataTable 消费链。

## 2.3 `E_ColleagueId`

- `ColleagueA`
- `ColleagueB`
- `ColleagueC`
- `Supervisor`

## 2.4 `E_SkillUnlockState`

- `Locked`
- `AnomalySeen`
- `Unlocked`

## 2.5 `E_RouteBranch`

- `Neutral`
- `ReportSupervisor`
- `HandleMyself`

## 2.6 `E_MinigameType`

- `DependencyMatch`
- `WhackABug`
- `BugfixFinder`

切片实际执行的只有：

- `DependencyMatch`

## 2.7 `E_ReportResult`

- `Normal`
- `Detected`
- `Success`

> 这是 **future-reserved report evaluation enum**，不属于本轮 slice 最小必建项。
> 本轮切片里，`WBP_ReportEditor` 只负责提交；真正的结尾判定仍由 `BP_SignalGameFlowManager` / `BP_RouteStateManager` 控制。

---

## 3. 运行期 Struct（Runtime Structs）

这些 Struct 用于运行时缓存，不等同于 DataTable 行结构。

## 3.1 `ST_ChatMessageRecord`

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

## 3.2 `ST_HiddenOptionRecord`

### 字段

- `OptionId` : `Name`
- `TargetColleague` : `E_ColleagueId`
- `Label` : `Text`
- `RequiredAnomaly` : `E_AnomalyType`
- `bConsumed` : `Bool`
- `UnlockDay` : `Int`

---

## 4. Slice DataTable 行 Struct（Authoritative Row Structs）

> **字段名冻结规则：** 所有文本列统一使用 `Text`。不再使用 `ReplyText` / `DialogueText` / `SentenceText` 这些别名。

## 4.1 `ST_SupervisorLineRow`

对应表：`DT_SupervisorLines`

### 字段

- `LineId` : `Name`
- `DayIndex` : `Int`
- `UseCase` : `Name`
- `SpeakerId` : `E_ColleagueId`
- `Text` : `Text`
- `Weight` : `Int`

### `UseCase` 允许值

- `MorningIntro`
- `ChatThread`
- `AnomalyResponse`
- `ReportAck`

## 4.2 `ST_NormalReplyRow`

对应表：`DT_NormalReplies`

### 字段

- `ReplyId` : `Name`
- `DayIndex` : `Int`
- `ColleagueId` : `E_ColleagueId`
- `Text` : `Text`
- `Weight` : `Int`

## 4.3 `ST_HiddenDialogueRow`

对应表：`DT_HiddenDialogues`

### 字段

- `DialogueId` : `Name`
- `DayIndex` : `Int`
- `ColleagueId` : `E_ColleagueId`
- `SequenceOrder` : `Int`
- `Text` : `Text`
- `bGlitchEnter` : `Bool`
- `bGlitchExit` : `Bool`

## 4.4 `ST_SystemCopyRow`

对应表：`DT_SystemCopy`

### 字段

- `CopyId` : `Name`
- `Category` : `Name`
- `SubType` : `Name`
- `Text` : `Text`

### `Category` 当前切片允许值

- `Boot`
- `Interaction`
- `Objective`
- `System`
- `Anomaly`
- `HiddenOption`

## 4.5 `ST_ReportSentenceRow`

对应表：`DT_ReportSentencePools`

### 字段

- `SentenceId` : `Name`
- `DayIndex` : `Int`
- `SourceColleague` : `E_ColleagueId`
- `UnlockRequirement` : `Int`
- `StrengthScore` : `Float`
- `bIsFinalInjection` : `Bool`
- `Text` : `Text`

### `UnlockRequirement` 约定

- `0` = 默认可见
- `1` = 需要 `bHasConsumedColleagueAHiddenOption == true`（即 Colleague A 隐藏选项已消费、隐藏对话已完成）后才可见

## 4.6 `ST_EndingSubtitleRow`

对应表：`DT_EndingSubtitles`

### 字段

- `EndingId` : `Name`
- `SequenceOrder` : `Int`
- `Text` : `Text`

### `EndingId` 当前切片允许值

- `BadEnding`
- `GoodEnding`

---

## 5. DataTable 消费方映射（Consumer Mapping）

| DataTable | 权威消费方 | 切片中的用途 |
|---|---|---|
| `DT_SystemCopy` | `BP_SignalPlayerController` / `BP_ComputerTerminal` / `BP_AirConditionerUnit` / `BP_MinigameManager` / `WBP_AnomalyChoicePopup` | Boot 文案、交互提示、目标提示、系统提示、`FREEZE` 标题/正文/按钮文案 |
| `DT_SupervisorLines` | `BP_ChatConversationManager` | 主管开场、主管线程、异常汇报回应、日报确认 |
| `DT_NormalReplies` | `BP_ChatConversationManager` | 普通同事回复池 |
| `DT_HiddenDialogues` | `BP_ChatConversationManager` | 隐藏对话序列 |
| `DT_ReportSentencePools` | `WBP_ReportEditor` | 生成日报句子选项 |
| `DT_EndingSubtitles` | `WBP_EndingTitleCard` | 按 `EndingId` + `SequenceOrder` 播放简化结尾字幕 |

### 5.1 消费边界补充

- `BP_SignalGameFlowManager` 提供 `CurrentDayIndex`、路线状态和 phase gate，但 **不是** 这些表的 schema owner。
- `BP_RouteStateManager` 只提供 `HandleMyself / ReportSupervisor` 的结果与隐藏线状态，不直接读取文案表。
- `BP_AnomalyManager` 负责异常状态，不负责定义或持有文案 schema。
- `WBP_ReportEditor` 读取 `DT_ReportSentencePools`，但 **不** 决定 `Good / Bad` 结局。
- `WBP_EndingTitleCard` 读取 `DT_EndingSubtitles` 做显示，但 **不** 判定路线结果。

---

## 6. 关键 Blueprint 变量（Slice-only）

## 6.1 `BP_SignalGameFlowManager`

### 权威变量

- `CurrentDayIndex` : `Int`
- `CurrentPhase` : `E_GamePhase`
- `PreviousPhase` : `E_GamePhase`
- `RouteStateManagerRef` : `BP_RouteStateManager Ref`
- `AnomalyManagerRef` : `BP_AnomalyManager Ref`
- `ChatConversationManagerRef` : `BP_ChatConversationManager Ref`
- `MinigameManagerRef` : `BP_MinigameManager Ref`
- `HiddenDialogueUnlockerRef` : `BP_HiddenDialogueUnlocker Ref`
- `CachedPlayerController` : `BP_SignalPlayerController Ref`

### 权威函数

- `InitializeGameFlow`
- `StartDay(Int DayIndex)`
- `RequestPhaseChange(E_GamePhase NewPhase)`
- `HandlePhaseEntered(E_GamePhase NewPhase)`
- `CanEnterDesktop() -> Bool`
- `CanOpenReport() -> Bool`
- `SubmitSliceReport(Name SelectedSentenceId)`
- `TriggerEnding(Name EndingId)`

### 明确不属于本轮 slice

- `CurrentAnomalyType`（属于 `BP_AnomalyManager`）
- `DayConfigTable`
- `CachedHUDManager`
- `CachedReportManager`

## 6.2 `BP_RouteStateManager`

### 权威变量

- `ReportSupervisorCount` : `Int`
- `HandleMyselfCount` : `Int`
- `LastRouteChoice` : `E_RouteBranch`
- `ColleagueAState` : `E_SkillUnlockState`
- `bHasConsumedColleagueAHiddenOption` : `Bool`

## 6.3 `BP_AnomalyManager`

### 权威变量

- `CurrentAnomalyType` : `E_AnomalyType`
- `AccumulatedFreezeWeight` : `Float`
- `FreezeThreshold` : `Float`
- `bAnomalyActive` : `Bool`

### 切片约束

- 不要求 `AccumulatedBlackoutWeight`
- 不要求 `AccumulatedDiskCleanWeight`
- 不要求 `AnomalyConfigTable`

## 6.4 `BP_ChatConversationManager`

### 权威变量

- `CurrentConversationTarget` : `E_ColleagueId`
- `ConversationHistory` : `Array<ST_ChatMessageRecord>`
- `HiddenOptions` : `Array<ST_HiddenOptionRecord>`
- `SupervisorLinesTable` : `DataTable`
- `NormalRepliesTable` : `DataTable`
- `HiddenDialoguesTable` : `DataTable`
- `RouteStateManagerRef` : `BP_RouteStateManager Ref`
- `AnomalyManagerRef` : `BP_AnomalyManager Ref`

### 关键函数

- `InitializeChatSystem`
- `SwitchConversation(E_ColleagueId Target)`
- `SendPlayerMessage(Text MessageText)`
- `AppendReply(E_ColleagueId FromId, Text ReplyText)`
- `InjectHiddenOption(ST_HiddenOptionRecord OptionRecord)`
- `PlayHiddenDialogue(E_ColleagueId Target)`

## 6.5 `BP_MinigameManager`

### 权威变量

- `CurrentMinigameType` : `E_MinigameType`
- `CurrentMinigameWidget` : `UserWidget Ref`
- `ChoicePopupRef` : `WBP_AnomalyChoicePopup Ref`
- `bIsMinigameRunning` : `Bool`

## 6.6 `BP_SignalPlayerController`

### 权威变量

- `CachedDesktopRoot` : `WBP_DesktopRoot Ref`
- `CachedEndingTitleCard` : `WBP_EndingTitleCard Ref`
- `bIsInDesktopMode` : `Bool`
- `GameFlowManagerRef` : `BP_SignalGameFlowManager Ref`

## 6.7 `WBP_DesktopRoot`

### 权威变量

- `ChatAppWidget` : `WBP_ChatApp Ref`
- `ReportWidget` : `WBP_ReportEditor Ref`
- `CurrentAppWidget` : `UserWidget Ref`
- `GameFlowManagerRef` : `BP_SignalGameFlowManager Ref`
- `RouteStateManagerRef` : `BP_RouteStateManager Ref`
- `ChatConversationManagerRef` : `BP_ChatConversationManager Ref`
- `MinigameManagerRef` : `BP_MinigameManager Ref`

## 6.8 `WBP_ReportEditor`

### 权威变量

- `ReportSentencePoolsTable` : `DataTable`
- `DisplayedSentenceOptions` : `Array<ST_ReportSentenceRow>`
- `SelectedSentenceId` : `Name`
- `GameFlowManagerRef` : `BP_SignalGameFlowManager Ref`
- `RouteStateManagerRef` : `BP_RouteStateManager Ref`

### 明确移除

- `ReportManagerRef`

---

## 7. 切片最小必建项（Deterministic Build Minimum）

如果现在就开始创建枚举、Struct、DataTable 和 Blueprint 变量，最小必建集固定为：

### 枚举

- `E_GamePhase`
- `E_AnomalyType`
- `E_ColleagueId`
- `E_SkillUnlockState`
- `E_RouteBranch`
- `E_MinigameType`

### Struct

- `ST_ChatMessageRecord`
- `ST_HiddenOptionRecord`
- `ST_SupervisorLineRow`
- `ST_NormalReplyRow`
- `ST_HiddenDialogueRow`
- `ST_SystemCopyRow`
- `ST_ReportSentenceRow`
- `ST_EndingSubtitleRow`

### DataTable

- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

---

## 8. 最终冻结结论

本轮 Work Item 3 后，竖切片的数据契约固定为：

- **字段名统一：** 文本列统一叫 `Text`
- **表范围统一：** 只强制创建 6 张 slice DataTable
- **消费方统一：** Chat / Report / Ending 各自只消费自己的表
- **权威分工统一：**
  - schema / type / field / consumer mapping 看本文
  - import-ready rows 看 `docs/day1-day3-datatable-ready-script.md`
  - prose / 文案灵感 / 完整版参考看 `docs/narrative-script-and-system-copy.md`

只要按本文与 DataTable 导入稿建表，DataTable / Struct / Widget 消费链就是确定的。
