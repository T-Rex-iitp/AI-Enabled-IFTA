//---------------------------------------------------------------------------
#ifndef MDI_ChildH
#define MDI_ChildH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "..\..\Component\OpenGLPanel.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TChild : public TForm
{
__published:	// IDE-managed Components
    TSplitter *Splitter1;
    TOpenGLPanel *OpenGLPanel1;
    TTimer *Timer1;
    TScrollBox *ScrollBox1;
    TColorDialog *ColorDialog1;
    TPageControl *Control;
    TTabSheet *Colors;
    TShape *BackgroundColor;
    TShape *LightColor;
    TShape *ObjectColor;
    TStaticText *StaticText1;
    TStaticText *StaticText2;
    TStaticText *StaticText3;
    TTabSheet *Mode;
    TRadioGroup *RadioGroup1;
    TRadioButton *Verticies;
    TRadioButton *Wireframe;
    TRadioButton *Solid;
    TGroupBox *GroupBox1;
    TCheckBox *Lighting;
    TCheckBox *Antialiasing;
    TCheckBox *Smooth;
    TCheckBox *Animate;
    TTabSheet *TabSheet3;
    TTrackBar *RotX;
    TTrackBar *RotY;
    TTrackBar *RotZ;
    TStaticText *StaticText4;
    TStaticText *StaticText5;
    TStaticText *StaticText6;
    TCheckBox *Link;
    TCheckBox *Texture;
    void __fastcall OpenGLPanel1Init(TObject *Sender);
    void __fastcall OpenGLPanel1Paint(TObject *Sender);
    void __fastcall OpenGLPanel1Resize(TObject *Sender);
    void __fastcall BackgroundColorMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall LightColorMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall VerticiesClick(TObject *Sender);
    void __fastcall WireframeClick(TObject *Sender);
    void __fastcall SolidClick(TObject *Sender);
    void __fastcall LightingClick(TObject *Sender);
    void __fastcall SmoothClick(TObject *Sender);
    
    void __fastcall AntialiasingClick(TObject *Sender);
    void __fastcall RotXChange(TObject *Sender);
    void __fastcall RotYChange(TObject *Sender);
    void __fastcall RotZChange(TObject *Sender);
    void __fastcall AnimateClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    
    void __fastcall OpenGLPanel1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OpenGLPanel1MouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OpenGLPanel1MouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ObjectColorMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    
    void __fastcall TextureClick(TObject *Sender);
    
    
    
    
    
    
    void __fastcall OpenGLPanel1PreInit(TObject *Sender);
private:	// User declarations
public:		// User declarations
    GLfloat	Background[4];
    GLfloat OColor[5];
    GLfloat Scale[3];
    GLfloat RotXv,RotYv;
    Boolean MouseDown;
    int     LastX,LastY; 

    __fastcall TChild(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TChild *Child;
//---------------------------------------------------------------------------
#endif
          