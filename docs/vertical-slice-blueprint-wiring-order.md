# Signal Project 第一个可运行版本蓝图连线顺序

> **切片权威说明（Authoritative for Slice Wiring）**
> - 本文是 `48 小时竖切片` 的 wiring 施工顺序文档。
> - 本文必须服从 `vertical-slice-scope`、`game-state-machine`、`interaction-spec` 以及 `blueprint-responsibility-map` 的权威约束。
> - 本文只描述切片 runtime 的规范连线方式，不再把 `GetAllActorsOfClass` 当成默认实现路径。

---

## 1. 这份文档解决什么问题

这份文档专门解决：

- 哪些 actor 要先放到关卡里
- 哪些引用必须先手动接通
- phase 变化从哪里进、从哪里出
- 两条 anomaly 分支各自如何落地
- 第一个可运行切片应该按什么顺序施工

它只服务于这条切片主链：

```text
RoomExplore
 -> Use Computer
 -> DesktopIdle
 -> ChatActive
 -> Start Minigame
 -> MinigameActive
 -> FREEZE
 -> AnomalyChoice
    -> Report Supervisor -> Minigame closure -> ReportPhase
    -> Handle Myself -> HandleAnomaly3D -> AirConditioner -> RoomExplore -> Computer -> Chat Hidden Dialogue -> ReportPhase
 -> EndingSequence
```

---

## 2. 最小连线对象清单

- `BP_SignalGameMode`
- `BP_SignalPlayerController`
- `BP_SignalPlayerCharacter`
- `BP_SignalGameFlowManager`
- `BP_RouteStateManager`
- `BP_AnomalyManager`
- `BP_ComputerTerminal`
- `BP_AirConditionerUnit`
- `BPI_Interactable`
- `WBP_DesktopRoot`
- `WBP_ChatApp`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `WBP_MG_DependencyMatch`
- `WBP_AnomalyChoicePopup`
- `BP_HiddenDialogueUnlocker`
- `WBP_ReportEditor`
- `WBP_EndingTitleCard`

---

## 3. 连线总原则

1. 先把 **owner 和 ref 来源** 接对，再写功能细节。
2. phase 变化永远只经过 `BP_SignalGameFlowManager.RequestPhaseChange()`。
3. `Widget` 只收输入 / 发请求，不直接写全局状态。
4. level actor 之间用 `Instance Editable` 手动拖 refs。
5. widget refs 一律由创建者通过 `Expose on Spawn` 传入。
6. `GetAllActorsOfClass` 只允许调试时临时顶一下，不写成规范链路。

---

## 4. 施工顺序总览

按这个顺序最稳：

0. 关卡里放置 manager actor 并手动绑定 refs
1. `GameMode -> PlayerController -> Character`
2. `GameFlowManager -> PlayerController` runtime ref 注册
3. `Character -> BPI_Interactable` 房间交互
4. `ComputerTerminal -> GameFlowManager -> PlayerController` 进入桌面
5. `PlayerController -> DesktopRoot -> ChatApp` 桌面根和聊天
6. `ChatConversationManager -> AnomalyManager` 聊天副作用累积
7. `DesktopRoot / ChatApp -> MinigameManager -> WBP_MG_DependencyMatch` 启动小游戏
8. `AnomalyManager -> MinigameManager -> WBP_AnomalyChoicePopup` 触发 `FREEZE`
9. `WBP_AnomalyChoicePopup -> MinigameManager` 处理两条路线
10. `AirConditionerUnit -> AnomalyManager -> HiddenDialogueUnlocker -> GameFlowManager` 解决 `FREEZE`
11. 玩家手动回电脑 -> `ChatApp` 出隐藏选项 -> 播隐藏对话
12. `DesktopRoot -> ReportEditor -> GameFlowManager -> PlayerController` 报告与结尾

---

## 5. 详细连线步骤

## Step 0：先把 level actor 放到关卡里并手动绑定 refs

### 目标

- 在真正写蓝图逻辑前，把切片 runtime 的 owner 关系接好

### 关卡里必须存在的 actor

- `BP_SignalGameFlowManager`
- `BP_RouteStateManager`
- `BP_AnomalyManager`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `BP_HiddenDialogueUnlocker`
- `BP_ComputerTerminal`
- `BP_AirConditionerUnit`

### 必绑引用（Details 面板手动拖）

#### `BP_SignalGameFlowManager`

- `RouteStateManagerRef`
- `AnomalyManagerRef`
- `ChatConversationManagerRef`
- `MinigameManagerRef`
- `HiddenDialogueUnlockerRef`

#### `BP_AnomalyManager`

- `GameFlowManagerRef`
- `MinigameManagerRef`

#### `BP_ChatConversationManager`

- `AnomalyManagerRef`
- `RouteStateManagerRef`

#### `BP_MinigameManager`

- `GameFlowManagerRef`
- `AnomalyManagerRef`
- `RouteStateManagerRef`

#### `BP_HiddenDialogueUnlocker`

- `ChatConversationManagerRef`
- `RouteStateManagerRef`

#### `BP_ComputerTerminal`

- `GameFlowManagerRef`

#### `BP_AirConditionerUnit`

- `AnomalyManagerRef`
- `HiddenDialogueUnlockerRef`
- `GameFlowManagerRef`

### 完成后验收

- 关卡里不存在“运行时再猜谁是谁”的核心 manager ref
- `GetAllActorsOfClass` 还没写，主链 refs 也已经齐了

---

## Step 1：GameMode、PlayerController、Character 基础运行

### 目标

- 开游戏能进房间
- 玩家能移动和转头

### 连线顺序

#### 1. `BP_SignalGameMode`

- 设置 `Default Pawn Class = BP_SignalPlayerCharacter`
- 设置 `Player Controller Class = BP_SignalPlayerController`

#### 2. `BP_SignalPlayerCharacter`

- 先保证移动正常
- 暂不写 phase 逻辑

#### 3. `BP_SignalPlayerController`

- BeginPlay 先执行 `SetRoomInputMode`
- `SetRoomInputMode` 里：
  - `Show Mouse Cursor = false`
  - `Input Mode = Game Only`
  - 恢复 Pawn 移动

### 完成后验收

- 玩家可移动
- 没有鼠标
- 当前输入归 3D

---

## Step 2：`GameFlowManager -> PlayerController` runtime ref 注册

### 目标

- 让 PlayerController 拿到规范 manager refs，而不是自己满世界找

### 连线顺序

#### 1. `BP_SignalGameFlowManager.InitializeGameFlow`

- BeginPlay 缓存 `BP_SignalPlayerController`
- 调 `PlayerController.RegisterSliceRefs(...)`
- 把 `GameFlowManagerRef / RouteStateManagerRef / AnomalyManagerRef / ChatConversationManagerRef / MinigameManagerRef` 注册给 Controller
- 设置初始 phase 为 `RoomExplore`

#### 2. `BP_SignalPlayerController.RegisterSliceRefs`

- 保存这些 refs
- 但不在这里切 phase

### 完成后验收

- PlayerController 拿到桌面和结尾所需的 manager refs
- 不需要 `GetAllActorsOfClass` 才能创建 `DesktopRoot`

---

## Step 3：Character 和 `BPI_Interactable` 打通

### 目标

- 玩家对准物体看到交互提示
- 按 `E` 调用接口交互

### 连线顺序

#### 1. 创建 `BPI_Interactable`

- `CanInteract`
- `GetInteractionText`
- `Interact`

#### 2. `BP_SignalPlayerCharacter`

- `PerformInteractionTrace` 检测前方命中
- 命中实现接口的 actor 时，调用：
  - `CanInteract`
  - `GetInteractionText`
- 可交互时缓存到 `CurrentFocusedInteractable`

#### 3. 输入绑定

- `InputAction Interact / E`
- 调 `TryInteract`
- 对当前焦点对象执行接口 `Interact`

### 完成后验收

- 电脑出现 `[E] Use Computer`
- 空调在可用时出现 `[E] Adjust Air Conditioner`

---

## Step 4：电脑交互进入桌面模式

### 目标

- 电脑交互只负责发起进入桌面请求
- phase 与输入切换由 flow/controller 负责

### 连线顺序

#### 1. `BP_ComputerTerminal`

- `CanInteract` 只查询 `GameFlowManagerRef.CanEnterDesktop()`
- `Interact` 内：
  - 调 `GameFlowManagerRef.RequestPhaseChange(DesktopIdle)`

#### 2. `BP_SignalGameFlowManager.HandlePhaseEntered(DesktopIdle)`

- 调 `CachedPlayerController.ShowDesktopRoot()`
- 调 `CachedPlayerController.SetDesktopInputMode()`

#### 3. `BP_SignalPlayerController.ShowDesktopRoot`

- 若 `CachedDesktopRoot` 无效：`Create Widget(WBP_DesktopRoot)`，并通过 `Expose on Spawn` 传入必需 refs
- `AddToViewport`
- 设为 Visible
- 调 `MinigameManagerRef.RegisterDesktopRoot(CachedDesktopRoot)`

### 完成后验收

- 用电脑后进入 `DesktopIdle`
- 桌面根出现
- 鼠标出现，角色停止移动

---

## Step 5：`DesktopRoot` 和 `ChatApp` 打通

### 目标

- 玩家进入桌面后能稳定打开聊天

### 连线顺序

#### 1. `WBP_DesktopRoot`

- 放 `Chat` 按钮与 `Report` 按钮
- 有 `AppContentHost`
- `Chat` 按钮点击：调 `GameFlowManagerRef.RequestPhaseChange(ChatActive)`
- `Report` 按钮点击：先过 `CanOpenReport()`，再调 `GameFlowManagerRef.RequestPhaseChange(ReportPhase)`
- `OpenApp("Chat")` / `OpenApp("Report")` 只负责渲染 app，不负责声明全局 phase
- 创建 `WBP_ChatApp` / `WBP_ReportEditor` 时，必需 refs 通过 `Expose on Spawn` 传入

#### 2. `WBP_ChatApp`

- 最小布局：联系人、消息区、发送按钮、隐藏选项区
- 不直接切 phase

### 完成后验收

- `Chat` / `Report` 按钮不会绕过 `GameFlowManager` 直接改全局流程
- app 切换所有权清楚，只有 DesktopRoot 承载内容区

---

## Step 6：聊天发送累积异常权重

### 目标

- 给 A 发消息能推动 `FREEZE`

### 连线顺序

#### 1. `BP_ChatConversationManager.SendPlayerMessage`

顺序：

1. 插入玩家消息
2. 调 `AnomalyManagerRef.AccumulateSideEffect(CurrentConversationTarget, 1)`
3. 插入回复
4. 广播 `OnMessagesUpdated`

#### 2. `BP_AnomalyManager.AccumulateSideEffect`

- `ColleagueA` -> 增加 `AccumulatedFreezeWeight`
- B / C 可先留占位，不进入切片主链

### 完成后验收

- 连续给 A 发送消息，`FreezeWeight` 稳定增长

---

## Step 7：从桌面启动小游戏

### 目标

- 小游戏由 `BP_MinigameManager` 启动，DesktopRoot 负责承载显示

### 连线顺序

#### 1. 启动入口

可先在 `WBP_ChatApp` 或 `WBP_DesktopRoot` 放一个 `Start Task` 按钮：

- 点击 -> `MinigameManagerRef.StartMinigame(DependencyMatch)`

#### 2. `BP_MinigameManager.StartMinigame`

顺序：

1. 确认 `DesktopRootRef` 已由 PlayerController 注册
2. 记录 `CurrentMinigameType`
3. `Create Widget(WBP_MG_DependencyMatch)`，并通过 `Expose on Spawn` 传入 `MinigameManagerRef = self`
5. 调 `DesktopRootRef.ShowAppWidget(CurrentMinigameWidget)`
6. 设 `bIsMinigameRunning = true`
7. 调 `GameFlowManagerRef.RequestPhaseChange(MinigameActive)`

### 完成后验收

- 小游戏能出现
- phase 正确进入 `MinigameActive`
- 小游戏显示在 DesktopRoot 的 app 区域里

---

## Step 8：小游戏内触发 `FREEZE`

### 目标

- `FREEZE` 只能由 anomaly manager 触发
- popup 只能由 minigame manager 创建

### 连线顺序

#### 1. `BP_MinigameManager`

- BeginPlay / 初始化时先做：
  - 绑定 `AnomalyManagerRef.OnAnomalyTriggered -> HandleAnomalyTriggered`
- 开始小游戏后一小段时间，或在预定节点调用：
  - `AnomalyManagerRef.ResolveDominantAnomaly()`

#### 2. `BP_AnomalyManager.ResolveDominantAnomaly`

- 若 `AccumulatedFreezeWeight >= FreezeThreshold`
  - 调 `TriggerAnomaly(FREEZE)`

#### 3. `BP_AnomalyManager.TriggerAnomaly`

- 若 `bAnomalyActive == true` -> 直接 return
- 否则：
  - `CurrentAnomalyType = FREEZE`
  - `bAnomalyActive = true`
  - 广播 `OnAnomalyTriggered(FREEZE)`

#### 4. `BP_MinigameManager.HandleAnomalyTriggered`

顺序：

1. `PauseMinigame()`
2. `GameFlowManagerRef.RequestPhaseChange(AnomalyChoice)`
3. 创建 `WBP_AnomalyChoicePopup`
4. 创建时通过 `Expose on Spawn` 传入 `OwningMinigameManagerRef = self`
5. 调 `ChoicePopupRef.SetupForAnomaly(FREEZE)`
6. 显示 popup

### 完成后验收

- 小游戏里只会弹一个有效 `FREEZE` popup
- 当前 phase 正确变成 `AnomalyChoice`

---

## Step 9：异常选择弹窗分支连线

### 目标

- popup 只收输入
- 分支逻辑全部落回 `BP_MinigameManager`

### 连线顺序

#### 1. `WBP_AnomalyChoicePopup`

- `Report Supervisor` 按钮 -> `OwningMinigameManagerRef.HandleAnomalyChoice(ReportSupervisor)`
- `Handle Myself` 按钮 -> `OwningMinigameManagerRef.HandleAnomalyChoice(HandleMyself)`

#### 2. `BP_MinigameManager.HandleAnomalyChoice(ReportSupervisor)`

顺序：

1. `RouteStateManagerRef.RecordRouteChoice(ReportSupervisor)`
2. `AnomalyManagerRef.ResolveCurrentAnomaly()`
3. 移除 popup
4. `GameFlowManagerRef.RequestPhaseChange(MinigameActive)`
5. `ResumeMinigame()` 或直接进入统一的 `CompleteMinigame()` 收尾
6. 在 `ReportSupervisor` 分支里，小游戏闭合后再请求 `RequestPhaseChange(ReportPhase)`

#### 3. `BP_MinigameManager.HandleAnomalyChoice(HandleMyself)`

顺序：

1. `RouteStateManagerRef.RecordRouteChoice(HandleMyself)`
2. 移除 popup
3. 隐藏 / 结束当前小游戏 widget
4. `bIsMinigameRunning = false`
5. `GameFlowManagerRef.RequestPhaseChange(HandleAnomaly3D)`

### 完成后验收

- `Report Supervisor` 不会直接从 popup 跳结尾
- `Handle Myself` 会稳定进入 `HandleAnomaly3D`

---

## Step 10：从桌面回到房间处理异常

### 目标

- `Handle Myself` 后正确退出桌面并恢复 3D 输入

### 连线顺序

#### 1. `BP_SignalGameFlowManager.HandlePhaseEntered(HandleAnomaly3D)`

- `CachedPlayerController.HideDesktopRoot()`
- `CachedPlayerController.SetRoomInputMode()`

#### 2. `BP_AirConditionerUnit`

- `CanInteract` 仅在：
  - `CurrentPhase == HandleAnomaly3D`
  - `CurrentAnomalyType == FREEZE`
  - `bIsCurrentlyAvailable == true`

### 完成后验收

- 鼠标消失
- 玩家恢复 3D 行动
- 电脑因未解决异常被锁定
- 空调成为唯一正确目标

---

## Step 11：空调交互解决 `FREEZE`

### 目标

- 修空调后异常结束，但不自动回桌面

### 连线顺序

#### 1. `BP_AirConditionerUnit.Interact`

顺序：

1. `PlayRepairFeedback()`
2. `AnomalyManagerRef.ResolveCurrentAnomaly()`
3. `HiddenDialogueUnlockerRef.UnlockForColleague(ColleagueA)`
4. `bIsCurrentlyAvailable = false`
5. `GameFlowManagerRef.RequestPhaseChange(RoomExplore)`

#### 2. `BP_AnomalyManager.ResolveCurrentAnomaly`

- `bAnomalyActive = false`
- `CurrentAnomalyType = None`
- 广播 `OnAnomalyResolved`

### 完成后验收

- `FREEZE` 被清除
- phase 回到 `RoomExplore`
- 电脑锁定解除
- **不会** 自动进入 `DesktopIdle`

---

## Step 12：手动回电脑并注入隐藏选项

### 目标

- 玩家必须自己回电脑，之后 A 线出现隐藏选项

### 连线顺序

#### 1. `BP_HiddenDialogueUnlocker.UnlockForColleague(ColleagueA)`

- 生成 `ST_HiddenOptionRecord`
- 调 `ChatConversationManagerRef.InjectHiddenOption(OptionRecord)`
- 更新 `RouteStateManagerRef` 的 unlock 状态

#### 2. `BP_ChatConversationManager.InjectHiddenOption`

- 先检查 `OptionId` 是否已存在
- 不存在才加入 `HiddenOptions`
- 广播 `OnHiddenOptionInjected`

#### 3. 玩家手动回电脑

- 再次与 `BP_ComputerTerminal` 交互
- 进入 `DesktopIdle`
- 打开 `ChatApp`
- 看到 A 线隐藏选项

### 完成后验收

- 不手动回电脑就看不到桌面隐藏选项
- 同一条隐藏选项不会重复注入

---

## Step 13：点击隐藏选项播放隐藏对话

### 目标

- 隐藏对话完成后才允许报告

### 连线顺序

#### 1. `WBP_ChatApp.HandleHiddenOptionClicked(OptionId)`

- 调 `ConversationManagerRef.PlayHiddenDialogue(CurrentConversationTarget)`

#### 2. `BP_ChatConversationManager.PlayHiddenDialogue`

- 从隐藏对话数据源拉序列
- 顺序插入 `ConversationHistory`
- 更新 `RouteStateManagerRef.MarkHiddenOptionConsumed(...)`
- 广播 `OnMessagesUpdated`

#### 3. `BP_SignalGameFlowManager.CanOpenReport`

- `HandleMyself` 路线下，若隐藏对话未消费 -> 返回 false
- `ReportSupervisor` 路线下，小游戏闭合后可直接返回 true

### 完成后验收

- `HandleMyself` 路线下，隐藏对话没播完就打不开报告

---

## Step 14：日报界面和结尾卡串起来

### 目标

- 报告只收输入，FlowManager 决定结尾，Controller 显示结尾

### 连线顺序

#### 1. `WBP_DesktopRoot`

- `OpenApp("Report")` 前先检查 `GameFlowManagerRef.CanOpenReport()`
- 允许时才创建 / 显示 `WBP_ReportEditor`

#### 2. `WBP_ReportEditor`

- 选择句子 -> `SelectedSentenceId`
- 未选择句子时 `Submit` 禁用
- 点击 `Submit` -> `GameFlowManagerRef.SubmitSliceReport(SelectedSentenceId)`

#### 3. `BP_SignalGameFlowManager.SubmitSliceReport`

- 验证当前允许报告
- 查询 `RouteStateManagerRef.GetSliceEndingResult()`
- 调 `TriggerEnding(EndingType)`

#### 4. `BP_SignalGameFlowManager.TriggerEnding`

- 缓存 `PendingEndingType`
- `RequestPhaseChange(EndingSequence)`

#### 5. `BP_SignalPlayerController.ShowEndingTitleCard`

- 由 `HandlePhaseEntered(EndingSequence)` 间接调用
- 创建 / 缓存 `WBP_EndingTitleCard`
- `SetupEnding(EndingType)`
- 显示结尾卡

### 完成后验收

- 报告页不直接决定 `Good / Bad`
- 结尾卡由 PlayerController 显示

---

## 6. 第一版连线完成后的完整验收

如果以下都成立，说明 slice wiring 已经和 ownership 契约对齐：

1. 进入主关卡能移动
2. 关卡里的 manager refs 全部手动接好
3. 对准电脑出现提示
4. 用电脑后通过 `RequestPhaseChange(DesktopIdle)` 进入桌面
5. DesktopRoot 由 PlayerController 持有
6. 聊天能发送并累积 `FREEZE`
7. 小游戏由 MinigameManager 启动并被 DesktopRoot 承载
8. `FREEZE` 触发后 popup 由 MinigameManager 创建
9. `Report Supervisor` 会先回小游戏闭合，再进 `ReportPhase`
10. `Handle Myself` 会进 `HandleAnomaly3D`
11. 修空调后 phase 回 `RoomExplore`，不会自动回桌面
12. 手动回电脑后隐藏选项出现
13. 隐藏对话完成前打不开报告
14. 报告提交后由 FlowManager 判结果、Controller 显示结尾

---

## 7. 最后建议

排错时不要同时修 5 个系统，按 ownership 链条查：

- ref 有没有手动接好
- phase 有没有只走 `GameFlowManager`
- widget 有没有越权直接改状态
- popup 有没有绕过 `MinigameManager`
- AC 解决后是不是错误地直接回了 `DesktopIdle`
- report 有没有被过早开放

只要这几个 owner 不串位，切片主链就会稳得多。
