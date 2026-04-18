# Signal Project 蓝图职责与通信关系

> 目标：在“蓝图类清单”基础上，进一步明确职责边界，防止 Unreal 项目常见的 Manager 重叠和 Widget/Actor 互相绑死。

---

## 1. 总体原则

必须遵守三条规则：

1. `Manager` 决定流程，不直接画 UI
2. `Widget` 负责显示和输入，不负责推进核心剧情状态
3. `World Actor` 负责自己的交互和表现，不负责全局游戏流程

---

## 2. 核心蓝图职责边界

## 2.1 BP_SignalGameFlowManager

### 负责

- 当前天数
- 当前主状态 `E_GamePhase`
- 当天流程推进
- 状态切换统一入口
- 检查是否进入 Reveal / Ending

### 不负责

- 不直接操作具体 Widget 布局细节
- 不直接控制具体交互物件动画
- 不直接生成聊天消息内容

### 依赖

- `BP_RouteStateManager`
- `BP_AnomalyManager`
- `BP_ReportManager`
- `BP_SignalHUDManager`

---

## 2.2 BP_RouteStateManager

### 负责

- 记录玩家走了多少次汇报路线
- 记录已解锁多少隐藏 Skill
- 判断好结局/坏结局条件相关的中间状态

### 不负责

- 不决定具体何时播结局演出

### 通信

- 被 `BP_SignalGameFlowManager` 查询
- 被 `BP_HiddenDialogueUnlocker` 更新

---

## 2.3 BP_AnomalyManager

### 负责

- 异常积累
- 异常触发判断
- 当前异常类型
- 通知 UI 和世界层做异常表现

### 不负责

- 不直接控制小游戏具体玩法规则
- 不直接负责房间中每个 Actor 如何表现

### 通信

- 向 `BP_AnomalyEffectRouter` 广播
- 向 `BP_AnomalyChoiceDirector` 发起分支选择
- 接收 `BP_ChatConversationManager` 的副作用累计事件

---

## 2.4 BP_AnomalyEffectRouter

### 负责

- 把“异常类型”拆发给：
  - 世界效果
  - UI overlay
  - 当前小游戏

### 不负责

- 不决定异常何时开始和结束

---

## 2.5 BP_RoomStateManager

### 负责

- 3D 房间灯光态
- 房间环境材质态
- 异常期间世界层反馈

### 不负责

- 不决定剧情走向
- 不决定隐藏对话是否解锁

### 通信

- 接收 `BP_AnomalyEffectRouter`
- 接收 `BP_DayStateDirector`

---

## 2.6 BP_DayStateDirector

### 负责

- 每一天的房间基础状态
- 非异常下的日级别变化：布景、光照、压迫感升级

### 不负责

- 不负责即时异常

### 边界说明

- `BP_DayStateDirector` 管“这一天的静态基线”
- `BP_RoomStateManager` 管“当前异常和瞬时状态”

---

## 2.7 BP_ChatConversationManager

### 负责

- 对话线程数据
- 发送消息后插入回复
- 根据同事 ID 累积 anomaly side effect
- 决定隐藏选项何时注入

### 不负责

- 不直接绘制聊天 UI
- 不直接决定全局状态跳转

### 通信

- Widget 输入发给它
- 它把数据交给 `WBP_ChatApp`
- 它向 `BP_AnomalyManager` 报告 side effect

---

## 2.8 WBP_ChatApp

### 负责

- 展示聊天列表和消息线程
- 接收玩家点击和输入
- 调用 `BP_ChatConversationManager`

### 不负责

- 不持有全局剧情逻辑
- 不自己计算异常

---

## 2.9 BP_MinigameManager

### 负责

- 挑选当前小游戏
- 加载对应小游戏 Widget
- 判断小游戏完成 / 失败 / 中断

### 不负责

- 不负责异常触发条件本身

### 通信

- 被 `BP_SignalGameFlowManager` 调用开始
- 被 `BP_AnomalyManager` 中断至 `AnomalyChoice`

---

## 2.10 BP_ReportManager

### 负责

- 日报选项池
- 句子解锁规则
- 提交结果判定

### 不负责

- 不负责最终结局演出

---

## 2.11 BP_HiddenDialogueUnlocker

### 负责

- 在玩家成功自己处理异常后
- 为对应同事注入隐藏选项
- 更新 route/skill 解锁状态

### 不负责

- 不负责异常触发本身

---

## 2.12 BP_SupervisorRevealDirector

### 负责

- Day 6 揭露演出流程
- UI 破损、标签显示、主管头像变化

### 不负责

- 不负责整个游戏的天数推进

---

## 2.13 BP_SignalPlayerController

### 负责

- 输入模式切换
- 鼠标显示/隐藏
- UI 焦点切换
- 角色输入禁用/恢复

### 不负责

- 不决定剧情分支

---

## 2.14 BP_ComputerTerminal / BP_AirConditionerUnit 等交互物

### 负责

- 接收玩家交互
- 执行自身表现
- 向流程层发送“我被使用了”的事件

### 不负责

- 不保存全局状态
- 不决定后续剧情是否进入结局

---

## 3. 推荐通信方向

推荐使用这个方向：

```text
Player Input
 -> PlayerController
 -> Widget or Interactable
 -> Manager Layer
 -> GameFlowManager / State Managers
 -> back to UI / World presentation
```

不要让通信变成：

- Widget 直接强改 World Actor
- World Actor 直接创建结局 UI
- 小游戏 Widget 直接改全局天数

---

## 4. 常见通信案例

## 4.1 玩家点击发送聊天消息

```text
WBP_ChatApp
 -> BP_ChatConversationManager.SendMessage()
 -> BP_AnomalyManager.Accumulate()
 -> WBP_ChatApp refresh thread
```

## 4.2 小游戏中触发异常

```text
BP_MinigameManager / minigame widget
 -> BP_AnomalyManager.TriggerAnomaly()
 -> BP_AnomalyChoiceDirector.OpenChoice()
 -> WBP_AnomalyChoicePopup shown
```

## 4.3 玩家选择自己处理

```text
WBP_AnomalyChoicePopup
 -> BP_SignalGameFlowManager.RequestPhaseChange(HandleAnomaly3D)
 -> PlayerController switch input mode
 -> RoomStateManager enable anomaly world state
```

## 4.4 玩家修好空调

```text
BP_AirConditionerUnit.Interact()
 -> BP_AnomalyManager.ResolveCurrentAnomaly()
 -> BP_HiddenDialogueUnlocker.UnlockForColleague(A)
 -> BP_SignalGameFlowManager return to DesktopIdle
```

---

## 5. 48 小时切片的最小职责骨架

切片版本最少先建这些：

- `BP_SignalGameFlowManager`
- `BP_SignalPlayerController`
- `BP_SignalPlayerCharacter`
- `BP_ComputerTerminal`
- `BP_AirConditionerUnit`
- `BP_AnomalyManager`
- `BP_RouteStateManager`
- `BP_ChatConversationManager`
- `BP_MinigameManager`
- `BP_ReportManager`
- `WBP_DesktopRoot`
- `WBP_ChatApp`
- `WBP_AnomalyChoicePopup`
- `WBP_ReportEditor`

这套就够打通第一版闭环。
