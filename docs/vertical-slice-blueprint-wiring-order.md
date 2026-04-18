# Signal Project 第一个可运行版本蓝图连线顺序

> 目标：把 48 小时切片版真正需要的 Blueprint 连线顺序写成施工说明。
> 原则：先打通最短主链，再补表现。

---

## 1. 这份文档解决什么问题

前面的文档已经定义了：

- 要建哪些 Blueprint
- 每个 Blueprint 负责什么
- 主关卡怎么搭

这份文档再往前一步，解决的是：

- 先连哪个事件
- 哪几个 Blueprint 必须先互相拿到引用
- 每个阶段连完后应该怎么验收
- 如果只剩 48 小时，最短主链怎么通

这份文档只关注 `切片版第一条可运行主线`：

```text
进房间
 -> 看见电脑提示
 -> 按 E 进入桌面
 -> 打开聊天
 -> 发送消息
 -> 启动小游戏
 -> FREEZE 触发
 -> 弹异常选择
 -> 选自己处理
 -> 回房间
 -> 修空调
 -> 回电脑
 -> 出现隐藏选项
 -> 播隐藏对话
 -> 打开日报
 -> 提交
 -> 显示结尾
```

---

## 2. 最小连线对象清单

这份顺序只使用以下对象：

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

连线时遵守这几条：

1. 永远先打通引用，再写细节逻辑
2. 永远先让按钮/按键有反应，再让它变正确
3. 永远先让流程跑完，再补特效和 polish
4. 尽量让流程切换都经过 `BP_SignalGameFlowManager`
5. 不要让 Widget 直接控制关卡里具体 Actor

---

## 4. 施工顺序总览

按这个顺序搭：

1. `GameMode -> PlayerController -> Character` 基础运行
2. `Character -> Interactable` 房间交互
3. `Computer -> PlayerController -> DesktopRoot` 进入桌面
4. `DesktopRoot -> ChatApp -> ChatConversationManager` 聊天主链
5. `ChatApp -> MinigameManager -> DependencyMatch` 小游戏打开
6. `ChatConversationManager -> AnomalyManager` 聊天积累异常
7. `MinigameManager -> AnomalyManager -> ChoicePopup` 触发 FREEZE
8. `ChoicePopup -> GameFlowManager -> PlayerController` 回房间处理
9. `AirConditionerUnit -> AnomalyManager -> HiddenDialogueUnlocker` 处理异常
10. `HiddenDialogueUnlocker -> ChatConversationManager -> ChatApp` 注入隐藏选项
11. `DesktopRoot -> ReportEditor -> RouteStateManager -> EndingTitleCard` 收尾

---

## 5. 详细连线步骤

## Step 1：GameMode、PlayerController、Character 基础运行

### 目标

- 开游戏能进房间
- 玩家能移动和转头

### 连线顺序

#### 1. `BP_SignalGameMode`

- 设置 `Default Pawn Class = BP_SignalPlayerCharacter`
- 设置 `Player Controller Class = BP_SignalPlayerController`

#### 2. `BP_SignalPlayerCharacter`

- 使用 First Person 模板移动即可
- 暂时不加额外逻辑，只保证移动正常

#### 3. `BP_SignalPlayerController`

- BeginPlay 时先调用 `SetRoomInputMode`
- 在 `SetRoomInputMode` 里：
  - `Set Show Mouse Cursor = false`
  - `Set Input Mode Game Only`

### 完成后验收

- 进关卡后玩家能移动
- 没有鼠标
- 输入归 3D 控制

---

## Step 2：Character 和 BPI_Interactable 打通

### 目标

- 玩家能对准物体看到交互提示
- 按 `E` 能调用交互

### 连线顺序

#### 1. 创建 `BPI_Interactable`

- 函数 `CanInteract`
- 函数 `GetInteractionText`
- 函数 `Interact`

#### 2. `BP_SignalPlayerCharacter`

实现：

- `Event Tick` 或定时器调用 `PerformInteractionTrace`
- 从摄像机位置向前 Line Trace
- 命中对象如果实现 `BPI_Interactable`：
  - 调它的 `CanInteract`
  - 若返回真，缓存到 `CurrentFocusedInteractable`
  - 通知 Controller 或 HUD 显示提示文字

#### 3. 输入绑定

- 在 Character 或 PlayerController 里绑定 `E`
- 按下时调用 `TryInteract`
- 如果 `CurrentFocusedInteractable` 有效，执行接口 `Interact`

### 临时简化建议

- 交互提示可以先直接 `Print String`
- 等后面再换成正式 Widget

### 完成后验收

- 对准电脑能看到 `Use Computer`
- 对准空调预留位也能显示提示

---

## Step 3：电脑交互进入桌面模式

### 目标

- 按 `E` 用电脑后，显示桌面 UI，锁住玩家移动

### 连线顺序

#### 1. `BP_ComputerTerminal`

实现接口：

- `CanInteract` 返回 `true`
- `GetInteractionText` 返回 `[E] Use Computer`
- `Interact` 内部：
  - `Get Player Controller`
  - Cast 到 `BP_SignalPlayerController`
  - 调用 `ShowDesktopRoot`
  - 调用 `SetDesktopInputMode`

#### 2. `BP_SignalPlayerController`

实现：

- `ShowDesktopRoot`
  - 如果 `CachedDesktopRoot` 无效：`Create Widget WBP_DesktopRoot`
  - 保存引用
  - `Add to Viewport`
- `SetDesktopInputMode`
  - `Set Show Mouse Cursor = true`
  - `Set Input Mode Game and UI` 或 `UI Only`
  - 通知 Pawn 禁用移动

#### 3. `BP_SignalPlayerCharacter`

- 增加 `SetMovementEnabled(bool)`
- 由 Controller 调用，来启停 `CharacterMovement`

### 完成后验收

- 按电脑后能看到桌面 UI
- 鼠标出现
- 玩家不再乱跑

---

## Step 4：DesktopRoot 和 ChatApp 打通

### 目标

- 进入桌面后能打开聊天界面

### 连线顺序

#### 1. `WBP_DesktopRoot`

至少放：

- 一个“Chat”按钮
- 一个内容容器 `Panel`

实现：

- `OnClicked(ChatButton)` -> 调 `OpenApp("Chat")`
- `OpenApp` 内：
  - 如果没有 `WBP_ChatApp` 实例，创建并缓存
  - 把它放到内容容器里

#### 2. `WBP_ChatApp`

- 先做最小布局：
  - 联系人列表按钮
  - 文本滚动区
  - 发送按钮

### 完成后验收

- 桌面点 Chat 后能打开聊天页

---

## Step 5：ChatApp 和 ChatConversationManager 打通

### 目标

- 点击发送后，线程区能插入玩家消息和同事回复

### 连线顺序

#### 1. `BP_ChatConversationManager`

BeginPlay 或初始化时：

- 建立默认联系人
- 给 A/B/C/Supervisor 填占位对话数据

实现函数：

- `SwitchConversation(Target)`
- `SendPlayerMessage(MessageText)`
- `AppendReply(FromId, ReplyText)`
- `GetConversationHistory(Target)`

#### 2. `WBP_ChatApp`

保存：

- `ConversationManagerRef`

连线：

- 点击联系人 -> 调 `SwitchConversation`
- 点击发送 -> 调 `SendPlayerMessage`
- 之后拉取最新历史 -> 刷新滚动区

### 最小实现建议

- 线程区先不做复杂 Bubble Widget
- 先用垂直列表文本也可以
- 只要消息顺序对

### 完成后验收

- 选中 A 发送一条消息
- 能看到玩家消息
- 能看到 A 回复

---

## Step 6：聊天发送累积异常权重

### 目标

- 每次给同事 A 发消息，都会推动 `FREEZE` 触发条件

### 连线顺序

#### 1. `BP_AnomalyManager`

实现：

- `AccumulatedFreezeWeight`
- `AccumulateSideEffect(ColleagueId, Amount)`

规则：

- 如果 `ColleagueId == A`，`AccumulatedFreezeWeight += 1`

#### 2. `BP_ChatConversationManager`

在 `SendPlayerMessage` 里：

- 先插入玩家消息
- 再调用 `BP_AnomalyManager.AccumulateSideEffect(CurrentConversationTarget, 1)`
- 再插入同事回复

### 完成后验收

- 连续给 A 发几次消息后，Freeze 权重明显增长

---

## Step 7：聊天打开小游戏

### 目标

- 从聊天或桌面触发小游戏

### 连线顺序

#### 1. `BP_MinigameManager`

实现：

- `StartMinigame(Type)`
- `CompleteMinigame`
- `PauseMinigame`

#### 2. `WBP_DesktopRoot` 或 `WBP_ChatApp`

放一个“Start Task”按钮

OnClick：

- 找到 `BP_MinigameManager`
- 调 `StartMinigame(DependencyMatch)`

#### 3. `BP_MinigameManager`

在 `StartMinigame` 里：

- `Create Widget WBP_MG_DependencyMatch`
- 加到桌面内容区或全屏层

### 完成后验收

- 点击按钮后能出现连连看界面

---

## Step 8：小游戏内触发 FREEZE

### 目标

- 小游戏启动后，根据累计权重触发异常弹窗

### 连线顺序

#### 1. `BP_MinigameManager`

在 `StartMinigame` 或小游戏运行一小段时间后：

- 调 `BP_AnomalyManager.ResolveDominantAnomaly`

#### 2. `BP_AnomalyManager`

实现：

- 如果 `AccumulatedFreezeWeight >= Threshold`
  - `CurrentAnomalyType = FREEZE`
  - `bAnomalyActive = true`
  - 广播 `OnAnomalyTriggered(FREEZE)`

#### 3. 弹窗创建

最简单做法：

- `BP_MinigameManager` 监听 `OnAnomalyTriggered`
- 创建 `WBP_AnomalyChoicePopup`
- 调 `SetupForAnomaly(FREEZE)`

### 完成后验收

- 小游戏中能弹出 FREEZE 选择框

---

## Step 9：异常选择弹窗分支连线

### 目标

- 两个按钮能进入不同流程

### 连线顺序

#### 1. `WBP_AnomalyChoicePopup`

- `HandleReportSupervisor`
- `HandleHandleMyself`

#### 2. 选“汇报主管”

连线：

- 更新 `BP_RouteStateManager.RecordRouteChoice(ReportSupervisor)`
- 移除弹窗
- 调 `BP_AnomalyManager.ResolveCurrentAnomaly`
- 返回小游戏或直接准备日报

#### 3. 选“自己处理”

连线：

- 更新 `BP_RouteStateManager.RecordRouteChoice(HandleMyself)`
- 调 `BP_SignalGameFlowManager.RequestPhaseChange(HandleAnomaly3D)`

### 完成后验收

- 两个按钮都有效
- 选择不同按钮有不同后续

---

## Step 10：从桌面回到房间处理异常

### 目标

- 选“自己处理”后退出桌面，恢复 3D 输入

### 连线顺序

#### 1. `BP_SignalGameFlowManager`

在 `RequestPhaseChange(HandleAnomaly3D)` 里：

- 找到 PlayerController
- 调 `HideDesktopRoot`
- 调 `SetRoomInputMode`

#### 2. `BP_SignalPlayerController`

- `HideDesktopRoot`：先设为 Hidden 或 RemoveFromParent
- `SetRoomInputMode`：恢复 3D 输入、隐藏鼠标、恢复角色移动

#### 3. `BP_AirConditionerUnit`

- 当 `CurrentAnomalyType == FREEZE` 时 `bIsCurrentlyAvailable = true`

### 完成后验收

- 从弹窗选自己处理后，玩家回到房间
- 鼠标消失
- 能走路

---

## Step 11：空调交互解决 FREEZE

### 目标

- 玩家与空调交互后异常结束

### 连线顺序

#### 1. `BP_AirConditionerUnit`

实现接口：

- `CanInteract` 只有在 `bIsCurrentlyAvailable == true` 时返回真
- `GetInteractionText` 返回 `[E] Adjust Air Conditioner`
- `Interact`：
  - 播一个简单反馈
  - 调 `BP_AnomalyManager.ResolveCurrentAnomaly`
  - 调 `BP_HiddenDialogueUnlocker.UnlockForColleague(A)`
  - 可选：调 `BP_SignalGameFlowManager.RequestPhaseChange(DesktopIdle)`

#### 2. `BP_AnomalyManager.ResolveCurrentAnomaly`

- `bAnomalyActive = false`
- `CurrentAnomalyType = None`

### 完成后验收

- 修空调后异常状态被清掉

---

## Step 12：修完空调后回电脑并注入隐藏选项

### 目标

- 返回桌面后，A 聊天线程出现隐藏选项

### 连线顺序

#### 1. `BP_HiddenDialogueUnlocker`

实现：

- `UnlockForColleague(A)`
- 内部创建一条 `ST_HiddenOptionRecord`
- 调 `BP_ChatConversationManager.InjectHiddenOption`
- 同时把 `ColleagueAState = AnomalySeen` 或 `Unlocked`

#### 2. `BP_ChatConversationManager`

- 把 HiddenOption 加入 `HiddenOptions`
- 广播 `OnHiddenOptionInjected`

#### 3. `WBP_ChatApp`

- 刷新当前线程
- 如果当前目标是 A，就在消息底部显示隐藏选项卡

### 完成后验收

- 回电脑打开 A 对话时，能看到 `关于那个空调……`

---

## Step 13：点击隐藏选项播放隐藏对话

### 目标

- 隐藏选项可点击并生成隐藏消息序列

### 连线顺序

#### 1. `WBP_ChatApp`

- `HandleHiddenOptionClicked(OptionId)`
- 调 `BP_ChatConversationManager.PlayHiddenDialogue(A)`

#### 2. `BP_ChatConversationManager`

- 从 `DT_HiddenDialogues` 拉 A 的序列
- 按顺序把隐藏消息插入 `ConversationHistory`

#### 3. UI 刷新

- 每插一条或一次性插完后刷新线程

### 最小实现建议

- 先不做逐字播放
- 先直接按顺序刷出 4 条即可

### 完成后验收

- 点隐藏选项后，A 的隐藏对话 4 句能显示出来

---

## Step 14：日报界面和结尾卡串起来

### 目标

- 当天流程收尾可提交并出现结尾画面

### 连线顺序

#### 1. `WBP_DesktopRoot`

- 放一个 `Report` 按钮
- 点击打开 `WBP_ReportEditor`

#### 2. `WBP_ReportEditor`

- 构建 2-3 个句子选项
- 点选后记录 `SelectedSentenceId`
- 点 `Submit`

#### 3. 提交逻辑

- 调 `BP_RouteStateManager` 查询：
  - 如果 `HandleMyselfCount > 0` -> Good 风格结尾
  - 否则 -> Bad 风格结尾

#### 4. `WBP_EndingTitleCard`

- 接收 `EndingType`
- 显示对应字幕文本

### 完成后验收

- 能从报告页进入简化结尾

---

## 6. 第一版连线完成后的完整验收

如果以下 12 条都成立，说明第一版已经能跑：

1. 进入主关卡能移动
2. 对准电脑出现提示
3. 按 E 进入桌面
4. 能打开聊天
5. 发送消息后有回复
6. 能打开小游戏
7. 小游戏中能触发 FREEZE
8. 弹窗能选自己处理
9. 回到房间后空调能交互
10. 修空调后回电脑有隐藏选项
11. 隐藏选项能播对话
12. 日报可提交并显示结尾

---

## 7. 最后建议

如果你们卡住了，不要同时修 5 个系统。

正确做法是按这条链一段段查：

- 电脑能不能进
- 聊天能不能发
- 小游戏能不能开
- 异常能不能弹
- 房间能不能回
- 空调能不能修
- 隐藏选项能不能出
- 结尾能不能收

这样你们 48 小时内最容易把主线顶出来。
