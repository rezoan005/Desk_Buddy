# Desk_Buddy

**DeskBuddy** is an interactive, Wi-Fi-enabled desktop companion powered by an ESP32 C3 super mini microcontroller and a round GC9A01 TFT display. It features multiple animated expressions, system uptime monitoring, a Pomodoro timer, custom message broadcasting, and interactive touch controls—all wrapped in a sleek local web interface.

---

## 🌟 Key Features

- **Expressive Expressions:** Displays 5 distinct animated faces (*Normal, Happy, Sad, Wink, Surprised*).
- **Multiple Display Modes:**
  - `Face Mode`: Shows interactive facial expressions.
  - `Uptime Mode`: Displays system run-time.
  - `Timer Mode`: 25-minute Pomodoro productivity timer.
  - `Message Mode`: Displays dynamic custom text sent via the Web UI.
- **Web Interface:** Built-in Wi-Fi Access Point and Web Server to control the bot from any phone or laptop.
- **Hardware Touch Switch:** Physical touch pin to cycle through modes instantly.
- **Buzzer Feedback:** Audio feedback for touch actions and timer completion alerts.

---

## 🛠️ Hardware Components

| Component | Specification / Pins |
| :--- | :--- |
| **Microcontroller** | ESP32 C3 Super Mini Board |
| **Display** | 1.28" GC9A01 Round Display (SPI) |
| **Touch Sensor** | TTP223 Capacitive Touch Module |
| **Buzzer** | Active / Passive Buzzer |

---

## 📌 Pin Mapping

| Component Pin | ESP32 GPIO Pin |
| :--- | :--- |
| **TFT SCL (CLK)** | GPIO 4 |
| **TFT SDA (DIN)** | GPIO 6 |
| **TFT RES (RST)** | GPIO 3 |
| **TFT DC** | GPIO 1 |
| **TFT CS** | GPIO 7 |
| **TOUCH SENSOR** | GPIO 2 |
| **BUZZER** | GPIO 0 |

---

## 💻 Software & Libraries Required

Make sure you have the following libraries installed in your Arduino IDE via **Library Manager**:

- `WiFi.h` (Built-in for ESP32)
- `WebServer.h` (Built-in for ESP32)
- `SPI.h` (Built-in)
- `Adafruit_GFX`
- `Adafruit_GC9A01A`

---

## 🚀 How to Setup and Flash

1. **Connect Hardware:** Wire up the display, touch sensor, and buzzer according to the pin mapping above.
2. **Open Arduino IDE:** Load the provided `.ino` source code.
3. **Install Board Package:** Ensure ESP32 board support is installed under `Tools > Board > Board Manager`.
4. **Upload Code:** Select your ESP32 board and COM port, then click **Upload**.

---

## 📱 How to Use

1. **Connect to Wi-Fi:**
   - Turn on DeskBot.
   - On your phone or PC, connect to the Wi-Fi Access Point:
     - **SSID:** `DeskBot_AP`
     - **Password:** `12345678`
2. **Access Control Panel:**
   - Open your web browser and navigate to `http://192.168.4.1`.
3. **Control DeskBot:**
   - Switch display modes (Face, Uptime, Timer, Message).
   - Change expression styles directly.
   - Type a custom message and hit **Display Text** to update the display in real-time.
4. **Physical Controls:**
   - Tap the touch sensor attached to `GPIO 2` to cycle through display modes manually.

---

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).
