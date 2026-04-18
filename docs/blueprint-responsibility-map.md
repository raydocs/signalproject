# Signal Project 蓝图职责与通信关系

> **切片权威说明（Authoritative for Slice Ownership）**
> - 本文是 `48 小时竖切片` 的 `运行时 ownership / phase authority / ref 传递` 权威文档。
> - 若本文与 `docs/vertical-slice-scope.md`、`docs/game-state-machine.md`、`docs/interaction-spec.md` 冲突，以那三份切片契约文档为准。
> - 若本文与其他实现文档冲突，则以本文为准，再由本文约束 `vertical-slice-18-blueprints-implementation-spec`、`vertical-slice-blueprint-wiring-order`、`blueprint-pseudonode-flowcharts`。

---

## 1. 切片必须遵守的 ownership 原则

必须锁死这几条：

1. `BP_SignalGameFlowManager` 是 **唯一** 可以写入 `CurrentPhase` 的对象。
2. `Manager` 负责状态和流程，`Widget` 只负责显示和输入转发。
3. 切片运行时使用的核心 manager actor 都是 **放在关卡里的 level actor**。
4. level actor 之间的核心引用采用 **Instance Editable + 手动拖拽赋值**。
5. Widget 引用必须由 **创建者以 Expose on Spawn 方式在 `Create Widget` 节点上传入**。
6. `GetAllActorsOfClass` 只允许作为调试 / fallback，不是规范实现路径。

---

## 2. 切片权威 ownership 总表

## 2.1 Level-owned actors（放在 `LV_ApartmentMain`）

这些对象在切片里是关卡中真实存在的 actor，并且跨 actor 引用要在关卡里手动指定：

- `BP_SignalGameFlowManager`
- `BP_RouteStateManager`
- `BP_AnomalyManager`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `BP_HiddenDialogueUnlocker`
- `BP_ComputerTerminal`
- `BP_AirConditionerUnit`
- 以及本关卡已有的房间 / day / 场景辅助 actor（若本轮切片真的在用）

## 2.2 PlayerController-owned widgets

以下 widget 在切片中由 `BP_SignalPlayerController` 创建、缓存、显示、隐藏：

- `WBP_DesktopRoot`
- `WBP_EndingTitleCard`

## 2.3 DesktopRoot-owned app children

以下 widget 的显示容器与 app 切换权归 `WBP_DesktopRoot`：

- `WBP_ChatApp`
- `WBP_ReportEditor`
- 当前小游戏 widget（切片中即 `WBP_MG_DependencyMatch`）

> 说明：小游戏 widget 可以由 `BP_MinigameManager` 创建并传入引用，但一旦进入桌面内容区，显示承载者仍是 `WBP_DesktopRoot.AppContentHost`。

## 2.4 MinigameManager-owned modal popup

以下临时 modal 的生命周期归 `BP_MinigameManager`：

- `WBP_AnomalyChoicePopup`

原因：

- 它是由小游戏中断触发的 modal
- 它的显示 / 移除 / 暂停 / 恢复都和小游戏状态强相关
- 它不应该由 HUD manager 或杂项 actor 接管

---

## 3. 权威状态 ownership

## 3.1 `BP_SignalGameFlowManager`

### 负责

- `CurrentDayIndex`
- `CurrentPhase`
- phase 切换唯一入口：`RequestPhaseChange(NewPhase)`
- phase 进入后的输入 / UI / app 打开协调
- `ReportPhase` 是否允许进入
- 报告提交后的切片结尾触发

### 不负责

- 不直接存聊天内容
- 不直接存异常权重
- 不直接存隐藏选项列表
- 不直接构建 Widget 布局

### 权威约束

- 任何对象都 **不能自己 Set `CurrentPhase`**
- 所有 phase 变化都必须经过 `RequestPhaseChange`
- `RequestPhaseChange(NewPhase)` 若 `NewPhase == CurrentPhase`，则必须是 `No-op`

## 3.2 `BP_RouteStateManager`

### 负责

- `ReportSupervisor` / `HandleMyself` 路线记录
- 路线计数
- 同事 A 隐藏线解锁状态
- 结尾查询所需的路线结果

### 不负责

- 不切 phase
- 不解析 anomaly
- 不创建 UI

## 3.3 `BP_AnomalyManager`

### 负责

- `CurrentAnomalyType`
- `bAnomalyActive`
- 异常权重累计
- `FREEZE` 的触发与解除

### 不负责

- 不决定 `CurrentPhase`
- 不直接创建结尾 / 报告 UI
- 不直接决定隐藏选项显示样式

### 权威约束

- `TriggerAnomaly(Type)` 若 `bAnomalyActive == true`，必须直接忽略
- 切片里真正执行的 anomaly 只有 `FREEZE`

## 3.4 `BP_ChatConversationManager`

### 负责

- `ConversationHistory`
- `HiddenOptions`
- 当前聊天目标
- 发送消息后的回复注入
- 隐藏选项注入与隐藏对话播放

### 不负责

- 不拥有 `CurrentPhase`
- 不直接打开报告或结尾
- 不直接决定小游戏 modal 生命周期

### 权威约束

- `InjectHiddenOption(OptionRecord)` 必须按 `OptionId` 去重
- `ReportSupervisor` 路线下不应注入切片隐藏选项

## 3.5 `BP_MinigameManager`

### 负责

- 当前小游戏类型
- 当前小游戏 widget 的运行引用
- 小游戏暂停 / 恢复 / 完成
- `WBP_AnomalyChoicePopup` 的创建、缓存、移除
- 异常选择后的分支协调

### 不负责

- 不拥有 `CurrentPhase`
- 不直接决定结尾
- 不直接修改聊天历史

### 权威约束

- 选择 `Report Supervisor` 时，必须：
  1. 记录路线
  2. 要求 `BP_AnomalyManager` 解除异常
  3. 移除 popup
  4. 恢复 / 收束小游戏
  5. 只有在小游戏闭合后才进入 `ReportPhase`
- 选择 `Handle Myself` 时，必须：
  1. 记录路线
  2. 移除 popup
  3. 关闭 / 退出小游戏显示
  4. 请求 `BP_SignalGameFlowManager` 进入 `HandleAnomaly3D`

## 3.6 `BP_HiddenDialogueUnlocker`

### 负责

- “自己处理成功后”解锁隐藏选项
- 更新对应同事的 route / unlock 状态

### 不负责

- 不负责 anomaly 触发
- 不负责 `CurrentPhase`
- 不负责报告提交

---

## 4. Widget 权威边界

## 4.1 `WBP_DesktopRoot`

### 负责

- 桌面模式根容器
- app 切换
- `Chat` / `Report` / 当前小游戏 widget 的显示承载
- 正常桌面状态下的按钮启用 / 禁用表现

### 不负责

- 不拥有 `CurrentPhase`
- 不自己决定何时允许 `ReportPhase`
- 不自己决定分支后果

### 权威约束

- `Chat` / `Report` 按钮点击后，必须请求 `BP_SignalGameFlowManager` 进入对应 phase，而不是直接把 app 打开当成全局状态变化
- `Report` 按钮是否可用，必须由 `BP_SignalGameFlowManager` 的可进入条件决定
- `DesktopRoot.OpenApp(...)` 是渲染函数，不是 phase owner

## 4.2 `WBP_ChatApp`

### 负责

- 联系人切换
- 消息显示
- 发送输入
- 隐藏选项点击

### 不负责

- 不直接 `RequestPhaseChange`
- 不直接设置路线计数
- 不自己决定异常是否触发

## 4.3 `WBP_AnomalyChoicePopup`

### 负责

- 展示 `Report Supervisor / Handle Myself` 两个按钮
- 把按钮意图转发给 `BP_MinigameManager`

### 不负责

- 不自己写 `CurrentPhase`
- 不直接 `ResolveCurrentAnomaly`
- 不直接 `ResumeMinigame`
- 不自己判断何时进入 `ReportPhase`

### 权威约束

- popup 按钮点击后，只做“通知 owning manager”
- popup 不能绕过 manager 直接写全局状态

## 4.4 `WBP_ReportEditor`

### 负责

- 显示句子池
- 记录当前选择
- 把 `Submit` 输入转交给 `BP_SignalGameFlowManager`

### 不负责

- 不自己决定 `Good / Bad` ending
- 不自己创建 `WBP_EndingTitleCard`
- 不自己推进 `CurrentPhase`

### 权威约束

- 未选句子时不可提交
- 当日链路未闭合时不可打开
- `HandleMyself` 路线下，隐藏对话未完成前不可打开

## 4.5 `WBP_EndingTitleCard`

### 负责

- 显示结尾标题与字幕
- 提供重试 / 退出输入

### 不负责

- 不决定路线结果
- 不决定何时进入 `EndingSequence`

---

## 5. World actor 权威边界

## 5.1 `BP_ComputerTerminal`

### 负责

- 响应 3D 交互
- 在条件允许时请求进入 `DesktopIdle`

### 不负责

- 不自己显示 / 创建桌面 widget
- 不自己切输入模式
- 不自己 Set `CurrentPhase`

### 权威约束

- 电脑能否进入，必须只由 `BP_SignalGameFlowManager.CanEnterDesktop()` 决定
- 成功交互后，应请求 `BP_SignalGameFlowManager.RequestPhaseChange(DesktopIdle)`

## 5.2 `BP_AirConditionerUnit`

### 负责

- 作为 `FREEZE` 的唯一 3D 解决目标
- 在 `HandleAnomaly3D` 时响应交互
- 成功后通知 anomaly 解除与隐藏线解锁

### 不负责

- 不自己把玩家弹回桌面
- 不自己决定结尾
- 不自己写 `CurrentPhase`

### 权威约束

- 交互成功后，应：
  1. `ResolveCurrentAnomaly`
  2. `UnlockForColleague(ColleagueA)`
  3. 请求 `BP_SignalGameFlowManager.RequestPhaseChange(RoomExplore)`
- 之后玩家必须手动回电脑，不允许空调直接把 phase 送进 `DesktopIdle`

---

## 6. 切片 reference-acquisition 规范

## 6.1 Level actor 之间

规范做法：

- 把核心 manager actor 放进 `LV_ApartmentMain`
- 把需要的 cross-ref 设为 `Instance Editable`
- 在关卡里手动拖拽赋值

切片里应手动指定的典型引用：

- `BP_SignalGameFlowManager`
  - `RouteStateManagerRef`
  - `AnomalyManagerRef`
  - `ChatConversationManagerRef`
  - `MinigameManagerRef`
  - `HiddenDialogueUnlockerRef`
- `BP_AnomalyManager`
  - `GameFlowManagerRef`
  - `MinigameManagerRef`
- `BP_ChatConversationManager`
  - `AnomalyManagerRef`
  - `RouteStateManagerRef`
- `BP_MinigameManager`
  - `GameFlowManagerRef`
  - `AnomalyManagerRef`
  - `RouteStateManagerRef`
- `BP_HiddenDialogueUnlocker`
  - `ChatConversationManagerRef`
  - `RouteStateManagerRef`
- `BP_ComputerTerminal`
  - `GameFlowManagerRef`
- `BP_AirConditionerUnit`
  - `GameFlowManagerRef`
  - `AnomalyManagerRef`
  - `HiddenDialogueUnlockerRef`

## 6.2 PlayerController 引用来源

`BP_SignalPlayerController` 不是 level-placed actor，所以它的 manager refs 不采用关卡手拖。

切片规范做法：

- `BP_SignalGameFlowManager.InitializeGameFlow` 在 BeginPlay 时缓存 `BP_SignalPlayerController`
- 然后由 `BP_SignalGameFlowManager` 把切片需要的 manager refs 注册给 PlayerController

## 6.3 Widget 引用来源

Widget 不能自己到世界里搜 manager。

规范做法：

- `BP_SignalPlayerController` 创建 `WBP_DesktopRoot` / `WBP_EndingTitleCard` 时，用 `Expose on Spawn` 传 refs
- `WBP_DesktopRoot` 创建 `WBP_ChatApp` / `WBP_ReportEditor` 时，用 `Expose on Spawn` 传 refs
- `BP_MinigameManager` 创建 `WBP_MG_DependencyMatch` / `WBP_AnomalyChoicePopup` 时，用 `Expose on Spawn` 传 refs

## 6.4 `GetAllActorsOfClass` 规则

- 调试时可临时使用
- 文档中的切片 **规范实现** 不以它为默认路径
- 一旦主链跑通，应把它替换为手动 refs / creator injection

---

## 7. 切片权威通信方向

规范方向：

```text
Player Input
 -> PlayerController / Character
 -> Widget or Interactable
 -> Owning Manager
 -> BP_SignalGameFlowManager（如需 phase 变化）
 -> PlayerController / DesktopRoot / World presentation
```

不要这样做：

- `Widget -> 直接 Set CurrentPhase`
- `World Actor -> 直接 Create Ending UI`
- `WBP_ReportEditor -> 直接判定 Good/Bad ending 并播结尾`
- `WBP_AnomalyChoicePopup -> 直接 ResumeMinigame + 直接改 phase`

---

## 8. 切片关键通信案例（冻结版）

## 8.1 玩家用电脑进入桌面

```text
BP_ComputerTerminal.Interact()
 -> BP_SignalGameFlowManager.RequestPhaseChange(DesktopIdle)
 -> BP_SignalGameFlowManager.HandlePhaseEntered(DesktopIdle)
 -> BP_SignalPlayerController.ShowDesktopRoot()
 -> BP_SignalPlayerController.SetDesktopInputMode()
```

## 8.2 小游戏中触发异常

```text
BP_MinigameManager.StartMinigame()
 -> BP_SignalGameFlowManager.RequestPhaseChange(MinigameActive)
 -> BP_AnomalyManager.ResolveDominantAnomaly()
 -> BP_AnomalyManager.TriggerAnomaly(FREEZE)
 -> BP_MinigameManager.HandleAnomalyTriggered(FREEZE)
 -> BP_SignalGameFlowManager.RequestPhaseChange(AnomalyChoice)
 -> WBP_AnomalyChoicePopup shown
```

## 8.3 玩家选择 `Report Supervisor`

```text
WBP_AnomalyChoicePopup.ReportButton
 -> BP_MinigameManager.HandleAnomalyChoice(ReportSupervisor)
 -> BP_RouteStateManager.RecordRouteChoice(ReportSupervisor)
 -> BP_AnomalyManager.ResolveCurrentAnomaly()
 -> BP_SignalGameFlowManager.RequestPhaseChange(MinigameActive)
 -> BP_MinigameManager.Resume / Complete Minigame
 -> [小游戏闭合后]
 -> BP_SignalGameFlowManager.RequestPhaseChange(ReportPhase)
```

## 8.4 玩家选择 `Handle Myself`

```text
WBP_AnomalyChoicePopup.HandleButton
 -> BP_MinigameManager.HandleAnomalyChoice(HandleMyself)
 -> BP_RouteStateManager.RecordRouteChoice(HandleMyself)
 -> BP_SignalGameFlowManager.RequestPhaseChange(HandleAnomaly3D)
 -> BP_SignalPlayerController.HideDesktopRoot()
 -> BP_SignalPlayerController.SetRoomInputMode()
```

## 8.5 玩家修好空调

```text
BP_AirConditionerUnit.Interact()
 -> BP_AnomalyManager.ResolveCurrentAnomaly()
 -> BP_HiddenDialogueUnlocker.UnlockForColleague(ColleagueA)
 -> BP_SignalGameFlowManager.RequestPhaseChange(RoomExplore)
 -> [玩家手动走回电脑]
 -> BP_ComputerTerminal.Interact()
 -> DesktopIdle / ChatActive
```

## 8.6 提交报告并播结尾

```text
WBP_ReportEditor.Submit
 -> BP_SignalGameFlowManager.SubmitSliceReport(SelectedSentenceId)
 -> BP_SignalGameFlowManager.TriggerEnding(EndingType)
 -> BP_SignalGameFlowManager.RequestPhaseChange(EndingSequence)
 -> BP_SignalPlayerController.ShowEndingTitleCard(EndingType) [由 HandlePhaseEntered 触发]
```

---

## 9. 切片 edge rules（ownership 版）

以下规则这四份实现文档都必须一致：

- 重复 `RequestPhaseChange(CurrentPhase)` 是 `No-op`
- `TriggerAnomaly` 在 `bAnomalyActive == true` 时忽略
- `InjectHiddenOption` 按 `OptionId` 去重
- `WBP_ReportEditor` 未选句子不可提交
- `ReportPhase` 在小游戏 / 异常 / 隐藏对话链未闭合前不可进入
- `ReportSupervisor` 路线不会解锁隐藏选项
- `HandleMyself` 路线不会绕过 `RoomExplore` 自动回桌面

---

## 10. 本轮切片明确不作为 authority 的对象

以下对象即使存在于完整版文档，也不是本轮切片 ownership 的一部分：

- `BP_ReportManager`
- `BP_AnomalyChoiceDirector`
- `BP_AnomalyEffectRouter`
- `BP_SignalHUDManager`
- `BP_RoomStateManager`
- `BP_DayStateDirector`
- `BP_SupervisorRevealDirector`

它们可以留在完整版规划里，但在 48 小时切片里不应成为实现依赖。
