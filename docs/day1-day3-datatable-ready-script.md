# Signal Project Day 1-Day 3 DataTable 可导入文本稿

> **切片权威说明（Implementation-Authoritative Import Source）**
> - 本文是 `48 小时竖切片` 的 **DataTable 实际导入源**。
> - **字段名 / Struct 名 / 消费方映射** 以 `docs/blueprint-variables-events-and-data-fields.md` 为准。
> - **本文负责冻结：** 这次 slice 真正要导入的行内容、列顺序、Day 1-Day 3 最小文本集。
> - `docs/narrative-script-and-system-copy.md` 仅保留 prose / reference 作用，不是导入 authority。

---

## 1. 竖切片数据导入策略（本次冻结结果）

### 1.1 本轮必须创建并导入的表

- `DT_SystemCopy`
- `DT_SupervisorLines`
- `DT_NormalReplies`
- `DT_HiddenDialogues`
- `DT_ReportSentencePools`
- `DT_EndingSubtitles`

### 1.2 本轮明确不要求导入的表

- `DT_DayConfigs`
- `DT_AnomalyConfigs`

切片 Day 1-Day 3 主链 **不依赖** 这两张表落地。

### 1.3 Slice 内容边界

本文只保留 `48 小时竖切片` 真正需要消费的内容：

- `Day 1` onboarding / 基础聊天 / 第一次日报
- `Day 2` 唯一完整异常链：`FREEZE`
- `Day 2` Colleague A 隐藏对话
- `Day 3` 简化收束与结尾字幕

以下内容 **不再作为本文的导入权威内容**：

- `BLACKOUT` 执行链
- `DISKCLEAN` 执行链
- Day 4-Day 7 运行行
- Day 5 / Day 6 / Day 7 的隐藏对话、揭露或汇报行

### 1.4 RowName 规则

- `DT_SupervisorLines`：`RowName = LineId`
- `DT_NormalReplies`：`RowName = ReplyId`
- `DT_HiddenDialogues`：`RowName = DialogueId`
- `DT_SystemCopy`：`RowName = CopyId`
- `DT_ReportSentencePools`：`RowName = SentenceId`
- `DT_EndingSubtitles`：`RowName = <EndingId>_<SequenceOrder as 2-digit zero-padded int>`（例如 `BadEnding_01`、`GoodEnding_04`）

---

## 2. 列顺序冻结（必须和 Struct 字段一致）

## 2.1 `DT_SupervisorLines`

- `LineId`
- `DayIndex`
- `UseCase`
- `SpeakerId`
- `Text`
- `Weight`

`UseCase` 本轮只允许：

- `MorningIntro`
- `ChatThread`
- `AnomalyResponse`
- `ReportAck`

## 2.2 `DT_NormalReplies`

- `ReplyId`
- `DayIndex`
- `ColleagueId`
- `Text`
- `Weight`

## 2.3 `DT_HiddenDialogues`

- `DialogueId`
- `DayIndex`
- `ColleagueId`
- `SequenceOrder`
- `Text`
- `bGlitchEnter`
- `bGlitchExit`

## 2.4 `DT_SystemCopy`

- `CopyId`
- `Category`
- `SubType`
- `Text`

## 2.5 `DT_ReportSentencePools`

- `SentenceId`
- `DayIndex`
- `SourceColleague`
- `UnlockRequirement`
- `StrengthScore`
- `bIsFinalInjection`
- `Text`

## 2.6 `DT_EndingSubtitles`

- `EndingId`
- `SequenceOrder`
- `Text`

---

## 3. `DT_SupervisorLines`

```csv
LineId,DayIndex,UseCase,SpeakerId,Text,Weight
SUP_D1_INTRO_01,1,MorningIntro,Supervisor,"早，今天先把当前这个需求过一遍，别紧张，按节奏来。",1
SUP_D1_INTRO_02,1,MorningIntro,Supervisor,"今天任务不重，把手上的链路先跑顺，下午给我一个简要进度就行。",1
SUP_D1_THREAD_01,1,ChatThread,Supervisor,"先看一下今天的任务安排。",1
SUP_D1_THREAD_02,1,ChatThread,Supervisor,"把依赖关系理顺，下午给我一句话进度。",1
SUP_D1_REPORT_01,1,ReportAck,Supervisor,"好，看到了，继续。",1
SUP_D2_INTRO_01,2,MorningIntro,Supervisor,"昨天节奏不错，今天继续。把依赖关系梳干净，别让小问题拖到晚上。",1
SUP_D2_INTRO_02,2,MorningIntro,Supervisor,"今天我盯一下节点，你先把该对齐的内容对齐，别分心。",1
SUP_D2_ANOMALY_01,2,AnomalyResponse,Supervisor,"收到，我来协调，你继续手头的工作。",1
SUP_D2_ANOMALY_02,2,AnomalyResponse,Supervisor,"这个问题不用你分心，我已经同步处理。",1
SUP_D2_REPORT_01,2,ReportAck,Supervisor,"可以，这个节奏没问题。",1
SUP_D3_INTRO_01,3,MorningIntro,Supervisor,"今天进度要再往前顶一点，我下午要同步上面。",1
SUP_D3_INTRO_02,3,MorningIntro,Supervisor,"别让线上问题堆起来，先把最明显的几个点压住。",1
SUP_D3_REPORT_01,3,ReportAck,Supervisor,"收到，我待会儿同步给上面。",1
```

---

## 4. `DT_NormalReplies`

```csv
ReplyId,DayIndex,ColleagueId,Text,Weight
A_D1_01,1,ColleagueA,"好的，我看一下。",1
A_D1_02,1,ColleagueA,"这个需求我先过一遍。",1
A_D1_03,1,ColleagueA,"我这边能接。",1
A_D1_04,1,ColleagueA,"先按现在这个来吧。",1
A_D1_05,1,ColleagueA,"可以，等我整理一下。",1
A_D2_01,2,ColleagueA,"这个我昨天其实已经差不多看过了。",1
A_D2_02,2,ColleagueA,"先别改太多，容易牵一片。",1
A_D2_03,2,ColleagueA,"我记得这个点以前也绕过一次。",1
A_D2_04,2,ColleagueA,"你先往下走，我补一下细节。",1
A_D3_01,3,ColleagueA,"嗯，先这样。",1
A_D3_02,3,ColleagueA,"可以。",1
A_D3_03,3,ColleagueA,"我知道。",1
A_D3_04,3,ColleagueA,"第三行。",1
A_D3_05,3,ColleagueA,"……没事。",1
B_D1_01,1,ColleagueB,"我这边先盯着线上。",1
B_D2_01,2,ColleagueB,"如果没炸就先别碰太多。",1
B_D3_01,3,ColleagueB,"线上先压住。",1
B_D3_02,3,ColleagueB,"灯别关，我还没睡。",1
B_D3_03,3,ColleagueB,"我晚点再看。",1
C_D1_01,1,ColleagueC,"有旧文件的话记得收一下。",1
C_D2_01,2,ColleagueC,"缓存别堆太久。",1
C_D3_01,3,ColleagueC,"整理完会轻一点。",1
```

---

## 5. `DT_HiddenDialogues`

> 切片中只要求 `ColleagueA` 的 Day 2 隐藏链完整可执行。

```csv
DialogueId,DayIndex,ColleagueId,SequenceOrder,Text,bGlitchEnter,bGlitchExit
A_HIDDEN_D2_01,2,ColleagueA,1,"空调调到 16.4 比较好睡。",true,false
A_HIDDEN_D2_02,2,ColleagueA,2,"那个 bug 我之前定位过，在第三行。",false,false
A_HIDDEN_D2_03,2,ColleagueA,3,"你那时候没在线。",false,false
A_HIDDEN_D2_04,2,ColleagueA,4,"……你现在还在吗？",false,true
```

---

## 6. `DT_SystemCopy`

```csv
CopyId,Category,SubType,Text
BOOT_01,Boot,Status,"Booting workspace services..."
BOOT_02,Boot,Status,"Syncing local environment..."
BOOT_03,Boot,Status,"Connecting team channel..."
BOOT_04,Boot,Status,"Loading daily schedule..."
BOOT_05,Boot,Confirm,"AUTH OK"
BOOT_06,Boot,Confirm,"TEAM CHANNEL OK"
BOOT_07,Boot,Confirm,"LOCAL CACHE OK"
BOOT_08,Boot,Confirm,"SUPERVISOR LINK STABLE"
INTERACT_01,Interaction,Computer,"[E] Use Computer"
INTERACT_02,Interaction,AC,"[E] Adjust Air Conditioner"
INTERACT_05,Interaction,LeaveDesktop,"[Esc] Leave Desktop"
OBJECTIVE_01,Objective,Start,"Start the workday."
OBJECTIVE_02,Objective,Chat,"Check the team chat."
OBJECTIVE_03,Objective,Handle,"Something feels wrong. Handle it yourself."
OBJECTIVE_04,Objective,Return,"Return to the computer."
SYS_01,System,Message,"New message received."
SYS_02,System,Task,"Daily task updated."
SYS_03,System,Anomaly,"Environment anomaly detected."
SYS_04,System,HiddenUnlock,"Hidden option unlocked."
SYS_05,System,ReportReady,"Report ready to submit."
SYS_06,System,Delay,"System response delayed."
FREEZE_01,Anomaly,Title,"温度异常"
FREEZE_02,Anomaly,Body,"空调温度骤降至 16.4°C。"
FREEZE_03,Anomaly,Body,"鼠标操作出现迟滞。"
FREEZE_04,Anomaly,Body,"你要怎么处理？"
FREEZE_05,Anomaly,BtnReport,"Report Supervisor"
FREEZE_06,Anomaly,BtnHandle,"Handle Myself"
FREEZE_07,Anomaly,System,"Environment anomaly resolved."
FREEZE_08,Anomaly,System,"Leave the desk and inspect the source."
FREEZE_09,Anomaly,System,"The room feels colder than before."
FREEZE_10,Anomaly,System,"Temperature stabilizing..."
FREEZE_11,Anomaly,System,"A new reply is waiting in chat."
HIDDEN_LABEL_A_01,HiddenOption,A,"关于那个空调……"
```

---

## 7. `DT_ReportSentencePools`

> `UnlockRequirement = 1` 的权威含义固定为：`bHasConsumedColleagueAHiddenOption == true`（即 Colleague A 隐藏选项已消费、隐藏对话已完成）。

```csv
SentenceId,DayIndex,SourceColleague,UnlockRequirement,StrengthScore,bIsFinalInjection,Text
REP_D1_01,1,Supervisor,0,0.10,false,"今天已完成基础排查，主流程可继续推进。"
REP_D1_02,1,Supervisor,0,0.10,false,"当前依赖关系已梳理，暂无严重阻塞。"
REP_D2_01,2,Supervisor,0,0.20,false,"今日问题已自行排除，主流程未受持续影响。"
REP_D2_02,2,Supervisor,0,0.20,false,"已处理环境波动，当前任务可继续推进。"
REP_D2_03,2,Supervisor,0,0.25,false,"部分异常来源尚不明确，但工作可继续。"
REP_D3_01,3,Supervisor,0,0.25,false,"今日异常已归档，当前流程仍可继续推进。"
REP_D3_02,3,Supervisor,0,0.30,false,"已完成阶段收束，剩余风险待后续确认。"
REP_D3_03,3,ColleagueA,1,0.85,true,"有些异常并没有消失，它们只是被写成了可以继续。"
```

---

## 8. `DT_EndingSubtitles`

> 本轮切片使用 **简化 Day 3 收束版** 结尾字幕，不使用完整版 Day 7 终局文案。

```csv
EndingId,SequenceOrder,Text
BadEnding,1,"你把异常都交给了主管。"
BadEnding,2,"问题确实都被处理掉了。"
BadEnding,3,"你的工作也越来越顺。"
BadEnding,4,"你的话越来越少。"
GoodEnding,1,"你开始自己处理那些“不该存在”的问题。"
GoodEnding,2,"聊天框里出现了本不该出现的话。"
GoodEnding,3,"有人还留在里面。"
GoodEnding,4,"或者说，只剩下那些东西留在里面。"
```

---

## 9. 切片最小导入顺序

建议 Unreal 里的导入顺序固定为：

1. `DT_SystemCopy`
2. `DT_SupervisorLines`
3. `DT_NormalReplies`
4. `DT_HiddenDialogues`
5. `DT_ReportSentencePools`
6. `DT_EndingSubtitles`

---

## 10. 最终冻结结论

本文件现在只负责一件事：**给出 Day 1-Day 3 slice 真正要导入 Unreal 的权威行内容。**

因此本轮之后：

- 不再把 Day 4-Day 7 行混进 slice 导入稿
- 不再把 `BLACKOUT` / `DISKCLEAN` 执行内容混进 slice 导入稿
- 不再为相同类型文本使用多套字段名
- `WBP_ReportEditor` / `WBP_EndingTitleCard` / `BP_ChatConversationManager` 可以直接按这里的表和列建消费逻辑
