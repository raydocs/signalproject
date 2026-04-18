# Signal Project Blueprint 伪节点流程图

> 目标：把 48 小时切片里最关键的 Blueprint 行为，写成接近 Unreal Blueprint 节点连线的伪流程图。
> 用法：打开 UE 蓝图编辑器时，按“事件入口 -> 分支 -> 调用 -> 状态更新 -> UI 刷新”的顺序照着搭。

---

## 1. 阅读方式

文档中的格式：

- `Event BeginPlay`、`InputAction Interact` 代表蓝图事件节点
- `Branch`、`Sequence`、`Set`、`Cast To` 代表常用蓝图节点
- `->` 代表节点执行流
- `[]` 内是关键判断或说明

这不是逐像素还原的蓝图截图，而是 `足够接近 Blueprint 连线逻辑` 的施工稿。

---

## 2. 全局第一条主链

切片版要先跑通这一整条：

```text
RoomExplore
 -> 交互电脑
 -> DesktopRoot
 -> ChatApp
 -> SendMessage
 -> 累积 FREEZE
 -> StartMinigame
 -> Trigger FREEZE
 -> AnomalyChoicePopup
 -> Handle Myself
 -> 回房间
 -> 交互空调
 -> Resolve FREEZE
 -> 注入隐藏选项
 -> 播放隐藏对话
 -> 打开 Report
 -> Submit
 -> EndingCard
```

下面所有伪节点，都是为这条主链服务。

---

## 3. BP_SignalPlayerController

## 3.1 BeginPlay 初始化

```text
Event BeginPlay
 -> Create Widget (Class = WBP_DesktopRoot)
 -> Promote to Variable [CachedDesktopRoot]
 -> Set Visibility (Hidden) [可选，如果先 Add 到 Viewport]
 -> SetRoomInputMode()
```

## 3.2 SetRoomInputMode

```text
Custom Event SetRoomInputMode
 -> Set Show Mouse Cursor = false
 -> Set Input Mode Game Only
 -> Get Controlled Pawn
 -> Cast To BP_SignalPlayerCharacter
 -> Call SetMovementEnabled(true)
 -> Set bIsInDesktopMode = false
```

## 3.3 SetDesktopInputMode

```text
Custom Event SetDesktopInputMode
 -> Set Show Mouse Cursor = true
 -> Set Input Mode Game and UI
 -> Get Controlled Pawn
 -> Cast To BP_SignalPlayerCharacter
 -> Call SetMovementEnabled(false)
 -> Set bIsInDesktopMode = true
```

## 3.4 ShowDesktopRoot

```text
Custom Event ShowDesktopRoot
 -> Branch [IsValid(CachedDesktopRoot)?]
    True:
      -> Add To Viewport [如果尚未 Add]
      -> Set Visibility(Visible)
    False:
      -> Create Widget (WBP_DesktopRoot)
      -> Promote to CachedDesktopRoot
      -> Add To Viewport
 -> Call CachedDesktopRoot.InitializeDesktopIfNeeded [可选]
```

## 3.5 HideDesktopRoot

```text
Custom Event HideDesktopRoot
 -> Branch [IsValid(CachedDesktopRoot)?]
    True:
      -> Set Visibility(Hidden) [或 Remove From Parent]
```

## 3.6 Leave Desktop 输入

```text
InputAction LeaveDesktop / Esc
 -> Branch [bIsInDesktopMode]
    True:
      -> HideDesktopRoot
      -> SetRoomInputMode
      -> Get All Actors Of Class (BP_SignalGameFlowManager) [切片阶段可先这样]
      -> Call RequestPhaseChange(RoomExplore)
```

---

## 4. BP_SignalPlayerCharacter

## 4.1 Tick 交互检测

```text
Event Tick
 -> Branch [bCanInteract]
    True:
      -> PerformInteractionTrace
```

## 4.2 PerformInteractionTrace

```text
Custom Event PerformInteractionTrace
 -> Get Follow Camera World Location
 -> Get Follow Camera Forward Vector
 -> Vector * InteractionTraceDistance
 -> LineTraceByChannel
 -> Branch [Hit Actor Valid?]
    False:
      -> Set CurrentFocusedInteractable = None
      -> Send interaction prompt empty [PrintString 或 HUD]
    True:
      -> Branch [Does Implement Interface BPI_Interactable]
          False:
            -> Set CurrentFocusedInteractable = None
          True:
            -> Interface Call CanInteract
            -> Branch [CanInteract == true]
                True:
                  -> Set CurrentFocusedInteractable = HitActor
                  -> Interface Call GetInteractionText
                  -> Show Prompt(Text)
                False:
                  -> Set CurrentFocusedInteractable = None
                  -> Clear Prompt
```

## 4.3 输入交互

```text
InputAction Interact / E
 -> Call TryInteract
```

## 4.4 TryInteract

```text
Custom Event TryInteract
 -> Branch [IsValid(CurrentFocusedInteractable)]
    True:
      -> Interface Call Interact(CurrentFocusedInteractable)
```

## 4.5 SetMovementEnabled

```text
Custom Event SetMovementEnabled(bEnabled)
 -> Set bCanMove = bEnabled
 -> Branch [bEnabled]
    True:
      -> CharacterMovement Set Movement Mode Walking
      -> Controller Ignore Move Input(false)
      -> Controller Ignore Look Input(false) [如不想锁视角]
    False:
      -> Controller Ignore Move Input(true)
```

---

## 5. BPI_Interactable

建议接口：

```text
Function CanInteract -> returns Bool
Function GetInteractionText -> returns Text
Function Interact
```

切片阶段不要加太多参数，先把共用交互跑通。

---

## 6. BP_ComputerTerminal

## 6.1 CanInteract

```text
Interface Function CanInteract
 -> Return bIsUsable
```

## 6.2 GetInteractionText

```text
Interface Function GetInteractionText
 -> Return InteractionText [默认 = "[E] Use Computer"]
```

## 6.3 Interact

```text
Interface Function Interact
 -> Get Player Controller
 -> Cast To BP_SignalPlayerController
 -> Call ShowDesktopRoot
 -> Call SetDesktopInputMode
 -> Get All Actors Of Class (BP_SignalGameFlowManager)
 -> Call RequestPhaseChange(DesktopIdle)
```

## 6.4 可选镜头切换

如果第一版要加桌面镜头切换：

```text
Interact
 -> Get Player Controller
 -> Set View Target With Blend (Target = DeskViewAnchor Owner / CameraActor)
 -> ShowDesktopRoot
 -> SetDesktopInputMode
```

第一版如果来不及，先不做镜头 blend，只做 UI 切换也可以。

---

## 7. WBP_DesktopRoot

## 7.1 Construct

```text
Event Construct
 -> Branch [bInitialized]
    False:
      -> Bind ChatButton.OnClicked -> HandleOpenChat
      -> Bind ReportButton.OnClicked -> HandleOpenReport
      -> Set bInitialized = true
```

## 7.2 打开聊天

```text
Custom Event HandleOpenChat
 -> OpenApp("Chat")
```

## 7.3 打开日报

```text
Custom Event HandleOpenReport
 -> OpenApp("Report")
```

## 7.4 OpenApp

```text
Custom Event OpenApp(AppTag)
 -> Switch on Name / Branch chain
    Chat:
      -> Branch [IsValid(ChatAppWidget)?]
          False:
            -> Create Widget(WBP_ChatApp)
            -> Promote to ChatAppWidget
      -> Clear Children(ContentPanel)
      -> Add Child(ChatAppWidget)
    Report:
      -> Branch [IsValid(ReportWidget)?]
          False:
            -> Create Widget(WBP_ReportEditor)
            -> Promote to ReportWidget
      -> Clear Children(ContentPanel)
      -> Add Child(ReportWidget)
```

## 7.5 打开小游戏容器

如果你把小游戏也挂在 DesktopRoot 里：

```text
Custom Event ShowMinigameWidget(MinigameWidget)
 -> Clear Children(ContentPanel)
 -> Add Child(MinigameWidget)
```

---

## 8. BP_ChatConversationManager

## 8.1 初始化

```text
Custom Event InitializeChatSystem
 -> Set CurrentConversationTarget = ColleagueA
 -> Clear ConversationHistory
 -> Load initial seed lines [可直接 Append 默认消息]
```

## 8.2 SwitchConversation

```text
Custom Event SwitchConversation(Target)
 -> Set CurrentConversationTarget = Target
 -> Broadcast OnConversationChanged(Target)
```

## 8.3 SendPlayerMessage

```text
Custom Event SendPlayerMessage(MessageText)
 -> Make ST_ChatMessageRecord [Player]
 -> Add to ConversationHistory
 -> Get All Actors Of Class (BP_AnomalyManager)
 -> Call AccumulateSideEffect(CurrentConversationTarget, 1.0)
 -> GetReplyTextFor(CurrentConversationTarget)
 -> AppendReply(CurrentConversationTarget, ReplyText)
 -> Broadcast OnMessagesUpdated
```

## 8.4 AppendReply

```text
Custom Event AppendReply(FromId, ReplyText)
 -> Make ST_ChatMessageRecord [NPC]
 -> Add to ConversationHistory
```

## 8.5 InjectHiddenOption

```text
Custom Event InjectHiddenOption(OptionRecord)
 -> Add OptionRecord to HiddenOptions
 -> Broadcast OnHiddenOptionInjected(OptionRecord.TargetColleague)
```

## 8.6 PlayHiddenDialogue

```text
Custom Event PlayHiddenDialogue(Target)
 -> Get Hidden Dialogue Rows for Target
 -> ForEachLoop
    -> Make ST_ChatMessageRecord [bIsHiddenFragment = true]
    -> Add to ConversationHistory
 -> Broadcast OnMessagesUpdated
```

### 切片建议

- 第一版不做复杂多线程历史分桶
- 先用单数组 + 当前联系人过滤就够

---

## 9. WBP_ChatApp

## 9.1 Construct

```text
Event Construct
 -> Branch [ConversationManagerRef Valid?]
    False:
      -> Get All Actors Of Class (BP_ChatConversationManager) [如果做成 Actor]
      -> Set ConversationManagerRef
 -> Bind Buttons:
    -> ContactAButton -> HandleContactSelected(ColleagueA)
    -> ContactBButton -> HandleContactSelected(ColleagueB)
    -> ContactCButton -> HandleContactSelected(ColleagueC)
    -> SupervisorButton -> HandleContactSelected(Supervisor)
    -> SendButton -> HandleSendClicked
 -> Bind to ConversationManagerRef.OnMessagesUpdated -> RefreshConversation
 -> Bind to ConversationManagerRef.OnConversationChanged -> RefreshConversation
 -> Call RefreshConversation
```

## 9.2 HandleContactSelected

```text
Custom Event HandleContactSelected(Target)
 -> ConversationManagerRef.SwitchConversation(Target)
 -> Set CurrentConversationTarget = Target
 -> RefreshConversation
```

## 9.3 HandleSendClicked

```text
Custom Event HandleSendClicked
 -> Get text from preset button / input box
 -> Branch [Message not empty]
    True:
      -> ConversationManagerRef.SendPlayerMessage(MessageText)
      -> Clear input [如有]
```

## 9.4 RefreshConversation

```text
Custom Event RefreshConversation
 -> Clear Children(MessageList)
 -> Get filtered messages for CurrentConversationTarget
 -> ForEachLoop
    -> CreateWidget(MessageBubble or simple TextRow)
    -> SetText / SetStyle
    -> Add Child to MessageList
 -> Get hidden options for CurrentConversationTarget
 -> ForEachLoop
    -> CreateWidget(WBP_HiddenOptionCard) [或简单按钮]
    -> Bind OnClicked -> HandleHiddenOptionClicked(OptionId)
    -> Add Child to MessageList
```

## 9.5 HandleHiddenOptionClicked

```text
Custom Event HandleHiddenOptionClicked(OptionId)
 -> ConversationManagerRef.PlayHiddenDialogue(CurrentConversationTarget)
 -> RefreshConversation
```

---

## 10. BP_AnomalyManager

## 10.1 AccumulateSideEffect

```text
Custom Event AccumulateSideEffect(ColleagueId, Amount)
 -> Switch on E_ColleagueId
    ColleagueA:
      -> Set AccumulatedFreezeWeight = AccumulatedFreezeWeight + Amount
    ColleagueB:
      -> Set AccumulatedBlackoutWeight = AccumulatedBlackoutWeight + Amount
    ColleagueC:
      -> Set AccumulatedDiskCleanWeight = AccumulatedDiskCleanWeight + Amount
```

## 10.2 ResolveDominantAnomaly

```text
Custom Event ResolveDominantAnomaly
 -> Branch [AccumulatedFreezeWeight >= FreezeThreshold]
    True:
      -> TriggerAnomaly(FREEZE)
    False:
      -> [切片阶段可直接 return None]
```

## 10.3 TriggerAnomaly

```text
Custom Event TriggerAnomaly(Type)
 -> Set CurrentAnomalyType = Type
 -> Set bAnomalyActive = true
 -> Broadcast OnAnomalyTriggered(Type)
```

## 10.4 ResolveCurrentAnomaly

```text
Custom Event ResolveCurrentAnomaly
 -> Set bAnomalyActive = false
 -> Set CurrentAnomalyType = None
 -> Set AccumulatedFreezeWeight = 0 [切片可先直接清零]
 -> Broadcast OnAnomalyResolved
```

---

## 11. BP_MinigameManager

## 11.1 StartMinigame

```text
Custom Event StartMinigame(Type)
 -> Set CurrentMinigameType = Type
 -> Branch [Type == DependencyMatch]
    True:
      -> Create Widget(WBP_MG_DependencyMatch)
      -> Promote to CurrentMinigameWidget
      -> Get Player Controller
      -> Cast to BP_SignalPlayerController
      -> CachedDesktopRoot.ShowMinigameWidget(CurrentMinigameWidget) [或直接 Add To Viewport]
      -> Set bIsMinigameRunning = true
      -> Get All Actors Of Class (BP_SignalGameFlowManager)
      -> Call RequestPhaseChange(MinigameActive)
      -> Delay(1.0) [可选]
      -> Get All Actors Of Class (BP_AnomalyManager)
      -> Call ResolveDominantAnomaly
```

## 11.2 监听异常

```text
Event BeginPlay / Initialize
 -> Get AnomalyManager Ref
 -> Bind OnAnomalyTriggered -> HandleAnomalyTriggered
```

## 11.3 HandleAnomalyTriggered

```text
Custom Event HandleAnomalyTriggered(Type)
 -> PauseMinigame
 -> Create Widget(WBP_AnomalyChoicePopup)
 -> Promote to ChoicePopupRef
 -> Add To Viewport
 -> ChoicePopupRef.SetupForAnomaly(Type)
```

## 11.4 PauseMinigame

```text
Custom Event PauseMinigame
 -> Set bIsMinigameRunning = false
 -> [如果小游戏里有 Tick，设一个 bPaused = true]
```

## 11.5 ResumeMinigame

```text
Custom Event ResumeMinigame
 -> Set bIsMinigameRunning = true
```

---

## 12. WBP_MG_DependencyMatch

## 12.1 Construct

```text
Event Construct
 -> BuildGrid
 -> StartTimer
```

## 12.2 BuildGrid

```text
Custom Event BuildGrid
 -> Generate simple shuffled pairs
 -> ForEach Cell
    -> Create Tile Widget / Button
    -> Bind OnClicked -> HandleTileClicked(TileId)
```

## 12.3 HandleTileClicked

```text
Custom Event HandleTileClicked(TileId)
 -> Branch [bPaused]
    False:
      -> Add TileId to SelectedTiles
      -> Branch [SelectedTiles length == 2]
          True:
            -> ComparePair
```

## 12.4 ComparePair

```text
Custom Event ComparePair
 -> Branch [Tiles Match]
    True:
      -> Mark tiles cleared
      -> Update score
      -> Branch [All tiles cleared]
          True:
            -> Get MinigameManager Ref
            -> CompleteMinigame(true)
    False:
      -> Clear selection after short delay
```

### FREEZE 最小效果接法

```text
OnFreezeActive
 -> Show frost overlay image
 -> Optional: Delay click response by 0.1 - 0.2s
```

---

## 13. WBP_AnomalyChoicePopup

## 13.1 SetupForAnomaly

```text
Custom Event SetupForAnomaly(Type)
 -> Set CurrentAnomalyType = Type
 -> Switch on Type
    FREEZE:
      -> Set TitleText = "温度异常"
      -> Set BodyText = "空调温度骤降至 16.4°C。鼠标操作出现迟滞。你要怎么处理？"
```

## 13.2 HandleReportSupervisor

```text
OnClicked ReportButton
 -> Get RouteStateManager Ref
 -> RecordRouteChoice(ReportSupervisor)
 -> Get AnomalyManager Ref
 -> ResolveCurrentAnomaly
 -> Remove From Parent
 -> Get All Actors Of Class (BP_SignalGameFlowManager)
 -> RequestPhaseChange(ReportPhase) [或 ResumeMinigame，切片可直接进日报]
```

## 13.3 HandleHandleMyself

```text
OnClicked HandleButton
 -> Get RouteStateManager Ref
 -> RecordRouteChoice(HandleMyself)
 -> Remove From Parent
 -> Get All Actors Of Class (BP_SignalGameFlowManager)
 -> RequestPhaseChange(HandleAnomaly3D)
```

---

## 14. BP_SignalGameFlowManager

## 14.1 BeginPlay 初始化

```text
Event BeginPlay
 -> Set CurrentDayIndex = 1
 -> Set CurrentPhase = RoomExplore
 -> Cache refs [可选]
```

## 14.2 RequestPhaseChange

```text
Custom Event RequestPhaseChange(NewPhase)
 -> Set PreviousPhase = CurrentPhase
 -> Set CurrentPhase = NewPhase
 -> HandlePhaseEntered(NewPhase)
```

## 14.3 HandlePhaseEntered

```text
Custom Event HandlePhaseEntered(NewPhase)
 -> Switch on E_GamePhase
    RoomExplore:
      -> PC.HideDesktopRoot
      -> PC.SetRoomInputMode
    DesktopIdle:
      -> PC.ShowDesktopRoot
      -> PC.SetDesktopInputMode
    MinigameActive:
      -> PC.ShowDesktopRoot
      -> PC.SetDesktopInputMode
    HandleAnomaly3D:
      -> PC.HideDesktopRoot
      -> PC.SetRoomInputMode
    ReportPhase:
      -> PC.ShowDesktopRoot
      -> PC.SetDesktopInputMode
      -> PC.CachedDesktopRoot.OpenApp("Report")
    EndingSequence:
      -> PC.ShowDesktopRoot [可选]
```

## 14.4 TriggerEnding

```text
Custom Event TriggerEnding(EndingType)
 -> RequestPhaseChange(EndingSequence)
 -> Create Widget(WBP_EndingTitleCard) [如果未缓存]
 -> Setup Ending Data
 -> Add To Viewport
```

---

## 15. BP_AirConditionerUnit

## 15.1 CanInteract

```text
Interface Function CanInteract
 -> Return bIsCurrentlyAvailable
```

## 15.2 GetInteractionText

```text
Interface Function GetInteractionText
 -> Return "[E] Adjust Air Conditioner"
```

## 15.3 Interact

```text
Interface Function Interact
 -> Branch [bIsCurrentlyAvailable]
    True:
      -> PlayRepairFeedback
      -> Get AnomalyManager Ref
      -> ResolveCurrentAnomaly
      -> Get HiddenDialogueUnlocker Ref
      -> UnlockForColleague(ColleagueA)
      -> Set bIsCurrentlyAvailable = false
      -> Get GameFlowManager Ref
      -> RequestPhaseChange(DesktopIdle)
```

## 15.4 PlayRepairFeedback

```text
Custom Event PlayRepairFeedback
 -> Play Sound / Print String("Temperature stabilizing...")
 -> Toggle emissive/light state [可选]
```

---

## 16. BP_HiddenDialogueUnlocker

## 16.1 UnlockForColleague

```text
Custom Event UnlockForColleague(Target)
 -> Switch on Target
    ColleagueA:
      -> Make ST_HiddenOptionRecord
         OptionId = A_AboutAC
         TargetColleague = ColleagueA
         Label = "关于那个空调……"
         RequiredAnomaly = FREEZE
      -> Get ChatConversationManager Ref
      -> InjectHiddenOption(OptionRecord)
      -> Get RouteStateManager Ref
      -> SetColleagueUnlockState(ColleagueA, Unlocked)
```

---

## 17. WBP_ReportEditor

## 17.1 Construct

```text
Event Construct
 -> BuildOptionList
```

## 17.2 BuildOptionList

```text
Custom Event BuildOptionList
 -> Clear Children(OptionsPanel)
 -> For each fixed slice option
    -> Create Button / Sentence Row
    -> Bind OnClicked -> HandleSentenceSelected(SentenceId)
```

## 17.3 HandleSentenceSelected

```text
Custom Event HandleSentenceSelected(SentenceId)
 -> Set SelectedSentenceId = SentenceId
 -> Refresh selection highlight
```

## 17.4 HandleSubmitClicked

```text
OnClicked SubmitButton
 -> Get RouteStateManager Ref
 -> Branch [HandleMyselfCount > 0]
    True:
      -> EndingType = Good
    False:
      -> EndingType = Bad
 -> Get GameFlowManager Ref
 -> TriggerEnding(EndingType)
```

---

## 18. WBP_EndingTitleCard

## 18.1 SetupEnding

```text
Custom Event SetupEnding(EndingType)
 -> Switch on EndingType
    Good:
      -> Set Title = "GOOD ENDING"
      -> Set SubtitleLines from DT_EndingSubtitles(GoodEnding)
    Bad:
      -> Set Title = "BAD ENDING"
      -> Set SubtitleLines from DT_EndingSubtitles(BadEnding)
```

## 18.2 Restart

```text
OnClicked RestartButton
 -> Open Level(LV_ApartmentMain)
```

---

## 19. 推荐调试顺序

每完成一步就测，不要憋到最后。

### 第一轮

- Step 1-3：只测“进电脑”

### 第二轮

- Step 4-7：只测“聊天 -> 小游戏 -> 异常弹窗”

### 第三轮

- Step 8-12：只测“回房间 -> 修空调 -> 隐藏选项”

### 第四轮

- Step 13-18：只测“隐藏对话 -> 日报 -> 结尾”

---

## 20. 第一版最常见故障点

### 故障 1：进电脑后角色还能动

排查：

- `SetDesktopInputMode` 是否真的调用了
- `SetMovementEnabled(false)` 是否生效

### 故障 2：聊天能发但没有回复

排查：

- `ConversationManagerRef` 是否有效
- `SendPlayerMessage` 后是否调用了 `AppendReply`

### 故障 3：小游戏弹不出异常

排查：

- `AccumulateSideEffect` 是否真的增长了 FreezeWeight
- `ResolveDominantAnomaly` 是否在小游戏后被调用

### 故障 4：回房间后空调不能交互

排查：

- `bIsCurrentlyAvailable` 是否切为 true
- 当前主状态是否真的切到 `HandleAnomaly3D`

### 故障 5：修完空调没有隐藏选项

排查：

- `UnlockForColleague` 是否被调用
- `InjectHiddenOption` 是否往 `HiddenOptions` 里加了数据

### 故障 6：提交日报没反应

排查：

- `SelectedSentenceId` 是否成功记录
- `TriggerEnding` 是否被调用

---

## 21. 最后建议

如果你们 48 小时真要交付，蓝图搭建时最重要的是：

- 不要先追求完美架构
- 不要先追求 Bubble 美术
- 不要先追求复杂效果

而是先让这条主线真正能跑完。

一旦这条线通了，剩下所有 polish 都只是加法。
