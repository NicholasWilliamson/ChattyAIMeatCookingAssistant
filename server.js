// Chatty AI Meat Cooking Assistant JavaScript Code
// Code development by Nicholas Williamson
// Copyright 2025. All rights reserved.
// This code is provided as Open Source Code.
// You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.

const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const { askGPT } = require('./openai');
const app = express();
const port = 3000;
const crypto = require('crypto');
const fs = require('fs');
const { exec } = require('child_process');

app.use(cors());
app.use(bodyParser.json());
app.use(express.static('frontend'));

app.post('/ask', async (req, res) => {
  const question = req.body.question;
  const gptResponse = await getGptResponse(question);

  // Create hash of response to use as filename
  const hash = crypto.createHash('md5').update(gptResponse).digest('hex');
  const filePath = `public/audio/${hash}.mp3`;

  if (fs.existsSync(filePath)) {
    console.log("Using cached audio:", filePath);
    res.send({ response: gptResponse, audio: `/audio/${hash}.mp3` });
  } else {
    const command = `python3 generate_tts.py "${gptResponse}" "${filePath}"`;
    exec(command, (err) => {
      if (err) {
        console.error("TTS generation error:", err);
        return res.status(500).send({ error: "TTS failed" });
      }
      console.log("Generated new audio:", filePath);
      res.send({ response: gptResponse, audio: `/audio/${hash}.mp3` });
    });
  }
});

app.post('/temperature', async (req, res) => {
  const { temp, protein, cut, weight, doneness } = req.body;
  const prompt = `I am your meat cooking assistant. The current meat temperature is ${temp}°C. 
You are cooking ${weight}g of ${cut} ${protein}, aiming for ${doneness}. 
Respond with one of the following actions based on doneness:
- Say "flip" if it's time to flip.
- Say "done" or "ready" if meat is ready.
- Otherwise, give helpful advice and include "thinking" or "answering" for status indication.
`;
  const reply = await askGPT(prompt);
  res.json({ response: reply });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
