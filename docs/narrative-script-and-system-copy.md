# Signal Project 具体文本稿 + 系统提示文案

> **文案参考说明（Reference / Prose Source Only）**
> - 本文用于保留文案语气、剧情表达、文本来源说明。
> - **本文不是 slice schema authority，也不是 DataTable 导入 authority。**
> - `docs/blueprint-variables-events-and-data-fields.md` 负责枚举 / Struct / 字段名 / 消费方映射。
> - `docs/day1-day3-datatable-ready-script.md` 负责 Day 1-Day 3 竖切片的实际导入行内容。
> - 若本文与前两者冲突，以前两者为准。

---

## 1. 本文在竖切片中的作用

这份文档现在只承担三个职责：

1. 说明文本风格与叙事口吻
2. 作为 Day 1-Day 3 slice 文案的 prose source / 写作参考
3. 保留 Day 4-Day 7 与未来异常的参考性文本方向

### 1.1 明确不由本文负责的内容

本文 **不再负责**：

- 冻结 DataTable 列名
- 冻结 Struct 字段名
- 冻结 row id / row schema
- 冻结实际导入范围

这些都已经在 Work Item 3 中转移为：

- **实现权威：** `docs/blueprint-variables-events-and-data-fields.md`
- **导入权威：** `docs/day1-day3-datatable-ready-script.md`

---

## 2. 文案风格原则

### 2.1 同事 A / B / C

- 表面上像正常同事
- 语气简洁
- 逐渐出现模板感、错位感、重复感
- 隐藏对话里开始暴露“活在旧时间切片中”的感觉

### 2.2 主管

- 前期像一个疲惫但可靠的真人主管
- 说话带鼓励和 KPI 压力
- 会“记得”前文，显得很像人
- 但细看会发现他所有情绪都紧贴绩效节点

### 2.3 系统提示

- 克制
- 伪企业软件口吻
- 不要像游戏教程
- 优先做“系统状态提示”，不要抢叙事主体

---

## 3. 竖切片权威文本边界（与导入稿对齐）

48 小时竖切片中，真正会被消费的文本边界固定为：

- `Day 1`：开场、基础聊天、第一次日报
- `Day 2`：`FREEZE` 完整异常链、Colleague A 隐藏对话、第二次日报
- `Day 3`：简化收束、第三次日报、简化结尾字幕

### 3.1 Slice 中真正执行的异常

只有：

- `FREEZE`

### 3.2 Slice 中只保留参考感、不进入执行链的内容

- `BLACKOUT`
- `DISKCLEAN`
- Day 4-Day 7 的运行内容
- B / C 的隐藏对话执行链
- 完整 Day 7 终局话术

这些内容在本文中最多只保留为 **future reference**，不代表本轮应导入 Unreal。

---

## 4. Day 1-Day 3 竖切片 prose source（对齐导入稿）

> 本节是写作参考；真正的表、行 id、列顺序，以 `docs/day1-day3-datatable-ready-script.md` 为准。

## 4.1 Day 1：规则建立 / onboarding

### 主管开场语气

- `早，今天先把当前这个需求过一遍，别紧张，按节奏来。`
- `今天任务不重，把手上的链路先跑顺，下午给我一个简要进度就行。`

### 主管线程感受

- 主管不是在聊天，而是在分派工作节奏
- 文案应该让玩家快速理解“今天的工作要开始了”

### 同事 A 基础回复感受

- `好的，我看一下。`
- `这个需求我先过一遍。`
- `我这边能接。`
- `先按现在这个来吧。`

### 同事 B / C 的存在感

- B 负责“线上压力 / 还在值守”的语气
- C 负责“整理 / 清理 / 缓存”的语气
- 在 slice 中，他们只需要提供存在感，不需要完整隐藏链

### Day 1 日报口吻

- 偏冷静、标准、可复制
- 重点是“工作推进”“没有严重阻塞”

---

## 4.2 Day 2：唯一完整异常链（`FREEZE`）

### 异常弹窗语气

- 标题要短
- 正文要像监控系统弹窗
- 按钮文案使用已冻结的英文：
  - `Report Supervisor`
  - `Handle Myself`

### `Report Supervisor` 路线语气

- 像问题被上级迅速接管
- 主管反馈应该高效、稳定、去情绪化
- 不产生隐藏对话

### `Handle Myself` 路线语气

- 玩家必须感到“离开桌面去处理现实中的异常”
- 处理成功后的提示语要让玩家知道：
  - 温度恢复了
  - 该回电脑了
  - 聊天里有了新的不对劲内容

### Colleague A 隐藏对话语气

- 像从工作话术缝隙里漏出来的旧记忆
- 不要过度解释世界观
- 第一条就要把现实异常和聊天异样连接起来

参考基调：

- `空调调到 16.4 比较好睡。`
- `那个 bug 我之前定位过，在第三行。`
- `你那时候没在线。`
- `……你现在还在吗？`

### Day 2 日报口吻

- 玩家已经经历异常，但文本表面仍然像普通日报
- 句子应允许“问题已处理”与“来源仍不明确”同时成立

---

## 4.3 Day 3：简化收束 / 结尾包装

### Slice 中 Day 3 的定位

Day 3 在竖切片里的作用是：

- 承接 Day 2 路线后果
- 给出主管可疑感与真相靠近感
- 进入简化结尾

### 明确不是 Day 3 slice 执行内容

以下内容不再视为 Day 3 slice 的执行文本：

- `BLACKOUT` 异常弹窗
- B 的完整隐藏链
- 新的 3D 处理目标

如果需要保留这些想法，它们只能视为完整版参考，不再代表本轮导入内容。

### Day 3 报告语气

- 正常路线文本仍保持企业汇报口吻
- 若 `bHasConsumedColleagueAHiddenOption == true`（即 Colleague A 隐藏选项已消费、隐藏对话已完成），可出现一句明显更不稳定、更接近真相的句子

### 简化坏结尾基调

- `你把异常都交给了主管。`
- `问题确实都被处理掉了。`
- `你的工作也越来越顺。`
- `你的话越来越少。`

### 简化好结尾基调

- `你开始自己处理那些“不该存在”的问题。`
- `聊天框里出现了本不该出现的话。`
- `有人还留在里面。`
- `或者说，只剩下那些东西留在里面。`

---

## 5. 表与 prose 的关系（避免双重 authority）

### 5.1 现在的权威分工

- `DT_SystemCopy`、`DT_SupervisorLines`、`DT_NormalReplies`、`DT_HiddenDialogues`、`DT_ReportSentencePools`、`DT_EndingSubtitles`
  - **实际行内容**：看 `docs/day1-day3-datatable-ready-script.md`
- 字段名如 `Text`、`SequenceOrder`、`UnlockRequirement`
  - **实际字段定义**：看 `docs/blueprint-variables-events-and-data-fields.md`
- 这份文档
  - **只保留写作理由、文案口吻、来源说明**

### 5.2 本文可以写什么

可以写：

- 某类台词应该是什么气质
- 某条隐藏线应该传达什么感觉
- 某段结尾的情绪目标是什么

不应该再写：

- `ReplyText` / `DialogueText` / `SentenceText` 这类字段名
- 哪张表该怎么建列
- 哪些 Day 4-Day 7 行也该一起导入 slice

---

## 6. 完整版 / 未来扩展参考（Reference Only）

以下内容保留为未来完整版的 prose 方向，不代表本轮 slice 导入范围：

### 6.1 `BLACKOUT`

可延续的语气方向：

- `Lighting system unavailable.`
- `Visibility reduced.`
- `Only active targets remain visible.`

### 6.2 `DISKCLEAN`

可延续的语气方向：

- `Unauthorized cleanup process detected.`
- `Current task interrupted.`
- `Local cache integrity unstable.`

### 6.3 Day 4-Day 7

可延续的主题方向：

- 主管越来越像“绩效与推进”的人格壳
- B / C 的隐藏线逐渐把“被保留 / 被删除 / 被替代”的主题展开
- Day 7 的汇报句池可以更具攻击性与操控感

> 这些都属于完整版参考，不是本轮 DataTable/Struct 创建依据。

---

## 7. 最终冻结结论

Work Item 3 之后，这份文档已经被降级为：

- **文案参考 / prose source**
- **future reference 容器**

而不再是：

- schema authority
- import authority
- slice runtime contract

因此，如果实现者要开始建 `Struct / DataTable / Widget`：

1. 先看 `docs/blueprint-variables-events-and-data-fields.md`
2. 再按 `docs/day1-day3-datatable-ready-script.md` 导入
3. 只有在需要理解文案气质时，才回看本文
