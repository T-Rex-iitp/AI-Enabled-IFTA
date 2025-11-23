//---------------------------------------------------------------------------
// WhisperSTT.cpp - Implementation of Whisper-based Speech-to-Text
//---------------------------------------------------------------------------

#pragma hdrstop
#include "WhisperSTT.h"

// Include mmsystem.h only in cpp file to avoid mmeapi.h conflicts
// This prevents type ambiguity errors
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <mmsystem.h>

#include <cmath>
#include <algorithm>

#pragma package(smart_init)
#pragma link "mmsystem.lib"

//---------------------------------------------------------------------------
// TWhisperSTT Implementation
//---------------------------------------------------------------------------

TWhisperSTT::TWhisperSTT()
    : hWaveIn(NULL),
      isRecording(false),
      isInitialized(false),
      currentModel(WhisperModelType::BASE),
      useLocalModel(true),  // Always use local model
      recognitionThread(NULL),
      stopEvent(NULL),
      processingAudio(false)
{
    // Initialize wave headers as pointers
    waveHeaders[0] = NULL;
    waveHeaders[1] = NULL;
    
    InitializeCriticalSection(&audioLock);
    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//---------------------------------------------------------------------------

TWhisperSTT::~TWhisperSTT()
{
    StopListening();
    CleanupAudioCapture();
    
    // Ensure wave headers are cleaned up
    for (int i = 0; i < 2; i++)
    {
        if (waveHeaders[i])
        {
            WAVEHDR* pWaveHdr = (WAVEHDR*)waveHeaders[i];
            if (pWaveHdr->lpData)
            {
                delete[] pWaveHdr->lpData;
            }
            delete pWaveHdr;
            waveHeaders[i] = NULL;
        }
    }
    
    if (stopEvent)
        CloseHandle(stopEvent);
        
    DeleteCriticalSection(&audioLock);
}

//---------------------------------------------------------------------------

bool TWhisperSTT::Initialize(WhisperModelType model)
{
    if (isInitialized)
        return true;
        
    currentModel = model;
    
    if (!InitializeAudioCapture())
    {
        lastError = L"Failed to initialize audio capture device";
        return false;
    }
    
    isInitialized = true;
    return true;
}

//---------------------------------------------------------------------------

bool TWhisperSTT::InitializeAudioCapture()
{
    WAVEFORMATEX waveFormat;
    ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));
    
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = CHANNELS;
    waveFormat.nSamplesPerSec = SAMPLE_RATE;
    waveFormat.wBitsPerSample = BITS_PER_SAMPLE;
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    
    HWAVEIN hWaveInActual = NULL;
    MMRESULT result = waveInOpen(&hWaveInActual, WAVE_MAPPER, &waveFormat,
                                  (DWORD_PTR)WaveInProc, (DWORD_PTR)this,
                                  CALLBACK_FUNCTION);
    hWaveIn = (HWAVEIN_PTR)hWaveInActual;
    
    if (result != MMSYSERR_NOERROR)
    {
        lastError = L"Failed to open wave input device. Error code: " + IntToStr((int)result);
        return false;
    }
    
    // Allocate and prepare wave headers
    for (int i = 0; i < 2; i++)
    {
        WAVEHDR* pWaveHdr = new WAVEHDR;
        ZeroMemory(pWaveHdr, sizeof(WAVEHDR));
        pWaveHdr->lpData = new char[BUFFER_SIZE * 2]; // 2 bytes per sample
        pWaveHdr->dwBufferLength = BUFFER_SIZE * 2;
        pWaveHdr->dwBytesRecorded = 0;
        pWaveHdr->dwUser = 0;
        pWaveHdr->dwFlags = 0;
        pWaveHdr->dwLoops = 0;
        
        waveInPrepareHeader((HWAVEIN)hWaveIn, pWaveHdr, sizeof(WAVEHDR));
        waveHeaders[i] = (WAVEHDR_PTR)pWaveHdr;
    }
    
    return true;
}

//---------------------------------------------------------------------------

void TWhisperSTT::CleanupAudioCapture()
{
    if (hWaveIn)
    {
        HWAVEIN hWaveInActual = (HWAVEIN)hWaveIn;
        waveInReset(hWaveInActual);
        
        for (int i = 0; i < 2; i++)
        {
            if (waveHeaders[i])
            {
                WAVEHDR* pWaveHdr = (WAVEHDR*)waveHeaders[i];
                waveInUnprepareHeader(hWaveInActual, pWaveHdr, sizeof(WAVEHDR));
                if (pWaveHdr->lpData)
                {
                    delete[] pWaveHdr->lpData;
                }
                delete pWaveHdr;
                waveHeaders[i] = NULL;
            }
        }
        
        waveInClose(hWaveInActual);
        hWaveIn = NULL;
    }
    
    isInitialized = false;
}

//---------------------------------------------------------------------------

bool TWhisperSTT::StartListening()
{
    if (!isInitialized)
    {
        lastError = L"Whisper STT not initialized. Call Initialize() first.";
        return false;
    }
    
    if (isRecording)
        return true;
    
    // Clear previous audio data
    EnterCriticalSection(&audioLock);
    audioBuffer.clear();
    LeaveCriticalSection(&audioLock);
    
    ResetEvent(stopEvent);
    
    // Start worker thread
    recognitionThread = CreateThread(NULL, 0, RecognitionThreadProc, this, 0, NULL);
    
    if (!recognitionThread)
    {
        lastError = L"Failed to create recognition thread";
        return false;
    }
    
    // Start recording
    HWAVEIN hWaveInActual = (HWAVEIN)hWaveIn;
    for (int i = 0; i < 2; i++)
    {
        if (waveHeaders[i])
        {
            WAVEHDR* pWaveHdr = (WAVEHDR*)waveHeaders[i];
            waveInAddBuffer(hWaveInActual, pWaveHdr, sizeof(WAVEHDR));
        }
    }
    
    if (waveInStart(hWaveInActual) != MMSYSERR_NOERROR)
    {
        lastError = L"Failed to start audio recording";
        return false;
    }
    
    isRecording = true;
    return true;
}

//---------------------------------------------------------------------------

void TWhisperSTT::StopListening()
{
    if (!isRecording)
        return;
    
    isRecording = false;
    
    // Stop recording
    if (hWaveIn)
    {
        HWAVEIN hWaveInActual = (HWAVEIN)hWaveIn;
        waveInStop(hWaveInActual);
        waveInReset(hWaveInActual);
    }
    
    // Signal worker thread to stop
    SetEvent(stopEvent);
    
    if (recognitionThread)
    {
        WaitForSingleObject(recognitionThread, 5000);
        CloseHandle(recognitionThread);
        recognitionThread = NULL;
    }
}

//---------------------------------------------------------------------------

void CALLBACK TWhisperSTT::WaveInProc(void* hwi, unsigned int uMsg, void* dwInstance,
                                      void* dwParam1, void* dwParam2)
{
    if (uMsg != WIM_DATA)
        return;
    
    TWhisperSTT* pThis = (TWhisperSTT*)dwInstance;
    WAVEHDR* pWaveHdr = (WAVEHDR*)dwParam1;
    
    if (!pThis || !pThis->isRecording)
        return;
    
    // Copy audio data to buffer
    EnterCriticalSection(&pThis->audioLock);
    
    short* pSamples = (short*)pWaveHdr->lpData;
    int numSamples = pWaveHdr->dwBytesRecorded / 2;
    
    for (int i = 0; i < numSamples; i++)
    {
        pThis->audioBuffer.push_back(pSamples[i]);
    }
    
    LeaveCriticalSection(&pThis->audioLock);
    
    // Re-add buffer for continuous recording
    waveInAddBuffer((HWAVEIN)hwi, pWaveHdr, sizeof(WAVEHDR));
}

//---------------------------------------------------------------------------

unsigned long WINAPI TWhisperSTT::RecognitionThreadProc(void* lpParam)
{
    TWhisperSTT* pThis = (TWhisperSTT*)lpParam;
    TVoiceActivityDetector vad;
    
    const int PROCESS_INTERVAL = 500; // Process every 500ms
    const int MIN_SPEECH_SAMPLES = SAMPLE_RATE * 1; // Minimum 1 second of speech
    
    std::vector<short> speechBuffer;
    bool wasSpeaking = false;
    
    while (WaitForSingleObject(pThis->stopEvent, (DWORD)PROCESS_INTERVAL) == WAIT_TIMEOUT)
    {
        std::vector<short> currentBuffer;
        
        // Copy current audio buffer
        EnterCriticalSection(&pThis->audioLock);
        if (pThis->audioBuffer.size() > BUFFER_SIZE)
        {
            currentBuffer = pThis->audioBuffer;
            pThis->audioBuffer.clear();
        }
        LeaveCriticalSection(&pThis->audioLock);
        
        if (currentBuffer.empty())
            continue;
        
        // Voice activity detection
        bool isSpeaking = vad.DetectVoice(currentBuffer);
        
        if (isSpeaking)
        {
            // Accumulate speech data
            speechBuffer.insert(speechBuffer.end(), 
                              currentBuffer.begin(), 
                              currentBuffer.end());
            wasSpeaking = true;
        }
        else if (wasSpeaking)
        {
            // Speech ended, process accumulated data
            if (speechBuffer.size() >= MIN_SPEECH_SAMPLES)
            {
                // Always use local model
                std::wstring recognizedText = pThis->RunLocalWhisper(speechBuffer);
                
                // Trigger callback
                if (!recognizedText.empty() && pThis->onRecognitionCallback)
                {
                    pThis->onRecognitionCallback(recognizedText);
                }
            }
            
            speechBuffer.clear();
            wasSpeaking = false;
            vad.Reset();
        }
    }
    
    return 0;
}

//---------------------------------------------------------------------------

std::wstring TWhisperSTT::RunLocalWhisper(const std::vector<short>& audioData)
{
    // Use local whisper.cpp for speech recognition
    
    try
    {
        // Save audio to temporary file
        std::wstring tempFile = L"temp_audio.wav";
        if (!SaveWavFile(tempFile, audioData))
        {
            lastError = L"Failed to save temporary audio file";
            return L"";
        }
        
        // Find main.exe in common locations
        std::wstring mainExePath;
        std::vector<std::wstring> possiblePaths = {
            localModelPath + L"\\build\\bin\\Release\\main.exe",
            localModelPath + L"\\build\\Release\\main.exe",
            localModelPath + L"\\main.exe",
            localModelPath + L"\\build\\main.exe"
        };
        
        bool found = false;
        for (const auto& path : possiblePaths)
        {
            if (FileExists(String(path.c_str())))
            {
                mainExePath = path;
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            lastError = L"main.exe not found. Checked paths:\n" + localModelPath + L"\\build\\bin\\Release\\main.exe\n" +
                       localModelPath + L"\\build\\Release\\main.exe\n" +
                       localModelPath + L"\\main.exe";
            DeleteFileW(tempFile.c_str());
            return L"";
        }
        
        // Find model file
        std::wstring modelName = L"ggml-base.bin";  // Default model
        switch (currentModel)
        {
            case WhisperModelType::TINY:   modelName = L"ggml-tiny.bin"; break;
            case WhisperModelType::BASE:   modelName = L"ggml-base.bin"; break;
            case WhisperModelType::SMALL:  modelName = L"ggml-small.bin"; break;
            case WhisperModelType::MEDIUM: modelName = L"ggml-medium.bin"; break;
            case WhisperModelType::LARGE:  modelName = L"ggml-large.bin"; break;
        }
        
        std::wstring modelPath = localModelPath + L"\\models\\" + modelName;
        if (!FileExists(String(modelPath.c_str())))
        {
            lastError = L"Model file not found: " + modelPath;
            DeleteFileW(tempFile.c_str());
            return L"";
        }
        
        // Build command line for whisper.cpp
        std::wstring command = L"\"" + mainExePath + L"\" ";
        command += L"-m \"" + modelPath + L"\" ";
        command += L"-f \"" + tempFile + L"\" ";
        command += L"--output-txt ";
        command += L"--no-timestamps ";  // Remove timestamps for cleaner output
        
        // Execute whisper.cpp
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));
        
        // Create process with command line
        wchar_t* cmdLine = new wchar_t[command.length() + 1];
        wcscpy(cmdLine, command.c_str());
        
        if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE,
                         CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        {
            // Wait for process to complete (with timeout - 30 seconds for processing)
            DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000);
            
            if (waitResult == WAIT_TIMEOUT)
            {
                TerminateProcess(pi.hProcess, 1);
                lastError = L"Whisper processing timeout (30 seconds)";
            }
            else
            {
                // Read output file
                std::wstring outputFile = tempFile + L".txt";
                TStringList* lines = new TStringList();
                try
                {
                    if (FileExists(String(outputFile.c_str())))
                    {
                        lines->LoadFromFile(String(outputFile.c_str()));
                        std::wstring result = String(lines->Text.c_str()).c_str();
                        
                        // Trim whitespace
                        while (!result.empty() && (result.back() == L' ' || result.back() == L'\n' || result.back() == L'\r'))
                            result.pop_back();
                        
                        // Cleanup
            DeleteFileW(outputFile.c_str());
            DeleteFileW(tempFile.c_str());
                        
                        delete lines;
                        delete[] cmdLine;
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                        return result;
                    }
                    else
                    {
                        lastError = L"Output file not found: " + outputFile;
                    }
                }
                __finally
                {
                    delete lines;
                }
            }
            
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            ::DWORD error = ::GetLastError();
            lastError = L"Failed to launch Whisper process. Error code: " + IntToStr((int)error);
        }
        
        delete[] cmdLine;
        DeleteFileW(tempFile.c_str());
    }
    catch (Exception& e)
    {
        lastError = L"Exception in RunLocalWhisper: " + e.Message;
    }
    
    return L"";
}

//---------------------------------------------------------------------------

bool TWhisperSTT::SaveWavFile(const std::wstring& filename, 
                              const std::vector<short>& audioData)
{
    try
    {
        FILE* fp = _wfopen(filename.c_str(), L"wb");
        if (!fp)
            return false;
        
        // WAV file header
        struct WAVHeader
        {
            char riff[4] = {'R', 'I', 'F', 'F'};
            int32_t fileSize;
            char wave[4] = {'W', 'A', 'V', 'E'};
            char fmt[4] = {'f', 'm', 't', ' '};
            int32_t fmtSize = 16;
            int16_t audioFormat = 1; // PCM
            int16_t numChannels = CHANNELS;
            int32_t sampleRate = SAMPLE_RATE;
            int32_t byteRate = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8);
            int16_t blockAlign = CHANNELS * (BITS_PER_SAMPLE / 8);
            int16_t bitsPerSample = BITS_PER_SAMPLE;
            char data[4] = {'d', 'a', 't', 'a'};
            int32_t dataSize;
        } header;
        
        header.dataSize = audioData.size() * sizeof(short);
        header.fileSize = 36 + header.dataSize;
        
        // Write header
        fwrite(&header, sizeof(WAVHeader), 1, fp);
        
        // Write audio data
        fwrite(audioData.data(), sizeof(short), audioData.size(), fp);
        
        fclose(fp);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

//---------------------------------------------------------------------------

void TWhisperSTT::SetLocalModelPath(const std::wstring& path)
{
    localModelPath = path;
}

//---------------------------------------------------------------------------

void TWhisperSTT::UseLocalModel(bool useLocal)
{
    // Always use local model - API mode is disabled
    useLocalModel = true;
}

//---------------------------------------------------------------------------

void TWhisperSTT::SetRecognitionCallback(RecognitionCallback callback)
{
    onRecognitionCallback = callback;
}

//---------------------------------------------------------------------------

void TWhisperSTT::SetModel(WhisperModelType model)
{
    currentModel = model;
}

//---------------------------------------------------------------------------

std::wstring TWhisperSTT::ModelTypeToString(WhisperModelType model)
{
    switch (model)
    {
        case WhisperModelType::TINY:   return L"tiny";
        case WhisperModelType::BASE:   return L"base";
        case WhisperModelType::SMALL:  return L"small";
        case WhisperModelType::MEDIUM: return L"medium";
        case WhisperModelType::LARGE:  return L"large";
        default: return L"base";
    }
}

//---------------------------------------------------------------------------

std::vector<std::wstring> TWhisperSTT::GetAvailableModels()
{
    return {L"tiny", L"base", L"small", L"medium", L"large"};
}

//---------------------------------------------------------------------------
// TVoiceActivityDetector Implementation
//---------------------------------------------------------------------------

TVoiceActivityDetector::TVoiceActivityDetector()
    : energyThreshold(0.02f),
      silenceFrames(0),
      maxSilenceFrames(10),
      isSpeaking(false)
{
}

//---------------------------------------------------------------------------

bool TVoiceActivityDetector::DetectVoice(const std::vector<short>& audioData)
{
    float energy = CalculateEnergy(audioData);
    
    if (energy > energyThreshold)
    {
        isSpeaking = true;
        silenceFrames = 0;
    }
    else
    {
        silenceFrames++;
        
        if (silenceFrames > maxSilenceFrames)
        {
            isSpeaking = false;
        }
    }
    
    return isSpeaking;
}

//---------------------------------------------------------------------------

float TVoiceActivityDetector::CalculateEnergy(const std::vector<short>& audioData)
{
    if (audioData.empty())
        return 0.0f;
    
    double sum = 0.0;
    for (size_t i = 0; i < audioData.size(); i++)
    {
        double normalized = audioData[i] / 32768.0;
        sum += normalized * normalized;
    }
    
    return sqrt(sum / audioData.size());
}

//---------------------------------------------------------------------------

void TVoiceActivityDetector::Reset()
{
    silenceFrames = 0;
    isSpeaking = false;
}

//---------------------------------------------------------------------------

void TVoiceActivityDetector::SetThreshold(float threshold)
{
    energyThreshold = threshold;
}

//---------------------------------------------------------------------------

void TVoiceActivityDetector::SetMaxSilenceFrames(int frames)
{
    maxSilenceFrames = frames;
}

//---------------------------------------------------------------------------
// TAudioBufferManager Implementation
//---------------------------------------------------------------------------

TAudioBufferManager::TAudioBufferManager()
{
    InitializeCriticalSection(&queueLock);
    dataAvailableEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//---------------------------------------------------------------------------

TAudioBufferManager::~TAudioBufferManager()
{
    Clear();
    DeleteCriticalSection(&queueLock);
    CloseHandle(dataAvailableEvent);
}

//---------------------------------------------------------------------------

void TAudioBufferManager::PushBuffer(const std::vector<short>& buffer)
{
    EnterCriticalSection(&queueLock);
    bufferQueue.push_back(buffer);
    LeaveCriticalSection(&queueLock);
    
    SetEvent(dataAvailableEvent);
}

//---------------------------------------------------------------------------

bool TAudioBufferManager::PopBuffer(std::vector<short>& buffer, unsigned long timeout)
{
    if (WaitForSingleObject(dataAvailableEvent, timeout) == WAIT_TIMEOUT)
        return false;
    
    EnterCriticalSection(&queueLock);
    
    if (!bufferQueue.empty())
    {
        buffer = bufferQueue.front();
        bufferQueue.erase(bufferQueue.begin());
        
        if (!bufferQueue.empty())
            SetEvent(dataAvailableEvent);
        
        LeaveCriticalSection(&queueLock);
        return true;
    }
    
    LeaveCriticalSection(&queueLock);
    return false;
}

//---------------------------------------------------------------------------

void TAudioBufferManager::Clear()
{
    EnterCriticalSection(&queueLock);
    bufferQueue.clear();
    LeaveCriticalSection(&queueLock);
    
    ResetEvent(dataAvailableEvent);
}

//---------------------------------------------------------------------------

size_t TAudioBufferManager::GetQueueSize()
{
    EnterCriticalSection(&queueLock);
    size_t size = bufferQueue.size();
    LeaveCriticalSection(&queueLock);
    
    return size;
}

//---------------------------------------------------------------------------

