// Chatty AI Meat Cooking Assistant JavaScript Code
// Code development by Nicholas Williamson
// Copyright 2025. All rights reserved.
// This code is provided as Open Source Code.
// You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.

const { Configuration, OpenAIApi } = require('openai');
require('dotenv').config();

const configuration = new Configuration({
  apiKey: process.env.OPENAI_API_KEY,
});
const openai = new OpenAIApi(configuration);

async function askGPT(question) {
  const completion = await openai.createChatCompletion({
    model: "gpt-4",
    messages: [{ role: "user", content: question }],
  });
  return completion.data.choices[0].message.content.trim();
}

module.exports = { askGPT };
