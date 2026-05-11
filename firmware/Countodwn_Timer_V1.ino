#include <LiquidCrystal.h>
#include <Preferences.h>
#include <WiFi.h>

// ── Pins ──────────────────────────────────────────────────────────────────────
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

const int BTN_UP     = 33;
const int BTN_DOWN   = 32;
const int BTN_SELECT = 18;
const int BTN_BACK   = 19;
const int BACKLIGHT  = 2;
const int BATT_PIN   = 34;

// ── Backlight levels ──────────────────────────────────────────────────────────
const int BL_BRIGHT = 200;
const int BL_DIM    = 30;
const int BL_OFF    = 0;

// ── Timeout constants (ms) ────────────────────────────────────────────────────
const unsigned long T_DIM          =  60000UL;
const unsigned long T_OFF_NOTHING  = 180000UL;
const unsigned long T_OFF_CURGOAL  = 240000UL;
const unsigned long AUTOFS_TIMEOUT = 120000UL;
const unsigned long SAVE_INTERVAL  =  10000UL;

// ── Slot indices ──────────────────────────────────────────────────────────────
const int SLOT_POMODORO = 0;
const int SLOT_CURGOAL  = 1;
const int SLOT_GENERAL  = 2;

// ── Custom characters ─────────────────────────────────────────────────────────
byte charFilled[8]    = { 0b00000,0b01110,0b11111,0b11111,0b11111,0b01110,0b00000,0b00000 };
byte charHollow[8]    = { 0b00000,0b01110,0b10001,0b10001,0b10001,0b01110,0b00000,0b00000 };
byte charArrow[8]     = { 0b01000,0b01100,0b01110,0b01111,0b01110,0b01100,0b01000,0b00000 };
byte charSnakeNeck[8] = { 0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b00000,0b00000 };
byte charSnakeBody[8] = { 0b00000,0b00000,0b00100,0b01110,0b01110,0b00100,0b00000,0b00000 };
byte charDuck[8]      = { 0b00110,0b00111,0b01110,0b11111,0b01110,0b01010,0b01010,0b00000 };
byte charBullet[8]    = { 0b00000,0b00100,0b01111,0b11111,0b01111,0b00100,0b00000,0b00000 };

byte charHeadR[8] = { 0b10000,0b11000,0b11100,0b11110,0b11100,0b11000,0b10000,0b00000 };
byte charHeadL[8] = { 0b00001,0b00011,0b00111,0b01111,0b00111,0b00011,0b00001,0b00000 };
byte charHeadD[8] = { 0b11111,0b11111,0b01110,0b01110,0b00100,0b00100,0b00000,0b00000 };
byte charHeadU[8] = { 0b00000,0b00100,0b00100,0b01110,0b01110,0b11111,0b11111,0b00000 };

// ── Countdown Slots ───────────────────────────────────────────────────────────
const int   NUM_SLOTS             = 3;
const char* SLOT_NAMES[NUM_SLOTS] = { "Pomodoro", "Cur.Goal", "General " };
const char* NVS_NS[NUM_SLOTS]     = { "slot0",    "slot1",    "slot2"    };

enum SlotState { SLOT_IDLE, SLOT_RUNNING, SLOT_FINISHED };

struct Slot {
  SlotState     state;
  long          setDays, setHours, setMins;
  unsigned long remainingSeconds;
  unsigned long targetMillis;
};
Slot slots[NUM_SLOTS];

// ── Stopwatch Timer ───────────────────────────────────────────────────────────
struct StopwatchState {
  bool          running;
  bool          paused;
  unsigned long elapsedSeconds;   // total elapsed at last pause
  unsigned long startMillis;      // millis() when last started/resumed
};
StopwatchState sw = { false, false, 0, 0 };

unsigned long swCurrentSeconds() {
  if (!sw.running) return sw.elapsedSeconds;
  return sw.elapsedSeconds + (millis() - sw.startMillis) / 1000UL;
}

void swStart() {
  sw.elapsedSeconds = 0;
  sw.startMillis    = millis();
  sw.running        = true;
  sw.paused         = false;
}

void swPause() {
  if (sw.running && !sw.paused) {
    sw.elapsedSeconds = swCurrentSeconds();
    sw.paused         = true;
    sw.running        = false;
  }
}

void swResume() {
  if (sw.paused) {
    sw.startMillis = millis();
    sw.running     = true;
    sw.paused      = false;
  }
}

void swStop() {
  sw.running        = false;
  sw.paused         = false;
  sw.elapsedSeconds = 0;
  sw.startMillis    = 0;
}

// ── Screens ───────────────────────────────────────────────────────────────────
enum Screen {
  SCR_MAIN,
  SCR_SLOT_MENU,
  SCR_SET_DAYS, SCR_SET_HOURS, SCR_SET_MINS,
  SCR_FULLSCREEN,
  SCR_CONFIRM_RESUME,
  SCR_GAME,
  SCR_IDLE_SNAKE,
  SCR_IDLE_FULLSCREEN,
  SCR_TIMER_MENU,
  SCR_TIMER_FULLSCREEN
};
Screen currentScreen    = SCR_MAIN;
Screen screenBeforeIdle = SCR_MAIN;

int selectedSlot   = 0;
int menuCursor     = 0;   // 0-4 (4 = Dodge Duck)
int slotMenuCursor = 0;
int timerMenuCursor = 0;  // 0=Full Screen  1=Stop & Reset

long tmpDays = 0, tmpHours = 0, tmpMins = 0;

// ── Timing ────────────────────────────────────────────────────────────────────
unsigned long lastActivity = 0;
unsigned long lastSaveTime = 0;

// ── Backlight state ───────────────────────────────────────────────────────────
enum BlState { BL_STATE_BRIGHT, BL_STATE_DIM, BL_STATE_OFF };
BlState blState = BL_STATE_BRIGHT;

// ── Alert ─────────────────────────────────────────────────────────────────────
int           alertSlot    = -1;
unsigned long alertShownAt = 0;
bool          showingAlert = false;

// ── Debounce ──────────────────────────────────────────────────────────────────
unsigned long lastBtnTime    = 0;
const unsigned long DEBOUNCE = 200UL;

// ── Snake animation ───────────────────────────────────────────────────────────
unsigned long lastSnakeTick = 0;
int           snakePos      = 0;
int           lastSnakeDir  = -1;
const unsigned long SNAKE_TICK = 120UL;

// ── Dodge Duck game ───────────────────────────────────────────────────────────
const unsigned long DODGE_TICK_START     = 300UL;
const unsigned long DODGE_TICK_MIN       =  70UL;
const unsigned long DODGE_SPEED_INTERVAL = 5000UL;

struct DodgeState {
  int           playerRow;
  int           obs1Col, obs1Row;
  int           obs2Col, obs2Row;
  unsigned long lastSecondTick;
  int           displayScore;
  bool          dead;
  bool          started;
  bool          paused;
  unsigned long tickMs;
  unsigned long lastSpeedUp;
  int           obsPattern;
  int           obsPatternCount;
  int           sameRowCount;
} dg;

unsigned long lastDodgeTick = 0;

Preferences prefs;

// ─────────────────────────────────────────────────────────────────────────────
// BATTERY
// ─────────────────────────────────────────────────────────────────────────────
const float battVoltTable[] = { 4.10, 3.95, 3.80, 3.70, 3.60, 3.50, 3.40, 3.30 };
const int   battPctTable[]  = {  100,   85,   70,   55,   40,   25,   10,    0  };
const int   battTableLen    = 8;

int getBattery() {
  long sum = 0;
  for (int i = 0; i < 5; i++) { sum += analogReadMilliVolts(BATT_PIN); delay(2); }
  float mv    = sum / 5.0f;
  float battV = (mv / 1000.0f) * 2.0f;
  if (battV >= battVoltTable[0])                return 100;
  if (battV <= battVoltTable[battTableLen - 1]) return 0;
  for (int i = 0; i < battTableLen - 1; i++) {
    if (battV <= battVoltTable[i] && battV >= battVoltTable[i + 1]) {
      float range = battVoltTable[i]     - battVoltTable[i + 1];
      float pos   = battV                - battVoltTable[i + 1];
      float frac  = pos / range;
      int   pct   = battPctTable[i + 1] + (int)(frac * (battPctTable[i] - battPctTable[i + 1]));
      return constrain(pct, 0, 100);
    }
  }
  return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// BACKLIGHT
// ─────────────────────────────────────────────────────────────────────────────
void setBacklight(int val) { ledcWrite(BACKLIGHT, val); }

void wakeScreen() {
  blState      = BL_STATE_BRIGHT;
  lastActivity = millis();
  setBacklight(BL_BRIGHT);
}

void updateBacklight() {
  if (currentScreen == SCR_GAME || showingAlert) return;
  unsigned long elapsed = millis() - lastActivity;
  bool pomRunning     = (slots[SLOT_POMODORO].state == SLOT_RUNNING);
  bool curGoalRunning = (slots[SLOT_CURGOAL].state  == SLOT_RUNNING);
  bool genRunning     = (slots[SLOT_GENERAL].state  == SLOT_RUNNING);
  bool otherRunning   = pomRunning || genRunning;
  bool anyRunning     = pomRunning || curGoalRunning || genRunning || sw.running;

  if (blState == BL_STATE_BRIGHT) {
    if (elapsed > T_DIM) { blState = BL_STATE_DIM; setBacklight(BL_DIM); }
  } else if (blState == BL_STATE_DIM) {
    if (otherRunning || sw.running) return;
    if (curGoalRunning && !otherRunning) {
      if (elapsed > T_OFF_CURGOAL) { blState = BL_STATE_OFF; setBacklight(BL_OFF); }
      return;
    }
    if (!anyRunning) {
      if (elapsed > T_OFF_NOTHING) { blState = BL_STATE_OFF; setBacklight(BL_OFF); }
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// PRINT HELPERS
// ─────────────────────────────────────────────────────────────────────────────
void print2(int v) { if (v < 10) lcd.print('0'); lcd.print(v); }

void printPadded(const char* str, int width) {
  int len = strlen(str);
  lcd.print(str);
  for (int i = len; i < width; i++) lcd.print(' ');
}

void printBattery() {
  int pct = getBattery();
  lcd.setCursor(12, 0);
  char buf[5];
  snprintf(buf, sizeof(buf), "%3d%%", pct);
  lcd.print(buf);
}

void printCompactFull(unsigned long secs) {
  long d = secs / 86400;
  long h = (secs % 86400) / 3600;
  long m = (secs % 3600) / 60;
  long s = secs % 60;
  print2((int)d); lcd.print("d ");
  print2((int)h); lcd.print("h ");
  print2((int)m); lcd.print("m ");
  print2((int)s); lcd.print("s");
}

void printTimerTime(unsigned long secs) {
  long h = secs / 3600;
  long m = (secs % 3600) / 60;
  long s = secs % 60;
  print2((int)h); lcd.print("h ");
  print2((int)m); lcd.print("m ");
  print2((int)s); lcd.print("s  ");
}

// ─────────────────────────────────────────────────────────────────────────────
// NVS
// ─────────────────────────────────────────────────────────────────────────────
void saveSlot(int i) {
  prefs.begin(NVS_NS[i], false);
  prefs.putInt("state",  (int)slots[i].state);
  prefs.putLong("days",  slots[i].setDays);
  prefs.putLong("hrs",   slots[i].setHours);
  prefs.putLong("mins",  slots[i].setMins);
  prefs.putULong("rem",  slots[i].remainingSeconds);
  prefs.end();
}

bool loadAllSlots() {
  bool hadRunning = false;
  for (int i = 0; i < NUM_SLOTS; i++) {
    prefs.begin(NVS_NS[i], true);
    slots[i].state            = (SlotState)prefs.getInt("state", (int)SLOT_IDLE);
    slots[i].setDays          = prefs.getLong("days", 0);
    slots[i].setHours         = prefs.getLong("hrs",  0);
    slots[i].setMins          = prefs.getLong("mins", 0);
    slots[i].remainingSeconds = prefs.getULong("rem", 0);
    slots[i].targetMillis     = 0;
    prefs.end();
    if (slots[i].state == SLOT_RUNNING) {
      if (slots[i].remainingSeconds == 0) {
        slots[i].state = SLOT_FINISHED;
        saveSlot(i);
      } else {
        hadRunning = true;
      }
    }
  }
  return hadRunning;
}

void resumeRunningSlots() {
  for (int i = 0; i < NUM_SLOTS; i++)
    if (slots[i].state == SLOT_RUNNING && slots[i].remainingSeconds > 0)
      slots[i].targetMillis = millis() + slots[i].remainingSeconds * 1000UL;
}

void clearSlot(int i) {
  slots[i] = { SLOT_IDLE, 0, 0, 0, 0, 0 };
  saveSlot(i);
}

// Returns shortest running countdown slot — timer takes priority in idle logic
int shortestRunningSlot() {
  int best = -1;
  unsigned long bestSecs = 0xFFFFFFFF;
  for (int i = 0; i < NUM_SLOTS; i++) {
    if (slots[i].state == SLOT_RUNNING && slots[i].remainingSeconds < bestSecs) {
      bestSecs = slots[i].remainingSeconds;
      best = i;
    }
  }
  return best;
}

bool anyCountdownRunning() {
  for (int i = 0; i < NUM_SLOTS; i++)
    if (slots[i].state == SLOT_RUNNING) return true;
  return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// SNAKE ANIMATION
// ─────────────────────────────────────────────────────────────────────────────
void snakeIndexToPos(int idx, int& col, int& row) {
  idx = ((idx % 30) + 30) % 30;
  if      (idx < 16)  { col = idx;         row = 0; }
  else if (idx == 16) { col = 15;          row = 1; }
  else                { col = 15-(idx-16); row = 1; }
}

int snakeDirection(int pos) {
  int idx = ((pos % 30) + 30) % 30;
  if      (idx < 15)  return 0;
  else if (idx == 15) return 1;
  else if (idx == 16) return 2;
  else                return 2;
}

void loadSnakeHeadChar(int dir) {
  if (lastSnakeDir == dir) return;
  lastSnakeDir = dir;
  byte* h;
  switch (dir) {
    case 0: h = charHeadR; break;
    case 1: h = charHeadD; break;
    case 2: h = charHeadL; break;
    default:h = charHeadU; break;
  }
  lcd.createChar(2, h);
}

void drawSnake() {
  lcd.clear();
  printBattery();
  const int SNAKE_LEN = 8;
  int dir = snakeDirection(snakePos);
  loadSnakeHeadChar(dir);
  for (int i = 0; i < SNAKE_LEN; i++) {
    int col, row;
    snakeIndexToPos(snakePos - i, col, row);
    if (row == 0 && col >= 12) continue;
    lcd.setCursor(col, row);
    if      (i == 0) lcd.write((uint8_t)2);
    else if (i == 1) lcd.write((uint8_t)3);
    else             lcd.write((uint8_t)4);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// DODGE DUCK GAME
// ─────────────────────────────────────────────────────────────────────────────
int nextObsRow(int lastRow) {
  dg.obsPatternCount++;
  if (dg.sameRowCount >= 3) {
    dg.sameRowCount    = 0;
    dg.obsPatternCount = 0;
    dg.obsPattern      = random(0, 2);
    return 1 - lastRow;
  }
  if (dg.obsPatternCount >= 4) {
    dg.obsPattern      = random(0, 3);
    dg.obsPatternCount = 0;
  }
  int nextRow;
  switch (dg.obsPattern) {
    case 0: nextRow = 1 - lastRow;  break;
    case 1: nextRow = lastRow;      break;
    default:nextRow = random(0, 2); break;
  }
  if (nextRow == lastRow) dg.sameRowCount++;
  else                    dg.sameRowCount = 0;
  return nextRow;
}

void dgInit() {
  lcd.createChar(5, charDuck);
  lcd.createChar(6, charBullet);
  dg.playerRow       = 1;
  dg.obs1Col         = 15;
  dg.obs1Row         = random(0, 2);
  dg.obs2Col         = 8;
  dg.obs2Row         = 1 - dg.obs1Row;
  dg.lastSecondTick  = 0;
  dg.displayScore    = 0;
  dg.dead            = false;
  dg.started         = false;
  dg.paused          = false;
  dg.tickMs          = DODGE_TICK_START;
  dg.lastSpeedUp     = 0;
  dg.obsPattern      = 0;
  dg.obsPatternCount = 0;
  dg.sameRowCount    = 0;
  lastDodgeTick      = millis();
}

void drawDodge() {
  lcd.clear();
  printBattery();
  if (!dg.started) {
    lcd.setCursor(1, 0); lcd.print("DODGE DUCK!");
    lcd.setCursor(1, 1); lcd.print("UP/DN to start");
    return;
  }
  if (dg.paused) {
    lcd.setCursor(0, dg.playerRow); lcd.write((uint8_t)5);
    if (dg.obs1Col >= 1 && dg.obs1Col < 16) { lcd.setCursor(dg.obs1Col, dg.obs1Row); lcd.write((uint8_t)6); }
    if (dg.obs2Col >= 1 && dg.obs2Col < 16) { lcd.setCursor(dg.obs2Col, dg.obs2Row); lcd.write((uint8_t)6); }
    lcd.setCursor(11, 1);
    char buf[6]; snprintf(buf, sizeof(buf), "%3ds", dg.displayScore); lcd.print(buf);
    lcd.setCursor(4, 0); lcd.print("PAUSED");
    return;
  }
  if (dg.dead) {
    lcd.setCursor(2, 0); lcd.print("** DEAD **");
    lcd.setCursor(2, 1); lcd.print("OK=retry ");
    lcd.setCursor(11, 1);
    char buf[6]; snprintf(buf, sizeof(buf), "%3ds", dg.displayScore); lcd.print(buf);
    return;
  }
  lcd.setCursor(0, dg.playerRow); lcd.write((uint8_t)5);
  if (dg.obs1Col >= 1 && dg.obs1Col < 16) { lcd.setCursor(dg.obs1Col, dg.obs1Row); lcd.write((uint8_t)6); }
  if (dg.obs2Col >= 1 && dg.obs2Col < 16) { lcd.setCursor(dg.obs2Col, dg.obs2Row); lcd.write((uint8_t)6); }
  lcd.setCursor(11, 1);
  char buf[6]; snprintf(buf, sizeof(buf), "%3ds", dg.displayScore); lcd.print(buf);
}

void dgTick() {
  if (!dg.started || dg.dead || dg.paused) return;
  unsigned long now = millis();
  if (now - dg.lastSpeedUp >= DODGE_SPEED_INTERVAL) {
    dg.lastSpeedUp = now;
    if (dg.tickMs > DODGE_TICK_MIN) { dg.tickMs -= 20UL; if (dg.tickMs < DODGE_TICK_MIN) dg.tickMs = DODGE_TICK_MIN; }
  }
  if (now - dg.lastSecondTick >= 1000UL) { dg.lastSecondTick = now; dg.displayScore++; }
  dg.obs1Col--; dg.obs2Col--;
  if (dg.obs1Col < 0) { dg.obs1Col = 15; dg.obs1Row = nextObsRow(dg.obs2Row); }
  if (dg.obs2Col < 0) { dg.obs2Col = 15; dg.obs2Row = nextObsRow(dg.obs1Row); }
  if ((dg.obs1Col == 0 && dg.obs1Row == dg.playerRow) ||
      (dg.obs2Col == 0 && dg.obs2Row == dg.playerRow)) {
    dg.dead = true; drawDodge(); return;
  }
  drawDodge(); printBattery();
}

void handleDodge() {
  if (buttonPressed(BTN_BACK)) {
    lcd.createChar(2, charArrow);
    lcd.clear(); currentScreen = SCR_MAIN; redraw(); waitRelease(BTN_BACK); return;
  }
  if (buttonPressed(BTN_UP)) {
    if (!dg.started) {
      dg.started=true; dg.paused=false;
      dg.lastSpeedUp=millis(); dg.lastSecondTick=millis(); lastDodgeTick=millis(); drawDodge();
    } else if (!dg.dead && !dg.paused) { dg.playerRow=0; drawDodge(); }
    waitRelease(BTN_UP);
  }
  if (buttonPressed(BTN_DOWN)) {
    if (!dg.started) {
      dg.started=true; dg.paused=false;
      dg.lastSpeedUp=millis(); dg.lastSecondTick=millis(); lastDodgeTick=millis(); drawDodge();
    } else if (!dg.dead && !dg.paused) { dg.playerRow=1; drawDodge(); }
    waitRelease(BTN_DOWN);
  }
  if (buttonPressed(BTN_SELECT)) {
    if (dg.dead) {
      dgInit(); dg.started=true;
      dg.lastSpeedUp=millis(); dg.lastSecondTick=millis(); lastDodgeTick=millis(); drawDodge();
    } else if (dg.started) {
      dg.paused=!dg.paused;
      if (!dg.paused) { lastDodgeTick=millis(); dg.lastSpeedUp=millis(); dg.lastSecondTick=millis(); }
      drawDodge();
    }
    waitRelease(BTN_SELECT);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// DRAW SCREENS
// ─────────────────────────────────────────────────────────────────────────────
void drawMainMenu() {
  lcd.clear();
  // 5 items: 0=Pomodoro 1=CurGoal 2=General 3=Timer 4=DodgeDuck
  int topItem = (menuCursor == 0) ? 0 : menuCursor - 1;
  if (topItem > 3) topItem = 3;

  for (int row = 0; row < 2; row++) {
    int item = topItem + row;
    if (item > 4) { lcd.setCursor(0, row); lcd.print("                "); continue; }
    lcd.setCursor(0, row);
    lcd.write(item == menuCursor ? (uint8_t)2 : (uint8_t)' ');

    if (item < NUM_SLOTS) {
      // Countdown slots
      printPadded(SLOT_NAMES[item], 9);
      lcd.setCursor(10, row);
      if      (slots[item].state == SLOT_RUNNING)  lcd.write((uint8_t)0);
      else if (slots[item].setDays  > 0 ||
               slots[item].setHours > 0 ||
               slots[item].setMins  > 0)            lcd.write((uint8_t)1);
      else                                          lcd.print(' ');
      lcd.setCursor(11, row); lcd.print(' ');
    } else if (item == 3) {
      // Timer stopwatch
      printPadded("Timer", 9);
      lcd.setCursor(10, row);
      if      (sw.running) lcd.write((uint8_t)0);
      else if (sw.paused)  lcd.write((uint8_t)1);
      else                 lcd.print(' ');
      lcd.setCursor(11, row); lcd.print(' ');
    } else {
      // Dodge Duck + duck icon
      printPadded("Dodge Duck", 10);
      lcd.setCursor(10, row);
      lcd.write((uint8_t)5); // duck char
      lcd.print(' ');
    }
  }
  printBattery();
}

const char* slotMenuLabels[4] = { "Start/Stop ", "Set Time   ", "Full Screen", "Reset      " };

void drawSlotMenu() {
  lcd.clear();
  Slot& s = slots[selectedSlot];
  lcd.setCursor(0, 0);
  printPadded(SLOT_NAMES[selectedSlot], 8); lcd.print(' ');
  if      (s.state == SLOT_RUNNING)  lcd.write((uint8_t)0);
  else if (s.state == SLOT_FINISHED) lcd.print('!');
  else if (s.setDays>0||s.setHours>0||s.setMins>0) lcd.write((uint8_t)1);
  else                               lcd.print(' ');
  lcd.print("  ");
  printBattery();
  lcd.setCursor(0, 1); lcd.write((uint8_t)2);
  printPadded(slotMenuLabels[slotMenuCursor], 15);
}

void drawTimerMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer     ");
  if      (sw.running) lcd.write((uint8_t)0);
  else if (sw.paused)  lcd.write((uint8_t)1);
  else                 lcd.print(' ');
  lcd.print("  ");
  printBattery();
  lcd.setCursor(0, 1);
  lcd.write((uint8_t)2);
  // Two options when running/paused, one option when stopped
  if (sw.running || sw.paused) {
    if (timerMenuCursor == 0) lcd.print("Full Screen    ");
    else                      lcd.print("Stop & Reset   ");
  } else {
    lcd.print("Start          ");
  }
}

void drawTimerFullScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer       ");
  printBattery();
  lcd.setCursor(0, 1);
  unsigned long secs = swCurrentSeconds();
  printTimerTime(secs);
  // Show pause indicator if paused
  if (sw.paused) {
    lcd.setCursor(13, 1);
    lcd.print("||");
  }
}

void drawSetScreen(const char* label, long val, const char* unit) {
  lcd.clear();
  lcd.setCursor(0, 0); printPadded(label, 12); printBattery();
  lcd.setCursor(0, 1); lcd.write((uint8_t)2); lcd.print(' ');
  lcd.print(val); lcd.print(' '); printPadded(unit, 10);
}

void drawFullScreen(int si) {
  lcd.clear();
  lcd.setCursor(0, 0); printPadded(SLOT_NAMES[si], 12); printBattery();
  lcd.setCursor(0, 1);
  if (slots[si].state == SLOT_FINISHED)       lcd.print("** TIME'S UP ** ");
  else if (slots[si].remainingSeconds > 0)    printCompactFull(slots[si].remainingSeconds);
  else                                        lcd.print("Not started     ");
}

void drawResumePrompt() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Resume Timers?  "); printBattery();
  lcd.setCursor(0, 1); lcd.print("OK=Yes  BACK=No ");
}

void drawAlert(int si) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("** DONE **      "); printBattery();
  lcd.setCursor(0, 1); printPadded(SLOT_NAMES[si], 16);
}

// ─────────────────────────────────────────────────────────────────────────────
// MASTER REDRAW
// ─────────────────────────────────────────────────────────────────────────────
void redraw() {
  switch (currentScreen) {
    case SCR_MAIN:             drawMainMenu();               break;
    case SCR_SLOT_MENU:        drawSlotMenu();               break;
    case SCR_SET_DAYS:         drawSetScreen("Set Days:",  tmpDays,  "day(s)"); break;
    case SCR_SET_HOURS:        drawSetScreen("Set Hours:", tmpHours, "hr(s)");  break;
    case SCR_SET_MINS:         drawSetScreen("Set Mins:",  tmpMins,  "min(s)"); break;
    case SCR_FULLSCREEN:       drawFullScreen(selectedSlot); break;
    case SCR_CONFIRM_RESUME:   drawResumePrompt();           break;
    case SCR_GAME:             drawDodge();                  break;
    case SCR_IDLE_SNAKE:       drawSnake();                  break;
    case SCR_TIMER_MENU:       drawTimerMenu();              break;
    case SCR_TIMER_FULLSCREEN: drawTimerFullScreen();        break;
    case SCR_IDLE_FULLSCREEN: {
      // Timer takes priority over countdowns in idle fullscreen
      if (sw.running || sw.paused) drawTimerFullScreen();
      else {
        int ss = shortestRunningSlot();
        if (ss >= 0) drawFullScreen(ss);
      }
      break;
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// BUTTON HELPERS
// ─────────────────────────────────────────────────────────────────────────────
bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW && (millis() - lastBtnTime) > DEBOUNCE) {
    lastBtnTime = millis(); return true;
  }
  return false;
}

void waitRelease(int pin) {
  unsigned long start = millis();
  delay(DEBOUNCE);
  while (digitalRead(pin) == LOW) {
    delay(10);
    if (millis() - start > 2000UL) break;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// SCREEN HANDLERS
// ─────────────────────────────────────────────────────────────────────────────
void handleMainMenu() {
  if (buttonPressed(BTN_BACK)) {
    screenBeforeIdle = SCR_MAIN;
    // Timer takes priority for idle fullscreen
    if (sw.running || sw.paused) {
      currentScreen = SCR_IDLE_FULLSCREEN;
    } else {
      int ss = shortestRunningSlot();
      currentScreen = (ss >= 0) ? SCR_IDLE_FULLSCREEN : SCR_IDLE_SNAKE;
      if (currentScreen == SCR_IDLE_SNAKE) {
        lastSnakeDir = -1;
        lcd.createChar(3, charSnakeNeck);
        lcd.createChar(4, charSnakeBody);
      }
    }
    lcd.clear(); redraw(); waitRelease(BTN_BACK); return;
  }
  if (buttonPressed(BTN_UP)) {
    menuCursor = (menuCursor - 1 + 5) % 5; redraw(); waitRelease(BTN_UP);
  }
  if (buttonPressed(BTN_DOWN)) {
    menuCursor = (menuCursor + 1) % 5; redraw(); waitRelease(BTN_DOWN);
  }
  if (buttonPressed(BTN_SELECT)) {
    if (menuCursor < NUM_SLOTS) {
      selectedSlot = menuCursor; slotMenuCursor = 0;
      currentScreen = SCR_SLOT_MENU;
    } else if (menuCursor == 3) {
      currentScreen = SCR_TIMER_MENU;
    } else {
      dgInit(); currentScreen = SCR_GAME;
    }
    redraw(); waitRelease(BTN_SELECT);
  }
}

void handleSlotMenu() {
  Slot& s = slots[selectedSlot];
  if (buttonPressed(BTN_UP))   { slotMenuCursor=(slotMenuCursor-1+4)%4; redraw(); waitRelease(BTN_UP); }
  if (buttonPressed(BTN_DOWN)) { slotMenuCursor=(slotMenuCursor+1)%4;   redraw(); waitRelease(BTN_DOWN); }
  if (buttonPressed(BTN_BACK)) { lcd.clear(); currentScreen=SCR_MAIN; redraw(); waitRelease(BTN_BACK); }

  if (buttonPressed(BTN_SELECT)) {
    switch (slotMenuCursor) {
      case 0:
        if (s.state == SLOT_RUNNING) {
          s.state = SLOT_IDLE; saveSlot(selectedSlot);
        } else {
          if (s.setDays==0 && s.setHours==0 && s.setMins==0) {
            lcd.setCursor(0,1); lcd.print("Set time first! "); delay(1500);
          } else {
            unsigned long total = ((unsigned long)s.setDays*86400UL)
                                + ((unsigned long)s.setHours*3600UL)
                                + ((unsigned long)s.setMins*60UL);
            s.remainingSeconds=total; s.targetMillis=millis()+total*1000UL;
            s.state=SLOT_RUNNING; saveSlot(selectedSlot);
          }
        }
        redraw(); break;
      case 1:
        tmpDays=s.setDays; tmpHours=s.setHours; tmpMins=s.setMins;
        currentScreen=SCR_SET_DAYS; redraw(); break;
      case 2: currentScreen=SCR_FULLSCREEN; redraw(); break;
      case 3: clearSlot(selectedSlot); redraw(); break;
    }
    waitRelease(BTN_SELECT);
  }
}

void handleTimerMenu() {
  if (buttonPressed(BTN_BACK)) {
    timerMenuCursor = 0;
    lcd.clear(); currentScreen = SCR_MAIN; redraw(); waitRelease(BTN_BACK); return;
  }

  // UP/DOWN only matter when timer is running or paused (two options)
  if (sw.running || sw.paused) {
    if (buttonPressed(BTN_UP)) {
      timerMenuCursor = (timerMenuCursor - 1 + 2) % 2;
      redraw(); waitRelease(BTN_UP);
    }
    if (buttonPressed(BTN_DOWN)) {
      timerMenuCursor = (timerMenuCursor + 1) % 2;
      redraw(); waitRelease(BTN_DOWN);
    }
  }

  if (buttonPressed(BTN_SELECT)) {
    if (!sw.running && !sw.paused) {
      // Start fresh — go to full screen
      swStart();
      timerMenuCursor = 0;
      currentScreen = SCR_TIMER_FULLSCREEN;
      redraw();
    } else {
      if (timerMenuCursor == 0) {
        // Full Screen
        currentScreen = SCR_TIMER_FULLSCREEN;
        redraw();
      } else {
        // Stop & Reset
        swStop();
        timerMenuCursor = 0;
        redraw();
      }
    }
    waitRelease(BTN_SELECT);
  }
}

void handleTimerFullScreen() {
  if (buttonPressed(BTN_BACK)) {
    // Go back to timer menu, keep running
    lcd.clear(); currentScreen=SCR_TIMER_MENU; redraw(); waitRelease(BTN_BACK); return;
  }
  if (buttonPressed(BTN_SELECT)) {
    // Pause / resume toggle
    if (sw.running) swPause();
    else if (sw.paused) swResume();
    redraw(); waitRelease(BTN_SELECT);
  }
}

void handleSetTime() {
  long* val=nullptr; long maxVal=0;
  if (currentScreen==SCR_SET_DAYS)  { val=&tmpDays;  maxVal=999; }
  if (currentScreen==SCR_SET_HOURS) { val=&tmpHours; maxVal=23;  }
  if (currentScreen==SCR_SET_MINS)  { val=&tmpMins;  maxVal=59;  }
  if (!val) return;

  const unsigned long HOLD_THRESHOLD = 1500UL;
  const unsigned long HOLD_REPEAT    = 200UL;
  static unsigned long upHeldSince   = 0;
  static unsigned long downHeldSince = 0;
  static unsigned long lastRepeat    = 0;

  bool upDown   = (digitalRead(BTN_UP)   == LOW);
  bool downDown = (digitalRead(BTN_DOWN) == LOW);
  unsigned long now = millis();

  if (upDown) {
    if (upHeldSince == 0) { upHeldSince=now; lastRepeat=now; *val=(*val+1>maxVal)?0:*val+1; redraw(); }
    else if ((now-upHeldSince>HOLD_THRESHOLD)&&(now-lastRepeat>HOLD_REPEAT)) {
      lastRepeat=now; *val=(*val+1>maxVal)?0:*val+1; redraw();
    }
  } else { upHeldSince=0; }

  if (downDown) {
    if (downHeldSince == 0) { downHeldSince=now; lastRepeat=now; *val=(*val-1<0)?maxVal:*val-1; redraw(); }
    else if ((now-downHeldSince>HOLD_THRESHOLD)&&(now-lastRepeat>HOLD_REPEAT)) {
      lastRepeat=now; *val=(*val-1<0)?maxVal:*val-1; redraw();
    }
  } else { downHeldSince=0; }

  if (buttonPressed(BTN_BACK)) {
    upHeldSince=downHeldSince=0;
    if      (currentScreen==SCR_SET_DAYS)  currentScreen=SCR_SLOT_MENU;
    else if (currentScreen==SCR_SET_HOURS) currentScreen=SCR_SET_DAYS;
    else if (currentScreen==SCR_SET_MINS)  currentScreen=SCR_SET_HOURS;
    redraw(); waitRelease(BTN_BACK);
  }
  if (buttonPressed(BTN_SELECT)) {
    upHeldSince=downHeldSince=0;
    if      (currentScreen==SCR_SET_DAYS)  currentScreen=SCR_SET_HOURS;
    else if (currentScreen==SCR_SET_HOURS) currentScreen=SCR_SET_MINS;
    else if (currentScreen==SCR_SET_MINS) {
      Slot& s=slots[selectedSlot];
      s.setDays=tmpDays; s.setHours=tmpHours; s.setMins=tmpMins;
      if (s.state==SLOT_RUNNING) {
        unsigned long total=((unsigned long)s.setDays*86400UL)
                           +((unsigned long)s.setHours*3600UL)
                           +((unsigned long)s.setMins*60UL);
        s.remainingSeconds=total; s.targetMillis=millis()+total*1000UL;
      }
      saveSlot(selectedSlot); currentScreen=SCR_SLOT_MENU;
    }
    redraw(); waitRelease(BTN_SELECT);
  }
}

void handleFullScreen() {
  if (buttonPressed(BTN_UP)||buttonPressed(BTN_DOWN)||
      buttonPressed(BTN_SELECT)||buttonPressed(BTN_BACK)) {
    lcd.clear(); currentScreen=SCR_SLOT_MENU; redraw(); delay(DEBOUNCE);
  }
}

void handleIdleScreen() {
  if (buttonPressed(BTN_UP)||buttonPressed(BTN_DOWN)||
      buttonPressed(BTN_SELECT)||buttonPressed(BTN_BACK)) {
    lcd.createChar(2, charArrow);
    wakeScreen(); lcd.clear(); currentScreen=screenBeforeIdle; redraw(); delay(DEBOUNCE);
  }
}

void handleResumePrompt() {
  if (buttonPressed(BTN_SELECT)) {
    resumeRunningSlots(); currentScreen=SCR_MAIN;
    lcd.clear(); redraw(); waitRelease(BTN_SELECT);
  }
  if (buttonPressed(BTN_BACK)) {
    for (int i=0;i<NUM_SLOTS;i++)
      if (slots[i].state==SLOT_RUNNING) { slots[i].state=SLOT_IDLE; saveSlot(i); }
    lcd.clear(); currentScreen=SCR_MAIN; redraw(); waitRelease(BTN_BACK);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// COUNTDOWN TICK
// ─────────────────────────────────────────────────────────────────────────────
void tickAllSlots() {
  unsigned long now = millis();
  for (int i=0;i<NUM_SLOTS;i++) {
    if (slots[i].state!=SLOT_RUNNING) continue;
    if (slots[i].targetMillis > 0 && now >= slots[i].targetMillis) {
      slots[i].remainingSeconds=0; slots[i].state=SLOT_FINISHED;
      saveSlot(i); wakeScreen();
      bool viewingThis=(currentScreen==SCR_SLOT_MENU    && selectedSlot==i)||
                       (currentScreen==SCR_FULLSCREEN   && selectedSlot==i)||
                       (currentScreen==SCR_IDLE_FULLSCREEN && !sw.running && shortestRunningSlot()==i);
      if (!viewingThis) { alertSlot=i; alertShownAt=now; showingAlert=true; drawAlert(i); }
      else redraw();
    } else if (slots[i].targetMillis > 0) {
      unsigned long newRem=(slots[i].targetMillis-now)/1000UL;
      if (newRem!=slots[i].remainingSeconds) {
        slots[i].remainingSeconds=newRem;
        bool showingLive=currentScreen==SCR_MAIN||
                         (currentScreen==SCR_SLOT_MENU  && selectedSlot==i)||
                         (currentScreen==SCR_FULLSCREEN && selectedSlot==i)||
                         (currentScreen==SCR_IDLE_FULLSCREEN && !sw.running);
        if (showingLive&&!showingAlert) redraw();
      }
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_OFF);
  btStop();
  setCpuFrequencyMhz(80);

  lcd.begin(16, 2);
  lcd.createChar(0, charFilled);
  lcd.createChar(1, charHollow);
  lcd.createChar(2, charArrow);
  lcd.createChar(3, charSnakeNeck);
  lcd.createChar(4, charSnakeBody);
  lcd.createChar(5, charDuck);
  lcd.createChar(6, charBullet);

  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK,   INPUT_PULLUP);

  ledcAttach(BACKLIGHT, 5000, 8);
  setBacklight(BL_BRIGHT);

  lastActivity = millis();
  lastSaveTime = millis();

  bool hadRunning  = loadAllSlots();
  currentScreen    = hadRunning ? SCR_CONFIRM_RESUME : SCR_MAIN;
  screenBeforeIdle = SCR_MAIN;
  redraw();
}

// ─────────────────────────────────────────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── Alert dismiss after 2s ──
  if (showingAlert && (now-alertShownAt>2000)) {
    showingAlert=false; alertSlot=-1; redraw();
  }

  // ── Idle screen trigger (2 minutes) ──
  bool isIdleScreen = (currentScreen==SCR_IDLE_SNAKE || currentScreen==SCR_IDLE_FULLSCREEN);
  if (!isIdleScreen && !showingAlert &&
      currentScreen != SCR_GAME &&
      currentScreen != SCR_CONFIRM_RESUME &&
      (now-lastActivity > AUTOFS_TIMEOUT)) {
    screenBeforeIdle = currentScreen;
    // Timer takes priority
    if (sw.running || sw.paused) {
      currentScreen = SCR_IDLE_FULLSCREEN;
    } else {
      int ss = shortestRunningSlot();
      currentScreen = (ss >= 0) ? SCR_IDLE_FULLSCREEN : SCR_IDLE_SNAKE;
      if (currentScreen == SCR_IDLE_SNAKE) {
        lastSnakeDir=-1;
        lcd.createChar(3, charSnakeNeck);
        lcd.createChar(4, charSnakeBody);
      }
    }
    lcd.clear(); redraw();
  }

  // ── Snake animation tick ──
  if (currentScreen==SCR_IDLE_SNAKE && (now-lastSnakeTick>SNAKE_TICK)) {
    lastSnakeTick=now; snakePos++; drawSnake();
  }

  // ── Idle fullscreen — update timer display every second ──
  if (currentScreen==SCR_IDLE_FULLSCREEN && !showingAlert) {
    if (!sw.running && !sw.paused && shortestRunningSlot()<0) {
      lastSnakeDir=-1;
      lcd.createChar(3, charSnakeNeck);
      lcd.createChar(4, charSnakeBody);
      currentScreen=SCR_IDLE_SNAKE; lcd.clear(); redraw();
    }
  }

  // ── Timer full screen live update every second ──
  if ((currentScreen==SCR_TIMER_FULLSCREEN || currentScreen==SCR_IDLE_FULLSCREEN) && sw.running) {
    static unsigned long lastTimerDraw = 0;
    if (now - lastTimerDraw >= 1000UL) { lastTimerDraw=now; redraw(); }
  }

  // ── Button detection ──
  bool anyDown=(digitalRead(BTN_UP)==LOW||digitalRead(BTN_DOWN)==LOW||
                digitalRead(BTN_SELECT)==LOW||digitalRead(BTN_BACK)==LOW);
  if (anyDown) {
    lastActivity=now;
    if (blState!=BL_STATE_BRIGHT) { wakeScreen(); delay(DEBOUNCE); }
    else if (showingAlert) { showingAlert=false; alertSlot=-1; redraw(); delay(DEBOUNCE); }
    else {
      switch (currentScreen) {
        case SCR_MAIN:             handleMainMenu();      break;
        case SCR_SLOT_MENU:        handleSlotMenu();      break;
        case SCR_SET_DAYS:
        case SCR_SET_HOURS:
        case SCR_SET_MINS:         handleSetTime();       break;
        case SCR_FULLSCREEN:       handleFullScreen();    break;
        case SCR_CONFIRM_RESUME:   handleResumePrompt();  break;
        case SCR_GAME:             handleDodge();         break;
        case SCR_TIMER_MENU:       handleTimerMenu();     break;
        case SCR_TIMER_FULLSCREEN: handleTimerFullScreen(); break;
        case SCR_IDLE_SNAKE:
        case SCR_IDLE_FULLSCREEN:  handleIdleScreen();    break;
      }
    }
  }

  // ── Game tick ──
  if (currentScreen==SCR_GAME) {
    if (dg.started && !dg.dead && !dg.paused &&
        (millis()-lastDodgeTick >= dg.tickMs)) {
      lastDodgeTick=millis(); dgTick();
    }
  }

  // ── Backlight management ──
  updateBacklight();

  // ── Countdown tick ──
  if (!showingAlert && currentScreen!=SCR_GAME && currentScreen!=SCR_CONFIRM_RESUME)
    tickAllSlots();

  // ── Periodic NVS save ──
  if (now-lastSaveTime>SAVE_INTERVAL) {
    lastSaveTime=now;
    for (int i=0;i<NUM_SLOTS;i++)
      if (slots[i].state==SLOT_RUNNING) saveSlot(i);
  }

  delay(50);
}
