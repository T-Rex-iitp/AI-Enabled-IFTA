//---------------------------------------------------------------------------
#ifndef TrianglePanelsH
#define TrianglePanelsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "..\..\Component\OpenGLPanel.h"
#include <ExtCtrls.hpp>
#include "TrianglePanels.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TOpenGLPanel *OpenGLPanel1;
	TOpenGLPanel *OpenGLPanel2;
	TTimer *Timer1;
	TTimer *Timer2;
	void __fastcall OpenGLPanel1Init(TObject *Sender);
	void __fastcall OpenGLPanel1Paint(TObject *Sender);
	void __fastcall OpenGLPanel1Resize(TObject *Sender);
	
	void __fastcall OpenGLPanel2Init(TObject *Sender);
	void __fastcall OpenGLPanel2Resize(TObject *Sender);
	void __fastcall OpenGLPanel2Paint(TObject *Sender);
	
	
	void __fastcall FormResize(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
        
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    float      rot1,rot2;
};
//---------------------------------------------------------------------------
extern TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
