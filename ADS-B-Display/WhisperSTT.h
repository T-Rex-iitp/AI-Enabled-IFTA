//---------------------------------------------------------------------------
// WhisperSTT.h - Whisper-based Speech-to-Text Implementation
// Uses OpenAI Whisper for advanced speech recognition
//---------------------------------------------------------------------------

#ifndef WhisperSTTH
#define WhisperSTTH

#include <vcl.h>
#include <System.Classes.hpp>
#include <windows.h>
#include <mmsystem.h>
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
    // Audio recording members
    HWAVEIN hWaveIn;
    WAVEHDR waveHeaders[2];
    bool isRecording;
    bool isInitialized;
    std::vector<short> audioBuffer;
    CRITICAL_SECTION audioLock;
    
    // Whisper API members
    WhisperModelType currentModel;
    std::wstring apiKey;
    std::wstring apiEndpoint;
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
    std::wstring SendToWhisperAPI(const std::vector<short>& audioData);
    std::wstring RunLocalWhisper(const std::vector<short>& audioData);
    bool SaveWavFile(const std::wstring& filename, const std::vector<short>& audioData);
    
    // Static callback for audio recording
    static void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance,
                                    DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    
    // Static thread procedure
    static DWORD WINAPI RecognitionThreadProc(LPVOID lpParam);
    
public:
    TWhisperSTT();
    ~TWhisperSTT();
    
    // Configuration methods
    bool Initialize(WhisperModelType model = WhisperModelType::BASE);
    void SetAPIKey(const std::wstring& key);
    void SetAPIEndpoint(const std::wstring& endpoint);
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
    bool PopBuffer(std::vector<short>& buffer, DWORD timeout = INFINITE);
    void Clear();
    size_t GetQueueSize();
};

#endif

