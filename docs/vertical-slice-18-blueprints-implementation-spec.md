# Signal Project 48 小时切片 18 个 Blueprint 逐个实现说明

> **切片权威说明（Authoritative for Slice Runtime Ownership）**
> - 本文是 `18 个切片对象` 的落地实现说明。
> - 它必须服从 `docs/vertical-slice-scope.md`、`docs/game-state-machine.md`、`docs/interaction-spec.md` 的切片契约，以及 `docs/blueprint-responsibility-map.md` 的 ownership 约束。
> - 若与完整版对象设计冲突，以当前切片版本为准。

---

## 1. 切片版 18 个核心对象

1. `BP_SignalGameMode`
2. `BP_SignalPlayerController`
3. `BP_SignalPlayerCharacter`
4. `BP_SignalGameFlowManager`
5. `BP_RouteStateManager`
6. `BP_AnomalyManager`
7. `BP_ComputerTerminal`
8. `BP_AirConditionerUnit`
9. `WBP_DesktopRoot`
10. `WBP_ChatApp`
11. `BP_ChatConversationManager`
12. `BP_MinigameManager`
13. `WBP_MG_DependencyMatch`
14. `WBP_AnomalyChoicePopup`
15. `BP_HiddenDialogueUnlocker`
16. `WBP_ReportEditor`
17. `WBP_EndingTitleCard`
18. `BPI_Interactable`

> 本轮切片 **不引入** `BP_ReportManager`、`BP_AnomalyChoiceDirector`、`BP_SignalHUDManager` 等完整版对象作为实现依赖。

---

## 2. 切片实现总规则

### 2.1 放置与 ownership 规则

- `BP_SignalGameFlowManager`、`BP_RouteStateManager`、`BP_AnomalyManager`、`BP_ChatConversationManager`、`BP_MinigameManager`、`BP_HiddenDialogueUnlocker`、`BP_ComputerTerminal`、`BP_AirConditionerUnit` 都是 **level-placed actor**。
- `WBP_DesktopRoot`、`WBP_EndingTitleCard` 由 `BP_SignalPlayerController` 创建和持有。
- `WBP_ChatApp`、`WBP_ReportEditor`、当前小游戏 widget 由 `WBP_DesktopRoot` 承载显示。
- `WBP_AnomalyChoicePopup` 的生命周期由 `BP_MinigameManager` 管理。

### 2.2 引用规则

- level actor 间核心引用：`Instance Editable` + 关卡手动赋值。
- `BP_SignalPlayerController` 的 manager refs 由 `BP_SignalGameFlowManager.InitializeGameFlow` 在运行时注册。
- widget refs 必须由创建者以 `Expose on Spawn` 在 `Create Widget` 节点上传入。
- `GetAllActorsOfClass` 只作临时调试，不作为规范实现。

### 2.3 Phase authority 规则

- 只有 `BP_SignalGameFlowManager` 拥有并修改 `CurrentPhase`。
- 任何 widget / world actor 都不能直接 `Set CurrentPhase`。
- 所有 phase 切换只能通过 `RequestPhaseChange(NewPhase)`。
- 若 `NewPhase == CurrentPhase`，则 `RequestPhaseChange` 必须 `No-op`。

---

## 3. 逐个实现说明

## 3.1 `BP_SignalGameMode`

### 作用

- 指定默认 Pawn 和 PlayerController
- 提供关卡启动入口

### 必做设置

- `Default Pawn Class = BP_SignalPlayerCharacter`
- `Player Controller Class = BP_SignalPlayerController`

### 不要做

- 不写切片流程逻辑
- 不缓存 manager refs

### 完成判定

- 进入主关卡后正确生成 Pawn / Controller

---

## 3.2 `BP_SignalPlayerController`

### ownership

- **PlayerController-owned widgets 的唯一持有者**

### 负责

- 输入模式切换
- 鼠标显示/隐藏
- `WBP_DesktopRoot` 创建、缓存、显示、隐藏
- `WBP_EndingTitleCard` 创建、缓存、显示
- 角色输入启停

### 必做变量

- `CachedDesktopRoot`
- `CachedEndingTitleCard`
- `bIsInDesktopMode`
- `GameFlowManagerRef`
- `RouteStateManagerRef`
- `AnomalyManagerRef`
- `ChatConversationManagerRef`
- `MinigameManagerRef`

### 必做函数

- `RegisterSliceRefs(...)`
- `SetRoomInputMode`
- `SetDesktopInputMode`
- `ShowDesktopRoot`
- `HideDesktopRoot`
- `ShowEndingTitleCard(EndingType)`
- `RegisterDesktopRootToMinigameManager`

### 切片实现要求

- `ShowDesktopRoot` 首次创建 `WBP_DesktopRoot` 时，必须用 `Expose on Spawn` 传入它的必需 refs
- 创建或重新显示 `WBP_DesktopRoot` 后，必须调用 `RegisterDesktopRootToMinigameManager`，把当前 `DesktopRoot` 注册给 `BP_MinigameManager`
- `Leave Desktop / Esc` 只在 `DesktopIdle` / `ChatActive` 生效
- 不得由 Controller 自己决定进入 `ReportPhase` / `EndingSequence`

### 完成判定

- 桌面与房间输入切换稳定
- 桌面根和结尾卡的所有权清晰且不重复创建

---

## 3.3 `BP_SignalPlayerCharacter`

### 作用

- 房间内移动
- 做交互射线检测

### 必做变量

- `InteractionTraceDistance`
- `CurrentFocusedInteractable`
- `bCanInteract`

### 必做函数

- `PerformInteractionTrace`
- `TryInteract`
- `SetMovementEnabled`

### 切片实现要求

- 只通过 `BPI_Interactable` 调用交互
- 不直接改 phase

### 完成判定

- 玩家能稳定交互电脑与空调

---

## 3.4 `BP_SignalGameFlowManager`

### ownership

- **`CurrentPhase` 的唯一 owner**

### 负责

- 切片 Day 1-Day 3 流程
- `CurrentPhase`
- `CurrentDayIndex`
- phase 进入条件与退出协调
- 报告提交后的结尾触发
- 把 manager refs 注册给 PlayerController

### 必做变量

- `CurrentDayIndex`
- `CurrentPhase`
- `PreviousPhase`
- `RouteStateManagerRef`
- `AnomalyManagerRef`
- `ChatConversationManagerRef`
- `MinigameManagerRef`
- `HiddenDialogueUnlockerRef`
- `CachedPlayerController`

### 必做函数

- `InitializeGameFlow`
- `StartDay`
- `RequestPhaseChange`
- `HandlePhaseEntered`
- `CanEnterDesktop()`
- `CanOpenReport()`
- `SubmitSliceReport(SelectedSentenceId)`
- `TriggerEnding(EndingType)`

### 切片实现要求

- 只支持切片 phase：`Boot / RoomExplore / DesktopIdle / ChatActive / MinigameActive / AnomalyChoice / HandleAnomaly3D / ReportPhase / EndingSequence`
- `RequestPhaseChange` 若目标 phase 等于当前 phase，则直接返回
- `HandleMyself` 路线下，`CanOpenReport()` 在隐藏对话完成前必须返回 `false`
- `CanEnterDesktop()` 是电脑是否可进入桌面的唯一 helper 来源
- `TriggerEnding(EndingType)` 必须先缓存结果，再通过 `RequestPhaseChange(EndingSequence)` 进入结尾
- `WBP_EndingTitleCard` 由 `HandlePhaseEntered(EndingSequence)` 间接交给 PlayerController 显示

### 不要做

- 不存 `CurrentAnomalyType`
- 不直接写聊天历史
- 不直接构建 DesktopRoot 子控件

### 完成判定

- 所有 phase 切换都能追溯到 `RequestPhaseChange`

---

## 3.5 `BP_RouteStateManager`

### ownership

- **路线结果与隐藏线状态 owner**

### 负责

- `ReportSupervisorCount`
- `HandleMyselfCount`
- `LastRouteChoice`
- `ColleagueAState`
- 隐藏线已消费 / 已解锁状态

### 必做变量

- `ReportSupervisorCount`
- `HandleMyselfCount`
- `LastRouteChoice`
- `ColleagueAState`
- `bHasConsumedColleagueAHiddenOption`

### 必做函数

- `RecordRouteChoice`
- `SetColleagueUnlockState`
- `MarkHiddenOptionConsumed`
- `GetSliceEndingResult`

### 切片实现要求

- 切片只要求 A 线完整可用
- B / C 可以留占位字段但不参与主链判断

### 完成判定

- `Report Supervisor` / `Handle Myself` 两条路线能稳定产出不同结尾倾向

---

## 3.6 `BP_AnomalyManager`

### ownership

- **异常状态 owner**

### 负责

- 累积聊天 side effect
- `CurrentAnomalyType`
- `bAnomalyActive`
- `FREEZE` 触发与解除

### 必做变量

- `AccumulatedFreezeWeight`
- `CurrentAnomalyType`
- `bAnomalyActive`
- `FreezeThreshold`
- `GameFlowManagerRef`
- `MinigameManagerRef`

### 必做函数

- `AccumulateSideEffect`
- `ResolveDominantAnomaly`
- `TriggerAnomaly`
- `ResolveCurrentAnomaly`

### 切片实现要求

- 只实现 `FREEZE`
- `TriggerAnomaly(Type)` 若 `bAnomalyActive == true` 必须忽略
- `ResolveCurrentAnomaly()` 后清掉 `CurrentAnomalyType` 并广播已解除

### 完成判定

- 小游戏阶段只会触发一个有效 `FREEZE`

---

## 3.7 `BP_ComputerTerminal`

### ownership

- **World actor**，只负责电脑交互

### 必做变量

- `InteractionText`
- `DeskViewAnchor`
- `GameFlowManagerRef`

### 必做函数

- `CanInteract`
- `GetInteractionText`
- `Interact`

### 切片实现要求

- `CanInteract()` 必须只查询 `GameFlowManagerRef.CanEnterDesktop()`
- `Interact()` 只请求 `GameFlowManagerRef.RequestPhaseChange(DesktopIdle)`
- 不直接创建 `WBP_DesktopRoot`
- 不直接切输入模式

### 完成判定

- 玩家靠电脑时能稳定进入桌面 phase

---

## 3.8 `BP_AirConditionerUnit`

### ownership

- **World actor**，切片唯一 `FREEZE` 解决点

### 必做变量

- `bIsCurrentlyAvailable`
- `HandledAnomalyType`
- `AnomalyManagerRef`
- `HiddenDialogueUnlockerRef`
- `GameFlowManagerRef`

### 必做函数

- `CanInteract`
- `GetInteractionText`
- `Interact`
- `PlayRepairFeedback`

### 切片实现要求

- 仅在 `HandleAnomaly3D + CurrentAnomalyType == FREEZE` 时可交互
- 成功交互后按顺序：
  1. `ResolveCurrentAnomaly()`
  2. `UnlockForColleague(ColleagueA)`
  3. `RequestPhaseChange(RoomExplore)`
- **不能** 直接把玩家送回 `DesktopIdle`

### 完成判定

- 修空调后异常解除、隐藏线解锁、玩家仍处于 `RoomExplore`

---

## 3.9 `WBP_DesktopRoot`

### ownership

- **PlayerController-owned** 的桌面根

### 负责

- app 容器
- `Chat` / `Report` / minigame widget 显示承载
- 桌面按钮启用 / 禁用表现

### 必做变量

- `ChatAppWidget`
- `ReportWidget`
- `CurrentAppWidget`
- `GameFlowManagerRef`
- `RouteStateManagerRef`
- `ChatConversationManagerRef`
- `MinigameManagerRef`

### 必做函数

- `OpenApp(AppTag)`
- `ShowAppWidget(WidgetRef)`
- `RefreshAppAvailability`

### 切片实现要求

- `Chat` 按钮点击后应请求 `GameFlowManagerRef.RequestPhaseChange(ChatActive)`
- `Report` 按钮点击前必须检查 `GameFlowManagerRef.CanOpenReport()`，通过后请求 `RequestPhaseChange(ReportPhase)`
- `OpenApp(...)` 只负责显示具体 app，不负责宣告全局 phase
- 当前小游戏 widget 由 `ShowAppWidget()` 承载到 `AppContentHost`
- `DesktopRoot` 不决定 `CurrentPhase`

### 完成判定

- 聊天、报告、小游戏能在一个根容器内切换且不抢全局 ownership

---

## 3.10 `WBP_ChatApp`

### ownership

- **DesktopRoot-owned child widget**

### 必做变量

- `ConversationManagerRef`
- `RouteStateManagerRef`
- `CurrentConversationTarget`

### 必做函数

- `RefreshConversation`
- `HandleSendClicked`
- `HandleHiddenOptionClicked`
- `HandleContactSelected`

### 切片实现要求

- 只负责输入与展示
- `HandleHiddenOptionClicked` 只调用 `ConversationManagerRef.PlayHiddenDialogue(...)`
- 隐藏选项显示必须受 `HiddenOptions` 数据驱动

### 完成判定

- A 线正常聊天、隐藏选项、隐藏对话都可显示

---

## 3.11 `BP_ChatConversationManager`

### ownership

- **聊天数据 owner**

### 必做变量

- `CurrentConversationTarget`
- `ConversationHistory`
- `HiddenOptions`
- `AnomalyManagerRef`
- `RouteStateManagerRef`

### 必做函数

- `InitializeChatSystem`
- `SwitchConversation`
- `SendPlayerMessage`
- `AppendReply`
- `InjectHiddenOption`
- `PlayHiddenDialogue`

### 切片实现要求

- 只要求 A 线完整
- `SendPlayerMessage` 内要调用 `AnomalyManagerRef.AccumulateSideEffect(...)`
- `InjectHiddenOption` 必须按 `OptionId` 去重
- 播完隐藏对话后应更新 `RouteStateManagerRef.MarkHiddenOptionConsumed(...)`

### 完成判定

- 不会重复插入同一个隐藏选项

---

## 3.12 `BP_MinigameManager`

### ownership

- **小游戏运行 owner + popup lifecycle owner**

### 必做变量

- `CurrentMinigameWidget`
- `CurrentMinigameType`
- `ChoicePopupRef`
- `bIsMinigameRunning`
- `DesktopRootRef`
- `GameFlowManagerRef`
- `AnomalyManagerRef`
- `RouteStateManagerRef`

### 必做函数

- `RegisterDesktopRoot`
- `BindAnomalyCallbacks`
- `StartMinigame`
- `PauseMinigame`
- `ResumeMinigame`
- `CompleteMinigame`
- `HandleAnomalyTriggered`
- `HandleAnomalyChoice(Branch)`

### 切片实现要求

- 只支持 `DependencyMatch`
- `RegisterDesktopRoot(DesktopRootRef)` 由 `BP_SignalPlayerController` / `WBP_DesktopRoot` 初始化后调用
- `BindAnomalyCallbacks` 必须把 `AnomalyManagerRef.OnAnomalyTriggered` 绑定到 `HandleAnomalyTriggered`
- `StartMinigame` 要求 `GameFlowManagerRef.RequestPhaseChange(MinigameActive)`
- `HandleAnomalyTriggered` 要求：暂停小游戏 + `RequestPhaseChange(AnomalyChoice)` + 创建 `WBP_AnomalyChoicePopup`
- `HandleAnomalyChoice(ReportSupervisor)` 要求：
  - 记录路线
  - 解除异常
  - 移除 popup
  - 返回 `MinigameActive`
  - 小游戏闭合后进入 `ReportPhase`
- `HandleAnomalyChoice(HandleMyself)` 要求：
  - 记录路线
  - 移除 popup
  - 结束小游戏显示
  - 进入 `HandleAnomaly3D`

### 完成判定

- popup lifecycle 和小游戏 lifecycle 不会分家

---

## 3.13 `WBP_MG_DependencyMatch`

### ownership

- **由 `BP_MinigameManager` 创建并由 `WBP_DesktopRoot` 承载显示**

### 必做变量 / 区域

- `bPaused`
- `MinigameManagerRef` [Expose on Spawn]
- 网格区域
- 计时 / 提示区

### 必做函数

- `BuildGrid`
- `HandleTileClicked`
- `SetPaused`

### 切片实现要求

- 只做最小可玩版
- 只响应自己的输入，不写全局状态

### 完成判定

- 可被 pause / resume / complete 稳定驱动

---

## 3.14 `WBP_AnomalyChoicePopup`

### ownership

- **由 `BP_MinigameManager` 创建和回收**

### 必做变量

- `CurrentAnomalyType`
- `OwningMinigameManagerRef`

### 必做函数

- `SetupForAnomaly`
- `HandleReportSupervisor`
- `HandleHandleMyself`

### 切片实现要求

- `OwningMinigameManagerRef` 必须通过 `Expose on Spawn` 传入
- 按钮点击只转发给 `OwningMinigameManagerRef.HandleAnomalyChoice(...)`
- 不直接 `RequestPhaseChange`
- 不直接 `ResolveCurrentAnomaly`

### 完成判定

- popup 只承担 UI 输入，不偷做流程逻辑

---

## 3.15 `BP_HiddenDialogueUnlocker`

### ownership

- **隐藏选项解锁协调 owner**

### 必做变量

- `ChatConversationManagerRef`
- `RouteStateManagerRef`

### 必做函数

- `UnlockForColleague`
- `MarkAnomalyHandled`

### 切片实现要求

- 只支持 `ColleagueA`
- `UnlockForColleague` 只负责：
  - 生成 `ST_HiddenOptionRecord`
  - 调用 `ChatConversationManagerRef.InjectHiddenOption(...)`
  - 更新 `RouteStateManagerRef` 的 unlock 状态

### 完成判定

- 同一条隐藏选项不会因为重复交互被注入多次

---

## 3.16 `WBP_ReportEditor`

### ownership

- **DesktopRoot-owned child widget**

### 必做变量

- `SelectedSentenceId`
- `GameFlowManagerRef`
- `RouteStateManagerRef`

### 必做函数

- `BuildOptionList`
- `HandleSentenceSelected`
- `HandleSubmitClicked`
- `RefreshSubmitState`

### 切片实现要求

- `Submit` 按钮在未选句子时禁用
- `HandleSubmitClicked` 只调用 `GameFlowManagerRef.SubmitSliceReport(SelectedSentenceId)`
- 不自己决定 `Good / Bad`
- 不自己创建 `WBP_EndingTitleCard`
- 不自己切到 `EndingSequence`

### 完成判定

- 报告提交 ownership 清晰：Editor 收输入，FlowManager 判结果，Controller 显示结尾

---

## 3.17 `WBP_EndingTitleCard`

### ownership

- **PlayerController-owned widget**

### 必做变量

- `CurrentEndingType`

### 必做函数

- `SetupEnding(EndingType)`
- `HandleRestart`
- `HandleQuit`

### 切片实现要求

- 只做显示与按钮
- 标题和字幕由外部传入 ending type 后刷新

### 完成判定

- 结尾卡不会被其它 widget 重复创建

---

## 3.18 `BPI_Interactable`

### 作用

- 统一 3D 交互接口

### 必做函数

- `CanInteract`
- `GetInteractionText`
- `Interact`

### 切片实现要求

- 电脑和空调都走这套接口
- 接口实现内部不能直接写 `CurrentPhase`

### 完成判定

- Character 无需知道交互对象具体类型也能完成切片主链

---

## 4. 推荐开发顺序（按 ownership 收敛）

1. `BPI_Interactable`
2. `BP_SignalGameMode`
3. `BP_SignalPlayerController`
4. `BP_SignalPlayerCharacter`
5. 在关卡中放置并手动绑定：`BP_SignalGameFlowManager / BP_RouteStateManager / BP_AnomalyManager / BP_ChatConversationManager / BP_MinigameManager / BP_HiddenDialogueUnlocker`
6. `BP_ComputerTerminal`
7. `WBP_DesktopRoot`
8. `WBP_ChatApp`
9. `BP_ChatConversationManager`
10. `BP_MinigameManager`
11. `WBP_MG_DependencyMatch`
12. `BP_AnomalyManager`
13. `WBP_AnomalyChoicePopup`
14. `BP_AirConditionerUnit`
15. `BP_HiddenDialogueUnlocker`
16. `WBP_ReportEditor`
17. `WBP_EndingTitleCard`
18. ownership / guards / phase edge cases 联调

---

## 5. 切片必须满足的 edge rules

- `RequestPhaseChange(CurrentPhase)` = `No-op`
- `TriggerAnomaly` 在 `bAnomalyActive == true` 时忽略
- `InjectHiddenOption` 必须按 `OptionId` 去重
- `WBP_ReportEditor` 不选句子不得提交
- `HandleMyself` 路线下，隐藏对话未完成前 `ReportPhase` 不得开放
- `ReportSupervisor` 路线下，不得进入 `HandleAnomaly3D`
- 修好空调后必须先回 `RoomExplore`，再手动回电脑

---

## 6. 最低完成标准

不是“全部做漂亮”，而是：

- ownership 清楚
- refs 来源单一
- phase 切换只有一个 owner
- 两条分支后果不会互相串线
- 切片主链可从头到尾稳定跑通

如果这 18 个对象按本文实现，后续 wiring 和 pseudonode 就不会再因为 owner 不清而打架。
