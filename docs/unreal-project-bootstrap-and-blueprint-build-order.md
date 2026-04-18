# Signal Project Unreal 工程初始化清单 + 全量蓝图创建顺序

> **完整版参考说明（Full-version Reference; not slice authority）**
> - 本文保留 `Signal` 完整版工程初始化思路、长期目录结构和全量蓝图创建顺序。
> - 对 `48 小时竖切片` 而言，**本文不是最高优先级实现契约**。
> - 切片开工时应优先遵循：
>   - `docs/vertical-slice-scope.md`
>   - `docs/game-state-machine.md`
>   - `docs/interaction-spec.md`
>   - `docs/vertical-slice-18-blueprints-implementation-spec.md`
>   - `docs/vertical-slice-blueprint-wiring-order.md`
>   - `docs/blueprint-variables-events-and-data-fields.md`
>   - `docs/day1-day3-datatable-ready-script.md`
> - 若本文与上述切片权威文档冲突，以切片权威文档为准。
>
> 目标：让团队可以从 0 开始，按顺序创建 Unreal 工程、目录、基础数据、核心蓝图、UI 和交互骨架。
> 原则：先搭“能跑的骨架”，再补内容和表现。

---

## 0. 竖切片开工闸门（Kickoff Gates）

### 0.1 这份文档在切片开工时怎么用

对 48 小时竖切片，本文件现在只承担三种作用：

- 提供 Unreal 工程初始化与目录搭建参考
- 提供完整版长期 build-order 总表
- 作为切片开工时的“哪些工作现在就能做 / 哪些工作必须依赖冻结契约”的闸门说明

### 0.2 现在可立即开做（Safe to Start Immediately）

在当前仓库里，Work Items 1-3 已完成，因此以下工作可以直接开始：

1. Unreal 工程创建与基础配置
2. `Content/Signal/` 目录与子目录搭建
3. 按冻结命名创建切片所需枚举、Struct、DataTable 资产
4. 创建 `Levels/Gameplay/LV_ApartmentMain` 与基础测试关卡
5. `LV_ApartmentMain` 白盒搭建：玩家出生点、基础灯光、电脑工位、空调占位
6. 创建切片 18 个对象的空壳 Blueprint / Widget
7. 按冻结 widget 名称创建桌面 / 聊天 / 报告 / 结尾的 UI shell

> 上述“命名冻结”与“最小必建项”以 `blueprint-variables-events-and-data-fields.md` 和 `vertical-slice-18-blueprints-implementation-spec.md` 为准，不以本文旧版完整版清单为准。

### 0.3 历史上被 contract freeze 阻塞、现在才允许开的工作

以下工作在 Work Items 1-3 完成前不应启动；现在因为契约已冻结，才算正式解锁：

- `CurrentPhase` 切换与状态机 wiring
- manager / widget / world actor 的引用获取与 ownership wiring
- DataTable 导入与消费方 hookup
- `FREEZE` 分支行为、隐藏对话 gate、报告与结尾判定
- anomaly / report / ending 的完整切片调用链

### 0.4 切片开工时不要从本文直接抄的内容

本文下方仍保留完整版内容，因此 **不要把以下内容直接当作切片 must-build 清单**：

- Day 4-Day 7 专用系统
- `BLACKOUT` / `DISKCLEAN` 执行链
- `BP_ReportManager`、`BP_AnomalyChoiceDirector`、`BP_SignalHUDManager` 等完整版对象
- 与切片 schema 文档不一致的旧 Struct / DataTable 名称

---

## 1. 工程初始化目标

初始化阶段要完成的不是“美术完成度”，而是：

- 工程可打开
- 主关卡可运行
- 玩家可移动
- 可进入电脑
- 可打开桌面 UI
- 核心 Manager 和枚举存在
- 后续团队可以并行开发

---

## 2. 创建工程

## 2.1 新建项目建议

- Unreal Engine 5.x
- 模板：`First Person` 或 `Blank + Starter Content`
- 推荐：`First Person`，因为切片阶段更省事

## 2.2 创建时建议配置

- Blueprint Project
- Desktop / Console
- Maximum Quality 可按机器情况调整
- Starter Content：可开，用来快速占位

### 项目名建议

- `SignalProject`

---

## 3. 第一批目录创建顺序

在 `Content/Signal/` 下按这个顺序建目录：

1. `Blueprints/`
2. `Data/`
3. `Levels/`
4. `UI/`
5. `Materials/`
6. `Meshes/`
7. `Textures/`
8. `Audio/`
9. `Niagara/`
10. `Sequences/`

接着再细分：

### Blueprints

- `Core/`
- `Framework/`
- `Player/`
- `Interactables/`
- `World/`
- `UI/`
- `Dialogue/`
- `Anomalies/`
- `Minigames/`
- `Report/`
- `Endings/`
- `Debug/`

### Data

- `Enums/`
- `Structs/`
- `DataTables/`
- `DataAssets/`

### UI

- `Widgets/`
- `Styles/`
- `Icons/`
- `Fonts/`

---

## 4. 第一批必须创建的基础资源（完整版参考）

这些资源是完整版总表里的“基础资源”参考，不等同于当前竖切片的最小必建集。

- 竖切片最小必建枚举 / Struct / DataTable：以 `docs/blueprint-variables-events-and-data-fields.md` 为准。
- 竖切片实际导入表与行内容：以 `docs/day1-day3-datatable-ready-script.md` 为准。

## 4.1 枚举创建顺序

1. `E_GamePhase`
2. `E_AnomalyType`
3. `E_ColleagueId`
4. `E_SkillUnlockState`
5. `E_RouteBranch`
6. `E_MinigameType`
7. `E_ReportResult`

## 4.2 Struct 创建顺序

1. `ST_ChatMessageRecord`
2. `ST_HiddenOptionRecord`
3. `ST_AnomalyConfig`
4. `ST_ReportSentenceOption`
5. `ST_ColleagueDefinition`
6. `ST_DayConfig`
7. `ST_EndingBeat`

## 4.3 DataTable 创建顺序

1. `DT_DayConfigs`
2. `DT_AnomalyConfigs`
3. `DT_NormalReplies`
4. `DT_HiddenDialogues`
5. `DT_ReportSentencePools`
6. `DT_EndingSubtitles`

---

## 5. 关卡初始化顺序（完整版参考；切片按权威文档裁剪）

## 5.1 第一张主关卡

创建：

- `Levels/Gameplay/LV_ApartmentMain`

## 5.2 第一张测试关卡

> 下列测试关卡是完整版 / 长期工程参考，不是当前竖切片必须一次性全部创建的列表。

创建：

- `Levels/Test/LV_Test_DesktopUI`
- `Levels/Test/LV_Test_Interactions`
- `Levels/Test/LV_Test_Minigame_DependencyMatch`

## 5.3 主关卡必须先摆的 Actor

> 这里是完整版场景初始化参考，不是当前切片的强制 must-place 清单。
> 对 48 小时竖切片，优先放置的对象以 `vertical-slice-18-blueprints-implementation-spec.md` 和相关 level / routing 文档为准；像 `BP_DayStateDirector` 这类完整版 actor 不应被误认为当前 slice 前置条件。

- 玩家出生点
- 基础灯光
- 地面和墙体占位
- 电脑工位占位
- 空调占位
- `BP_SignalGameFlowManager`
- `BP_RoomStateManager`
- `BP_DayStateDirector`

---

## 6. 全量蓝图创建顺序（完整版参考）

这里保留的是 **完整版工程** 的全量 build order，不是 48 小时竖切片的权威 must-build 顺序。

如果目标是当前竖切片实现，请优先看：

- `docs/vertical-slice-18-blueprints-implementation-spec.md`
- `docs/vertical-slice-blueprint-wiring-order.md`

## Phase 1：骨架层

### 6.1 核心框架蓝图

1. `BP_SignalGameInstance`
2. `BP_SignalGameMode`
3. `BP_SignalPlayerController`
4. `BP_SignalPlayerCharacter`
5. `BP_SignalHUDManager`
6. `BP_SignalGameFlowManager`
7. `BP_RouteStateManager`
8. `BP_AnomalyManager`
9. `BP_ReportManager`

### 目标

- 工程能启动
- 角色能动
- 有全局状态管理点

---

## Phase 2：接口层

### 6.2 接口蓝图

1. `BPI_Interactable`
2. `BPI_ComputerScreenTarget`
3. `BPI_AnomalyReactable`
4. `BPI_RoomStateListener`

### 目标

- 后续所有交互物不至于乱写

---

## Phase 3：玩家与交互层

### 6.3 玩家相关

1. `BP_InteractionTraceComponent`
2. `BP_PlayerModeComponent`
3. `BP_SignalPlayerCameraRig`
4. `BP_DeskViewAnchor`

### 6.4 第一批交互物

1. `BP_ComputerTerminal`
2. `BP_AirConditionerUnit`
3. `BP_LightControlPanel`
4. `BP_DiskCleanTerminal`

### 目标

- 玩家能交互电脑和异常物件

---

## Phase 4：世界状态层

### 6.5 世界控制蓝图

1. `BP_RoomStateManager`
2. `BP_DayStateDirector`
3. `BP_EnvironmentVariantController`
4. `BP_PostProcessStateController`
5. `BP_AmbientAudioStateController`

### 目标

- 房间能随状态变化

---

## Phase 5：桌面 UI 根层

### 6.6 根 UI 蓝图

1. `WBP_DesktopRoot`
2. `WBP_DesktopWallpaper`
3. `WBP_TopHUD`
4. `WBP_SystemNotification`
5. `WBP_SystemModal`
6. `WBP_BootSequence`
7. `WBP_ShutdownSequence`

### 目标

- 玩家进入电脑时能看到完整桌面壳子

---

## Phase 6：聊天系统层

### 6.7 聊天管理蓝图

1. `BP_ChatConversationManager`
2. `BP_ChatMessageFactory`

### 6.8 聊天 Widget

1. `WBP_ChatApp`
2. `WBP_ChatContactList`
3. `WBP_ChatThreadView`
4. `WBP_ChatInputBar`
5. `WBP_MessageBubble_Player`
6. `WBP_MessageBubble_Colleague`
7. `WBP_MessageBubble_Supervisor`
8. `WBP_MessageBubble_System`
9. `WBP_MessageBubble_HiddenFragment`
10. `WBP_HiddenOptionCard`
11. `WBP_TypingIndicator`

### 目标

- 聊天功能完整跑通

---

## Phase 7：异常系统层

### 6.9 异常流程蓝图

1. `BP_AnomalyEffectRouter`
2. `BP_AnomalyChoiceDirector`
3. `BP_HiddenDialogueUnlocker`

### 6.10 异常具体实现

1. `BP_Anomaly_FREEZE`
2. `BP_Anomaly_BLACKOUT`
3. `BP_Anomaly_DISKCLEAN`
4. `BP_FREEZE_WorldEffect`
5. `BP_BLACKOUT_WorldEffect`
6. `BP_DISKCLEAN_WorldEffect`

### 6.11 异常 UI

1. `WBP_AnomalyChoicePopup`
2. `WBP_AnomalyAlertBanner`
3. `WBP_FREEZE_Overlay`
4. `WBP_BLACKOUT_Overlay`
5. `WBP_DISKCLEAN_Overlay`

### 目标

- 异常能触发、能弹窗、能进入处理流程

---

## Phase 8：小游戏系统层

### 6.12 共享蓝图

1. `BP_MinigameManager`
2. `BP_MinigameAnomalyBridge`
3. `WBP_MinigameShell`
4. `WBP_MinigameResultPanel`

### 6.13 连连看

1. `WBP_MG_DependencyMatch`
2. `WBP_MG_DependencyTile`
3. `BP_MG_DependencyMatchController`
4. `BP_MG_DependencyLinkFX`

### 6.14 打地鼠

1. `WBP_MG_WhackABug`
2. `WBP_MG_BugTarget`
3. `BP_MG_WhackABugController`
4. `BP_MG_BugSpawnPattern`

### 6.15 模拟改 Bug

1. `WBP_MG_BugfixFinder`
2. `WBP_MG_CodeLineEntry`
3. `BP_MG_BugfixFinderController`
4. `BP_MG_CodeSnippetProvider`

### 目标

- 先连连看跑通，再扩另外两个

---

## Phase 9：日报与终局层

### 6.16 日报系统

1. `WBP_ReportEditor`
2. `WBP_ReportSentenceOption`
3. `WBP_ReportSubmitPanel`
4. `BP_PromptInjectionDirector`
5. `BP_SupervisorDetectionModel`
6. `WBP_DetectionWarningOverlay`
7. `WBP_FinalInjectionHighlight`

### 目标

- 普通日报与最终注入界面都能承接

---

## Phase 10：剧情揭露与结局层

### 6.17 叙事蓝图

1. `BP_DialogueDirector`
2. `BP_SupervisorRevealDirector`
3. `BP_EndingNarrativeDirector`
4. `BP_BadEndingDirector`
5. `BP_GoodEndingDirector`

### 6.18 对应 UI

1. `WBP_DialogueOverlay`
2. `WBP_RevealTagOverlay`
3. `WBP_DayTransitionCard`
4. `WBP_FlashbackCard`
5. `WBP_EndingTitleCard`
6. `WBP_EndingSubtitleCard`
7. `WBP_PostEndingSummary`

---

## Phase 11：调试与辅助层

### 6.19 调试蓝图

1. `BP_DebugSignalCheats`
2. `WBP_DebugPanel`
3. `BP_WidgetLayerManager`
4. `BP_GlitchFXDirector`
5. `BP_ScreenDistortionController`
6. `BP_LightingCueController`

---

## 7. 48 小时切片实际创建顺序

如果你们真的只剩 48 小时，不要全建完再做功能。

> 本节是切片 kickoff 摘要；具体对象 ownership、wiring 顺序、字段命名和 DataTable 范围，分别以切片权威文档为准。

切片实战顺序应为：

1. 按冻结命名创建枚举、Struct、DataTable
2. `BP_SignalGameMode`
3. `BP_SignalPlayerController`
4. `BP_SignalPlayerCharacter`
5. `BP_SignalGameFlowManager`
6. `BP_RouteStateManager`
7. `BP_AnomalyManager`
8. `BP_ComputerTerminal`
9. `BP_AirConditionerUnit`
10. `WBP_DesktopRoot`
11. `WBP_ChatApp`
12. `BP_ChatConversationManager`
13. `BP_MinigameManager`
14. `WBP_MG_DependencyMatch`
15. `WBP_AnomalyChoicePopup`
16. `BP_HiddenDialogueUnlocker`
17. `WBP_ReportEditor`
18. `WBP_EndingTitleCard`

这 18 个是切片版真正必须优先落地的。

---

## 8. 每阶段完成判定

## 阶段 1 完成标准

- 工程能开
- 主关卡能进
- 玩家能移动

## 阶段 2 完成标准

- 玩家能看见电脑交互提示
- 按 `E` 进入桌面模式

## 阶段 3 完成标准

- 桌面能打开聊天
- 聊天能发消息并回消息

## 阶段 4 完成标准

- 小游戏能开始和结束

## 阶段 5 完成标准

- 异常能触发弹窗
- 选自己处理能回到 3D
- 空调能解除异常

## 阶段 6 完成标准

- 回电脑后能出现隐藏选项
- 点击后能播放隐藏对话

## 阶段 7 完成标准

- 日报能提交
- 结尾能显示

---

## 9. 初始化时最容易犯的错

### 错误 1

- 先做太多表现，不先把输入切换跑通

### 错误 2

- Widget 直接改全局状态

### 错误 3

- 一个蓝图里塞聊天、异常、结局全部逻辑

### 错误 4

- 还没打通第一条主线就去做第二第三小游戏

### 错误 5

- 先追求完整 7 天，而不是先做 Day 1-Day 3 切片闭环

---

## 10. 最终建议

如果目标真的是 48 小时交一版，正确做法不是“按完整蓝图表全建完”，而是：

- 用这份文档当 **完整版参考总表**
- 但实际执行优先遵循 `切片权威文档 + 第 0 节开工闸门 + 第 7 节切片实际创建顺序`

也就是说：

- 完整表用来保证未来不会乱
- 切片权威文档用来保证当前不会跑偏
- 第 0 节闸门用来保证团队知道哪些工作现在就能开始，哪些工作必须在冻结契约之后再做
