# Signal Project 还需要的基础文档

> **历史文档说明（Historical / stale gap list）**
> - 这份文档记录的是项目早期的“缺哪些基础文档”的历史判断。
> - 它 **不是当前版本的 active backlog，也不是 48 小时竖切片的 authority doc list**。
> - 文中提到的许多关键文档现在都已经存在并被冻结，因此不要再把本文当成“仍待补齐的当前任务清单”。
> - 当前切片开工请优先看：
>   - `docs/vertical-slice-scope.md`
>   - `docs/game-state-machine.md`
>   - `docs/interaction-spec.md`
>   - `docs/vertical-slice-18-blueprints-implementation-spec.md`
>   - `docs/vertical-slice-blueprint-wiring-order.md`
>   - `docs/blueprint-variables-events-and-data-fields.md`
>   - `docs/day1-day3-datatable-ready-script.md`
>
> 目标：在已经有 `美术清单` 和 `项目结构 + 蓝图类清单` 的基础上，补齐真正能支撑立项、分工、实现、验收的核心文档。
> 原则：优先补“会卡开发”的，不优先补“好看但不影响落地”的。

---

## 0. 当前状态（读这份文档前先看）

### 0.1 本文为什么现在是历史文档

本文写成时，项目还缺少许多“执行型文档”。在当前仓库状态下，这些核心缺口已经大幅被补齐，所以本文的主要价值变成了：

- 记录当时为什么需要补文档
- 作为历史背景说明哪些问题曾经卡过开工
- 帮助后来者理解文档体系是如何长出来的

### 0.2 现在已经存在、不要再当成缺失项的文档

以下文档现在已经存在，不应继续被视为“待补齐”：

- `vertical-slice-scope.md`
- `day-by-day-content-sheet.md`
- `game-state-machine.md`
- `ui-wireframes-and-screen-flow.md`
- `blueprint-responsibility-map.md`
- `interaction-spec.md`
- 以及后续补充的 slice wiring / data / implementation docs

### 0.3 现在该怎么用这份文档

- **可以用**：理解历史缺口、回看文档优先级思路
- **不要用**：驱动当前 slice scope、决定当前 must-do backlog、覆盖已冻结的 slice 契约

---

## 1. 当前已有文档

目前 `signalproject` 里已经有：

- `art-asset-checklist.md`：完整内容量美术资产清单
- `project-structure-and-blueprint-list.md`：Unreal 项目结构和完整蓝图类清单

这两份文档已经解决了两件事：

- 知道大概要做多少内容
- 知道工程大概怎么搭

但还没有解决下面这些会直接卡住开发的问题：

- 先做什么，后做什么
- 7 天内容每天具体发生什么
- 哪些系统状态会互相影响
- UI 页面到底长什么样，怎么跳转
- 蓝图之间谁驱动谁
- 48 小时版本和完整版到底怎么切分

---

## 2. 历史上必须补齐的基础文档（现多已完成）

这是项目早期认为最核心、最值得优先补齐的 6 份文档。现在阅读时，应把它们理解为 **历史缺口记录**，而不是当前待办清单。

### 2.1 48 小时竖切片范围文档

建议文件名：`vertical-slice-scope.md`

作用：

- 定义 `48 小时版本` 到底做哪些功能
- 明确哪些是必须完成，哪些是时间够再做
- 防止团队开发过程中不断加需求

必须回答的问题：

- 本次只做 `1 个异常 + 1 个小游戏 + 1 条隐藏对话`，还是做更多
- 是否做完整 `Day 1-Day 7`，还是只做 `Day 1-Day 3` 的可玩切片
- 是否包含好坏结局
- 哪些系统只做假数据和演出，不做完整逻辑

没有这份文档，项目最容易死于范围失控。

### 2.2 Day 1-Day 7 内容总表

建议文件名：`day-by-day-content-sheet.md`

作用：

- 把叙事、玩法、异常、UI、演出内容全部落到“每天发生什么”
- 让程序、叙事、美术知道每一天分别要支持哪些内容

每一天至少要写清楚：

- 主管开场内容
- 当天同事聊天主题
- 当天小游戏类型
- 是否触发异常
- 异常后分支点
- 自己处理后的隐藏对话内容
- 当天收尾和日报内容
- 是否有环境变化或视觉升级

这是完整项目内容设计里最缺的一份执行型文档。

### 2.3 游戏状态机与流程图文档

建议文件名：`game-state-machine.md`

作用：

- 定义整个游戏有哪些主状态
- 定义状态之间如何切换
- 让蓝图逻辑不会越做越乱

至少要包含：

- `Room Mode`
- `Desktop Mode`
- `Chat`
- `Minigame`
- `Anomaly Choice`
- `3D Handle Anomaly`
- `Report`
- `Reveal`
- `Ending`

还要写清楚：

- 进入条件
- 退出条件
- 可用输入
- 是否暂停别的系统
- 哪些 UI 层显示，哪些隐藏

这份文档对程序最重要，尤其是 Unreal 蓝图实现阶段。

### 2.4 UI 线框与页面流文档

建议文件名：`ui-wireframes-and-screen-flow.md`

作用：

- 定义所有关键页面的版式和信息层级
- 定义页面之间怎么跳转
- 帮助 UI 美术和 UMG 实现统一口径

至少需要覆盖：

- 开场/启动界面
- 电脑桌面主界面
- 聊天界面
- 三个小游戏界面
- 异常选择弹窗
- 日报编辑器
- 主管揭露界面
- 结局界面

这份如果不写，后面 UI 会边做边改，返工非常大。

### 2.5 蓝图职责与通信关系文档

建议文件名：`blueprint-responsibility-map.md`

作用：

- 在已经有“蓝图类清单”的基础上，再补一层“谁负责什么、谁调用谁”
- 防止后面多个 Manager 职责重叠

要写清楚：

- `BP_SignalGameFlowManager` 负责什么，不负责什么
- `BP_AnomalyManager` 负责什么，不负责什么
- `BP_ChatConversationManager` 和 `WBP_ChatApp` 的边界
- `BP_RoomStateManager` 和 `BP_DayStateDirector` 的边界
- Widget 和 World Actor 之间通过谁通信

这份文档能大幅降低 Unreal 项目后期“蓝图互相绑死”的风险。

### 2.6 交互清单与判定规则文档

建议文件名：`interaction-spec.md`

作用：

- 规定玩家在 3D 房间和 2D 界面里到底能做哪些操作
- 规定每个交互物的触发条件和反馈规则

至少要列出：

- 靠近电脑如何进入桌面模式
- 退出桌面模式的条件
- 空调、灯、磁盘清理物件如何交互
- 交互距离、提示文案、判定时机
- 失败反馈、成功反馈、重复交互限制

这份文档会直接影响角色蓝图、交互接口和 UI 提示设计。

---

## 3. 第二优先级文档（历史建议）

这些是项目早期的“第二批建议文档”。它们可以继续作为参考，但不应覆盖当前已经存在的权威切片文档。

### 3.1 叙事台词总表

建议文件名：`narrative-script-sheet.md`

包含：

- 同事 A/B/C 普通回复池
- 隐藏对话完整文本
- 主管日常台词
- 汇报反馈台词
- 揭露段文本
- 结局字幕

### 3.2 音频需求清单

建议文件名：`audio-asset-checklist.md`

包含：

- 环境底噪
- UI 点击音
- 异常触发音
- glitch 音效
- 主管语音
- 结局音频需求

### 3.3 关卡与布景说明文档

建议文件名：`room-layout-and-set-dressing.md`

包含：

- 房间布局草图
- 电脑、空调、灯、磁盘清理交互点的位置
- 摄像机朝向和关键构图
- 7 天状态变化下的房间布景差异

### 3.4 测试用例与验收文档

建议文件名：`qa-checklist.md`

包含：

- 主流程是否可从头到尾跑通
- 分支是否正常进入
- UI 和 3D 模式切换是否正常
- 异常处理完成后隐藏对话是否解锁
- 结局触发条件是否正确

---

## 4. 不建议现在优先写的文档

下面这些可以以后再补，现在优先级不高：

- 很正式的发行计划文档
- 商业化文档
- 成就系统设计文档
- 本地化完整规范
- 平台适配长文档
- 复杂存档系统设计

这些对当前“先把项目立起来”帮助不大。

---

## 5. 推荐补文档顺序（历史记录）

如果按项目早期真正能推进项目的顺序，当时建议这样补：

1. `vertical-slice-scope.md`
2. `day-by-day-content-sheet.md`
3. `game-state-machine.md`
4. `ui-wireframes-and-screen-flow.md`
5. `blueprint-responsibility-map.md`
6. `interaction-spec.md`
7. `narrative-script-sheet.md`
8. `qa-checklist.md`

前 4 份补齐之后，项目基本就具备“可以正式拆工”的基础。

---

## 6. 最小立项文档包（历史建议）

如果站在项目早期、文档尚未补齐时，建议至少包含这 6 份：

- `art-asset-checklist.md`
- `project-structure-and-blueprint-list.md`
- `vertical-slice-scope.md`
- `day-by-day-content-sheet.md`
- `game-state-machine.md`
- `ui-wireframes-and-screen-flow.md`

有了这 6 份，程序、美术、叙事就都能开始动起来。

---

## 7. 结论（以当前仓库状态重读）

本文最初想表达的是：项目最缺的不是“更多灵感型文档”，而是“执行型文档”。

这个判断本身仍然成立；但在当前仓库状态下，相关核心执行文档已经基本补齐，所以本文不应再被理解为当前 gap list。

真正会决定项目能不能做出来的，是这几类文档：

- 范围切分
- 日内容表
- 状态机
- UI 页面流
- 蓝图职责边界
- 交互规则

如果回到本文写成当时，我会建议下一步直接补：

1. `vertical-slice-scope.md`
2. `day-by-day-content-sheet.md`
3. `game-state-machine.md`

而在当前仓库状态下，这三份文档已经存在并被后续 slice 契约文档补强，所以这里应理解为历史建议，而不是当前待办。
