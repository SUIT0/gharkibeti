from fastapi import FastAPI, UploadFile, File, HTTPException
import vosk
import json
import google.generativeai as genai
import uvicorn
import wave
import os
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI()

# Load Vosk model
vosk_model = vosk.Model("vosk-model-small-en-us-0.15")  # Path to the model folder

# Configure Gemini API (hardcoded API key)
genai.configure(api_key="AIzaSyD7sKAI9wx1A30n42grvTpMufTBr5c4p2c")  # Replace with your actual API key

def transcribe_audio_vosk(audio_path: str):
    """Convert speech to text using Vosk."""
    wf = wave.open(audio_path, "rb")
    recognizer = vosk.KaldiRecognizer(vosk_model, wf.getframerate())
    text_result = ""
    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        if recognizer.AcceptWaveform(data):
            text_result += json.loads(recognizer.Result())["text"] + " "
    text_result += json.loads(recognizer.FinalResult())["text"]
    return text_result.strip()

def process_command_with_gemini(text: str):
    """Send the transcribed text to Gemini for command extraction."""
    model = genai.GenerativeModel('gemini-pro')
    prompt = f"Extract the home automation command from this text: '{text}'. Respond in JSON format like {{'device': 'light', 'state': 'off'}}."
    response = model.generate_content(prompt)
    return response.text  # Extracted command JSON

@app.post("/process-voice/")
async def process_voice(file: UploadFile = File(...)):
    try:
        logger.info("Received audio file for processing")
        
        # Validate file type
        if not file.filename.endswith(".wav"):
            raise HTTPException(status_code=400, detail="Only .wav files are supported")
        
        # Save uploaded file temporarily
        audio = await file.read()
        with open("temp.wav", "wb") as f:
            f.write(audio)
        
        # Convert speech to text using Vosk
        text_command = transcribe_audio_vosk("temp.wav")
        logger.info(f"Transcribed text: {text_command}")
        
        # Process text command using Gemini
        output = process_command_with_gemini(text_command)
        logger.info(f"Gemini output: {output}")
        
        return {"transcribed_text": text_command, "command_output": output}
    
    except Exception as e:
        logger.error(f"Error processing audio file: {e}")
        raise HTTPException(status_code=500, detail=str(e))
    
    finally:
        # Clean up temporary file
        if os.path.exists("temp.wav"):
            os.remove("temp.wav")

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
    