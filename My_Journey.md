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

## The Build - Hardware

The hardware side of this build was the part I was most comfortable with coming in, and even then it surprised me.

I started the way I always start - wiring diagram first. Understanding how power flows, how communication works, what connects to what and why. Then temporary wiring on a breadboard to test everything before committing to anything permanent.

The ESP32 was new territory. I had never touched Arduino IDE, never flashed a microcontroller this way, never written code for a device with no operating system underneath it. Everything had to be thought about differently - no threads, no background processes, just a single loop running over and over, handling everything simultaneously without blocking.

The wiring itself came together using CAT5 cable strands - solid core, thin, exactly right for point to point work inside a small case. All ground connections meeting at a single star junction. Power flowing from the TP4056 directly to everything. Simple and clean.

Claude helped significantly with the firmware. The state machine architecture, the NVS save and resume logic, the backlight management, the game. I would describe what I wanted, we would work through it together, and I would push back when something was wrong - which happened more than once. The custom LCD characters, some of the menu navigation logic, certain game behaviours - those needed correction and iteration the same way the hardware did. The collaboration worked because I knew what I wanted the device to do, even when I did not know exactly how to make it do that.

Without that collaboration I am certain I would have spent significantly longer on the firmware. I am also certain I would have wired something backwards and damaged the ESP32 at least once.

---

## The Build - Case

This is where the real learning happened.

I opened Fusion 360 with no prior experience. I watched enough to understand the basic sketch and extrude workflow. Then I started building.

The first few days of CAD were humbling. I had assumed it would not be that difficult. I was wrong. Getting dimensions accurate, understanding how constraints work, how to build parametrically so that changing one value does not break everything else - none of it was intuitive at first. It took time and repetition and a lot of undoing things I had just done.

But it came. Slowly at first, then faster.

The case design went through more iterations than I initially expected. Every print revealed something that needed changing. The LCD cutout was too tight. The port openings were slightly off. The button holes needed more clearance. The support columns inside were the wrong height. Each iteration meant printing, fitting, identifying the problem, going back to Fusion, changing the dimension, printing again.

I have a box. It is sitting on my desk right now. It is full of every failed and partial print from this build - every wrong version of every part. Every piece of PLA that taught me something. That box is probably the most honest representation of what this build actually looked like from the inside.

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

*Kewal Shah*
*V1 - 07/05/2026 -*
*Cyberdeck - on the way*