# Countdown Timer V1

<div align="center">
  <img width="700" alt="Countdown Timer Hero Shot" src="https://github.com/user-attachments/assets/e96e554a-d238-4fd0-9683-bef82af7542d" />
</div>

<br/>

<div align="center">
  A battery-powered handheld countdown timer built on ESP32 with a 3D printed snap-fit case - track multiple goals simultaneously, run a stopwatch, save state across power cycles, and play a cute little game while you wait.
</div>

---

## Badges

<div align="center">

![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Code-Arduino%20C%2B%2B-teal)
![Material](https://img.shields.io/badge/Case-PLA%203D%20Printed-red)
![Version](https://img.shields.io/badge/Version-V1.0-orange)
![License](https://img.shields.io/badge/License-MIT-green)

</div>

---

## Gallery

<div align="center">

| | |
|:---:|:---:|
| <img width="340" alt="Countdown Timer - Build Front" src="https://github.com/user-attachments/assets/71c1e5af-6bbc-4985-ad0b-c8554a4f16e9" /> | <img width="340" alt="Countdown Timer - Build Back" src="https://github.com/user-attachments/assets/be137510-c871-4f5e-9221-5b12efe8f925" /> |
| Build - Front | Build - Back |
| <img width="340" alt="Countdown Timer - Front" src="https://github.com/user-attachments/assets/a9d54556-75af-4896-bd52-eff8494a49be" /> | <img width="340" alt="Countdown Timer - Back" src="https://github.com/user-attachments/assets/0d7747f0-71ac-4ce3-b670-ffec28e5c35e" /> |
| Render - Front | Render - Back |

</div>

---

## Table of Contents

- [What Is This](#what-is-this)
- [Features](#features)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [How It Works - Software](#how-it-works---software)
- [Case Design](#case-design)
- [Programming the Device](#programming-the-device)
- [Skills Learned](#skills-learned)
- [Major Problems and Solutions](#major-problems-and-solutions)
- [V2 Planned Improvements](#v2-planned-improvements)
- [Repository Structure](#repository-structure)
- [License](#license)
- [Author](#author)

---

> This project exists because I needed to learn 3D Modelling and the only way I prefer learning something is by building something real with it, not by simply following tutorials.
>
> One week. Zero prior CAD experience. A functional snap-fit enclosure with port cutouts, engraved logos, ventilation geometry, toleranced joints, and a deadline I set for myself.
>
> The timer on the device was literally counting down to its own completion ;)

---

## What Is This

I generally build dedicated hardware devices for my own use - tools that solve specific problems in how I work and think. This is one of them.

I was deep into building a handheld Cyberdeck - most of the electronics and software were done, but the case design remained. I had never done 3D modelling before and needed to learn it properly with accurate dimensions, custom fits, and proper strain free joints. I needed a smaller, simpler project to learn on before tackling the Cyberdeck case with complex fixtures and fittings.

The project idea came one night when I was thinking about the Cyberdeck itself. I had set a deadline to finish it but the deadline kept slipping - I was only at 60% completion and had been for a while. The problem was not motivation, it was visibility. If I cannot physically see a deadline, I tend to procrastinate or keep adding scope without realising how much is already on my plate. That is true for the Cyberdeck (I kept adding features after features on top of an already stacked lineup) and for most of my tasks.

So the idea was simple - build a physical device that shows me exactly how much time is left for my current goal. Not a phone app. Not a widget on a screen I share with twenty other distractions. A dedicated physical object sitting on my desk with one job: show me the time I have left!

I also tend to prefer staying disconnected from my phone for most of my work - I use it for calls and messages, which is what phones were originally for. Separate dedicated devices for music, timers, and entertainment keep me focused. This device fits that philosophy.

As a side project it hit everything I needed - a real use case, materials I already had, a chance to learn 3D modelling and 3D printing before the Cyberdeck case, and something genuinely useful when done. It also turned into a much deeper learning experience than expected, covering embedded systems, battery management, NVS flash storage, CAD, slicing, and debugging across both hardware and software.

---

## Features

<div align="center">
  <img width="700" alt="Main Menu Go Through" src="https://github.com/user-attachments/assets/24ad1d43-f99e-4905-a58d-7e0481af2976" />
</div>

<br/>

### Countdown Timers
- **Three independent named slots** - Pomodoro, Current Goal, General - all running simultaneously in the background
- Set time in days, hours and minutes per slot
- Each slot has its own Start/Stop, Set Time, Full Screen and Reset options
- Finished slot triggers an alert regardless of which screen you are currently on

### Stopwatch
- Counts up from zero like a stopwatch
- Pause and resume with the OK button
- Runs in the background when you navigate away
- Takes display priority over countdowns when running

### Smart Backlight and Power Management
- **Dims automatically** after 1 minute of no interaction - always
- **Screen off** after 3 minutes if nothing is running, 4 minutes if only Current Goal is running
- **Stays dimmed indefinitely** if Pomodoro or General is running - glanceable without waking
- Any button press wakes to full brightness instantly
- WiFi and Bluetooth disabled on boot, CPU throttled to 80MHz - battery focused

### State Save and Resume
- Countdown state saved to NVS flash every 10 seconds while running
- On reboot, prompts to resume from last saved point
- Survives full power loss - picks up within ~10 seconds of where it left off

### Idle Behaviour
- **2 minutes idle** - switches to fullscreen countdown of the most urgent timer
- Stopwatch takes display priority over countdowns when running
- If nothing is running - snake animation across the display perimeter
- Backlight dim logic applies to all idle screens

### Battery Indicator
- Live battery percentage always visible in top right corner of every screen
- LiPo discharge curve lookup table for accurate mid-range readings
- 5-reading ADC average to reduce noise

### Dodge Duck Game
- Built-in dodge game accessible from the main menu
- Score is seconds survived
- Obstacle speed increases every 5 seconds
- OK to pause/resume, BACK to exit, score displayed bottom right

### Physical Controls and Ports
- 4 navigation buttons - UP, DOWN, OK, BACK - labelled on case
- BOOT and RESET accessible from back of case without opening
- USB-C charging port on right face, Micro USB upload port on left face
- Snap fit 3D printed case - no screws

---

## Hardware

| Component | Model | Purpose |
|---|---|---|
| Microcontroller | [ESP-WROOM-32](https://robu.in/product/esp-wroom-32-wifi-bluetooth-networking-smart-component-development-board) | Main processor, NVS storage, GPIO |
| Display | [LCD1602 Parallel - Blue Backlight](https://robu.in/product/basic-16x2-character-lcd-white-on-blue-on-blue-5v) | 16x2 character display |
| Charging Module | [TP4056 Type-C with Overcurrent Protection](https://robu.in/product/tp4056-1a-li-ion-lithium-battery-charging-module-with-current-protection-type-c) | LiPo battery charging and protection |
| Battery | [KP 403450 1000mAh LiPo](https://zbotic.in/product/kp-original-403450-1000mah-3-7vsingle-cell-rechargeable-lipo-battery/) | Power source |
| Buttons | [6x6x8mm Tactile switches x 6](https://robu.in/product/6x6x8mm-tactile-push-button-switch-pack-of-20) | 4 navigation + BOOT + RESET |
| Resistors | [10kΩ x 2](https://robu.in/product/mf25-10k-multicomp-pro-through-hole-resistor-10-kohm-250-mw-%c2%b1-1-axial-leaded-250-v), [470Ω x 1](https://robu.in/product/470-ohm-0-25w-metal-film-resistor-pack-of-100) | Voltage divider (battery sense) + backlight current limit |
| Wire | CAT5 cable strands 24 AWG | Internal point to point wiring |

<br/>

### Power Architecture

The TP4056 handles all charging via USB-C. Its OUT+ rail powers everything directly - ESP32 VIN, LCD VDD, and the top of the voltage divider. There is no separate 3.3V or 5V boost converter - the system runs at raw battery voltage (3.3V-4.2V) which the ESP32 and LCD both handle within spec.

```
Battery (+) ──► TP4056 B+
TP4056 OUT+ ──► ESP32 VIN
           ──► LCD Pin 2 (VDD)
           ──► Voltage Divider R1 top
TP4056 OUT- ──► Star Ground Junction
           ──► ESP32 GND
           ──► LCD Pin 1 (VSS)
           ──► All button GND legs
           ──► Voltage divider R2 bottom
```

### Voltage Divider - Battery Sense

Two 10kΩ resistors form a divider from OUT+ to GND. The midpoint connects to ESP32 GPIO 34 (input only, no pullup). At full charge (4.2V) the midpoint sits at 2.1V - safely within the ESP32's 3.3V ADC limit. LCD Pin 3 (V0/contrast) is tied directly to GND - no resistor needed, works correctly for this specific module at battery voltage range.

```
OUT+ ──── R1 (10kΩ) ──── junction ──── R2 (10kΩ) ──── GND
                              |
                          GPIO 34
```

---

## Wiring

<div align="center">
  <img width="800" alt="Countdown Timer V1 - Wiring Diagram" src="https://github.com/user-attachments/assets/db9e9a15-c195-48a2-b54f-7b2c5e9f2de4" />
</div>

<br/>

> For the full annotated diagram with pin tables, power connections, voltage divider breakdown and GND star junction detail - download the [PDF version](wiring/Countdown_Timer_V1_Wiring_Diagram.pdf)

<br/>

### Pin Mapping

| ESP32 Pin | Connects To | Purpose |
|---|---|---|
| VIN | TP4056 OUT+ | Main power input |
| GND | TP4056 OUT- | System ground |
| GPIO 2 | 470Ω - LCD Pin 15 (A) | Backlight PWM |
| GPIO 12 | LCD Pin 6 (E) | LCD Enable |
| GPIO 13 | LCD Pin 4 (RS) | LCD Register Select |
| GPIO 14 | LCD Pin 11 (D4) | LCD Data bit 4 |
| GPIO 25 | LCD Pin 14 (D7) | LCD Data bit 7 |
| GPIO 26 | LCD Pin 13 (D6) | LCD Data bit 6 |
| GPIO 27 | LCD Pin 12 (D5) | LCD Data bit 5 |
| GPIO 32 | Button DOWN | Navigation |
| GPIO 33 | Button UP | Navigation |
| GPIO 18 | Button OK | Navigation |
| GPIO 19 | Button BACK | Navigation |
| GPIO 34 | Voltage divider junction | Battery sense (input only) |
| EN | Button RESET | External reset |
| GPIO 0 (BOOT pad) | Button BOOT | External flash mode |

<br/>

### LCD Pin Reference

| LCD Pin | Label | Connects To |
|---|---|---|
| Pin 1 | VSS | GND |
| Pin 2 | VDD | TP4056 OUT+ |
| Pin 3 | V0 | GND (contrast fixed) |
| Pin 4 | RS | GPIO 13 |
| Pin 5 | RW | GND |
| Pin 6 | E | GPIO 12 |
| Pin 7-10 | D0-D3 | Not connected (4-bit mode) |
| Pin 11 | D4 | GPIO 14 |
| Pin 12 | D5 | GPIO 27 |
| Pin 13 | D6 | GPIO 26 |
| Pin 14 | D7 | GPIO 25 |
| Pin 15 | A (Anode) | 470Ω - GPIO 2 |
| Pin 16 | K (Cathode) | GND |

<br/>

### Component Layer Stack

<div align="center">
  <img width="700" alt="Countdown Timer - Stack" src="https://github.com/user-attachments/assets/51af5cdc-dd9e-4490-b2c6-7f9c00b17071" />
</div>

<br/>

### Internal Wiring Notes
- All wiring is point to point using individual CAT5 cable strands - solid core 24 AWG
- All GND connections meet at a single star junction point - TP4056 OUT- and ESP32 GND together, everything else radiates from there
- BOOT button soldered directly to the onboard BOOT button pads on the ESP32 PCB since GPIO 0 is not broken out on the header of this board variant

---

## How It Works - Software

The firmware is a state machine built in Arduino C++ running on ESP32. Every screen in the UI is a defined state - the main loop reads the current state, handles button input, updates the display, and manages all background tasks simultaneously without blocking. All three countdown slots tick independently every loop iteration. The display only redraws when something actually changes - a second ticking, a button press, a slot finishing - keeping the LCD flicker free.

<br/>

### Countdown Timers

<div align="center">
  <img width="600" alt="Countdown Time Set and Full Screen" src="https://github.com/user-attachments/assets/49176bfc-91ed-4956-9aa2-8d3ff62d74dc" />
</div>

<br/>

Each slot stores its target as an absolute `millis()` timestamp. Every loop iteration computes remaining seconds from that target. Only redraws when the second value changes - no flickering.

<br/>

### Stopwatch

<div align="center">
  <img width="700" alt="Timer Demo" src="https://github.com/user-attachments/assets/40f877c6-b124-438a-a2fd-21605d98984d" />
</div>

<br/>

The stopwatch tracks elapsed time by storing a `startMillis` timestamp and accumulated `elapsedSeconds` at each pause point. On resume it sets a new `startMillis` and adds to the accumulator - no drift, no dependency on absolute time.

<br/>

### Smart Backlight and Power Management

<div align="center">
  <img width="700" alt="Auto Dim While Countdown" src="https://github.com/user-attachments/assets/c2f70e6a-f008-47a9-bfee-9b118bb4e335" />
</div>

<br/>

Three backlight states managed by a runtime check every loop iteration. Behaviour depends on which slots are currently running - not a fixed timer.

| Condition | 1 Minute | 3 Minutes | 4 Minutes |
|---|---|---|---|
| Nothing running | DIM | OFF | - |
| Only Cur.Goal running | DIM | - | OFF |
| Pomodoro or General running | DIM | stays DIM forever | - |
| Stopwatch running | DIM | stays DIM forever | - |

WiFi and Bluetooth are disabled immediately on boot. CPU is throttled to 80MHz. Combined with smart backlight this gives approximately **20-28 hours** of battery life from the 1000mAh cell depending on usage pattern.

<br/>

### NVS State Save and Resume

<div align="center">
  <img width="700" alt="Reset and Resume Countdown" src="https://github.com/user-attachments/assets/afba1b91-ced3-4486-909f-85e7f52db0d7" />
</div>

<br/>

Each running countdown slot saves its remaining seconds to ESP32 NVS flash every 10 seconds. On reboot the firmware reads all three slots, checks for any that were running, and presents a resume prompt before the main loop starts ticking - preventing a race condition where the tick would mark slots as finished before the user could respond. The stopwatch does not save to NVS - a stopwatch resuming from an unknown elapsed time after power loss serves no practical purpose.

<br/>

### Idle Behaviour

<div align="center">

| | |
|:---:|:---:|
| <img width="340" alt="Snake Animation" src="https://github.com/user-attachments/assets/426cfd50-5926-4063-ae2f-25ec9fd56cdc" /> | <img width="340" alt="Auto Dim While Countdown" src="https://github.com/user-attachments/assets/c2f70e6a-f008-47a9-bfee-9b118bb4e335" /> |
| Snake animation - nothing running | Auto dim - countdown running |

</div>

<br/>

After 2 minutes of no interaction the device switches automatically to an idle screen. Priority order:

1. **Stopwatch running** - stopwatch fullscreen
2. **Countdown running** - fullscreen of the slot with least time remaining (most urgent)
3. **Nothing running** - snake animation around the display perimeter

The snake has a directional triangle head that rotates to face its direction of travel, a solid neck block, and small circle body dots.

<br/>

### Dodge Duck Game

<div align="center">
  <img width="600" alt="Dodge Duck Gameplay" src="https://github.com/user-attachments/assets/698579f0-4ec5-4fb5-a14e-9c8051cc01af" />
</div>

<br/>

Accessible from the main menu as the 5th item. A duck on the left dodges bullets coming from the right. UP and DOWN switch rows. Score is seconds survived. Speed increases every 5 seconds. A pattern system controls obstacle rows - maximum 3 consecutive same-row bullets before a forced row switch. OK pauses/resumes, BACK exits cleanly to the main menu. The game tick runs independently of button presses - bullets move on their own clock regardless of player input.

---

## Case Design

Designed in **Autodesk Fusion 360**. Snap fit construction - no screws, no inserts. The case splits into two halves that press together and hold by friction fit tabs. Everything inside sits in dedicated pockets sized to the exact component dimensions with 0.2-0.3mm clearance for print tolerance.

<br/>

### Assembly

<div align="center">

| | |
|:---:|:---:|
| <img width="340" alt="Countdown Timer Exploded" src="https://github.com/user-attachments/assets/a8be8e02-b32f-4844-94fe-428348dab274" /> | <img width="340" alt="Countdown Timer Component Assembly" src="https://github.com/user-attachments/assets/4aa3d529-c5bf-4d6b-ace0-e53cf757cb50" /> |
| Exploded view | Assembly sequence |
| <img width="340" alt="Countdown Timer Front Buttons" src="https://github.com/user-attachments/assets/36e65d95-1e35-4d4b-8907-eb5aa4507e58" /> | <img width="340" alt="Countdown Timer Back Cover Buttons" src="https://github.com/user-attachments/assets/587702f3-c270-45b6-9e3a-d698dd32f3c8" /> |
| Front button assembly | Back cover button assembly |

</div>

<br/>

### Design Decisions

**Snap fit - no screws**
The two halves interlock via friction fit tabs along the inner perimeter. No screws means no inserts to install, no stripping risk, and faster assembly and disassembly. The wall thickness of 2.0mm gives enough rigidity that the tabs hold firmly without cracking under normal use.

**Screen placement**
The LCD sits in a dedicated bezel pocket on the front face. The cutout is sized to expose the full display area while the bezel hides the PCB edges cleanly.

**Button placement**
Four navigation buttons sit along the top edge of the front face with engraved labels below each - UP, DOWN, OK, BACK. Spacing is wide enough for comfortable thumb use without accidental adjacent presses.

**BOOT and RESET on back**
The two programming buttons are recessed on the back face with engraved labels. Separated from the navigation buttons so they cannot be pressed accidentally during normal use.

**Ventilation**
A dot matrix pattern covers most of the back face. Provides passive airflow around the Battery, TP4056 Charger and ESP32 - and acts as the primary aesthetic feature of the back alongside the engraved logo, version number and build date.

**Port cutouts**

<div align="center">

| | |
|:---:|:---:|
| <img width="200" alt="Countdown Timer - USB C Cut" src="https://github.com/user-attachments/assets/ca266ea6-9367-4783-bcd2-dede5b151ebe" /> | <img width="200" alt="Countdown Timer - Micro USB Cut" src="https://github.com/user-attachments/assets/1f965abd-7ca2-4775-a946-1e959b80b263" /> |
| USB-C - right face (charging) | Micro USB - left face (upload) |

</div>

Both ports accessible from outside the case without opening. USB-C on the right face for charging via TP4056. Micro USB on the left face for code upload to ESP32. Cutouts have 0.4mm clearance on each side for print tolerance.

**Logo engraving**
The character silhouette on the back is engraved at 0.4mm depth - exactly one nozzle width - which gives clean crisp edges at this scale. The logo is custom made and its SVG was imported into Fusion 360 directly from Inkscape and extruded as a cut.

<br/>

### Print Settings

Print settings can be changed depending on your filament and printer type.

| Setting | Value |
|---|---|
| Printer | Bambu A1 Mini |
| Material | PLA |
| Layer height | 0.2mm |
| Wall thickness | 2.0mm (5 perimeters at 0.4mm nozzle) |
| Wall loop | 3 |
| Infill | 20% Rectilinear |
| Nozzle | 0.4mm |
| Engraving depth | 0.4mm |
| Order of Walls | Outer/Inner |
| Print speed | 25mm/s Initial Layer, 50mm/s Initial Layer Infill |
|  | 60-70mm/s Outer Walls, 150mm/s Inner Walls |
| Min - Max Fan Speed | 40 - 40 |

<br/>

### Case Dimensions

| Dimension | Value |
|---|---|
| Width | ~86mm |
| Height | ~28.5mm |
| Depth | ~50mm |

---

## Programming the Device

### Requirements

| Tool | Details |
|---|---|
| Arduino IDE | 2.x or later |
| ESP32 Board Package | Version 3.x - install via Boards Manager, search ESP32 by Espressif |
| LiquidCrystal Library | By Arduino - install via Library Manager |
| Preferences | Built into ESP32 board package - no install needed |
| WiFi.h | Built into ESP32 board package - no install needed |

<br/>

### Board Settings in Arduino IDE

| Setting | Value |
|---|---|
| Board | ESP32 Dev Module |
| Upload Speed | 115200 |
| CPU Frequency | 240MHz (set in code to 80MHz at runtime) |
| Flash Size | 4MB |
| Partition Scheme | Default |
| Port | whichever COM port your ESP32 appears on |
| Firmware | CP210x_Windows_Drivers (for my specific board, yours can be different) |

<br/>

### Upload Procedure

Since the ESP32 and BOOT button are inside the case, use the external BOOT and RESET buttons on the back face.

<div align="center">
  <img width="600" alt="Boot Button Demo" src="https://github.com/user-attachments/assets/cf1cd8e2-c9ea-4a9a-85b6-bd76663306c6" />
</div>

<br/>

1. Connect Micro USB cable to the left face port
2. Open Arduino IDE and select the correct COM port
3. Hold the **BOOT** button on the back of the case
4. While holding BOOT, press and release the **RESET** button
5. Release BOOT
6. Click **Upload** in Arduino IDE
7. Wait for upload to complete
8. Press **RESET** once to restart the device and run the new firmware

> If upload fails with an MD5 mismatch error, try a different USB cable - power-only cables have no data lines and look identical to data cables. Also try enabling Clear Flash while uploading new firmware in Tools and lowering upload speed to 115200 in Tools - Upload Speed.

<br/>

### First Flash - NVS

On the very first upload to a fresh ESP32, NVS is empty and the device boots straight to the main menu. No action needed. If you are reflashing over an existing build and want to clear saved timer state, add this temporarily to the top of `setup()` and upload once, then remove and upload again:

```cpp
prefs.begin("slot0", false); prefs.clear(); prefs.end();
prefs.begin("slot1", false); prefs.clear(); prefs.end();
prefs.begin("slot2", false); prefs.clear(); prefs.end();
```

<br/>

### Navigation Reference

| Button | General Use | In Game | In Set Time |
|---|---|---|---|
| **UP** | Scroll up / increment | Move duck to top row | +1 (hold for auto repeat) |
| **DOWN** | Scroll down / decrement | Move duck to bottom row | -1 (hold for auto repeat) |
| **OK** | Confirm / enter / start | Pause / resume / retry | Confirm and next field |
| **BACK** | Go back one level | Exit to main menu | Previous field / exit |

---

## Skills Learned

This project covered a much broader range of skills than expected - embedded C++, ESP32 architecture, battery management, parallel LCD communication, CAD, 3D printing, slicing, and systematic hardware debugging.

The full breakdown with specifics on each skill, what was learned from zero, and what surprised me most is in the [build journey document](My_Journey.md).

---

## Major Problems and Solutions

Five significant bugs and hardware issues were encountered and resolved during this build - GPIO silent pullup failure, battery ADC double correction, NVS type mismatch producing garbage resume values, a boot sequence race condition marking timers as finished before the resume prompt appeared, and an infinite blocking loop freezing the device on button press.

Full root cause analysis, fixes and lessons for each in [My_Journey.md](My_Journey.md).

---

## V2 Planned Improvements

V1 is complete and functional. These are the improvements planned for the next revision.

<br/>

### Hardware Changes

**Passive Buzzer - Audio Alerts**
A passive buzzer on any free GPIO pin for countdown completion alerts. Currently the only alert is a screen notification which is easy to miss if the device is face down or in a bag.

**Screen Tilt - Better Desk Viewing Angle**
Tilting the screen pocket back 10-15 degrees in Fusion 360 for better readability when the device sits on a desk without picking it up.

**Power Toggle Switch**
A slide or toggle switch inline on the battery positive wire for clean power off without disconnecting the battery physically.

<br/>

### Software Changes

**DS3231 RTC Module - Accurate Power-Off Time Tracking**
Currently the device treats power-off as a pause and resumes from the last saved point. A DS3231 RTC module connected via I2C would calculate exact elapsed time during power-off and subtract it from remaining seconds on resume - making the countdown accurate across reboots.

---

## Repository Structure

```
Countdown-Timer-V1/
│
├── firmware/
│   └── Countdown_Timer_V1.ino
│
├── case/
│   └── Countdown_Timer_V1_Case.3mf
│
├── wiring/
│   ├── Countdown_Timer_V1_Wiring_Diagram.png
│   └── Countdown_Timer_V1_Wiring_Diagram.pdf
│
├── images/
│   ├── Countdown_Timer_Build_Front.jpeg
│   ├── Countdown_Timer_Build_Back.jpeg
│   ├── Countdown_Timer_Front.png
│   ├── Countdown_Timer_Back.png
│   ├── Countdown_Timer_Stack.png
│   ├── Countdown_Timer_USB_C_Cut.png
│   └── Countdown_Timer_Micro_USB_Cut.png
│
├── My_Journey.md
├── LICENSE
└── README.md
```

<br/>

### File Notes

**firmware/Countdown_Timer_V1.ino** - Single file sketch, no external dependencies beyond the ESP32 board package and LiquidCrystal library.

**case/Countdown_Timer_V1_Case.3mf** - Both case halves and all internal fittings. Supports needed only for the front outer piece. Keep flat faces on the plate and clean the bed with IPA before printing.

**wiring/Countdown_Timer_V1_Wiring_Diagram.pdf** - Full annotated reference with pin tables and connection detail. Use this over the PNG for proper zoom.

---

## License

This project is open source under the [MIT License](LICENSE).

You are free to use, modify, and distribute this project for personal or commercial purposes. Attribution appreciated but not required.

---

## Author

**Kewal Shah**

B.E. in Information Technology, self-taught builder, and someone who finds it genuinely difficult to stop once something is interesting enough.

I work across embedded systems, software, cybersecurity, and hardware design - not because I planned a neat career path across all of them, but because each problem I solve tends to open three more that I cannot leave alone. Cybersecurity is where I am currently going deep - drawn to it for the same reason I am drawn to hardware: there is always something underneath the surface that most people do not look at.

This project is a good example of how I learn. I needed to know Fusion 360 for the Cyberdeck case. Rather than doing tutorials, I built something real, from scratch, under self-imposed pressure, in about a week - and ended up with a fully functional snap-fit enclosure with port cutouts, engraved logos, ventilation geometry, and print tolerancing that actually works. That is the only way learning sticks for me.

V1 completed 07/05/2026 - technically in the early hours of 8th May around 3am, but I was not going to redesign the case and reprint just to change a date, so 7th May it is :)

V2 after Cyberdeck completion. [Read the full build journey](My_Journey.md).

<br/>

| | |
|---|---|
| GitHub | [@Work-KewalShah](https://github.com/Work-KewalShah) |
| LinkedIn | [Kewal Shah](https://www.linkedin.com/in/kewal-shah-work/) |

<br/>

> *"If I can't physically see the deadline, I tend to procrastinate - so I built something that makes it impossible to ignore."*

---

<div align="center">

**If this project helped you or inspired a build of your own, consider leaving a ⭐ on the repository.**

</div>
