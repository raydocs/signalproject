# Signal Project 第一版 UI Widget 树结构稿

> 目标：把切片版和完整版的关键 UI 用 `Widget Tree` 结构写出来，方便 2 位美术和 1 位程序分工。
> 原则：先让层级清楚、容器清楚、谁负责哪个 Widget 清楚。

---

## 1. 团队分工建议

你们目前是：

- 2 个美术
- 1 个策划
- 1 个程序

我建议 UI 相关这样拆：

### 美术 A：系统 UI / 桌面 / 聊天

- `WBP_DesktopRoot`
- `WBP_ChatApp`
- 聊天气泡视觉
- 异常弹窗视觉
- 日报页视觉

### 美术 B：小游戏 UI / 结尾卡 / 特效覆盖层

- `WBP_MG_DependencyMatch`
- FREEZE 覆盖层
- 结尾卡视觉
- 桌面背景、图标、状态条资源

### 程序

- 建 Widget 蓝图骨架
- 搭容器和变量
- 绑定按钮和事件

### 策划

- 核对页面信息层级
- 确认文案位置和状态切换是否合理

---

## 2. UI 分层总览

推荐把 UI 分成 4 层：

1. `World Prompt Layer`
- 房间里的交互提示

2. `Desktop Root Layer`
- 桌面主界面与内部 App

3. `Modal Layer`
- 异常弹窗、系统警告、结尾前中断提示

4. `Ending Layer`
- 结局卡片

---

## 3. WBP_DesktopRoot Widget Tree

## 3.1 用途

- 桌面模式总壳子
- 承载 Chat、Report、Minigame

## 3.2 推荐树结构

```text
WBP_DesktopRoot
└── CanvasPanel RootCanvas
    ├── Image DesktopWallpaper
    ├── Border DesktopFrame
    │   └── Overlay DesktopOverlay
    │       ├── VerticalBox DesktopMain
    │       │   ├── Border TopHUDBar
    │       │   │   └── HorizontalBox TopHUDContent
    │       │   │       ├── TextBlock DayText
    │       │   │       ├── Spacer
    │       │   │       ├── TextBlock TempText
    │       │   │       ├── TextBlock BrightnessText
    │       │   │       └── TextBlock DiskText
    │       │   ├── Overlay DesktopBody
    │       │   │   ├── CanvasPanel IconLayer
    │       │   │   │   ├── Button ChatAppButton
    │       │   │   │   │   └── VerticalBox ChatIconContent
    │       │   │   │   ├── Button ReportAppButton
    │       │   │   │   │   └── VerticalBox ReportIconContent
    │       │   │   │   └── Button TaskAppButton [可选]
    │       │   │   └── Border AppContentFrame
    │       │   │       └── NamedSlot / SizeBox AppContentHost
    │       │   └── Border BottomStatusBar
    │       │       └── HorizontalBox BottomStatusContent
    │       │           ├── TextBlock StatusHintText
    │       │           └── HorizontalBox NotificationMiniArea
    │       └── CanvasPanel OverlayEffectsLayer
    │           ├── Image ScanlineOverlay
    │           ├── Image GlitchOverlay
    │           └── Image FreezeEdgeOverlay
    └── CanvasPanel NotificationLayer
        └── VerticalBox NotificationStack
```

## 3.3 程序重点变量

- `ChatAppButton`
- `ReportAppButton`
- `AppContentHost`
- `NotificationStack`
- `StatusHintText`

## 3.4 美术重点区域

- 桌面壁纸
- 顶部状态条
- 图标样式
- App 内容边框
- 叠层 glitch / frost 视觉

---

## 4. WBP_ChatApp Widget Tree

## 4.1 用途

- 聊天核心界面

## 4.2 推荐树结构

```text
WBP_ChatApp
└── Border ChatRootFrame
    └── VerticalBox ChatRoot
        ├── Border ChatHeader
        │   └── HorizontalBox HeaderContent
        │       ├── TextBlock AppTitleText
        │       ├── Spacer
        │       └── TextBlock CurrentContactText
        ├── HorizontalBox MainArea
        │   ├── Border ContactListPanel
        │   │   └── VerticalBox ContactList
        │   │       ├── Button ContactAButton
        │   │       │   └── HorizontalBox ContactARow
        │   │       ├── Button ContactBButton
        │   │       ├── Button ContactCButton
        │   │       └── Button SupervisorButton
        │   └── Border ThreadPanel
        │       └── VerticalBox ThreadArea
        │           ├── ScrollBox MessageScrollBox
        │           │   └── VerticalBox MessageListPanel
        │           └── Border InputPanel
        │               └── HorizontalBox InputArea
        │                   ├── TextBlock PresetInputLabel [或 EditableTextBox]
        │                   ├── Spacer
        │                   └── Button SendButton
        └── Border FooterHintBar
            └── TextBlock FooterHintText
```

## 4.3 消息列表建议

`MessageListPanel` 动态添加：

- 普通消息行
- 隐藏选项卡
- 隐藏对话气泡

## 4.4 程序重点变量

- `ContactAButton`
- `ContactBButton`
- `ContactCButton`
- `SupervisorButton`
- `MessageListPanel`
- `SendButton`
- `CurrentContactText`

## 4.5 美术重点区域

- 联系人列表视觉
- 当前联系人高亮
- 线程背景
- 输入条样式

---

## 5. 消息行子 Widget 建议

## 5.1 WBP_MessageBubble_Player

```text
WBP_MessageBubble_Player
└── HorizontalBox Root
    ├── Spacer
    └── Border PlayerBubble
        └── TextBlock MessageText
```

## 5.2 WBP_MessageBubble_Colleague

```text
WBP_MessageBubble_Colleague
└── HorizontalBox Root
    ├── Border AvatarStub [可选]
    └── Border ColleagueBubble
        └── TextBlock MessageText
```

## 5.3 WBP_MessageBubble_HiddenFragment

```text
WBP_MessageBubble_HiddenFragment
└── Border HiddenFragmentFrame
    └── Overlay
        ├── Image NoiseOverlay
        ├── Image AccentBar
        └── TextBlock MessageText
```

## 5.4 WBP_HiddenOptionCard

```text
WBP_HiddenOptionCard
└── Button RootButton
    └── Border CardFrame
        └── HorizontalBox CardContent
            ├── Border AccentBar
            └── TextBlock OptionLabel
```

---

## 6. WBP_MG_DependencyMatch Widget Tree

## 6.1 用途

- 切片版唯一小游戏界面

## 6.2 推荐树结构

```text
WBP_MG_DependencyMatch
└── Border GameRootFrame
    └── Overlay
        ├── VerticalBox MainLayout
        │   ├── Border HeaderBar
        │   │   └── HorizontalBox HeaderContent
        │   │       ├── TextBlock TitleText
        │   │       ├── Spacer
        │   │       ├── TextBlock TimerText
        │   │       └── TextBlock ScoreText
        │   ├── Border BoardFrame
        │   │   └── UniformGridPanel TileGrid
        │   └── Border BottomHintBar
        │       └── TextBlock HintText
        └── CanvasPanel EffectOverlayLayer
            ├── Image FreezeEdgeOverlay
            ├── Image CursorTrailFakeOverlay [可选]
            └── TextBlock FreezeWarningText
```

## 6.3 程序重点变量

- `TileGrid`
- `TimerText`
- `ScoreText`
- `HintText`
- `FreezeEdgeOverlay`

## 6.4 美术重点区域

- Header bar
- Board frame
- Tile 样式
- Freeze overlay

---

## 7. Tile 子 Widget 建议

## 7.1 WBP_MG_DependencyTile

```text
WBP_MG_DependencyTile
└── Button TileButton
    └── Overlay
        ├── Image TileBackground
        ├── Image TileIcon
        └── Border SelectedHighlight
```

程序只需要控制：

- 图标
- 选中态
- 已匹配隐藏/禁用态

---

## 8. WBP_AnomalyChoicePopup Widget Tree

## 8.1 用途

- 异常发生时的二选一弹窗

## 8.2 推荐树结构

```text
WBP_AnomalyChoicePopup
└── CanvasPanel RootCanvas
    ├── Image DimBackground
    └── Border PopupFrame
        └── VerticalBox PopupContent
            ├── TextBlock TitleText
            ├── TextBlock BodyText
            └── HorizontalBox ButtonRow
                ├── Button ReportButton
                │   └── TextBlock ReportButtonText
                └── Button HandleButton
                    └── TextBlock HandleButtonText
```

## 8.3 程序重点变量

- `TitleText`
- `BodyText`
- `ReportButton`
- `HandleButton`

## 8.4 美术重点区域

- 背景 dim
- Popup frame
- 两种按钮视觉区分

---

## 9. WBP_ReportEditor Widget Tree

## 9.1 用途

- 切片版收尾日报页

## 9.2 推荐树结构

```text
WBP_ReportEditor
└── Border ReportRootFrame
    └── VerticalBox ReportLayout
        ├── TextBlock ReportTitle
        ├── TextBlock ReportSubtitle
        ├── ScrollBox OptionsScrollBox
        │   └── VerticalBox OptionsPanel
        └── HorizontalBox BottomRow
            ├── TextBlock SelectedHintText
            ├── Spacer
            └── Button SubmitButton
                └── TextBlock SubmitButtonText
```

## 9.3 句子选项子 Widget

```text
WBP_ReportSentenceOption
└── Button OptionButton
    └── Border OptionFrame
        └── TextBlock SentenceText
```

---

## 10. WBP_EndingTitleCard Widget Tree

## 10.1 用途

- 显示切片版结尾

## 10.2 推荐树结构

```text
WBP_EndingTitleCard
└── CanvasPanel RootCanvas
    ├── Image FullscreenBackground
    └── Border EndingFrame
        └── VerticalBox EndingLayout
            ├── TextBlock EndingTitleText
            ├── VerticalBox SubtitleLinesPanel
            └── HorizontalBox ButtonRow
                ├── Button RestartButton
                └── Button QuitButton [可选]
```

## 10.3 程序重点变量

- `EndingTitleText`
- `SubtitleLinesPanel`
- `RestartButton`

---

## 11. 房间交互提示 Widget 建议

虽然切片版可以先 `PrintString`，但如果要正式一点，建议加一个轻量 Widget。

## 11.1 WBP_InteractionPrompt

```text
WBP_InteractionPrompt
└── Border PromptFrame
    └── TextBlock PromptText
```

用途：

- 房间里显示 `[E] Use Computer`
- `[E] Adjust Air Conditioner`

这部分更适合程序先占位，美术后换样式。

---

## 12. 桌面内动态内容宿主关系

推荐统一约定：

- `WBP_DesktopRoot.AppContentHost`
  - 当前打开的 App 只往这里塞

也就是：

```text
DesktopRoot
 -> AppContentHost
    -> ChatApp
    -> ReportEditor
    -> MinigameWidget
```

这样好处是：

- 程序只要切换一个容器内容
- 美术只要管一个壳子和几个内页

---

## 13. 切片版最小 UI 资产对应关系

## 程序先建空壳

- `WBP_DesktopRoot`
- `WBP_ChatApp`
- `WBP_MG_DependencyMatch`
- `WBP_AnomalyChoicePopup`
- `WBP_ReportEditor`
- `WBP_EndingTitleCard`

## 美术 A 优先接手

- `WBP_DesktopRoot`
- `WBP_ChatApp`
- `WBP_AnomalyChoicePopup`
- `WBP_ReportEditor`

## 美术 B 优先接手

- `WBP_MG_DependencyMatch`
- `WBP_MG_DependencyTile`
- `WBP_EndingTitleCard`
- `WBP_InteractionPrompt`
- 各种 overlay 图层

---

## 14. 程序实现时最容易踩的坑

### 坑 1

- 在 `WBP_ChatApp` 里直接保存全局流程状态

建议：

- Widget 只保留当前显示状态和引用

### 坑 2

- 不给 `AppContentHost`，每个 App 都自己 `AddToViewport`

建议：

- 所有桌面内页都挂在 `DesktopRoot` 的内容容器下

### 坑 3

- 消息行直接写死在 ChatApp 里，没有子 Widget

建议：

- 第一版可以简单，但至少留出“消息行是动态项”的结构

---

## 15. 第一版 UI 完成标准

当下面这些成立时，第一版 UI 结构就够用了：

- 桌面有根壳子
- Chat 能打开并显示消息
- 小游戏能显示在桌面里
- 异常弹窗能覆盖在顶部
- 日报页能独立打开
- 结尾卡能覆盖全屏

只要这几层都成立，美术后续再怎么替换样式都不会伤到程序主链。
