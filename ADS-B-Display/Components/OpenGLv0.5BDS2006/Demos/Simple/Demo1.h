//---------------------------------------------------------------------------
#ifndef Demo1H
#define Demo1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "..\..\Component\OpenGLPanel.h"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TOpenGLPanel *OpenGLPanel1;
    TPanel *Panel1;
    TTrackBar *TrackBar1;
    TTrackBar *TrackBar2;
    TSaveDialog *SaveDialog1;
    TPrintDialog *PrintDialog1;
    TButton *Button1;
    TButton *Button2;
    void __fastcall OpenGLPanel1Init(TObject *Sender);
    void __fastcall OpenGLPanel1Resize(TObject *Sender);
    void __fastcall OpenGLPanel1Paint(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall TrackBar1Change(TObject *Sender);
    void __fastcall TrackBar2Change(TObject *Sender);
    
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
    float RotX,RotY;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
