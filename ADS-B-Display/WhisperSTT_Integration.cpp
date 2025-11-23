//---------------------------------------------------------------------------
// WhisperSTT_Integration.cpp
// Example code showing how to integrate WhisperSTT into DisplayGUI
//---------------------------------------------------------------------------

/*
 * INTEGRATION GUIDE:
 * 
 * 1. Add WhisperSTT.h and WhisperSTT.cpp to your project
 * 
 * 2. In DisplayGUI.h, add:
 *    #include "WhisperSTT.h"
 *    
 *    In the private section of TForm1 class:
 *    TWhisperSTT *whisperSTT;
 * 
 * 3. Replace SAPI-related code as shown below
 */

//---------------------------------------------------------------------------
// STEP 1: In DisplayGUI.h, modify the class declaration
//---------------------------------------------------------------------------

/*
// OLD SAPI CODE (Remove or comment out):
#include "SpeechLib_OCX.h"
ISpeechRecoGrammar *SRGrammar;

// NEW WHISPER CODE (Add):
#include "WhisperSTT.h"
TWhisperSTT *whisperSTT;
*/

//---------------------------------------------------------------------------
// STEP 2: In DisplayGUI.cpp Constructor, initialize Whisper STT
//---------------------------------------------------------------------------

/*
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
    // ... existing initialization code ...
    
    // Initialize Whisper STT
    whisperSTT = new TWhisperSTT();
    
    // Configure for OpenAI API usage
    whisperSTT->SetAPIKey(L"your-openai-api-key-here");
    whisperSTT->UseLocalModel(false); // Use OpenAI API
    
    // OR Configure for local Whisper usage
    // whisperSTT->SetLocalModelPath(L"C:\\whisper.cpp");
    // whisperSTT->UseLocalModel(true);
    
    // Initialize with desired model
    if (!whisperSTT->Initialize(WhisperModelType::BASE))
    {
        ShowMessage("Failed to initialize Whisper STT: " + 
                   whisperSTT->GetLastError());
    }
    
    // Set up recognition callback
    whisperSTT->SetRecognitionCallback([this](const std::wstring& text) {
        // This callback will be called when speech is recognized
        // Thread-safe UI update using Synchronize
        TThread::Synchronize(nullptr, [this, text]() {
            Memo1->Lines->Add(text.c_str());
            ProcessVoiceCommand(text);
        });
    });
}
*/

//---------------------------------------------------------------------------
// STEP 3: In DisplayGUI.cpp Destructor, cleanup
//---------------------------------------------------------------------------

/*
__fastcall TForm1::~TForm1()
{
    if (whisperSTT)
    {
        whisperSTT->StopListening();
        delete whisperSTT;
        whisperSTT = NULL;
    }
    
    // ... rest of cleanup code ...
}
*/

//---------------------------------------------------------------------------
// STEP 4: Replace LIstenClick function
//---------------------------------------------------------------------------

/*
// OLD SAPI CODE (Remove):
void __fastcall TForm1::LIstenClick(TObject *Sender)
{
    Memo1->Visible=true;
    SpSharedRecoContext1->EventInterests = SpeechRecoEvents::SREAllEvents;
    SRGrammar=SpSharedRecoContext1->CreateGrammar(Variant(0));
    SRGrammar->CmdSetRuleIdState(0, SpeechRuleState::SGDSActive);
    SRGrammar->DictationSetState(SpeechRuleState::SGDSActive);
}

// NEW WHISPER CODE:
void __fastcall TForm1::LIstenClick(TObject *Sender)
{
    Memo1->Visible = true;
    
    if (!whisperSTT->IsListening())
    {
        if (whisperSTT->StartListening())
        {
            LIsten->Caption = "Stop Listening";
            StatusBar1->SimpleText = "Whisper STT: Listening...";
        }
        else
        {
            ShowMessage("Failed to start listening: " + 
                       whisperSTT->GetLastError());
        }
    }
    else
    {
        whisperSTT->StopListening();
        LIsten->Caption = "Start Listening";
        StatusBar1->SimpleText = "Whisper STT: Stopped";
    }
}
*/

//---------------------------------------------------------------------------
// STEP 5: Remove old SAPI recognition handler
//---------------------------------------------------------------------------

/*
// DELETE THIS OLD FUNCTION:
void __fastcall TForm1::SpSharedRecoContext1Recognition(TObject *Sender, 
    long StreamNumber, Variant StreamPosition, 
    SpeechRecognitionType RecognitionType, ISpeechRecoResult *Result)
{
    Memo1->Lines->Add( Result->PhraseInfo->GetText(0, -1, True) );
}
*/

//---------------------------------------------------------------------------
// STEP 6: Add voice command processing (OPTIONAL)
//---------------------------------------------------------------------------

void __fastcall TForm1::ProcessVoiceCommand(const std::wstring& text)
{
    // Convert to lowercase for easier matching
    std::wstring lowerText = text;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
    
    // Example voice commands for ADS-B Display
    
    if (lowerText.find(L"zoom in") != std::wstring::npos)
    {
        // Zoom in on map
        ZoomIn();
    }
    else if (lowerText.find(L"zoom out") != std::wstring::npos)
    {
        // Zoom out on map
        ZoomOut();
    }
    else if (lowerText.find(L"center map") != std::wstring::npos)
    {
        // Center map on default location
        CenterMap();
    }
    else if (lowerText.find(L"track") != std::wstring::npos)
    {
        // Extract aircraft ID and track it
        // Example: "track american 123"
        TrackAircraftByVoice(text);
    }
    else if (lowerText.find(L"show all") != std::wstring::npos)
    {
        // Show all aircraft
        ShowAllAircraft();
    }
    else if (lowerText.find(L"clear tracks") != std::wstring::npos)
    {
        // Clear track history
        ClearTracks();
    }
    else if (lowerText.find(L"connect") != std::wstring::npos)
    {
        // Connect to SBS feed
        SBSConnectButtonClick(nullptr);
    }
    else if (lowerText.find(L"disconnect") != std::wstring::npos)
    {
        // Disconnect from SBS feed
        if (SBSConnectButton->Caption == "SBS Disconnect")
            SBSConnectButtonClick(nullptr);
    }
}

//---------------------------------------------------------------------------
// EXAMPLE: Advanced usage with multiple languages
//---------------------------------------------------------------------------

void __fastcall TForm1::SetupMultilingualSTT()
{
    // Whisper supports multiple languages automatically
    // You can specify language in API call or let it auto-detect
    
    whisperSTT->SetRecognitionCallback([this](const std::wstring& text) {
        TThread::Synchronize(nullptr, [this, text]() {
            // Detect language and process accordingly
            if (IsKorean(text))
            {
                ProcessKoreanCommand(text);
            }
            else if (IsEnglish(text))
            {
                ProcessEnglishCommand(text);
            }
            else
            {
                // Default processing
                Memo1->Lines->Add(L"[Unknown Language] " + String(text.c_str()));
            }
        });
    });
}

//---------------------------------------------------------------------------
// EXAMPLE: Continuous vs. Push-to-Talk mode
//---------------------------------------------------------------------------

void __fastcall TForm1::SetupPushToTalkMode()
{
    // For push-to-talk, start listening on key press
    // and stop on key release
    
    // In form's KeyDown event:
    void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
    {
        if (Key == VK_SPACE && Shift.Contains(ssCtrl))
        {
            // Ctrl+Space to start listening
            if (!whisperSTT->IsListening())
            {
                whisperSTT->StartListening();
                StatusBar1->SimpleText = "Recording... (Release Ctrl+Space to process)";
            }
        }
    }
    
    // In form's KeyUp event:
    void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
    {
        if (Key == VK_SPACE)
        {
            // Release space to stop and process
            if (whisperSTT->IsListening())
            {
                whisperSTT->StopListening();
                StatusBar1->SimpleText = "Processing speech...";
            }
        }
    }
}

//---------------------------------------------------------------------------
// EXAMPLE: Real-time transcription with timestamp
//---------------------------------------------------------------------------

void __fastcall TForm1::SetupRealTimeTranscription()
{
    whisperSTT->SetRecognitionCallback([this](const std::wstring& text) {
        TThread::Synchronize(nullptr, [this, text]() {
            // Add timestamp to transcription
            TDateTime now = Now();
            String timeStamp = now.FormatString("hh:nn:ss");
            
            String transcription = "[" + timeStamp + "] " + String(text.c_str());
            Memo1->Lines->Add(transcription);
            
            // Optionally save to log file
            if (TranscriptionLogFile)
            {
                TranscriptionLogFile->WriteLine(transcription);
            }
        });
    });
}

//---------------------------------------------------------------------------
// EXAMPLE: Model selection UI
//---------------------------------------------------------------------------

void __fastcall TForm1::ModelComboBoxChange(TObject *Sender)
{
    // Assume you have a ComboBox with model options
    String selectedModel = ModelComboBox->Text;
    
    WhisperModelType modelType;
    
    if (selectedModel == "Tiny (Fastest)")
        modelType = WhisperModelType::TINY;
    else if (selectedModel == "Base (Balanced)")
        modelType = WhisperModelType::BASE;
    else if (selectedModel == "Small (Good)")
        modelType = WhisperModelType::SMALL;
    else if (selectedModel == "Medium (Better)")
        modelType = WhisperModelType::MEDIUM;
    else if (selectedModel == "Large (Best)")
        modelType = WhisperModelType::LARGE;
    else
        modelType = WhisperModelType::BASE;
    
    bool wasListening = whisperSTT->IsListening();
    
    if (wasListening)
        whisperSTT->StopListening();
    
    whisperSTT->SetModel(modelType);
    
    if (wasListening)
        whisperSTT->StartListening();
    
    StatusBar1->SimpleText = "Model changed to: " + selectedModel;
}

//---------------------------------------------------------------------------
// EXAMPLE: Error handling and status updates
//---------------------------------------------------------------------------

void __fastcall TForm1::SetupWhisperWithErrorHandling()
{
    whisperSTT->SetRecognitionCallback([this](const std::wstring& text) {
        TThread::Synchronize(nullptr, [this, text]() {
            if (text.empty())
            {
                StatusBar1->SimpleText = "No speech detected";
                return;
            }
            
            Memo1->Lines->Add(text.c_str());
            
            // Update statistics
            RecognitionCount++;
            UpdateStatisticsDisplay();
        });
    });
    
    // Create a timer to check STT status
    Timer1->Interval = 1000; // Check every second
    Timer1->OnTimer = [this](TObject* Sender) {
        if (whisperSTT->IsListening())
        {
            StatusBar1->Panels->Items[0]->Text = "STT: Active";
        }
        else
        {
            StatusBar1->Panels->Items[0]->Text = "STT: Inactive";
        }
        
        // Check for errors
        std::wstring error = whisperSTT->GetLastError();
        if (!error.empty())
        {
            Memo1->Lines->Add(L"[ERROR] " + String(error.c_str()));
        }
    };
}

//---------------------------------------------------------------------------

