#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// Wi-Fi Access Point Credentials
const char* ap_ssid = "DeskBot_AP";
const char* ap_pass = "12345678"; // minimum 8 chars

WebServer server(80);

// Hardware Pins
#define TFT_SCL  4
#define TFT_SDA  6
#define TFT_RES  3
#define TFT_DC   1
#define TFT_CS   7

#define TOUCH_PIN  2
#define BUZZER_PIN 0

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RES);

// System Modes: 0=Face, 1=Clock, 2=Timer, 3=Custom Text
int currentMode = 0; 
int currentExpression = 0;
String customText = "Hello!";

// Timer Variables (25 Minutes = 1500 Seconds)
bool timerRunning = false;
unsigned long timerStartTime = 0;
const unsigned long TIMER_DURATION = 25 * 60;
unsigned long timerRemaining = TIMER_DURATION;

void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(60); 
  digitalWrite(BUZZER_PIN, LOW);
}

// --- Face Expressions ---
void drawNormal() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.fillCircle(80, 120, 35, GC9A01A_WHITE);
  tft.fillCircle(90, 110, 10, GC9A01A_BLACK);
  tft.fillCircle(160, 120, 35, GC9A01A_WHITE);
  tft.fillCircle(170, 110, 10, GC9A01A_BLACK);
}

void drawHappy() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.fillRect(45, 115, 70, 15, GC9A01A_WHITE);
  tft.fillRect(125, 115, 70, 15, GC9A01A_WHITE);
}

void drawSad() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.fillCircle(80, 130, 35, GC9A01A_WHITE);
  tft.fillRect(40, 95, 80, 30, GC9A01A_BLACK);
  tft.fillCircle(160, 130, 35, GC9A01A_WHITE);
  tft.fillRect(120, 95, 80, 30, GC9A01A_BLACK);
}

void drawWink() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.fillCircle(80, 120, 35, GC9A01A_WHITE);
  tft.fillCircle(90, 110, 10, GC9A01A_BLACK);
  tft.fillRect(125, 115, 70, 8, GC9A01A_WHITE);
}

void drawSurprised() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.fillCircle(80, 120, 42, GC9A01A_WHITE);
  tft.fillCircle(80, 120, 12, GC9A01A_BLACK);
  tft.fillCircle(160, 120, 42, GC9A01A_WHITE);
  tft.fillCircle(160, 120, 12, GC9A01A_BLACK);
}

void showExpression(int expr) {
  switch (expr) {
    case 0: drawNormal(); break;
    case 1: drawHappy(); break;
    case 2: drawSad(); break;
    case 3: drawWink(); break;
    case 4: drawSurprised(); break;
  }
}

// --- Screens ---
void drawClockScreen() {
  unsigned long totalSecs = millis() / 1000;
  unsigned long mins = (totalSecs / 60) % 60;
  unsigned long hrs = (totalSecs / 3600) % 24;

  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_CYAN);
  tft.setTextSize(2);
  tft.setCursor(75, 50);
  tft.println("RUN TIME");

  char timeStr[10];
  sprintf(timeStr, "%02lu:%02lu", hrs, mins);

  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(4);
  tft.setCursor(60, 105);
  tft.println(timeStr);
}

void drawTimerScreen() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(2);
  tft.setCursor(70, 50);
  tft.println("25m TIMER");

  unsigned long mins = timerRemaining / 60;
  unsigned long secs = timerRemaining % 60;

  char timerStr[10];
  sprintf(timerStr, "%02lu:%02lu", mins, secs);

  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(4);
  tft.setCursor(60, 105);
  tft.println(timerStr);
}

void drawCustomTextScreen() {
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_MAGENTA);
  tft.setTextSize(2);
  tft.setCursor(75, 45);
  tft.println("MESSAGE");

  tft.setTextColor(GC9A01A_YELLOW);
  tft.setTextSize(3);
  
  // Basic text centering
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(customText, 0, 0, &x1, &y1, &w, &h);
  int x = (240 - w) / 2;
  if (x < 10) x = 10;
  
  tft.setCursor(x, 110);
  tft.println(customText);
}

// --- Web Interface HTML Page ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;text-align:center;background:#1e1e2e;color:#fff;margin:0;padding:20px;}";
  html += "button{background:#89b4fa;border:none;color:#11111b;padding:12px 20px;margin:5px;font-size:16px;border-radius:8px;cursor:pointer;font-weight:bold;}";
  html += "input[type=text]{padding:10px;width:70%;font-size:16px;border-radius:6px;border:none;margin-top:10px;}</style></head><body>";
  html += "<h2>DeskBot Control</h2>";
  
  html += "<h3>Mode Switch</h3>";
  html += "<a href='/mode?id=0'><button>Face</button></a>";
  html += "<a href='/mode?id=1'><button>Uptime</button></a>";
  html += "<a href='/mode?id=2'><button>Timer</button></a>";
  html += "<a href='/mode?id=3'><button>Message</button></a>";

  html += "<h3>Emotions</h3>";
  html += "<a href='/expr?id=0'><button>Normal</button></a>";
  html += "<a href='/expr?id=1'><button>Happy</button></a>";
  html += "<a href='/expr?id=2'><button>Sad</button></a>";
  html += "<a href='/expr?id=3'><button>Wink</button></a>";
  html += "<a href='/expr?id=4'><button>Surprised</button></a>";

  html += "<h3>Send Custom Message</h3>";
  html += "<form action='/msg' method='GET'><input type='text' name='text' placeholder='Type here...' required>";
  html += "<br><br><button type='submit' style='background:#a6e3a1;'>Display Text</button></form>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleMode() {
  if (server.hasArg("id")) {
    currentMode = server.arg("id").toInt();
    shortBeep();
    if (currentMode == 0) showExpression(currentExpression);
    else if (currentMode == 1) drawClockScreen();
    else if (currentMode == 2) drawTimerScreen();
    else if (currentMode == 3) drawCustomTextScreen();
  }
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", ""); // Fixed status code 303
}

void handleExpr() {
  if (server.hasArg("id")) {
    currentExpression = server.arg("id").toInt();
    currentMode = 0; // Switch to face mode
    shortBeep();
    showExpression(currentExpression);
  }
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", ""); // Fixed status code 303
}

void handleMsg() {
  if (server.hasArg("text")) {
    customText = server.arg("text");
    currentMode = 3; // Switch to Custom Text mode
    shortBeep();
    drawCustomTextScreen();
  }
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", ""); // Fixed status code 303
}

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  tft.begin();
  tft.setRotation(0);

  // Start Access Point
  WiFi.softAP(ap_ssid, ap_pass);

  // Setup Web Server Routes
  server.on("/", handleRoot);
  server.on("/mode", handleMode);
  server.on("/expr", handleExpr);
  server.on("/msg", handleMsg);
  server.begin();

  shortBeep();
  showExpression(0);
}

bool lastTouch = false;

void loop() {
  server.handleClient(); // Handle Web Requests

  int touchState = digitalRead(TOUCH_PIN);

  // Touch Switch Logic
  if (touchState == HIGH && !lastTouch) {
    lastTouch = true;
    shortBeep(); 
    
    currentMode = (currentMode + 1) % 4; // Cycles through 0, 1, 2, 3

    if (currentMode == 0) showExpression(currentExpression);
    else if (currentMode == 1) drawClockScreen();
    else if (currentMode == 2) drawTimerScreen();
    else if (currentMode == 3) drawCustomTextScreen();
    
    delay(200);
  } else if (touchState == LOW) {
    lastTouch = false;
  }

  // Mode 2: Timer Logic
  if (currentMode == 2 && timerRunning) {
    unsigned long elapsed = (millis() - timerStartTime) / 1000;
    if (elapsed < TIMER_DURATION) {
      timerRemaining = TIMER_DURATION - elapsed;
    } else {
      timerRemaining = 0;
      timerRunning = false;
      for (int i = 0; i < 4; i++) {
        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);  delay(150);
      }
    }
    drawTimerScreen();
    delay(500);
  }
}