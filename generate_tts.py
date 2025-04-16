# Chatty AI Meat Cooking Assistant JavaScript Code
# Code development by Nicholas Williamson
# Copyright 2025. All rights reserved.
# This code is provided as Open Source Code.
# You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.

from gtts import gTTS
import sys

text = sys.argv[1]
file_path = sys.argv[2]
tts = gTTS(text)
tts.save(file_path)