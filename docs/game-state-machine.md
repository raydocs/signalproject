# Signal Project 游戏状态机

> **竖切片权威说明（Authoritative for Slice）**
> - 本文是 `48 小时竖切片` 的权威状态 / 跳转 / 输入归属文档。
> - 若本文与其他完整版流程文档冲突，以本文和 `docs/vertical-slice-scope.md`、`docs/interaction-spec.md` 为准。
> - 本文冻结的是 **切片运行时真正会进入的 phase**，不是完整版未来可能拥有的所有 phase。

---

## 1. 状态机设计原则

这个项目最核心的问题不是单个功能难，而是：

- 3D 房间和 2D 桌面不断切换
- 异常会同时影响 UI、小游戏、房间环境
- 同一个时刻不能让多个系统抢输入权

所以切片状态机必须做到：

- 任一时刻只有一个主状态拥有输入控制权
- 所有主状态切换都走统一入口（如 `RequestPhaseChange(NewPhase)`）
- 分支结果必须明确，不允许“这条线到底回小游戏还是直接进日报”之类的二义性
- 电脑重进、Esc 离开桌面、异常处理中断等边界都必须有固定答案

---

## 2. 竖切片实际使用的主状态

48 小时竖切片中，真正会被运行到的主状态固定为：

1. `Boot`
2. `RoomExplore`
3. `DesktopIdle`
4. `ChatActive`
5. `MinigameActive`
6. `AnomalyChoice`
7. `HandleAnomaly3D`
8. `ReportPhase`
9. `EndingSequence`

### 2.1 非切片主状态说明

以下状态即使保留在未来设计里，也 **不是本次竖切片必须进入的独立状态**：

- `RevealSequence`
- `Paused`

切片中的“揭露感 / 主管可疑感 / 真相反馈”应吸收到：

- `ChatActive`
- `ReportPhase`
- `EndingSequence`

而不是再扩一个独立的切片运行状态。

---

## 3. 各状态的权威定义

## 3.1 Boot

### 作用

- 开场
- 初始化运行数据
- 把玩家送入可操作的第一段流程

### 进入条件

- 进入主关卡后自动进入

### 退出条件

- 初始化完成，进入 `RoomExplore`

### 输入规则

- 仅允许 `继续 / 跳过`
- 不允许角色移动

---

## 3.2 RoomExplore

### 作用

- 玩家在 3D 房间中自由查看和移动
- 使用房间内可交互物（切片中重点是电脑与空调）

### 进入条件

- `Boot` 结束
- 从桌面正常退出
- 从 `HandleAnomaly3D` 结束后恢复到 3D 可行动状态

### 退出条件

- 与电脑交互，进入 `DesktopIdle`

### 输入规则

- 开启角色移动
- 开启视角转动
- 开启 3D 交互键
- 不显示桌面 UI 鼠标

---

## 3.3 DesktopIdle

### 作用

- 玩家已进入电脑，但当前不在聊天 / 小游戏 / 报告等强占输入流程中
- 作为桌面 Hub

### 进入条件

- 从 `RoomExplore` 使用电脑进入
- 从 `ChatActive` 返回
- 从 `HandleAnomaly3D` 解决异常并回到电脑后进入

### 退出条件

- 打开聊天进入 `ChatActive`
- 打开小游戏进入 `MinigameActive`
- 打开日报进入 `ReportPhase`
- 主动离开桌面回到 `RoomExplore`

### 输入规则

- 锁定角色移动
- 显示鼠标
- UI 获得焦点
- 允许执行 `Leave Desktop`（例如 `Esc`）

---

## 3.4 ChatActive

### 作用

- 与同事 / 主管聊天
- 推进剧情
- 积累异常 side effect
- 在条件满足时显示隐藏选项

### 进入条件

- 从 `DesktopIdle` 打开聊天应用

### 退出条件

- 关闭聊天回 `DesktopIdle`
- 聊天触发小游戏进入 `MinigameActive`
- 主动离开桌面回 `RoomExplore`

### 输入规则

- 只允许桌面 UI 输入
- 不允许 3D 移动
- 允许执行 `Leave Desktop`（例如 `Esc`）

---

## 3.5 MinigameActive

### 作用

- 运行切片唯一小游戏 `DependencyMatch`
- 允许 `FREEZE` 在此阶段触发

### 进入条件

- 从 `ChatActive` 或 `DesktopIdle` 启动小游戏

### 退出条件

- 小游戏完成 -> `ReportPhase`
- 触发异常 -> `AnomalyChoice`

### 输入规则

- 只允许小游戏输入
- 禁止角色移动
- 禁止其它桌面 UI 抢焦点
- 不允许 `Leave Desktop`

---

## 3.6 AnomalyChoice

### 作用

- 在 `FREEZE` 触发后中断当前小游戏
- 让玩家在 `Report Supervisor / Handle Myself` 中做选择

### 进入条件

- `MinigameActive` 中 `FREEZE` 达到触发条件

### 退出条件

- 选择 `Report Supervisor` -> 回 `MinigameActive`
- 选择 `Handle Myself` -> 进入 `HandleAnomaly3D`

### 输入规则

- 只允许弹窗 UI 输入
- 底层小游戏暂停
- 不允许 `Leave Desktop`
- 不允许关闭弹窗绕过选择

---

## 3.7 HandleAnomaly3D

### 作用

- 玩家离开桌面，回到 3D 房间亲自处理异常

### 进入条件

- `AnomalyChoice` 中选择 `Handle Myself`

### 退出条件

- 玩家与 `BP_AirConditionerUnit` 交互并成功解除 `FREEZE`
- `FREEZE` 解除后，本状态结束并回到 `RoomExplore`
- 电脑锁定解除，玩家必须手动走回电脑，才能再次进入 `DesktopIdle`

### 输入规则

- 开启角色移动与 3D 交互
- 桌面 UI 完全隐藏
- 鼠标不作为桌面输入使用
- 当异常未解除时，电脑重新进入被锁定

---

## 3.8 ReportPhase

### 作用

- 提交当日简化日报
- 锁定并落地当日路线结果
- 在 Day 3 进入简化结尾

### 进入条件

- 当日小游戏完成
- 且当日异常链已经闭合
- 若当前路线为 `HandleMyself`，则隐藏选项已消费 / 隐藏对话已完成

### 退出条件

- Day 1 / Day 2：提交完成后进入下一段日程包装
- Day 3：提交完成后进入 `EndingSequence`

### 输入规则

- UI 输入独占
- 不允许 `Leave Desktop`

---

## 3.9 EndingSequence

### 作用

- 播放切片简化好结局或坏结局反馈

### 进入条件

- `ReportPhase` 根据路线结果与切片收束条件进入

### 退出条件

- 返回主菜单或重开

### 输入规则

- 只允许 `继续 / 跳过 / 重试`

---

## 4. 竖切片权威跳转关系

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
 -> RoomExplore

MinigameActive
 -> AnomalyChoice
 -> ReportPhase

AnomalyChoice
 -> MinigameActive    (Report Supervisor)
 -> HandleAnomaly3D   (Handle Myself)

HandleAnomaly3D
 -> RoomExplore      (only after FREEZE resolved)

ReportPhase
 -> DesktopIdle / 下一日包装
 -> EndingSequence    (Day 3 slice wrap)

EndingSequence
 -> Boot / MainMenu
```

---

## 5. 分支结果与 phase 结果冻结

### 5.1 选择 `Report Supervisor`

这是切片中唯一权威的 phase 结果：

1. `WBP_AnomalyChoicePopup` 记录路线：`ReportSupervisor`
2. 当前 `FREEZE` 立即标记为已解决
3. 弹窗关闭
4. phase 回到 `MinigameActive`
5. 小游戏恢复或立即完成
6. 之后进入 `ReportPhase`
7. **不会** 进入 `HandleAnomaly3D`
8. **不会** 解锁隐藏选项

结论：

- `Report Supervisor` 分支不会直接从弹窗跳到结尾
- 也不会把 Day 2 直接跳过去
- 它只是让异常以“系统接管”的方式闭合，然后继续当日流程

### 5.2 选择 `Handle Myself`

这是切片中唯一权威的 phase 结果：

1. `WBP_AnomalyChoicePopup` 记录路线：`HandleMyself`
2. 弹窗关闭
3. phase 切到 `HandleAnomaly3D`
4. 桌面 UI 关闭，玩家回到 3D 房间
5. 在 `FREEZE` 未解决前，电脑不可重新进入
6. 玩家与空调交互并解决 `FREEZE`
7. phase 回到 `RoomExplore`，并解除电脑锁定
8. 玩家手动走回电脑，重新进入 `DesktopIdle`
9. 再进入 `ChatActive`，看到隐藏选项并完成隐藏对话
10. 再进入 `ReportPhase`

结论：

- `Handle Myself` 分支必须包含真实 3D 处理步骤
- 隐藏对话只能出现在异常解决之后
- 报告阶段必须发生在隐藏对话之后

---

## 6. 输入权规则（权威表）

| 主状态 | 输入拥有者 | 是否允许 3D 移动 | 是否允许桌面 UI | 是否允许 `Leave Desktop` | 备注 |
|---|---|---:|---:|---:|---|
| `Boot` | 开场/系统 | 否 | 限制性 | 否 | 只允许继续/跳过 |
| `RoomExplore` | 3D 角色 | 是 | 否 | 不适用 | 可与电脑/空调交互 |
| `DesktopIdle` | UI | 否 | 是 | 是 | 正常桌面状态 |
| `ChatActive` | UI | 否 | 是 | 是 | 正常桌面状态 |
| `MinigameActive` | 小游戏 | 否 | 仅小游戏 | 否 | 其它桌面 UI 不得抢焦点 |
| `AnomalyChoice` | 弹窗 | 否 | 仅弹窗 | 否 | 必须先做选择 |
| `HandleAnomaly3D` | 3D 角色 | 是 | 否 | 否 | 未解决前电脑锁定 |
| `ReportPhase` | 报告 UI | 否 | 是 | 否 | 提交前不得退出 |
| `EndingSequence` | 演出/结尾 UI | 否 | 限制性 | 否 | 只允许继续/跳过/重试 |

### 6.1 Leave Desktop 规则冻结

`Leave Desktop`（可由 `Esc` 或等价返回操作触发）在切片中的规则固定为：

- **允许**：`DesktopIdle`、`ChatActive`
- **禁止**：`MinigameActive`、`AnomalyChoice`、`ReportPhase`
- **不适用**：`RoomExplore`
- **在 `HandleAnomaly3D` 中视为无效**：因为玩家已经离开桌面，而且异常未解决前电脑被锁定

### 6.2 电脑重进规则冻结

当且仅当同时满足以下条件，玩家才能通过电脑进入桌面：

- 当前主状态为 `RoomExplore`
- 当前不存在“未解决的 `HandleAnomaly3D` 锁定”

因此：

- 正常探索时可以使用电脑
- `Handle Myself` 路线进入 3D 后，若 `FREEZE` 未解除，则 **不能** 用电脑回桌面
- `FREEZE` 解除后，电脑重新可用

---

## 7. 子系统暂停规则

### 当进入 `MinigameActive`

- 聊天列表暂停交互
- 房间移动暂停
- 桌面其它 app 不可点击

### 当进入 `AnomalyChoice`

- 小游戏暂停
- 其它桌面 UI 不可点击
- 玩家不能关闭弹窗逃避选择

### 当进入 `HandleAnomaly3D`

- 桌面 UI 完全隐藏
- 只保留必要世界提示
- 电脑交互锁定到异常解决为止

### 当进入 `ReportPhase` / `EndingSequence`

- 所有普通探索与桌面切换行为暂停

---

## 8. 切片必须遵守的状态守卫规则

以下规则在切片中视为权威实现约束：

- `RequestPhaseChange(NewPhase)` 若 `NewPhase == CurrentPhase`，则视为 `No-op`
- 当 `bAnomalyActive == true` 时，不允许再次触发新的异常
- `AnomalyChoice` 只能通过两个分支按钮退出
- `ReportPhase` 不能在“小游戏 / 异常链未闭合”时提前打开
- `HandleMyself` 路线下，隐藏选项不能在异常解决前出现
- `HandleMyself` 路线下，`ReportPhase` 不能早于隐藏对话完成
- `ReportSupervisor` 路线下，不得进入 `HandleAnomaly3D`

---

## 9. 推荐实现方式

### 主状态来源

- 使用 `BP_SignalGameFlowManager` 保存当前主状态
- 使用 `E_GamePhase` 枚举驱动状态切换

### 切换逻辑

- 所有主状态切换必须走统一入口，如 `RequestPhaseChange(NewPhase)`
- 切换时统一处理：
  - 输入模式
  - 鼠标显示
  - UI 栈显隐
  - 世界交互锁定

这样才能避免蓝图到处乱切状态，导致“桌面还开着但角色又能动”或“异常还没解决就又能回电脑”这类问题。
