# Signal Project 48 小时切片 18 个 Blueprint 逐个实现说明

> 目标：把切片版真正必须落地的 18 个 Blueprint / Widget 拆成“创建后要做什么”的执行说明。

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

---

## 2. 逐个实现说明

## 2.1 BP_SignalGameMode

### 作用

- 指定默认玩家控制器和角色
- 在主关卡中作为入口配置

### 必做设置

- `Default Pawn Class` 指向 `BP_SignalPlayerCharacter`
- `Player Controller Class` 指向 `BP_SignalPlayerController`

### 不要做

- 不要把流程逻辑写在这里

### 完成判定

- 进入主关卡后正确生成玩家和控制器

---

## 2.2 BP_SignalPlayerController

### 作用

- 管输入模式切换
- 管鼠标显示
- 管桌面 UI 显隐

### 必做变量

- `CachedDesktopRoot`
- `bIsInDesktopMode`

### 必做函数

- `SetRoomInputMode`
- `SetDesktopInputMode`
- `ShowDesktopRoot`
- `HideDesktopRoot`

### 实现要求

- `SetRoomInputMode`：隐藏鼠标、锁 UI 焦点、恢复角色输入
- `SetDesktopInputMode`：显示鼠标、聚焦桌面根 Widget、禁用角色移动

### 完成判定

- 按电脑交互后能正确切到桌面输入
- 离开桌面后能回到 3D 移动输入

---

## 2.3 BP_SignalPlayerCharacter

### 作用

- 房间内移动
- 做交互射线检测

### 必做变量

- `InteractionTraceDistance`
- `CurrentFocusedInteractable`

### 必做函数

- `PerformInteractionTrace`
- `TryInteract`

### 实现要求

- 每帧或低频检测前方交互物
- 命中实现 `BPI_Interactable` 的 Actor 时显示提示
- 按 `E` 调用 `Interact`

### 完成判定

- 玩家能看到 `[E] Use Computer`
- 对准空调时能看到对应交互提示

---

## 2.4 BP_SignalGameFlowManager

### 作用

- 切片版总流程控制器

### 必做变量

- `CurrentDayIndex`
- `CurrentPhase`
- `CurrentAnomalyType`
- `CachedRouteStateManager`
- `CachedAnomalyManager`

### 必做函数

- `InitializeGameFlow`
- `StartDay`
- `RequestPhaseChange`
- `CompleteCurrentDay`
- `TriggerEnding`

### 切片实现要求

- 先只支持 Day 1-Day 3
- 能在 `RoomExplore / DesktopIdle / ChatActive / MinigameActive / AnomalyChoice / HandleAnomaly3D / ReportPhase / EndingSequence` 间切换

### 完成判定

- 一条主流程从进房间到看到结尾卡能跑通

---

## 2.5 BP_RouteStateManager

### 作用

- 记录玩家选择

### 必做变量

- `ReportSupervisorCount`
- `HandleMyselfCount`
- `ColleagueAState`

### 必做函数

- `RecordRouteChoice`
- `SetColleagueUnlockState`

### 切片实现要求

- 当前只需要重点支持 A 线
- B/C 可留字段先不触发

### 完成判定

- 玩家选择不同分支后，结尾文案会不同

---

## 2.6 BP_AnomalyManager

### 作用

- 累积聊天副作用
- 触发 `FREEZE`

### 必做变量

- `AccumulatedFreezeWeight`
- `CurrentAnomalyType`
- `bAnomalyActive`

### 必做函数

- `AccumulateSideEffect`
- `ResolveDominantAnomaly`
- `TriggerAnomaly`
- `ResolveCurrentAnomaly`

### 切片实现要求

- 当前只实现 `FREEZE`
- 聊天发送到一定次数后，在小游戏中触发异常

### 完成判定

- 聊天后进入小游戏，能稳定触发 FREEZE 弹窗

---

## 2.7 BP_ComputerTerminal

### 作用

- 房间中的电脑交互物

### 必做组件

- Static Mesh
- Collision

### 必做变量

- `InteractionText`
- `DeskViewAnchor`

### 必做函数

- `Interact`
- `EnterDesktopMode`

### 切片实现要求

- 与玩家控制器打通
- 交互后切到桌面模式

### 完成判定

- 从房间进入电脑稳定可用

---

## 2.8 BP_AirConditionerUnit

### 作用

- FREEZE 异常处理物

### 必做变量

- `bIsCurrentlyAvailable`
- `HandledAnomalyType`

### 必做函数

- `Interact`
- `PlayRepairFeedback`

### 切片实现要求

- 仅当当前异常是 `FREEZE` 且处于 `HandleAnomaly3D` 时允许交互
- 成功交互后通知 `BP_AnomalyManager.ResolveCurrentAnomaly`

### 完成判定

- 调空调后能回到电脑并解锁隐藏选项

---

## 2.9 WBP_DesktopRoot

### 作用

- 桌面模式根界面

### 必做区域

- 顶部 HUD
- 桌面背景
- 应用容器
- 通知区

### 必做函数

- `OpenApp`
- `CloseCurrentApp`
- `ShowNotification`

### 切片实现要求

- 至少支持打开 `ChatApp` 和 `ReportEditor`

### 完成判定

- 进入桌面后能正常显示，并切 App

---

## 2.10 WBP_ChatApp

### 作用

- 聊天主界面

### 必做区域

- 联系人列表
- 消息线程区
- 发送区
- 隐藏选项展示区

### 必做函数

- `RefreshConversation`
- `HandleSendClicked`
- `HandleHiddenOptionClicked`

### 切片实现要求

- 先做固定联系人 A/B/C/Supervisor
- 先支持预设文案发送，不必真打字

### 完成判定

- 发消息 -> 收到回复 -> 解锁隐藏选项链完整成立

---

## 2.11 BP_ChatConversationManager

### 作用

- 聊天数据逻辑层

### 必做变量

- `CurrentConversationTarget`
- `ConversationHistory`
- `HiddenOptions`

### 必做函数

- `InitializeChatSystem`
- `SwitchConversation`
- `SendPlayerMessage`
- `AppendReply`
- `InjectHiddenOption`
- `PlayHiddenDialogue`

### 切片实现要求

- 先只用 DataTable 拉 A 线文本
- 同时给 B/C 放占位文本

### 完成判定

- A 线完整可跑，B/C 至少不报错

---

## 2.12 BP_MinigameManager

### 作用

- 统一启动/结束小游戏

### 必做变量

- `CurrentMinigameWidget`
- `CurrentMinigameType`
- `bIsMinigameRunning`

### 必做函数

- `StartMinigame`
- `PauseMinigame`
- `ResumeMinigame`
- `CompleteMinigame`

### 切片实现要求

- 只支持 `DependencyMatch`

### 完成判定

- 能从聊天流程打开小游戏并结束

---

## 2.13 WBP_MG_DependencyMatch

### 作用

- 连连看小游戏根界面

### 必做区域

- 标题
- 网格区域
- 计时
- 提示条

### 切片实现要求

- 可以做极简版，例如 `4x4` 或更小网格
- 只要有匹配与清除就行
- FREEZE 时覆盖 frost 反馈或延迟感

### 完成判定

- 小游戏能独立从开始玩到结束

---

## 2.14 WBP_AnomalyChoicePopup

### 作用

- 异常触发时的二选一弹窗

### 必做区域

- 标题
- 正文
- 两个按钮

### 必做函数

- `SetupForAnomaly`
- `HandleReportSupervisor`
- `HandleHandleMyself`

### 完成判定

- 选汇报和选自己处理都会进入正确后续

---

## 2.15 BP_HiddenDialogueUnlocker

### 作用

- 处理异常完成后给聊天界面注入隐藏选项

### 必做函数

- `UnlockForColleague`
- `MarkAnomalyHandled`

### 切片实现要求

- 先只支持 `ColleagueA`

### 完成判定

- 修空调后，A 聊天串底部出现隐藏选项

---

## 2.16 WBP_ReportEditor

### 作用

- 每天收尾的简化日报界面

### 必做区域

- 句子列表
- 选中高亮
- 提交按钮

### 切片实现要求

- Day 1-Day 3 使用固定 2-3 句选项
- 提交后进入结尾判断

### 完成判定

- 选句、提交、进入结尾稳定可用

---

## 2.17 WBP_EndingTitleCard

### 作用

- 简化结尾卡片

### 必做区域

- 标题
- 结尾字幕区
- 重试/退出按钮

### 切片实现要求

- 至少支持 `Good` / `Bad` 两套文本

### 完成判定

- 主流程结束后能稳定显示结尾

---

## 2.18 BPI_Interactable

### 作用

- 所有交互物统一接口

### 必做函数

- `CanInteract`
- `GetInteractionText`
- `Interact`

### 切片实现要求

- 电脑和空调都接这套接口

### 完成判定

- 玩家角色无需知道具体交互物类型也能调用

---

## 3. 推荐开发顺序

实际做的时候按下面顺序最稳：

1. `BPI_Interactable`
2. `BP_SignalGameMode`
3. `BP_SignalPlayerController`
4. `BP_SignalPlayerCharacter`
5. `BP_SignalGameFlowManager`
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
16. `BP_RouteStateManager`
17. `WBP_ReportEditor`
18. `WBP_EndingTitleCard`

---

## 4. 每个对象的最低完成标准

不是“做得漂亮”，而是：

- 有变量
- 有输入
- 有事件
- 能参与主线闭环

如果 18 个对象全都达到这个标准，切片版就能交。
