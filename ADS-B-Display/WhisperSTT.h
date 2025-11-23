//---------------------------------------------------------------------------
// WhisperSTT.h - Whisper-based Speech-to-Text Implementation
// Uses OpenAI Whisper for advanced speech recognition
//---------------------------------------------------------------------------

#ifndef WhisperSTTH
#define WhisperSTTH

#include <vcl.h>
#include <System.Classes.hpp>

// Windows API includes - avoid mmeapi.h conflicts by not including mmsystem.h in header
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Include only basic Windows types
#include <windows.h>

// Forward declarations for audio types (avoid mmsystem.h in header)
// mmsystem.h will be included only in .cpp file to prevent mmeapi.h conflicts
// Use opaque pointer types in header
struct WAVEHDR_IMPL;
typedef WAVEHDR_IMPL* WAVEHDR_PTR;
// Don't define HWAVEIN here - let mmsystem.h define it in cpp file
// Use void* in header to avoid typedef redefinition conflicts
typedef void* HWAVEIN_PTR;
typedef unsigned int MMRESULT;

#include <vector>
#include <string>
#include <functional>

// Audio recording parameters
#define SAMPLE_RATE 16000
#define CHANNELS 1
#define BITS_PER_SAMPLE 16
#define BUFFER_SIZE 3200  // 200ms buffer at 16kHz

// Whisper Model Types
enum class WhisperModelType {
    TINY,       // Fastest, less accurate
    BASE,       // Balanced
    SMALL,      // Good accuracy
    MEDIUM,     // Better accuracy
    LARGE       // Best accuracy, slower
};

// Recognition callback type
typedef std::function<void(const std::wstring&)> RecognitionCallback;

//---------------------------------------------------------------------------
// Main Whisper STT Class
//---------------------------------------------------------------------------
class TWhisperSTT
{
private:
    // Audio recording members (opaque pointers to avoid mmeapi.h conflicts)
    HWAVEIN_PTR hWaveIn;
    WAVEHDR_PTR waveHeaders[2];  // Use pointer array instead of array of structs
    bool isRecording;
    bool isInitialized;
    std::vector<short> audioBuffer;
    CRITICAL_SECTION audioLock;
    
    // Whisper Local Model members
    WhisperModelType currentModel;
    bool useLocalModel;
    std::wstring localModelPath;
    
    // Recognition callback
    RecognitionCallback onRecognitionCallback;
    
    // Worker thread
    HANDLE recognitionThread;
    HANDLE stopEvent;
    bool processingAudio;
    
    // Internal methods
    bool InitializeAudioCapture();
    void CleanupAudioCapture();
    bool StartRecording();
    void StopRecording();
    void ProcessAudioBuffer();
    std::wstring RunLocalWhisper(const std::vector<short>& audioData);
    bool SaveWavFile(const std::wstring& filename, const std::vector<short>& audioData);
    
    // Static callback for audio recording
    static void CALLBACK WaveInProc(void* hwi, unsigned int uMsg, void* dwInstance,
                                    void* dwParam1, void* dwParam2);
    
    // Static thread procedure
    static unsigned long WINAPI RecognitionThreadProc(void* lpParam);
    
public:
    TWhisperSTT();
    ~TWhisperSTT();
    
    // Configuration methods
    bool Initialize(WhisperModelType model = WhisperModelType::BASE);
    void SetLocalModelPath(const std::wstring& path);
    void UseLocalModel(bool useLocal);
    
    // Recording control
    bool StartListening();
    void StopListening();
    bool IsListening() const { return isRecording; }
    
    // Callback registration
    void SetRecognitionCallback(RecognitionCallback callback);
    
    // Model management
    void SetModel(WhisperModelType model);
    WhisperModelType GetCurrentModel() const { return currentModel; }
    
    // Status methods
    bool IsInitialized() const { return isInitialized; }
    std::wstring GetLastError() const { return lastError; }
    
    // Utility methods
    static std::wstring ModelTypeToString(WhisperModelType model);
    static std::vector<std::wstring> GetAvailableModels();
    
private:
    std::wstring lastError;
};

//---------------------------------------------------------------------------
// Utility class for VAD (Voice Activity Detection)
//---------------------------------------------------------------------------
class TVoiceActivityDetector
{
private:
    float energyThreshold;
    int silenceFrames;
    int maxSilenceFrames;
    bool isSpeaking;
    
public:
    TVoiceActivityDetector();
    
    bool DetectVoice(const std::vector<short>& audioData);
    void Reset();
    bool IsSpeaking() const { return isSpeaking; }
    void SetThreshold(float threshold);
    void SetMaxSilenceFrames(int frames);
    
private:
    float CalculateEnergy(const std::vector<short>& audioData);
};

//---------------------------------------------------------------------------
// Helper class for audio format conversion
//---------------------------------------------------------------------------
class TAudioConverter
{
public:
    static std::vector<short> ResampleAudio(const std::vector<short>& input,
                                           int inputRate, int outputRate);
    static std::vector<short> ConvertToMono(const std::vector<short>& stereoData);
    static std::vector<float> ConvertToFloat(const std::vector<short>& intData);
    static std::string EncodeBase64(const std::vector<short>& audioData);
};

//---------------------------------------------------------------------------
// Thread-safe audio buffer manager
//---------------------------------------------------------------------------
class TAudioBufferManager
{
private:
    std::vector<std::vector<short>> bufferQueue;
    CRITICAL_SECTION queueLock;
    HANDLE dataAvailableEvent;
    
public:
    TAudioBufferManager();
    ~TAudioBufferManager();
    
    void PushBuffer(const std::vector<short>& buffer);
    bool PopBuffer(std::vector<short>& buffer, unsigned long timeout = INFINITE);
    void Clear();
    size_t GetQueueSize();
};

#endif

