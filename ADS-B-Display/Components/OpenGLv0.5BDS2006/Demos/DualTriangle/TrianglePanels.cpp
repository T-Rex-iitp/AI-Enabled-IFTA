//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <gl\gl.h>
#include <gl\glu.h>

#include "TrianglePanels.h"
//---------------------------------------------------------------------------
#pragma link "OpenGLPanel"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel1Init(TObject *Sender)
{

 glViewport(0, 0, (GLsizei)OpenGLPanel1->Width,(GLsizei)OpenGLPanel1->Height);
 glLoadIdentity();
 glOrtho(-250.0, 250.0, -250.0, 250.0,1.0f, -1.0f);
 glShadeModel(GL_SMOOTH);
 glClearColor(1.0f,1.0f,1.0f,1.0f);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel1Paint(TObject *Sender)
{
 glPushMatrix();
 glRotatef(rot2,0.0,0.0,1.0);
 glClear(GL_COLOR_BUFFER_BIT);
 glBegin(GL_TRIANGLES);

 glColor3f(1.0f,0.0f,0.0f);
 glVertex3f(0.0f,200.0f,0.0f);

 glColor3f(0.0f,1.0f,0.0f);
 glVertex3f(200.0f,-200.0f,0.0f);

 glColor3f(0.0f,0.0f,1.0f);
 glVertex3f(-200.0f, -200.0f, 0.0f);
 glEnd();
 glPopMatrix();
 glFlush();

}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel1Resize(TObject *Sender)
{
 glViewport(0, 0,  (GLsizei)OpenGLPanel1->Width,(GLsizei)OpenGLPanel1->Height);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel2Init(TObject *Sender)
{
 glViewport(0, 0, (GLsizei)OpenGLPanel2->Width,(GLsizei)OpenGLPanel2->Height);
 glLoadIdentity();
 glOrtho(-250.0, 250.0, -250.0, 250.0,1.0f, -1.0f);
 glShadeModel(GL_SMOOTH);
 glClearColor(1.0f,1.0f,1.0f,1.0f);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel2Resize(TObject *Sender)
{
glViewport(0, 0,  (GLsizei)OpenGLPanel2->Width,(GLsizei)OpenGLPanel2->Height);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel2Paint(TObject *Sender)
{
 glPushMatrix();
 glRotatef(rot2,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT);
 glBegin(GL_TRIANGLES);

 glColor3f(1.0f,0.0f,0.0f);
 glVertex3f(0.0f,200.0f,0.0f);

 glColor3f(0.0f,1.0f,0.0f);
 glVertex3f(200.0f,-200.0f,0.0f);

 glColor3f(0.0f,0.0f,1.0f);
 glVertex3f(-200.0f, -200.0f, 0.0f);
 glEnd();
 glPopMatrix();
 glFlush();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormResize(TObject *Sender)
{
 OpenGLPanel1->Height=ClientHeight;
 OpenGLPanel2->Height=ClientHeight;
 OpenGLPanel1->Top=0;
 OpenGLPanel2->Top=0;
 OpenGLPanel1->Left=0;
 OpenGLPanel2->Left=ClientWidth/2;
 OpenGLPanel1->Width=ClientWidth/2;
 OpenGLPanel2->Width=ClientWidth/2;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
 rot1+=2.0;
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
rot2-=2.0;
 OpenGLPanel2->Repaint();	
}
//---------------------------------------------------------------------------
