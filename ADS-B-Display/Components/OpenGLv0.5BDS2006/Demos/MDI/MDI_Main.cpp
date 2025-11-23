//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MDI_main.h"
#include "MDI_Child.h"
#include "MDI_About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMain_Form *Main_Form;
//---------------------------------------------------------------------------
__fastcall TMain_Form::TMain_Form(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMain_Form::Tile1Click(TObject *Sender)
{
 Tile();    
}
//---------------------------------------------------------------------------

void __fastcall TMain_Form::Cascade1Click(TObject *Sender)
{
 Cascade();    
}
//---------------------------------------------------------------------------

void __fastcall TMain_Form::ArrangeAll1Click(TObject *Sender)
{
 ArrangeIcons();    
}
//---------------------------------------------------------------------------

void __fastcall TMain_Form::New1Click(TObject *Sender)
{
 new TChild(this);
}
//---------------------------------------------------------------------------

void __fastcall TMain_Form::Exit1Click(TObject *Sender)
{
 Close();    
}
//---------------------------------------------------------------------------


void __fastcall TMain_Form::About1Click(TObject *Sender)
{
 About->ShowModal();
}
//---------------------------------------------------------------------------


