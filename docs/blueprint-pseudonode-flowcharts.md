# Signal Project Blueprint 伪节点流程图

> **切片权威说明（Authoritative for Slice Runtime Flow）**
> - 本文把 `48 小时竖切片` 的关键 Blueprint 行为写成接近 Unreal 连线的伪节点。
> - 本文必须服从 `vertical-slice-scope`、`game-state-machine`、`interaction-spec`、`blueprint-responsibility-map`、`vertical-slice-blueprint-wiring-order`。
> - 本文只写切片权威流程，不再把 `GetAllActorsOfClass` 当成规范路径。

---

## 1. 阅读方式

- `Event BeginPlay`、`InputAction Interact` 表示事件入口
- `Branch`、`Sequence`、`Set`、`Switch` 表示常见蓝图节点
- `->` 表示执行流
- `[]` 表示关键条件或备注

本文的目标不是还原截图，而是：

- owner 对
- ref 来源对
- phase authority 对
- 分支后果对

---

## 2. 切片权威主链

```text
RoomExplore
 -> Use Computer
 -> DesktopIdle
 -> Open Chat
 -> SendMessage
 -> Start Minigame
 -> Trigger FREEZE
 -> AnomalyChoicePopup
    -> Report Supervisor -> Minigame closure -> ReportPhase
    -> Handle Myself -> HandleAnomaly3D -> AirConditioner -> RoomExplore -> Use Computer -> Hidden Dialogue -> ReportPhase
 -> EndingSequence
```

下面所有伪节点都围绕这条主链。

---

## 3. `BP_SignalPlayerController`

## 3.1 BeginPlay

```text
Event BeginPlay
 -> Call SetRoomInputMode
```

## 3.2 RegisterSliceRefs

```text
Custom Event RegisterSliceRefs(
    InGameFlowManager,
    InRouteStateManager,
    InAnomalyManager,
    InChatConversationManager,
    InMinigameManager)
 -> Set GameFlowManagerRef = InGameFlowManager
 -> Set RouteStateManagerRef = InRouteStateManager
 -> Set AnomalyManagerRef = InAnomalyManager
 -> Set ChatConversationManagerRef = InChatConversationManager
 -> Set MinigameManagerRef = InMinigameManager
```

## 3.3 SetRoomInputMode

```text
Custom Event SetRoomInputMode
 -> Set Show Mouse Cursor = false
 -> Set Input Mode Game Only
 -> Get Controlled Pawn
 -> Cast To BP_SignalPlayerCharacter
 -> Call SetMovementEnabled(true)
 -> Set bIsInDesktopMode = false
```

## 3.4 SetDesktopInputMode

```text
Custom Event SetDesktopInputMode
 -> Set Show Mouse Cursor = true
 -> Set Input Mode Game and UI
 -> Get Controlled Pawn
 -> Cast To BP_SignalPlayerCharacter
 -> Call SetMovementEnabled(false)
 -> Set bIsInDesktopMode = true
```

## 3.5 ShowDesktopRoot

```text
Custom Event ShowDesktopRoot
 -> Branch [IsValid(CachedDesktopRoot)?]
    False:
      -> Create Widget(WBP_DesktopRoot)
         [Expose on Spawn:
            GameFlowManagerRef,
            RouteStateManagerRef,
            ChatConversationManagerRef,
            MinigameManagerRef]
      -> Promote to CachedDesktopRoot
      -> Add To Viewport
    True:
      -> Branch [Is In Viewport?]
          False:
            -> Add To Viewport
 -> Set Visibility(Visible)
 -> Call CachedDesktopRoot.RefreshAppAvailability
 -> Call MinigameManagerRef.RegisterDesktopRoot(CachedDesktopRoot)
```

## 3.6 HideDesktopRoot

```text
Custom Event HideDesktopRoot
 -> Branch [IsValid(CachedDesktopRoot)?]
    True:
      -> Set Visibility(Hidden) [或 Remove From Parent]
```

## 3.7 ShowEndingTitleCard

```text
Custom Event ShowEndingTitleCard(EndingType)
 -> Branch [IsValid(CachedEndingTitleCard)?]
    False:
      -> Create Widget(WBP_EndingTitleCard)
      -> Promote to CachedEndingTitleCard
      -> Add To Viewport
 -> Call CachedEndingTitleCard.SetupEnding(EndingType)
 -> Set Visibility(Visible)
```

## 3.8 Leave Desktop / Esc

```text
InputAction LeaveDesktop / Esc
 -> Branch [bIsInDesktopMode == true]
    False:
      -> Return
    True:
      -> Branch [GameFlowManagerRef.CurrentPhase]
          DesktopIdle:
            -> Call GameFlowManagerRef.RequestPhaseChange(RoomExplore)
          ChatActive:
            -> Call GameFlowManagerRef.RequestPhaseChange(RoomExplore)
          MinigameActive:
            -> Return [切片禁止]
          AnomalyChoice:
            -> Return [切片禁止]
          ReportPhase:
            -> Return [切片禁止]
          Else:
            -> Return
```

---

## 4. `BP_SignalPlayerCharacter`

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
 -> CameraLocation
 -> CameraForwardVector * InteractionTraceDistance
 -> LineTraceByChannel
 -> Branch [HitActor Valid?]
    False:
      -> Set CurrentFocusedInteractable = None
      -> Clear Prompt
    True:
      -> Branch [Does Implement Interface BPI_Interactable]
          False:
            -> Set CurrentFocusedInteractable = None
            -> Clear Prompt
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

## 4.3 TryInteract

```text
InputAction Interact / E
 -> Call TryInteract

Custom Event TryInteract
 -> Branch [IsValid(CurrentFocusedInteractable)]
    True:
      -> Interface Call Interact(CurrentFocusedInteractable)
```

## 4.4 SetMovementEnabled

```text
Custom Event SetMovementEnabled(bEnabled)
 -> Set bCanInteract = bEnabled
 -> Branch [bEnabled]
    True:
      -> CharacterMovement Set Movement Mode Walking
      -> Controller Ignore Move Input(false)
    False:
      -> Controller Ignore Move Input(true)
```

---

## 5. `BPI_Interactable`

```text
Function CanInteract -> returns Bool
Function GetInteractionText -> returns Text
Function Interact
```

切片规则：

- 接口实现内部不直接写 `CurrentPhase`
- phase 变化必须委托给 `BP_SignalGameFlowManager`

---

## 6. `BP_ComputerTerminal`

## 6.1 CanInteract

```text
Interface Function CanInteract
 -> Return GameFlowManagerRef.CanEnterDesktop()
```

## 6.2 GetInteractionText

```text
Interface Function GetInteractionText
 -> Return "[E] Use Computer"
```

## 6.3 Interact

```text
Interface Function Interact
 -> Branch [CanInteract == true]
    True:
      -> Call GameFlowManagerRef.RequestPhaseChange(DesktopIdle)
```

---

## 7. `WBP_DesktopRoot`

## 7.1 Expose on Spawn refs

```text
Expose on Spawn Variables:
 - GameFlowManagerRef
 - RouteStateManagerRef
 - ChatConversationManagerRef
 - MinigameManagerRef
```

## 7.2 Construct

```text
Event Construct
 -> Branch [bInitialized]
    False:
      -> Bind ChatButton.OnClicked -> HandleOpenChat
      -> Bind ReportButton.OnClicked -> HandleOpenReport
      -> Set bInitialized = true
 -> Call RefreshAppAvailability
```

## 7.3 RefreshAppAvailability

```text
Custom Event RefreshAppAvailability
 -> Branch [GameFlowManagerRef.CanOpenReport()]
    True:
      -> Set ReportButton Enabled = true
    False:
      -> Set ReportButton Enabled = false
```

## 7.4 HandleOpenChat

```text
Custom Event HandleOpenChat
 -> Call GameFlowManagerRef.RequestPhaseChange(ChatActive)
```

## 7.5 HandleOpenReport

```text
Custom Event HandleOpenReport
 -> Branch [GameFlowManagerRef.CanOpenReport()]
    False:
      -> Return
    True:
      -> Call GameFlowManagerRef.RequestPhaseChange(ReportPhase)
```

## 7.6 OpenApp

```text
Custom Event OpenApp(AppTag)
 -> Switch on Name AppTag
    Chat:
      -> Branch [IsValid(ChatAppWidget)?]
          False:
            -> Create Widget(WBP_ChatApp)
               [Expose on Spawn:
                  ConversationManagerRef = ChatConversationManagerRef,
                  RouteStateManagerRef = RouteStateManagerRef]
            -> Promote to ChatAppWidget
      -> Call ShowAppWidget(ChatAppWidget)

    Report:
      -> Branch [IsValid(ReportWidget)?]
          False:
            -> Create Widget(WBP_ReportEditor)
               [Expose on Spawn:
                  GameFlowManagerRef = GameFlowManagerRef,
                  RouteStateManagerRef = RouteStateManagerRef]
            -> Promote to ReportWidget
      -> Call ShowAppWidget(ReportWidget)
```

## 7.7 ShowAppWidget

```text
Custom Event ShowAppWidget(WidgetRef)
 -> Clear Children(AppContentHost)
 -> Add Child(WidgetRef)
 -> Set CurrentAppWidget = WidgetRef
```

---

## 8. `BP_ChatConversationManager`

## 8.1 InitializeChatSystem

```text
Custom Event InitializeChatSystem
 -> Set CurrentConversationTarget = ColleagueA
 -> Clear ConversationHistory
 -> Clear HiddenOptions
 -> Seed initial slice lines
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
 -> Call AnomalyManagerRef.AccumulateSideEffect(CurrentConversationTarget, 1.0)
 -> Call AppendReply(CurrentConversationTarget, ReplyText)
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
 -> ForEach HiddenOptions
    -> Branch [Existing.OptionId == OptionRecord.OptionId]
        True:
          -> Return [dedupe]
 -> Add OptionRecord to HiddenOptions
 -> Broadcast OnHiddenOptionInjected(OptionRecord.TargetColleague)
```

## 8.6 PlayHiddenDialogue

```text
Custom Event PlayHiddenDialogue(Target)
 -> Get hidden dialogue rows for Target
 -> ForEachLoop
    -> Make ST_ChatMessageRecord [bIsHiddenFragment = true]
    -> Add to ConversationHistory
 -> Call RouteStateManagerRef.MarkHiddenOptionConsumed(Target)
 -> Broadcast OnMessagesUpdated
```

---

## 9. `WBP_ChatApp`

## 9.1 Expose on Spawn refs

```text
Expose on Spawn Variables:
 - ConversationManagerRef
 - RouteStateManagerRef
```

## 9.2 Construct

```text
Event Construct
 -> Bind ContactAButton -> HandleContactSelected(ColleagueA)
 -> Bind SupervisorButton -> HandleContactSelected(Supervisor)
 -> Bind SendButton -> HandleSendClicked
 -> Bind ConversationManagerRef.OnMessagesUpdated -> RefreshConversation
 -> Bind ConversationManagerRef.OnConversationChanged -> RefreshConversation
 -> Call RefreshConversation
```

## 9.3 HandleContactSelected

```text
Custom Event HandleContactSelected(Target)
 -> Set CurrentConversationTarget = Target
 -> Call ConversationManagerRef.SwitchConversation(Target)
 -> Call RefreshConversation
```

## 9.4 HandleSendClicked

```text
Custom Event HandleSendClicked
 -> Get selected preset / input text
 -> Branch [MessageText not empty]
    True:
      -> Call ConversationManagerRef.SendPlayerMessage(MessageText)
```

## 9.5 RefreshConversation

```text
Custom Event RefreshConversation
 -> Clear Children(MessageList)
 -> Get filtered messages for CurrentConversationTarget
 -> ForEachLoop
    -> CreateWidget(MessageRow)
    -> Add Child(MessageList)
 -> Get hidden options for CurrentConversationTarget
 -> ForEachLoop
    -> CreateWidget(HiddenOptionButton)
    -> Bind OnClicked -> HandleHiddenOptionClicked(OptionId)
    -> Add Child(MessageList)
```

## 9.6 HandleHiddenOptionClicked

```text
Custom Event HandleHiddenOptionClicked(OptionId)
 -> Call ConversationManagerRef.PlayHiddenDialogue(CurrentConversationTarget)
 -> Call RefreshConversation
```

---

## 10. `BP_AnomalyManager`

## 10.1 AccumulateSideEffect

```text
Custom Event AccumulateSideEffect(ColleagueId, Amount)
 -> Switch on E_ColleagueId
    ColleagueA:
      -> Set AccumulatedFreezeWeight = AccumulatedFreezeWeight + Amount
    Default:
      -> Return [切片不执行其它 anomaly]
```

## 10.2 ResolveDominantAnomaly

```text
Custom Event ResolveDominantAnomaly
 -> Branch [AccumulatedFreezeWeight >= FreezeThreshold]
    True:
      -> Call TriggerAnomaly(FREEZE)
    False:
      -> Return
```

## 10.3 TriggerAnomaly

```text
Custom Event TriggerAnomaly(Type)
 -> Branch [bAnomalyActive == true]
    True:
      -> Return
    False:
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

## 11. `BP_MinigameManager`

## 11.1 RegisterDesktopRoot

```text
Custom Event RegisterDesktopRoot(InDesktopRoot)
 -> Set DesktopRootRef = InDesktopRoot
```

## 11.2 BindAnomalyCallbacks

```text
Custom Event BindAnomalyCallbacks
 -> Bind AnomalyManagerRef.OnAnomalyTriggered -> HandleAnomalyTriggered
```

## 11.3 StartMinigame

```text
Custom Event StartMinigame(Type)
 -> Branch [IsValid(DesktopRootRef)]
    False:
      -> Return
 -> Set CurrentMinigameType = Type
 -> Branch [Type == DependencyMatch]
    True:
      -> Create Widget(WBP_MG_DependencyMatch)
         [Expose on Spawn:
            MinigameManagerRef = self]
      -> Promote to CurrentMinigameWidget
      -> Call DesktopRootRef.ShowAppWidget(CurrentMinigameWidget)
      -> Set bIsMinigameRunning = true
      -> Call GameFlowManagerRef.RequestPhaseChange(MinigameActive)
      -> Delay(1.0) [可选]
      -> Call AnomalyManagerRef.ResolveDominantAnomaly()
```

## 11.4 HandleAnomalyTriggered

```text
Custom Event HandleAnomalyTriggered(Type)
 -> Call PauseMinigame
 -> Call GameFlowManagerRef.RequestPhaseChange(AnomalyChoice)
 -> Create Widget(WBP_AnomalyChoicePopup)
    [Expose on Spawn:
       OwningMinigameManagerRef = self]
 -> Promote to ChoicePopupRef
 -> Call ChoicePopupRef.SetupForAnomaly(Type)
 -> Add To Viewport [或 Show In Desktop Overlay]
```

## 11.5 PauseMinigame

```text
Custom Event PauseMinigame
 -> Set bIsMinigameRunning = false
 -> Branch [IsValid(CurrentMinigameWidget)]
    True:
      -> Call CurrentMinigameWidget.SetPaused(true)
```

## 11.6 ResumeMinigame

```text
Custom Event ResumeMinigame
 -> Set bIsMinigameRunning = true
 -> Branch [IsValid(CurrentMinigameWidget)]
    True:
      -> Call CurrentMinigameWidget.SetPaused(false)
```

## 11.7 CompleteMinigame

```text
Custom Event CompleteMinigame
 -> Set bIsMinigameRunning = false
 -> Branch [IsValid(CurrentMinigameWidget)]
    True:
      -> Remove From Parent(CurrentMinigameWidget) [或清空 DesktopRoot 内容区]
```

## 11.8 HandleAnomalyChoice

```text
Custom Event HandleAnomalyChoice(Branch)
 -> Switch on E_RouteBranch

    ReportSupervisor:
      -> Call RouteStateManagerRef.RecordRouteChoice(ReportSupervisor)
      -> Call AnomalyManagerRef.ResolveCurrentAnomaly()
      -> Branch [IsValid(ChoicePopupRef)]
          True:
            -> Remove From Parent(ChoicePopupRef)
      -> Call GameFlowManagerRef.RequestPhaseChange(MinigameActive)
      -> Call ResumeMinigame
      -> [可选：短延迟后统一收尾]
      -> Call CompleteMinigame
      -> Call GameFlowManagerRef.RequestPhaseChange(ReportPhase)

    HandleMyself:
      -> Call RouteStateManagerRef.RecordRouteChoice(HandleMyself)
      -> Branch [IsValid(ChoicePopupRef)]
          True:
            -> Remove From Parent(ChoicePopupRef)
      -> Branch [IsValid(CurrentMinigameWidget)]
          True:
            -> Remove From Parent(CurrentMinigameWidget)
      -> Set bIsMinigameRunning = false
      -> Call GameFlowManagerRef.RequestPhaseChange(HandleAnomaly3D)
```

---

## 12. `WBP_MG_DependencyMatch`

## 12.1 Expose on Spawn refs

```text
Expose on Spawn Variables:
 - MinigameManagerRef
```

## 12.2 Construct

```text
Event Construct
 -> BuildGrid
```

## 12.3 HandleTileClicked

```text
Custom Event HandleTileClicked(TileId)
 -> Branch [bPaused]
    True:
      -> Return
    False:
      -> Continue Minigame Logic
```

## 12.4 On All Tiles Cleared

```text
Custom Event HandleAllTilesCleared
 -> Call MinigameManagerRef.CompleteMinigame
```

---

## 13. `WBP_AnomalyChoicePopup`

## 13.1 SetupForAnomaly

```text
Custom Event SetupForAnomaly(Type)
 -> Set CurrentAnomalyType = Type
 -> Switch on Type
    FREEZE:
      -> Set TitleText = "温度异常"
      -> Set BodyText = "空调温度骤降至 16.4°C。你要怎么处理？"
```

## 13.2 Expose on Spawn refs

```text
Expose on Spawn Variables:
 - OwningMinigameManagerRef
```

## 13.3 HandleReportSupervisor

```text
OnClicked ReportButton
 -> Call OwningMinigameManagerRef.HandleAnomalyChoice(ReportSupervisor)
```

## 13.4 HandleHandleMyself

```text
OnClicked HandleButton
 -> Call OwningMinigameManagerRef.HandleAnomalyChoice(HandleMyself)
```

---

## 14. `BP_SignalGameFlowManager`

## 14.1 BeginPlay / InitializeGameFlow

```text
Event BeginPlay
 -> Set CurrentDayIndex = 1
 -> Set PreviousPhase = Boot
 -> Get Player Controller
 -> Cast To BP_SignalPlayerController
 -> Promote to CachedPlayerController
 -> Call CachedPlayerController.RegisterSliceRefs(
      self,
      RouteStateManagerRef,
      AnomalyManagerRef,
      ChatConversationManagerRef,
      MinigameManagerRef)
 -> Call MinigameManagerRef.BindAnomalyCallbacks
 -> Call RequestPhaseChange(RoomExplore)
```

## 14.2 RequestPhaseChange

```text
Custom Event RequestPhaseChange(NewPhase)
 -> Branch [NewPhase == CurrentPhase]
    True:
      -> Return [No-op]
    False:
      -> Set PreviousPhase = CurrentPhase
      -> Set CurrentPhase = NewPhase
      -> Call HandlePhaseEntered(NewPhase)
```

## 14.3 HandlePhaseEntered

```text
Custom Event HandlePhaseEntered(NewPhase)
 -> Switch on E_GamePhase

    RoomExplore:
      -> CachedPlayerController.HideDesktopRoot
      -> CachedPlayerController.SetRoomInputMode

    DesktopIdle:
      -> CachedPlayerController.ShowDesktopRoot
      -> CachedPlayerController.SetDesktopInputMode
      -> CachedPlayerController.CachedDesktopRoot.RefreshAppAvailability

    ChatActive:
      -> CachedPlayerController.ShowDesktopRoot
      -> CachedPlayerController.SetDesktopInputMode
      -> CachedPlayerController.CachedDesktopRoot.OpenApp("Chat")

    MinigameActive:
      -> CachedPlayerController.ShowDesktopRoot
      -> CachedPlayerController.SetDesktopInputMode

    AnomalyChoice:
      -> CachedPlayerController.ShowDesktopRoot
      -> CachedPlayerController.SetDesktopInputMode

    HandleAnomaly3D:
      -> CachedPlayerController.HideDesktopRoot
      -> CachedPlayerController.SetRoomInputMode

    ReportPhase:
      -> CachedPlayerController.ShowDesktopRoot
      -> CachedPlayerController.SetDesktopInputMode
      -> CachedPlayerController.CachedDesktopRoot.RefreshAppAvailability
      -> CachedPlayerController.CachedDesktopRoot.OpenApp("Report")

    EndingSequence:
      -> CachedPlayerController.ShowEndingTitleCard(PendingEndingType)
```

## 14.4 CanEnterDesktop

```text
Function CanEnterDesktop -> returns Bool
 -> Branch [CurrentPhase == RoomExplore]
    True:
      -> Return true
    False:
      -> Return false
```

## 14.5 CanOpenReport

```text
Function CanOpenReport -> returns Bool
 -> Branch [CurrentPhase == MinigameActive or CurrentPhase == AnomalyChoice or CurrentPhase == HandleAnomaly3D]
    True:
      -> Return false
 -> Branch [RouteStateManagerRef.LastRouteChoice == HandleMyself]
    True:
      -> Branch [RouteStateManagerRef.bHasConsumedColleagueAHiddenOption]
          True:
            -> Return true
          False:
            -> Return false
    False:
      -> Branch [RouteStateManagerRef.LastRouteChoice == ReportSupervisor]
          True:
            -> Return true
          False:
            -> Return false
```

## 14.6 SubmitSliceReport

```text
Custom Event SubmitSliceReport(SelectedSentenceId)
 -> Branch [SelectedSentenceId is None]
    True:
      -> Return
 -> Branch [CanOpenReport() == false]
    True:
      -> Return
 -> EndingType = RouteStateManagerRef.GetSliceEndingResult()
 -> Call TriggerEnding(EndingType)
```

## 14.7 TriggerEnding

```text
Custom Event TriggerEnding(EndingType)
 -> Set PendingEndingType = EndingType
 -> Call RequestPhaseChange(EndingSequence)
```

---

## 15. `BP_AirConditionerUnit`

## 15.1 CanInteract

```text
Interface Function CanInteract
 -> Branch [GameFlowManagerRef.CurrentPhase == HandleAnomaly3D]
    False:
      -> Return false
    True:
      -> Branch [AnomalyManagerRef.CurrentAnomalyType == FREEZE]
          False:
            -> Return false
          True:
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
 -> Branch [CanInteract == true]
    True:
      -> Call PlayRepairFeedback
      -> Call AnomalyManagerRef.ResolveCurrentAnomaly
      -> Call HiddenDialogueUnlockerRef.UnlockForColleague(ColleagueA)
      -> Set bIsCurrentlyAvailable = false
      -> Call GameFlowManagerRef.RequestPhaseChange(RoomExplore)
```

## 15.4 PlayRepairFeedback

```text
Custom Event PlayRepairFeedback
 -> Play Sound / Print String("Temperature stabilizing...")
```

---

## 16. `BP_HiddenDialogueUnlocker`

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
      -> Call ChatConversationManagerRef.InjectHiddenOption(OptionRecord)
      -> Call RouteStateManagerRef.SetColleagueUnlockState(ColleagueA, Unlocked)
```

---

## 17. `WBP_ReportEditor`

## 17.1 Expose on Spawn refs

```text
Expose on Spawn Variables:
 - GameFlowManagerRef
 - RouteStateManagerRef
```

## 17.2 Construct

```text
Event Construct
 -> Call BuildOptionList
 -> Call RefreshSubmitState
```

## 17.3 HandleSentenceSelected

```text
Custom Event HandleSentenceSelected(SentenceId)
 -> Set SelectedSentenceId = SentenceId
 -> Call RefreshSubmitState
```

## 17.4 RefreshSubmitState

```text
Custom Event RefreshSubmitState
 -> Branch [SelectedSentenceId is None]
    True:
      -> Set SubmitButton Enabled = false
    False:
      -> Set SubmitButton Enabled = true
```

## 17.5 HandleSubmitClicked

```text
OnClicked SubmitButton
 -> Branch [SelectedSentenceId is None]
    True:
      -> Return
 -> Call GameFlowManagerRef.SubmitSliceReport(SelectedSentenceId)
```

---

## 18. `WBP_EndingTitleCard`

## 18.1 SetupEnding

```text
Custom Event SetupEnding(EndingType)
 -> Set CurrentEndingType = EndingType
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

### 第一轮

- Step 0-4：只测“RoomExplore -> Computer -> DesktopIdle”

### 第二轮

- Step 5-8：只测“Chat -> Minigame -> FREEZE -> Popup”

### 第三轮

- Step 9-15：只测“分支 -> 回房间 -> 修空调 / 或小游戏闭合”

### 第四轮

- Step 16-18：只测“隐藏对话 -> Report -> Ending”

---

## 20. 第一版最常见故障点（按 owner 排查）

### 故障 1：桌面打开后角色还能动

排查：

- `HandlePhaseEntered(DesktopIdle)` 是否真的调用了 `SetDesktopInputMode`
- `SetMovementEnabled(false)` 是否生效

### 故障 2：popup 按钮点了但 phase 乱跳

排查：

- popup 是否直接改了全局状态
- 是否正确转发给 `BP_MinigameManager.HandleAnomalyChoice`

### 故障 3：修空调后直接回桌面了

排查：

- `BP_AirConditionerUnit.Interact` 是否错误地请求了 `DesktopIdle`
- 正确目标必须是 `RoomExplore`

### 故障 4：报告过早开放

排查：

- `GameFlowManager.CanOpenReport()` 是否真的挡住了 `HandleMyself` 路线的隐藏对话前阶段
- `DesktopRoot.HandleOpenReport` 是否做了 guard

### 故障 5：同一隐藏选项重复出现

排查：

- `InjectHiddenOption` 是否按 `OptionId` 去重

---

## 21. 最后建议

如果切片要在 48 小时内稳定可跑，最重要的不是节点数少，而是：

- `CurrentPhase` 只有一个 owner
- widget 不越权
- popup 不乱管 phase
- AC 修好后不自动回桌面
- report 只在该开的时刻打开

只要这些权威规则被保住，蓝图图面再简陋，主链也能稳。
