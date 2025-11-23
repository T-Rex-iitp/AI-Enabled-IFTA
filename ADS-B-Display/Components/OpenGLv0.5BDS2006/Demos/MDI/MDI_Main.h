//---------------------------------------------------------------------------
#ifndef MDI_mainH
#define MDI_mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TMain_Form : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *Exit1;
    TMenuItem *N1;
    TMenuItem *New1;
    TMenuItem *Window1;
    TMenuItem *ArrangeAll1;
    TMenuItem *Cascade1;
    TMenuItem *Tile1;
    TMenuItem *Help1;
    TMenuItem *About1;
    void __fastcall Tile1Click(TObject *Sender);
    void __fastcall Cascade1Click(TObject *Sender);
    void __fastcall ArrangeAll1Click(TObject *Sender);
    
    void __fastcall New1Click(TObject *Sender);
    
    void __fastcall Exit1Click(TObject *Sender);
    
    void __fastcall About1Click(TObject *Sender);
    
private:	// User declarations
public:		// User declarations
    __fastcall TMain_Form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMain_Form *Main_Form;
//---------------------------------------------------------------------------
#endif
