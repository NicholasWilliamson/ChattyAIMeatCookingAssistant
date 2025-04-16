// Chatty AI Meat Cooking Assistant JavaScript Code
// Code development by Nicholas Williamson
// Copyright 2025. All rights reserved.
// This code is provided as Open Source Code.
// You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.

const synth = window.speechSynthesis;
let recognition;

function startCooking() {
  const protein = document.getElementById('protein').value;
  const cut = document.getElementById('cut').value;
  const weight = document.getElementById('weight').value;
  const doneness = document.getElementById('doneness').value;

  const data = { temp: 45, protein, cut, weight, doneness };

  fetch('http://localhost:3000/temperature', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(data)
  })
  .then(res => res.json())
  .then(data => speakText(data.response));

  sendConfigToESP32();

}

function sendConfigToESP32() {

  const protein = document.getElementById('protein').value;
  const cut = document.getElementById('cut').value;
  const weight = document.getElementById('weight').value;
  const doneness = document.getElementById('doneness').value;

  const config = { protein, cut, weight: parseInt(weight), doneness };

  fetch('http://<ESP32_IP>/config', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(config)
  })
  .then(res => res.json())
  .then(data => console.log("ESP32 config set:", data))
  .catch(err => console.error("ESP32 config error:", err));
}

  const ESP32_IP = 'http://192.168.1.42'; // Change to your ESP32 IP address

function speakText(text) {
  // Speak through browser
  const synth = window.speechSynthesis;
  const utterance = new SpeechSynthesisUtterance(text);
  synth.speak(utterance);

  // Send to ESP32 to speak
  fetch(`${ESP32_IP}/speak`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ message: text })
  }).catch(err => {
    console.error("Failed to send message to ESP32:", err);
  });
}

if ('webkitSpeechRecognition' in window) {
  const recognition = new webkitSpeechRecognition();
  recognition.continuous = true;

  recognition.onresult = (event) => {
    const transcript = event.results[event.resultIndex][0].transcript;
    console.log("Heard:", transcript);

    if (transcript.toLowerCase().includes("hello chatty")) {
      const question = transcript.split("hello chatty")[1].trim();
      console.log("Sending question:", question);

      fetch('http://localhost:3000/ask', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ question })
      })
      .then(res => res.json())
      .then(data => {
        console.log("ChatGPT-4.0 response:", data.response);
        speakText(data.response);
      })
      .catch(err => console.error("Error from /ask:", err));
    }
  };

  recognition.start();
}