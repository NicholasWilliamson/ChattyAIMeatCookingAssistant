// Modify node.js to /ask route to: Generate GPT-4 response, Run this script with the response as input, and Serve audio.mp3 in /public/audio.mp3
// Call generate_tts.py Python script that uses gTTS to generate .mp3 files from the ChatGPT-4.0 response.
// Chatty AI Meat Cooking Assistant JavaScript Code
// Code development by Nicholas Williamson
// Copyright 2025. All rights reserved.
// This code is provided as Open Source Code.
// You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.

const express = require('express');
const { exec } = require('child_process');
const bodyParser = require('body-parser');
const app = express();
const port = 3000;

app.use(bodyParser.json());
app.use(express.static('public'));

app.post('/ask', async (req, res) => {
  const question = req.body.question;
  const gptResponse = await getGptResponse(question); // Assume you have this

  // Save audio using gTTS
  exec(`python3 generate_tts.py "${gptResponse}"`, (err) => {
    if (err) {
      console.error("TTS generation error:", err);
      return res.status(500).send({ error: "Failed to generate audio" });
    }
    res.send({ response: gptResponse });
  });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});