# Signal Project 游戏状态机

> 目标：把 Unreal 中最容易失控的“模式切换”和“UI/世界联动”先定义清楚。

---

## 1. 状态机设计原则

这个项目最核心的问题不是单个功能难，而是：

- 3D 房间和 2D 桌面不断切换
- 异常会同时影响 UI、小游戏、房间环境
- 同一个时刻不能让多个系统抢输入权

所以状态机必须做到：

- 任一时刻只有一个主状态拥有输入控制权
- UI 状态和世界状态分层，但由主状态统一调度
- 所有分支进入和退出条件都明确

---

## 2. 主状态列表

完整项目主状态建议如下：

1. `Boot`
2. `RoomExplore`
3. `DesktopIdle`
4. `ChatActive`
5. `MinigameActive`
6. `AnomalyChoice`
7. `HandleAnomaly3D`
8. `ReportPhase`
9. `RevealSequence`
10. `EndingSequence`
11. `Paused`

---

## 3. 主状态定义

## 3.1 Boot

### 作用

- 开场
- 展示启动/登录/初始过场
- 初始化运行数据

### 进入条件

- 进入主关卡后自动进入

### 退出条件

- 初始化完成
- 玩家进入可操控状态

### 输入规则

- 可接受 `继续 / 跳过`
- 不允许角色移动

### 显示内容

- 开场界面
- 初始字幕/系统启动 UI

---

## 3.2 RoomExplore

### 作用

- 玩家在 3D 房间中自由查看和移动
- 寻找可交互物件

### 进入条件

- Boot 结束
- 从 Desktop 退出
- 从 HandleAnomaly3D 返回正常可移动状态

### 退出条件

- 玩家与电脑交互，进入 Desktop
- 进入特定剧情演出

### 输入规则

- 开启角色移动
- 开启视角转动
- 开启交互键
- 关闭鼠标 UI 控制

### 显示内容

- 3D 房间
- 世界交互提示

---

## 3.3 DesktopIdle

### 作用

- 玩家进入电脑模式，但当前不处于高强度交互流程
- 作为桌面主 Hub

### 进入条件

- 玩家在 RoomExplore 中使用电脑
- Chat / Report / Minigame 结束后回到桌面主界面

### 退出条件

- 打开聊天进入 ChatActive
- 启动小游戏进入 MinigameActive
- 打开日报进入 ReportPhase
- 主动退出桌面返回 RoomExplore

### 输入规则

- 锁定角色移动
- 开启鼠标
- UI 获得输入焦点

### 显示内容

- 桌面背景
- 顶部 HUD
- 可打开的应用入口

---

## 3.4 ChatActive

### 作用

- 与同事/主管聊天
- 推进剧情
- 积累 anomaly side effect

### 进入条件

- 从 DesktopIdle 打开聊天应用

### 退出条件

- 关闭聊天回 DesktopIdle
- 聊天驱动小游戏开始，进入 MinigameActive

### 输入规则

- 只允许 UI 输入
- 不允许 3D 移动

### 显示内容

- 聊天界面
- 线程列表
- 消息流
- 输入区/发送区/隐藏选项

---

## 3.5 MinigameActive

### 作用

- 运行当天小游戏
- 允许 anomaly 在此阶段触发

### 进入条件

- ChatActive 或 DesktopIdle 中启动小游戏

### 退出条件

- 小游戏正常完成 -> ReportPhase 或 DesktopIdle
- 触发异常 -> AnomalyChoice

### 输入规则

- 只允许小游戏输入
- 禁止角色移动
- 禁止其它桌面 UI 抢焦点

### 显示内容

- 小游戏根界面
- 计时/分数/干扰效果

---

## 3.6 AnomalyChoice

### 作用

- 在异常触发后暂停当前小游戏
- 让玩家在 `汇报主管 / 自己处理` 中做选择

### 进入条件

- MinigameActive 中 anomaly 达到触发条件

### 退出条件

- 选择汇报主管 -> 回 MinigameActive 或后续 ReportPhase
- 选择自己处理 -> HandleAnomaly3D

### 输入规则

- 只允许弹窗 UI 输入
- 底层小游戏暂停

### 显示内容

- 异常选择弹窗
- 异常标题、异常描述、两项按钮

---

## 3.7 HandleAnomaly3D

### 作用

- 玩家离开桌面，回到 3D 房间处理异常

### 进入条件

- AnomalyChoice 中选择 `自己处理`

### 退出条件

- 成功处理目标交互物 -> DesktopIdle 或 ChatActive
- 可选：放弃/失败 -> 负反馈后回 DesktopIdle

### 输入规则

- 开启角色移动与交互
- 关闭桌面 UI
- 保留必要 HUD/提示

### 显示内容

- 3D 房间异常态
- 交互提示
- 目标物件高亮或引导

---

## 3.8 ReportPhase

### 作用

- 提交当日汇报
- 正式推进到下一日或终局

### 进入条件

- 当日小游戏完成
- 当日异常链完成

### 退出条件

- 报告提交完成 -> DesktopIdle / DayTransition / Reveal / Ending

### 输入规则

- UI 输入独占

### 显示内容

- 日报界面
- 预设句选项
- 提交按钮
- 状态反馈

---

## 3.9 RevealSequence

### 作用

- 播放关键剧情揭露
- 暂停常规交互

### 进入条件

- 达到 Day 6 揭露条件

### 退出条件

- 揭露演出完成 -> DesktopIdle / ReportPhase / EndingSequence

### 输入规则

- 默认只允许 `继续/跳过`

### 显示内容

- 主管揭露 UI
- glitch 叠层
- 头像与标签变化

---

## 3.10 EndingSequence

### 作用

- 播放好结局或坏结局

### 进入条件

- 满足任一结局条件

### 退出条件

- 返回主菜单或重开

### 输入规则

- 只允许继续/跳过/重试

### 显示内容

- 结局卡片
- 结局字幕
- 最终房间/系统表现

---

## 3.11 Paused

### 作用

- 任意时刻暂停

### 进入条件

- 玩家主动暂停

### 退出条件

- 恢复

### 输入规则

- 暂停菜单接管输入

---

## 4. 核心状态跳转关系

```text
Boot
 -> RoomExplore

RoomExplore
 -> DesktopIdle

DesktopIdle
 -> ChatActive
 -> MinigameActive
 -> ReportPhase
 -> RoomExplore

ChatActive
 -> DesktopIdle
 -> MinigameActive

MinigameActive
 -> AnomalyChoice
 -> ReportPhase
 -> DesktopIdle

AnomalyChoice
 -> MinigameActive       (汇报主管)
 -> HandleAnomaly3D      (自己处理)

HandleAnomaly3D
 -> DesktopIdle
 -> ChatActive

ReportPhase
 -> DesktopIdle
 -> RevealSequence
 -> EndingSequence

RevealSequence
 -> DesktopIdle
 -> ReportPhase
 -> EndingSequence

EndingSequence
 -> Boot / MainMenu
```

---

## 5. 输入权规则

这个项目必须明确输入所有权：

- `RoomExplore` 和 `HandleAnomaly3D`：PlayerController 交给 3D 角色
- `DesktopIdle`、`ChatActive`、`MinigameActive`、`AnomalyChoice`、`ReportPhase`：PlayerController 交给 UI
- `RevealSequence`、`EndingSequence`：演出控制器优先，玩家只有跳过权限

绝对不要让“桌面 UI 还开着，但角色又能动”。

---

## 6. 子系统暂停规则

### 当进入 MinigameActive 时

- 聊天列表暂停交互
- 房间移动暂停

### 当进入 AnomalyChoice 时

- 小游戏暂停
- 桌面其它应用不可点击

### 当进入 HandleAnomaly3D 时

- 桌面 UI 完全隐藏
- 只保留必要世界引导

### 当进入 RevealSequence / EndingSequence 时

- 所有普通交互暂停

---

## 7. 48 小时切片实际使用状态

切片版本真正必须实现的状态只有：

- `Boot`
- `RoomExplore`
- `DesktopIdle`
- `ChatActive`
- `MinigameActive`
- `AnomalyChoice`
- `HandleAnomaly3D`
- `ReportPhase`
- `EndingSequence`

`RevealSequence` 可以作为简化剧情卡处理。

---

## 8. 推荐实现方式

### 主状态来源

- 使用 `BP_SignalGameFlowManager` 保存当前主状态
- 使用 `E_GamePhase` 枚举驱动状态切换

### 切换逻辑

- 所有主状态切换必须走统一入口，比如 `RequestPhaseChange(NewPhase)`
- 切换时统一处理：
  - 输入模式
  - 鼠标显示
  - UI 栈显隐
  - 世界 Actor 状态

这能避免蓝图到处乱切状态。
