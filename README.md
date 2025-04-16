# Chatty AI Meat Cooking Assistant

A smart ESP32-powered cooking companion that listens to your questions (e.g., "Is the steak ready to flip?") and responds via GPT-4 — using voice and screen output. It uses a TTGO ESP32 T-Display, MLX90614 temperature sensor, RGB LED, buzzer, microphone and speaker with voice interaction powered by Node.js, Google Speech API, and OpenAI.

---

## Features

- ChatGPT-4.0 powered responses
- Voice input via Web Speech API
- ESP32 talks back using audio playback via speaker
- Meat temperature monitoring via MLX90614 IR sensor
- Cached audio responses to save time and bandwidth
- Web interface with voice UI + ESP32 TFT display feedback
- RGB LED cooking alerts (Flip: Yellow, Done: Red, ChatGPT-4.0 Thinking: Green, ChatGPT-4.0 Speaking: Blue)
- Buzzer alerts

---

## Hardware Setup

- TTGO ESP32 T-Display
- MLX90614 IR temp sensor (I2C + 10KΩ resistors)
- RGB LED (with 220Ω resistors)
- Passive buzzer (220Ω resistor)
- PAM8403 stereo amp + speaker (connected to I2S pins)
- MAX9814 microphone (for optional expansion)
- Breadboard, 3.3V power rail, 3.3V replaceale lithium battery

Pins and physical layout documented in the provided Chatty_AI_Cooking_Assistant.fzz Fritzing Diagram

---

## Project Structure
chatty-ai-cooking-assistant/
├── backend/
│   ├── server.js             # Express server handling GPT interaction
│   ├── openai.js             # ChatGPT-4.0 integration module
│   ├── .env                  # Environment variable template
│   ├── generate_tts.py       # Python script to convert GPT response to MP3
│   └── package.json          # Node.js dependencies and scripts
│   └── node2.js              # Node2.js handles GPT-4, audio generation, and wake-word detection via browser
├── public/
│   ├── index.html            # Web interface for interaction
│   └── script.js             # Handles voice interaction and HTTP requests
│   └── audio/                # Cached MP3 responses
├── firmware/
│   └── chatty_ai_esp32.ino   # Arduino sketch for ESP32 with sensor and LED logic
├── README.md                 # Project overview and setup instructions
└── LICENSE.txt               # MIT license

---

## Setup Instructions

### 1. ESP32 Firmware

- Open `firmware/chatty_ai_esp32.ino` in Arduino IDE.
- Install these libraries via Library Manager:
  - `Adafruit MLX90614`
  - `TFT_eSPI`
  - `WiFi`
  - `HTTPClient`
  - `Wire`
  - `WebServer`
  - `ArduinoJson`
  - `ESP8266Audio`
  - `AudioFileSourceHTTPStream`
  - `AudioGeneratorMP3`
  - `AudioOutputI2S`
  - `AudioFileSourceBuffer`

---

- Connect your TTGO T-Display ESP32 and upload the sketch.

---

## Node.js Setup Instructions

> The backend handles GPT-4, audio generation, and wake-word detection via browser.
Integrate the contents of node2.js with the original Node.js file

---

### Prerequisites

- Node.js (v18+ recommended)
- Python 3 with `gTTS`
- OpenAI API key

---

### 2. Backend Setup

```bash
git clone https://github.com/yourname/chatty-ai-cooking-assistant
cd chatty-ai-cooking-assistant/backend

# Install Node packages
npm install

# Install Python TTS dependencies
pip install gTTS

---

### 3. Environment Variable
Edit the .env file:

OPENAI_API_KEY=your_api_key_here

---

### 4. Starting the Server

```bash
node server.js

 - Access the web UI at http://<YOUR_PC_IP>:3000/
 - The ESP32 will connect to http://<YOUR_PC_IP>:3000/speak

---

### 5. Audio Caching

 - GPT responses are hashed (md5) into filenames like audio/5e7d1.mp3
 - Prevents regenerating TTS for the same response
 - Python TTS script saves the audio in public/audio/

---

### 6. Buffered Audio Setup

AudioFileSourceHTTPStream *httpStream = new AudioFileSourceHTTPStream(audioURL);
AudioFileSourceBuffer *buff = new AudioFileSourceBuffer(httpStream, 2048); // 2KB buffer
buff->begin();
mp3->begin(buff, out);

 - This ensures smoother playback for longer MP3s streamed from the Node.js server.

---

### 7. Endpoint Summary

Endpoint	Method	Description
/ask		POST	Sends a question to GPT-4 and gets a reply and audio path
/speak		POST	ESP32 receives GPT-4 audio URL to play
/audio/:file	GET	Cached TTS audio file playback

---

### 8. Frontend Usage

Open frontend/index.html in a browser (preferably Chrome).

Select meat options and start cooking.

Say “Hello Chatty” followed by a question to ask ChatGPT-4.0

---

### 9. RGB LED Color Feedback

Event		Color
Thinking	Green
Speaking	Blue
Time to Flip	Yellow
Cooking Done	Red

---

### 10. Future Possible Improvements

 - Replay last spoken response via button on ESP32
 - /volume endpoint for controlling speaker volume remotely
 - Cleanup old audio cache files automatically
 - Progressive Web App for full-screen mobile interface
 - OTA firmware update support for ESP32

---

### 11. Troubleshooting

 - Ensure public/audio/ exists and is writable
 - Check ESP32 can reach the server (same Wi-Fi)
 - Test MP3 file URL in browser before testing on ESP32

---

### 12. Wake Word Setup

The web interface uses Google Web Speech API (via Chrome) for:

 - Wake-word: Hello Chatty
 - Follow-up question triggers /ask
 - Audio path is returned and played by the ESP32 via /speak
 - No TensorFlow Lite Agent Training or setup required.

---

### 13. Credits

 - Voice: Google TTS (via gTTS)
 - Intelligence: OpenAI ChatGPT-4.0
 - ESP32 audio: ArduinoAudioTools by pschatzmann (https://github.com/pschatzmann/arduino-audio-tools)
 - Wake-word detection: Google Web Speech

