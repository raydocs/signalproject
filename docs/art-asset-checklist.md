# Signal Project Art Asset Checklist

> Scope: full-content version of the game, not the 48-hour vertical slice.
> Engine direction: Unreal Engine with 3D room exploration and 2D desktop/gameplay UI.
> Goal: give art, tech art, UI, and audio-adjacent collaborators a single asset breakdown to estimate workload.

---

## 1. Content Baseline

This checklist assumes the full version includes:

- 7 in-game days
- 3 colleague skill lines: FREEZE, BLACKOUT, DISK_CLEAN
- 3 anomaly handling sequences in 3D space
- 3 minigames in desktop mode
- full chat UI, hidden dialogue UI, report editor UI
- 1 good ending, 1 bad ending, plus intermediate route feedback
- supervisor reveal sequence and final prompt-injection sequence

---

## 2. Asset Summary By Discipline

### 2D Art

- Desktop OS UI kit
- Chat interface art
- Hidden-option and glitch dialogue presentation art
- Minigame interface art for 3 minigames
- HUD, system alert, choice popup, report editor art
- 2D VFX overlays for anomalies and endings
- Character portraits, avatars, status icons, labels
- Typography treatment, signal/glitch motif assets, noise textures

### 3D Art

- One full apartment/home-office environment
- Interactive room props for anomaly handling
- Computer desk setup and hero interaction props
- Lighting states for normal and haunted variants
- 3D set dressing for story escalation over 7 days
- Ending-state environment variations

---

## 3. 2D Art Checklist

## 3.1 Desktop Shell And System UI

| Asset | Qty | Notes |
| --- | ---: | --- |
| Desktop wallpaper variants | 3 | Normal, late-night tense, endgame corrupted |
| Desktop background noise overlays | 4 | Clean static, scanline, mild glitch, severe glitch |
| Window frame style kit | 1 set | App window top bar, border, resize corners, inactive state |
| OS icon pack | 18-24 | Chat, reports, tasks, files, recycle bin, settings, terminal, alerts, shutdown, folder, attachments |
| Cursor set | 5-7 | Default, hover, text, disabled, anomaly lagged, corrupted |
| Top status bar/HUD | 1 set | Day indicator, signal/environment indicators, optional hidden route feedback |
| Notification toast styles | 3 | Normal system, anomaly alert, hidden story unlock |
| Modal frame kit | 1 set | Choice popup, warning popup, confirm popup |
| Button style kit | 3 families | Default UI, hidden option, danger/alert |
| Input field style kit | 1 set | Chat box, report editor, locked/disabled state |

## 3.2 Chat Interface

| Asset | Qty | Notes |
| --- | ---: | --- |
| Chat app main layout | 1 | Contact list, thread panel, input area |
| Message bubble styles | 5 | Player, colleague, supervisor, system, hidden fragment |
| Contact avatars | 5 | Player placeholder, colleague A, colleague B, colleague C, supervisor |
| Avatar corrupted variants | 4 | A/B/C and supervisor reveal state |
| Presence/status badges | 6-8 | Online, typing, idle, archived, hidden, high compute |
| Typing indicator animations | 3 | Normal, jittering, broken |
| Hidden option card style | 1 set | Special left bar, noise texture, subtle blue signal motif |
| Chat divider/date chips | 2-3 | Normal and corrupted variants |
| Attachment/file chip style | 1 set | Optional if fake work files appear in chat |
| Voice message card style | 2 | Normal supervisor voice, corrupted playback |

## 3.3 Character 2D Presentation

| Asset | Qty | Notes |
| --- | ---: | --- |
| Colleague portrait set | 3 | A/B/C portrait art or stylized profile image |
| Supervisor portrait set | 1 | Needs normal and reveal version |
| Portrait reveal/corrupted variants | 4 | Separate export if done as layered art |
| Colleague identity chips | 3 | Name tag plus anomaly symbol |
| Skill label graphics | 4 | FREEZE, BLACKOUT, DISK_CLEAN, SKILL_SUPERVISOR |
| Compute/route indicator labels | 3-5 | HIGH, LOW, ALLY, DISTILLED, etc. |

## 3.4 Minigame UI Art

### A. Lianliankan / Dependency Match

| Asset | Qty | Notes |
| --- | ---: | --- |
| Tile icon family | 12-16 pairs | Code/module themed, readable at small size |
| Board frame | 1 | Fits desktop app aesthetic |
| Link effect art | 2-3 | Normal connect line, lagged/frozen connect line |
| Combo/clear feedback | 3-4 | Match, chain, perfect, timeout |
| Timer/progress bar | 1 | Shared style okay if consistent |
| Freeze anomaly overlay elements | 4-6 | Frost edge, cursor trail sprite, cold pulse, condensation particles |

### B. Whack-a-Bug

| Asset | Qty | Notes |
| --- | ---: | --- |
| Bug enemy icons | 6-8 | Different silhouettes to avoid repetition |
| Bug glow variants | 6-8 | Needed for blackout readability |
| Spawn hole or panel markers | 6-9 | Depending on layout |
| Hit feedback sprites | 3-4 | Smash, critical, streak, miss |
| Blackout overlay kit | 4-5 | Full-screen mask, vignette, glow halo, light leak |
| Score combo UI | 1 set | Score digits, chain feedback |

### C. Bugfix Line Finder

| Asset | Qty | Notes |
| --- | ---: | --- |
| Pseudo-code panel style | 1 | Syntax-highlighted fake editor look |
| Syntax color theme | 2 | Normal and corrupted |
| Highlight states | 4 | Hover, selected, wrong, correct |
| Error marker icons | 3 | Hint, warning, reset |
| Disk clean interruption overlay | 4-5 | Progress wipe, deletion bands, data scramble, reset flash |

## 3.5 Report Editor And Ending UI

| Asset | Qty | Notes |
| --- | ---: | --- |
| Report editor screen | 1 | Main terminal-like or enterprise report window |
| Sentence option chips | 5 visual states | Locked, available, selected, injected, detected |
| Skill unlock influence markers | 3 | One marker per colleague contribution |
| Submit/result states | 3 | Normal ack, detected failure, successful injection |
| Supervisor detection warning art | 2-3 | Trace, scan, flagged |
| Final injection emphasis card | 1 | The fully unlocked final line |
| Ending title cards | 2 | Good ending, bad ending |
| Ending subtitle overlays | 4-6 | Major text beats during final montage |

## 3.6 Narrative UI And Choice Presentation

| Asset | Qty | Notes |
| --- | ---: | --- |
| Anomaly choice popup | 1 base + 3 themed variants | FREEZE, BLACKOUT, DISK_CLEAN can tint differently |
| Route feedback banner | 2 | Supervisor approval vs hidden route unlock |
| Day transition card | 7 | One per day, can share layout with different text/background |
| Flashback/reveal card | 3-5 | For supervisor reveal and memory fragments |
| Soft-lock pressure warning states | 3 | Subtle, medium, critical |
| System boot/shutdown cards | 2 | Opening and ending transitions |

## 3.7 2D VFX And Overlay Assets

| Asset | Qty | Notes |
| --- | ---: | --- |
| Glitch frame overlays | 8-12 | Can be looped or layered |
| RGB split masks | 3-5 | Mild, medium, intense |
| Scanline/noise textures | 5-8 | Tileable preferred |
| Frost border overlays | 3-4 | Different intensity levels |
| Condensation streak decals | 4-6 | For freeze state |
| Blackout masks | 3 | Full, pulsing, reveal |
| Data corruption textures | 4-6 | Disk clean and reveal events |
| Distortion cards | 3-5 | Used in chat reveals and endings |
| Subtitle distortion frames | 2-3 | For voice reveal and ending captions |

## 3.8 Branding, Typography, Motif Assets

| Asset | Qty | Notes |
| --- | ---: | --- |
| Title logo | 1 | Signal |
| Subtitle or jam branding lockup | 1-2 | Optional jam submission and splash use |
| Font treatment guide | 1 set | Desktop UI font, alert font, glitch font |
| Signal motif graphics | 4-8 | Bars, waveform, cold-blue markers, compute tags |
| Loading/boot glyphs | 6-10 | Reusable decorative terminal/system symbols |

---

## 4. 3D Art Checklist

## 4.1 Core Environment

| Asset | Qty | Notes |
| --- | ---: | --- |
| Main apartment room environment | 1 | Single explorable room is enough, but must support 7-day storytelling |
| Room material set | 1 set | Walls, floor, ceiling, trim |
| Window set | 1 | Supports outside lighting changes |
| Door set | 1-2 | Entry and maybe bathroom/storage suggestion |
| Ceiling light fixture | 1-2 | Needed for blackout interaction |
| AC unit | 1 hero prop | Must be interactable and visible |
| Desk setup | 1 hero set | Desk, monitor, keyboard, mouse, chair |
| Computer tower or disk source prop | 1 | Needed if disk clean is externalized into 3D action |
| Shelves/storage props | 4-8 | Set dressing |
| Everyday clutter props | 20-35 | Cups, notes, books, cables, tissues, chargers, boxes |
| Personal life props | 8-15 | Framed photos, mug, blanket, snack packs, medicine, fan, lamp |

## 4.2 Interactive Props

| Asset | Qty | Notes |
| --- | ---: | --- |
| Computer interaction state mesh/materials | 2-3 states | Off, on, corrupted/on-alert |
| AC interaction indicator | 1 set | LED or subtle material swap |
| Light switch or room light interaction cue | 1-2 | For blackout handling |
| Disk clean interaction prop | 1-2 | External drive, tower, breaker, utility panel, depending on design |
| Chair interaction animation support prop | 1 | If player sits at desk |
| Door/boundary blockers | 1 set | Keep scope contained |

## 4.3 Environment Variants Across The Full Game

| Asset | Qty | Notes |
| --- | ---: | --- |
| Day progression dressing states | 7 light variants | Day-by-day mood through lighting and subtle room state change |
| Late-night lighting variants | 3-4 | Normal overtime, tension, reveal, ending |
| Clutter escalation variants | 3 | Room gradually degrades under pressure |
| Frosted room state | 1 set | Material and decal support for freeze event |
| Blackout state | 1 set | Emergency darkness setup with selective emissives |
| Disk-clean corruption state | 1 set | Screen flicker, prop light pulses, material corruption |
| Bad ending room state | 1 | Sterile, system-owned, depersonalized feel |
| Good ending room state | 1 | Post-chaos aftermath or abrupt company collapse cue |

## 4.4 Materials, Decals, And Surface Effects

| Asset | Qty | Notes |
| --- | ---: | --- |
| Generic room material instances | 8-12 | Wall paint, wood desk, plastic, metal, fabric |
| Hero prop material instances | 5-8 | AC plastic, monitor glass, keyboard, mug, light fixture |
| Frost decals | 5-8 | Around AC, window, monitor edges |
| Dirt/wear decals | 6-10 | To evolve room over time |
| Signal/corruption decals | 4-6 | Subtle uncanny details near late game |
| Emissive material variants | 3-5 | Monitor glow, bug glow, light flicker |

## 4.5 3D VFX Support Assets

| Asset | Qty | Notes |
| --- | ---: | --- |
| Cold breath/frost particle textures | 2-4 | If player presence is visible enough |
| Dust motes | 1-2 | General atmosphere |
| Light flicker VFX textures | 2-3 | For blackout and reveal moments |
| Data corruption world-space particles | 2-4 | Disk-clean anomalies |
| Screen-space monitor distortion cards | 2-3 | Hybrid 2D/3D effect for computer interaction |

## 4.6 First-Person Or Third-Person Presentation Assets

| Asset | Qty | Notes |
| --- | ---: | --- |
| Player hands set | 1 | If first-person interactions are visible |
| Interaction prompt widgets | 1 set | Can be treated as diegetic UI |
| Sit/stand camera helper assets | 1 set | Mostly design/tech, but art may need seat alignment markers |

---

## 5. Cutscene And Reveal Art Needs

## 5.1 Supervisor Reveal

| Asset | Qty | Notes |
| --- | ---: | --- |
| Supervisor portrait corrupted state | 1 | Required |
| Compute tag reveal graphic | 1 | `[SKILL_SUPERVISOR | COMPUTE: HIGH]` |
| Voice playback UI corrupted state | 1 | Player must see the reveal in interface |
| Chat history corruption pass | 1 set | Prior messages feel newly mechanical |
| Glitch burst overlay package | 1 set | Short, high-impact sequence |

## 5.2 Ending Montage Assets

| Asset | Qty | Notes |
| --- | ---: | --- |
| Fake report/result screens | 3-5 | Boss reaction, system logging, post-action outcomes |
| Fake online backlash cards/screens | 3-6 | Error screenshots, launch failure panels, social posts if desired |
| Distilled employee terminal cards | 2-3 | Bad ending flavor |
| New employee onboarding card | 1 | Bad ending final sting |
| Company collapse or shutdown cards | 2-3 | Good ending wrap-up |

---

## 6. Recommended Folder Structure For Art Production

```text
signalproject/
  art/
    2d/
      ui/
        desktop/
        chat/
        report/
        choice/
      minigames/
        lianliankan/
        whackabug/
        bugfix/
      portraits/
      overlays/
      typography/
      endings/
    3d/
      environment/
        room/
        props/
        decals/
        materials/
      interactables/
        computer/
        ac/
        lights/
        disk_clean/
      variants/
        day_states/
        endings/
      vfx_support/
```

---

## 7. Workload Notes

If the team really wants the full-content target documented before scoping down, this is the minimum art reality:

- 2D art carries most of the gameplay readability and narrative clarity.
- 3D art carries atmosphere, embodiment, and anomaly handling.
- The full version is art-heavy because the same room and UI must evolve over 7 days without feeling static.
- The biggest hidden art cost is not character art, but state variants: corrupted UI, day progression, reveal states, ending states.

---

## 8. Suggested Next Documents

After this checklist, the next most useful production docs are:

1. asset priority list for the 48-hour build
2. concept brief for 3D room mood and camera framing
3. UI wireframe pack for desktop/chat/minigames/report flow
4. day-by-day content sheet matching art states to narrative beats
