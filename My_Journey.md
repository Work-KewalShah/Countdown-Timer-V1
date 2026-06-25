# The Journey - Countdown Timer V1

*This is not the technical documentation. The README covers that.*
*This is everything else.*

---

## The Night It Started

It was sometime in the middle of April. I was in bed, phone in hand, scrolling through YouTube looking for something interesting to watch. Nothing was landing. And somewhere between two videos I would not remember the next morning, a thought cut through the noise.

*How much more of my time is the Cyberdeck going to take?*

The Cyberdeck had been my main project for a while. Most of the electronics were done. The software was done. What remained was the controls, the buttons, and the case - the physical shell that would hold everything together. I had been putting it off because I had zero knowledge of CAD. I had never modelled anything in 3D. I had never designed a case, never thought about tolerances, never opened Fusion 360 for more than five minutes.

And then the second thought arrived, right behind the first.

*I don't have a real deadline for any of this.*

That was the honest truth. I had set dates for myself before. I had missed most of them. Not because I was not capable, but because a deadline you cannot see is a deadline you can negotiate with yourself. I was good at that negotiation. Too good.

I put the phone down and stared at the ceiling for a while.

*What if I built something that made the deadline impossible to ignore. A physical device. On my desk. Counting down. Always visible.*

And then the third thought, the practical one.

*I need to learn CAD anyway for the Cyberdeck case. This could be the project I learn it on.*

By the time I fell asleep, the idea was already real enough to start the next morning.

---

## What I Knew. What I Didn't.

I should be honest about where I was starting from.

I had worked with a Raspberry Pi before through the Cyberdeck project. I knew soldering. I knew how to read a wiring diagram. I had some sense of how power rails work and how to not fry something expensive.

What I did not know: ESP32. Arduino IDE. Flashing firmware. LCD displays. CAD. 3D printing. Slicing software. Print settings. Tolerances. Friction fitting. Any of it.

There is something worth mentioning here. In my first semester of college, Electronics was my worst subject. I almost failed it. The theory felt disconnected from anything real. I could not hold onto it because I could not see why it mattered.

What changed was not a class or a teacher or a textbook. It was the drive to make something. To see an idea become a physical object that worked. Once that drive existed, electronics stopped being a subject I was bad at and became a tool I needed to learn. I did not even notice it getting easier because I was too focused on what I was trying to build.

That pattern has repeated itself every time since.

---

## 7.5 Days

I gave myself until the 7th of May.

The build took 7.5 days. I finished at 3am on the 8th. I was not going to redesign the case and reprint everything just to change a date, so the 7th of May it is.

There was one day in the middle where I did not build anything. The prints kept warping. They were not sticking to the hotbed. Every iteration was failing and I had not slept properly in two days. I hit a wall - not a technical wall, a human one. So I stopped.

The second season of Sapne vs Everyone had just come out. It is one of my favourite series. I watched it, slept, and came back the next day.

Sometimes the most productive thing you can do is stop.

---

## Learning CAD - The Door Handle

Before I could touch the Countdown Timer case, I needed to actually learn Fusion 360. Not from tutorials - from a real problem.

The kitchen closet door handle had broken. I decided to model and print a replacement. Simple enough to be achievable in a day or two, complex enough to actually teach me something. Curves, extrudes, fillets, real-world dimensions - everything I would need for the case.

I did not have a digital caliper. I measured the original handle with a manual Vernier caliper and drew up the dimensions in Canva - because I know Canva well and it was the fastest way to get a clean dimensional reference I could work from.

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="Original Door Handle" src="https://github.com/user-attachments/assets/3da1f004-a17c-4542-a0ae-5c5c326d6c70" /> | <img width="320" alt="Canva Screen Door Handel Dimensions" src="https://github.com/user-attachments/assets/3ec76139-0a62-4785-bf2c-c2faffc5bfa2" /> |
| Reference handle | Dimensions mapped in Canva |

</div>

Then I opened Fusion 360 and started modelling. Getting the curves right, understanding how constraints work, building it so that changing one dimension does not collapse the whole sketch - none of it came easily at first. But the door handle was a concrete thing I could hold and measure and compare against. That made the learning stick in a way that a tutorial exercise never would have.

<div align="center">
  <img width="700" alt="Fusion Screen Door Handle Final" src="https://github.com/user-attachments/assets/6253c0ef-e08c-4347-9eb0-f46bdc768292" />
  <br/>
  <em>Final door handle model in Fusion 360 - timeline and browser visible</em>
</div>

<br/>

Then printed it.

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="3D Printing Door Handle" src="https://github.com/user-attachments/assets/b24e810e-343e-4305-a782-62a7d0a8acf0" /> | <img width="320" alt="3D Printer Printing Door Handle Gif" src="https://github.com/user-attachments/assets/ff5fede0-7169-42f1-819f-4df8a29968fa" /> |
| Printing the handle | Print in progress |

</div>

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="3D Printed Door Handle in Hand" src="https://github.com/user-attachments/assets/132e496e-bf9b-402b-97cd-8d6e4acb5016" /> | <img width="320" alt="3D Printed Door Handle Fitted" src="https://github.com/user-attachments/assets/017566fc-0cf7-40e3-a21e-1ae550ca043c" /> |
| Final Printed handle | Fitted in the closet |

</div>

It fit. It worked. The closet door opens and closes on a handle I designed and printed myself.

More importantly - I now knew enough Fusion 360 to start the case.

---

## The Build - Hardware

The hardware side of this build was the part I was most comfortable with coming in, and even then it surprised me.

I started the way I always start - wiring diagram first. Understanding how power flows, how communication works, what connects to what and why. Then temporary wiring on a breadboard to test everything before committing to anything permanent.

The ESP32 was new territory. I had never touched Arduino IDE, never flashed a microcontroller this way, never written code for a device with no operating system underneath it. Everything had to be thought about differently - no threads, no background processes, just a single loop running over and over, handling everything simultaneously without blocking.

The wiring itself came together using CAT5 cable strands - solid core, thin, exactly right for point to point work inside a small case. All ground connections meeting at a single star junction. Power flowing from the TP4056 directly to everything. Simple and clean.

At some point during the build I had the main component stack assembled and seated - ESP32, TP4056, LCD, battery all stacked and wired before the case was finalised. This is what the inside looked like before it was closed up.

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="Middle Component Stack Main" src="https://github.com/user-attachments/assets/10098959-5085-4431-9a47-fd50ae01d1d8" /> | <img width="320" alt="Middle Component Stack Port View 1" src="https://github.com/user-attachments/assets/c229766f-aa54-4310-84ff-02896ac96418" /> |
| Component stack - main view | Port side view 1 |
| <img width="320" alt="Middle Component Stack Port View 2" src="https://github.com/user-attachments/assets/35564713-1302-4086-a487-36556b79fced" /> | |
| Port side view 2 | |

</div>

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="Middle Component Stack Gif 1" src="https://github.com/user-attachments/assets/96b213a7-b781-4856-a4c0-7d099c86b798" /> | <img width="320" alt="Middle Component Stack Gif 2" src="https://github.com/user-attachments/assets/6e1a4a55-ca9e-45cc-b288-d9c20a4a42fb" /> |
| Component stack - with battery plate | Component stack - witout battery plate |

</div>

Claude helped significantly with the firmware. The state machine architecture, the NVS save and resume logic, the backlight management, the game. I would describe what I wanted, we would work through it together, and I would push back when something was wrong - which happened more than once. The custom LCD characters, some of the menu navigation logic, certain game behaviours - those needed correction and iteration the same way the hardware did. The collaboration worked because I knew what I wanted the device to do, even when I did not know exactly how to make it do that.

Without that collaboration I am certain I would have spent significantly longer on the firmware. I am also certain I would have wired something backwards and damaged the ESP32 at least once.

---

## The Build - Case

This is where the real learning happened.

The door handle had given me the basics. Now the case was a different challenge - internal pockets, port cutouts, button holes, snap fit joints, tolerancing everything for real-world print dimensions. Much more complex. Much more at stake because everything needed to fit together at the end.

<div align="center">
  <img width="700" alt="Fusion Screen Countdown Timer Final" src="https://github.com/user-attachments/assets/114f7f2e-fbd5-40af-995a-bb1b92555f64" />
  <br/>
  <em>Final Countdown Timer V1 case in Fusion 360</em>
</div>

<br/>

The case design went through more iterations than I initially expected. Every print revealed something that needed changing. The LCD cutout was too tight. The port openings were slightly off. The button holes needed more clearance. The support columns inside were the wrong height. Each iteration meant printing, fitting, identifying the problem, going back to Fusion, changing the dimension, printing again.

I have two boxes. They are sitting on my desk. They are full of every failed and partial print from this build - every wrong version of every part. Every piece of PLA that taught me something. Those boxes are probably the most honest representation of what this build actually looked like from the inside.

<div align="center">
  <img width="700" alt="Prototype Prints Filled Box" src="https://github.com/user-attachments/assets/b897ad51-a189-4ec0-a617-74c15907f8dd" />
  <br/>
  <em>Every failed print. Every lesson.</em>
</div>

<br/>

One of those iterations taught me something I did not expect. I had designed circular columns inside the case to hold the LCD and the LCD backplate in position. On one fitting attempt, one of the columns broke. I looked at it for a moment and thought - what if I just remove it entirely? The LCD was fitting tightly enough without it. The column was gone in the next version and the fit was actually cleaner. Sometimes the constraint you designed around is not necessary.

The print failures were their own education. Warping. Stringing. Layers not adhering. Prints lifting off the hotbed halfway through a two hour job. After not sleeping for two days, watching a print fail an hour in is a specific kind of frustration. I learned the slicing settings, the temperatures, the bed adhesion techniques, the support placement, when to use brims and when not to. None of this was in the plan when I started. All of it is now knowledge I will use on the Cyberdeck case.

The snap fit joints worked for this build. For the Cyberdeck I will use heat inserts and screws - snap fit loosens over time no matter how well it is designed. That is one of the things this build taught me that directly changes how I approach the next one.

One of the last things I did before calling it done was colour the engraved text and logo on the back with a black marker. The PLA was red. The engravings were shallow. The marker filled them and made everything readable. A simple thing that made a real difference to how the finished device looked.

---

## The Hardest Parts

**The case, overall.** Not any single element of it - the cumulative weight of iteration after iteration, print after print, dimension after dimension. Getting accurate sizes for the LCD cutout, the USB-C port, the Micro USB port, the button holes - all of it required understanding how print settings affect actual printed dimensions. A hole that is 28mm in CAD does not print as 28mm. Learning that gap between design and reality was the steepest part of the learning curve.

**The NVS resume bug.** On the software side this was the most stubborn problem. The device would save state correctly. On reboot it would show the DONE alert before the resume prompt even appeared. Then when we fixed that, the resumed time was completely wrong - 49 days instead of 57 minutes. The root cause was a type mismatch between NVS key types across different versions of the firmware. Tracking it down through Serial Monitor output, one hypothesis at a time, was a proper debugging exercise. It took multiple sessions.

**The button freezing.** Occasionally pressing a button would freeze the entire device. No display update, no response, nothing. Required a full battery disconnect to recover. The cause was an infinite blocking loop in the button release detection - a while loop with no timeout that would hang forever if a button contact stuck even slightly. The fix was simple once found. Finding it was not.

---

## The Unexpected Things

CAD was harder than I expected. I had assumed that with some geometry knowledge and some patience I would pick it up quickly. The geometry helped. The patience was tested.

The broken column that improved the design. I did not plan to remove it. The print forced the decision and the decision turned out to be right.

Coloring the engravings with a marker. That was not in any plan. It was a last minute idea at the end of a very long build. It was the right call and it is one of the details I am most happy with on the finished device.

How much the print settings matter. I knew in theory that slicing configuration affected output quality. I did not appreciate how much until I was debugging warping and adhesion failures at 1am.

---

## 3am. Done.

I was half asleep. I had not slept properly in days. The last print had come out right. The case had snapped together. The firmware was flashed. I picked up the device and powered it on.

The screen lit up. The main menu appeared. I navigated to Current Goal and started the countdown. It ran.

I held it in my hands for a moment.

The last time I felt that specific kind of pride was three or four years earlier. My team had placed second in the nation at the NSG Digirakshak Hackathon. I was the team leader. That was a different kind of achievement - collaborative, competitive, external validation.

This was different. This was just me, a box of failed prints, a reel of CAT5 wire, and something that existed now that did not exist a week ago.

Something of my own. A single piece in the world. Nothing else exists like it anywhere. Not in any store, not on any shelf, not in anyone else's hands. Just mine.

These photos were taken at 2:38am on 8th May 2026. The soldering table still visible in the background. The timestamp is in the image.

<div align="center">

| | |
|:---:|:---:|
| <img width="320" alt="Final Countdown Build Front" src="https://github.com/user-attachments/assets/f946f752-0dcb-4ec3-b2cf-5a6449460093" /> | <img width="320" alt="Final Countdown Build Back" src="https://github.com/user-attachments/assets/a8dc9a0a-c875-449a-b073-d06f51445090" /> |
| Front - 2:38am, 08/05/2026 | Back - 2:38am, 08/05/2026 |
| <img width="320" alt="Final Countdown Build Port Cutout USB C" src="https://github.com/user-attachments/assets/06987277-e1b2-4024-8c9d-bc626ed8efbb" /> | <img width="320" alt="Final Countdown Build Port Cutout Micro USB" src="https://github.com/user-attachments/assets/ab74885f-ffdb-486a-a6f9-8ffc741603e9" /> |
| USB-C port cutout | Micro USB port cutout |

</div>

I went to sleep.

---

## What It Changed

I use it every day now.

When I sit down to work I set a countdown for whatever I am trying to finish. When I study I run the Pomodoro timer. When I play games I start the stopwatch so I can see honestly how much time I am spending.

The awareness is the thing. Not the pressure - I am not watching the countdown anxiously. It is more that the device sitting on my desk makes time visible in a way that a phone app never did. The phone lives in my pocket, behind a lock screen, surrounded by everything else competing for my attention. The device has one job. It does it without asking for anything back.

I procrastinate less. Not because the countdown terrifies me, but because I can see the time passing and I cannot pretend I cannot.

---

## Where This Fits

When I was in school I was editing videos, making graphics, learning animation. I thought those were hobbies. I used all of them making the advertisement video for this build.

When I was in class eleven and twelve I was learning geometry. I thought it was abstract and theoretical. I used it in Fusion 360 sketches.

When I almost failed Electronics in my first semester I thought that subject and I were permanently incompatible. I have now built a working embedded device and am halfway through a Cyberdeck build that involves managing power, RF shielding, and complex wiring in a constrained space.

Nothing you learn is wasted. The only thing that changes is when it becomes useful.

This project is a step in a longer journey. Next is the Network+ exam. Then completing the Cyberdeck - which now has a working case designer behind it. Then Security+. Both this device and the Cyberdeck will be fully open source. I want to keep building things that are useful - to me first, and then to anyone else who wants them.

The maker's journey does not have a destination. It just has the next build.

---

## To Anyone Who Wants to Build Something

Start anyway.

You will not know everything you need to know before you begin. You are not supposed to. The knowing comes from the doing, and only from the doing.

You will hit walls. Some of them are technical. Some of them are just exhaustion and frustration wearing the costume of a technical problem. Learn to tell the difference. When it is the second kind - stop, rest, come back. The problem will still be there. So will you.

Use every tool available to you. AI, documentation, communities, people who have done it before. Using help is not a shortcut. It is how every builder who has ever built anything has worked. None of them did it entirely alone.

And when it is done - when you are holding it in your hands at 3am and it is working and it is real - that feeling belongs to you completely.

No one can build what you would build. That is the whole point.

---

*Kewal Shah* <br>
*V1 - 07/05/2026* <br>
*Cyberdeck - on the way*
