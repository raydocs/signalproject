# Signal Project 18 个核心 Blueprint 变量面板配置清单

> 目标：把切片版 18 个核心 Blueprint / Widget 在 Unreal 里需要创建的关键变量，按 `变量名 / 类型 / 默认值 / 是否实例可编辑 / 备注` 的方式整理出来。
> 适用：程序主搭蓝图时直接照着建变量；策划可据此核对哪些字段应该暴露给内容配置。

---

## 1. 使用说明

字段说明：

- `变量名`：建议直接使用的 Blueprint 变量名
- `类型`：Unreal 中建议类型
- `默认值`：初始默认值建议
- `实例可编辑`：是否勾选 `Instance Editable`
- `暴露在生成时`：是否勾选 `Expose on Spawn`
- `备注`：为什么要有这个变量

推荐规则：

- 会被关卡里逐个 Actor 调整的，用 `实例可编辑`
- 只在运行时缓存引用的，不要暴露
- Widget 内只保留显示和当前状态相关变量，不保存全局流程状态

---

## 2. 团队分工建议

### 程序

- 按这份清单建变量
- 统一命名和类型
- 决定哪些变量实例可编辑

### 策划

- 核对文案、状态、条件相关变量是否够用
- 指出哪些字段未来要给 DataTable 驱动

### 美术 1

- 关注 Widget 里与视觉样式挂钩的变量

### 美术 2

- 关注世界交互物中需要在场景里配置的变量

---

## 3. 核心对象变量清单

## 3.1 BP_SignalGameMode

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `bUseDesktopFlow` | Bool | true | false | false | 切片阶段可固定为 true |

说明：

- `GameMode` 尽量轻，不要在这里堆业务变量

---

## 3.2 BP_SignalPlayerController

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CachedDesktopRoot` | WBP_DesktopRoot Object Reference | None | false | false | 桌面根 Widget 缓存 |
| `CachedEndingCard` | WBP_EndingTitleCard Object Reference | None | false | false | 结尾卡缓存 |
| `bIsInDesktopMode` | Bool | false | false | false | 当前是否在桌面模式 |
| `bDesktopRootAdded` | Bool | false | false | false | 防止重复 AddToViewport |
| `DefaultDesktopWidgetClass` | Class Reference: WBP_DesktopRoot | WBP_DesktopRoot | false | false | 桌面根类 |
| `DefaultEndingWidgetClass` | Class Reference: WBP_EndingTitleCard | WBP_EndingTitleCard | false | false | 结尾卡类 |
| `CurrentInputModeTag` | Name | `Room` | false | false | 调试用 |

说明：

- 这里的 Widget 引用都不需要实例可编辑
- `CurrentInputModeTag` 对调试很有帮助

---

## 3.3 BP_SignalPlayerCharacter

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `InteractionTraceDistance` | Float | 220.0 | false | false | 交互射线长度 |
| `CurrentFocusedInteractable` | Actor Reference | None | false | false | 当前瞄准的交互物 |
| `bCanMove` | Bool | true | false | false | 是否允许移动 |
| `bCanInteract` | Bool | true | false | false | 是否允许交互 |
| `InteractionPromptText` | Text | Empty | false | false | 当前交互提示文本 |

说明：

- 第一版不需要把角色做得太复杂
- `InteractionPromptText` 方便先走 `PrintString`，以后再接 HUD

---

## 3.4 BP_SignalGameFlowManager

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentDayIndex` | Integer | 1 | false | false | 当前天数 |
| `CurrentPhase` | E_GamePhase | RoomExplore | false | false | 当前主状态 |
| `PreviousPhase` | E_GamePhase | Boot | false | false | 上一状态 |
| `CurrentAnomalyType` | E_AnomalyType | None | false | false | 当前异常 |
| `bIsEndingTriggered` | Bool | false | false | false | 防止重复触发结局 |
| `bIsRevealTriggered` | Bool | false | false | false | 完整版预留 |
| `CachedPlayerController` | BP_SignalPlayerController Reference | None | false | false | 性能与方便用 |
| `CachedRouteStateManager` | BP_RouteStateManager Reference | None | true | false | 建议关卡里手动绑或 BeginPlay 找 |
| `CachedAnomalyManager` | BP_AnomalyManager Reference | None | true | false | 同上 |
| `CachedMinigameManager` | BP_MinigameManager Reference | None | true | false | 同上 |
| `CachedChatManager` | BP_ChatConversationManager Reference | None | true | false | 同上 |
| `CurrentEndingTag` | Name | None | false | false | 当前结尾类型 |

说明：

- 这几个 `CachedXxxManager` 在切片里可以直接 `实例可编辑`，减少自动查找复杂度

---

## 3.5 BP_RouteStateManager

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `ReportSupervisorCount` | Integer | 0 | false | false | 汇报次数 |
| `HandleMyselfCount` | Integer | 0 | false | false | 自己处理次数 |
| `PressureScore` | Float | 0.0 | false | false | 完整版压力值 |
| `ColleagueAState` | E_SkillUnlockState | Locked | false | false | A 当前状态 |
| `ColleagueBState` | E_SkillUnlockState | Locked | false | false | B 当前状态 |
| `ColleagueCState` | E_SkillUnlockState | Locked | false | false | C 当前状态 |
| `UnlockedSkillCount` | Integer | 0 | false | false | 解锁技能数 |
| `LastRouteChoice` | E_RouteBranch | Neutral | false | false | 最近一次路线选择 |

说明：

- 切片版真正关键的是 `ReportSupervisorCount`、`HandleMyselfCount`、`ColleagueAState`

---

## 3.6 BP_AnomalyManager

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentAnomalyType` | E_AnomalyType | None | false | false | 当前异常类型 |
| `AccumulatedFreezeWeight` | Float | 0.0 | false | false | FREEZE 累积值 |
| `AccumulatedBlackoutWeight` | Float | 0.0 | false | false | 完整版预留 |
| `AccumulatedDiskCleanWeight` | Float | 0.0 | false | false | 完整版预留 |
| `FreezeThreshold` | Float | 2.0 | true | false | FREEZE 触发阈值 |
| `bAnomalyActive` | Bool | false | false | false | 是否已有异常 |
| `CurrentChoicePopup` | WBP_AnomalyChoicePopup Reference | None | false | false | 当前弹窗引用 |

说明：

- `FreezeThreshold` 建议实例可编辑，方便现场调试触发节奏

---

## 3.7 BP_ComputerTerminal

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `InteractionText` | Text | `[E] Use Computer` | true | false | 提示文本 |
| `bIsUsable` | Bool | true | true | false | 当前是否可用 |
| `DeskViewAnchor` | Actor Reference | None | true | false | 桌面视角锚点 |
| `MonitorMesh` | StaticMeshComponent Ref | None | false | false | 可选，方便后面做材质变化 |

说明：

- `DeskViewAnchor` 必须实例可编辑

---

## 3.8 BP_AirConditionerUnit

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `InteractionText` | Text | `[E] Adjust Air Conditioner` | true | false | 提示文本 |
| `HandledAnomalyType` | E_AnomalyType | FREEZE | true | false | 负责处理的异常 |
| `bIsCurrentlyAvailable` | Bool | false | false | false | 当前是否可交互 |
| `RepairSuccessText` | Text | `Temperature stabilizing...` | true | false | 修复反馈文案 |

说明：

- 只要切片版，`HandledAnomalyType` 固定 `FREEZE`

---

## 3.9 WBP_DesktopRoot

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `bInitialized` | Bool | false | false | false | 防止重复绑定 |
| `CurrentOpenAppTag` | Name | None | false | false | 当前打开的 App |
| `ChatAppWidget` | WBP_ChatApp Reference | None | false | false | 聊天 Widget 缓存 |
| `ReportWidget` | WBP_ReportEditor Reference | None | false | false | 日报 Widget 缓存 |
| `ContentPanel` | PanelWidget Reference | None | false | false | 内容容器 |
| `NotificationPanel` | PanelWidget Reference | None | false | false | 提示区 |
| `DesktopTitleText` | Text | `SIGNAL REMOTE WORK OS` | true | false | 桌面顶层标题 |

说明：

- `ContentPanel` 这类变量通常是 `Is Variable` 的 Widget 引用，不必额外实例可编辑

---

## 3.10 WBP_ChatApp

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `ConversationManagerRef` | BP_ChatConversationManager Reference | None | false | true | 创建时可直接传入 |
| `CurrentConversationTarget` | E_ColleagueId | ColleagueA | false | false | 当前联系人 |
| `MessageListPanel` | PanelWidget Reference | None | false | false | 消息列表容器 |
| `ContactAButton` | Button Reference | None | false | false | 联系人按钮 |
| `ContactBButton` | Button Reference | None | false | false | 联系人按钮 |
| `ContactCButton` | Button Reference | None | false | false | 联系人按钮 |
| `SupervisorButton` | Button Reference | None | false | false | 联系人按钮 |
| `SendButton` | Button Reference | None | false | false | 发送按钮 |
| `PresetSendText` | Text | `这个我先过一遍。` | true | false | 切片版默认发送句 |

说明：

- `ConversationManagerRef` 很适合 `Expose on Spawn`

---

## 3.11 BP_ChatConversationManager

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentConversationTarget` | E_ColleagueId | ColleagueA | false | false | 当前线程 |
| `ConversationHistory` | Array of ST_ChatMessageRecord | Empty | false | false | 聊天历史 |
| `HiddenOptions` | Array of ST_HiddenOptionRecord | Empty | false | false | 隐藏选项数据 |
| `CachedAnomalyManager` | BP_AnomalyManager Reference | None | true | false | 累积异常用 |
| `CachedRouteStateManager` | BP_RouteStateManager Reference | None | true | false | 解锁状态更新 |
| `DefaultPlayerDisplayName` | Text | `你` | true | false | 玩家显示名 |
| `InitialConversationSeedLoaded` | Bool | false | false | false | 防止重复灌初始消息 |

说明：

- 切片版先不拆多线程 Map，直接单数组也行

---

## 3.12 BP_MinigameManager

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentMinigameType` | E_MinigameType | DependencyMatch | false | false | 当前小游戏 |
| `CurrentMinigameWidget` | WBP_MG_DependencyMatch Reference | None | false | false | 当前 Widget |
| `bIsMinigameRunning` | Bool | false | false | false | 当前运行状态 |
| `CachedAnomalyManager` | BP_AnomalyManager Reference | None | true | false | 触发异常用 |
| `CachedGameFlowManager` | BP_SignalGameFlowManager Reference | None | true | false | 状态切换用 |
| `DefaultDependencyMatchClass` | Class Reference: WBP_MG_DependencyMatch | WBP_MG_DependencyMatch | false | false | 默认小游戏类 |

说明：

- 切片阶段这个 Manager 只需要服务一个小游戏

---

## 3.13 WBP_MG_DependencyMatch

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `GridRows` | Integer | 4 | true | false | 网格行数 |
| `GridColumns` | Integer | 4 | true | false | 网格列数 |
| `bPaused` | Bool | false | false | false | 是否暂停 |
| `SelectedTileIds` | Array of Int | Empty | false | false | 当前选中图块 |
| `MatchedTileIds` | Array of Int | Empty | false | false | 已匹配图块 |
| `CurrentScore` | Integer | 0 | false | false | 当前分数 |
| `RemainingTime` | Float | 60.0 | true | false | 倒计时 |
| `bFreezeVisualActive` | Bool | false | false | false | FREEZE 视觉态 |
| `MinigameManagerRef` | BP_MinigameManager Reference | None | false | true | 创建时传入 |

说明：

- 美术可以后面根据 `bFreezeVisualActive` 挂 Frost overlay

---

## 3.14 WBP_AnomalyChoicePopup

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentAnomalyType` | E_AnomalyType | None | false | true | 弹窗初始化用 |
| `PopupTitle` | Text | Empty | false | false | 标题 |
| `PopupBody` | Text | Empty | false | false | 正文 |
| `ReportButton` | Button Reference | None | false | false | 汇报按钮 |
| `HandleButton` | Button Reference | None | false | false | 自己处理按钮 |
| `CachedRouteStateManager` | BP_RouteStateManager Reference | None | true | false | 记录路线 |
| `CachedAnomalyManager` | BP_AnomalyManager Reference | None | true | false | 清异常 |
| `CachedGameFlowManager` | BP_SignalGameFlowManager Reference | None | true | false | 切阶段 |

说明：

- 如果要减少查找时间，三个 Manager 都可实例手绑

---

## 3.15 BP_HiddenDialogueUnlocker

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CachedChatConversationManager` | BP_ChatConversationManager Reference | None | true | false | 注入隐藏选项 |
| `CachedRouteStateManager` | BP_RouteStateManager Reference | None | true | false | 更新解锁状态 |
| `AHiddenOptionLabel` | Text | `关于那个空调……` | true | false | A 线隐藏选项文案 |

说明：

- 切片版只先写 A 的隐藏选项字段即可

---

## 3.16 WBP_ReportEditor

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `SentenceOptions` | Array of ST_ReportSentenceOption | Empty | false | false | 可选句 |
| `SelectedSentenceId` | Name | None | false | false | 当前选中的句子 |
| `OptionsPanel` | PanelWidget Reference | None | false | false | 句子列表容器 |
| `SubmitButton` | Button Reference | None | false | false | 提交按钮 |
| `CachedRouteStateManager` | BP_RouteStateManager Reference | None | true | false | 判断结尾 |
| `CachedGameFlowManager` | BP_SignalGameFlowManager Reference | None | true | false | 触发结尾 |

说明：

- `SentenceOptions` 可先在 Construct 里硬编码生成

---

## 3.17 WBP_EndingTitleCard

| 变量名 | 类型 | 默认值 | 实例可编辑 | 暴露在生成时 | 备注 |
| --- | --- | --- | --- | --- | --- |
| `CurrentEndingTag` | Name | None | false | true | 创建时传入更方便 |
| `TitleText` | Text | Empty | false | false | 标题文本 |
| `SubtitleLines` | Array of Text | Empty | false | false | 结尾字幕 |
| `RestartButton` | Button Reference | None | false | false | 重开按钮 |
| `QuitButton` | Button Reference | None | false | false | 退出按钮 |

说明：

- 切片版可只做 Restart

---

## 3.18 BPI_Interactable

接口本身没有变量，但建议所有实现者都具备这些字段：

- `InteractionText : Text`
- `bIsUsable : Bool`

这样角色检测逻辑更统一。

---

## 4. 程序优先创建的变量顺序

如果只有 1 个程序，建议先只建这些最小字段：

1. `bIsInDesktopMode`
2. `CachedDesktopRoot`
3. `InteractionTraceDistance`
4. `CurrentFocusedInteractable`
5. `CurrentDayIndex`
6. `CurrentPhase`
7. `CurrentAnomalyType`
8. `AccumulatedFreezeWeight`
9. `FreezeThreshold`
10. `ReportSupervisorCount`
11. `HandleMyselfCount`
12. `ColleagueAState`
13. `ConversationHistory`
14. `HiddenOptions`
15. `CurrentConversationTarget`
16. `CurrentMinigameWidget`
17. `bIsMinigameRunning`
18. `SelectedSentenceId`

---

## 5. 策划优先核对的字段

策划重点看这些是否够配内容：

- `CurrentDayIndex`
- `CurrentPhase`
- `ColleagueAState`
- `ConversationHistory`
- `HiddenOptions`
- `SentenceOptions`
- `CurrentEndingTag`

---

## 6. 美术优先关心的字段

### 美术 1：UI

- `CurrentOpenAppTag`
- `PopupTitle`
- `PopupBody`
- `DesktopTitleText`
- `TitleText`
- `SubtitleLines`
- `bFreezeVisualActive`

### 美术 2：场景 / 物件

- `InteractionText`
- `DeskViewAnchor`
- `bIsUsable`
- `bIsCurrentlyAvailable`
- `RepairSuccessText`

---

## 7. 切片版变量面板完成标准

当以下条件满足时，变量配置算合格：

- 程序不需要临时瞎加字段才能继续往下连线
- 策划能看懂哪些状态影响流程
- 美术知道哪些字段和视觉表现挂钩
- 关键 Actor 都能在关卡里实例化并配置
