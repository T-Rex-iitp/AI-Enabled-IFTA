//---------------------------------------------------------------------------

#ifndef AreaDialogH
#define AreaDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAreaConfirm : public TForm
{
__published:	// IDE-managed Components
	TButton *OkButton;
	TButton *CancelButton;
	TEdit *AreaName;
	TColorBox *ColorBox1;

	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
private:	// User declarations
         	DWORD      ColorCount;
public:		// User declarations
	__fastcall TAreaConfirm(TComponent* Owner);
	void __fastcall ShowDialog(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TAreaConfirm *AreaConfirm;
//---------------------------------------------------------------------------
#endif
