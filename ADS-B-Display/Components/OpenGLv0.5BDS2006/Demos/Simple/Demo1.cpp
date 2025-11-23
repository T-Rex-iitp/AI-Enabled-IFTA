//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Demo1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OpenGLPanel"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
 RotX=(float)TrackBar1->Position;
 RotY=(float)TrackBar2->Position;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel1Init(TObject *Sender)
{
   	glViewport(0,0,(GLsizei)OpenGLPanel1->Width,(GLsizei)OpenGLPanel1->Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    if ( OpenGLPanel1->Height==0)
     	gluPerspective(45, (GLdouble)OpenGLPanel1->Width, 1.0, 2000.0);
    else
        gluPerspective(45, (GLdouble)OpenGLPanel1->Width/
                           (GLdouble)OpenGLPanel1->Height,1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
        glClearColor(0.0,0.0,0.0,1.0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenGLPanel1Resize(TObject *Sender)
{
 glViewport(0,0,(GLsizei)OpenGLPanel1->Width,(GLsizei)OpenGLPanel1->Height);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 if ( OpenGLPanel1->Height==0)
    gluPerspective(45, (GLdouble)OpenGLPanel1->Width, 1.0, 2000.0);
 else
    gluPerspective(45, (GLdouble)OpenGLPanel1->Width/
                           (GLdouble)OpenGLPanel1->Height,1.0, 2000.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::OpenGLPanel1Paint(TObject *Sender)
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glPushMatrix();
 glTranslated(0.0, 0.0, -5.0);
 glRotatef(RotX, 1.0, 0.0, 0.0);
 glRotatef(RotY, 0.0, 1.0, 0.0);

 glBegin(GL_POLYGON);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(  1.0,  1.0,  1.0);
    glVertex3f( -1.0,  1.0,  1.0);
    glVertex3f( -1.0, -1.0,  1.0);
    glVertex3f(  1.0, -1.0,  1.0);
 glEnd();

 glBegin(GL_POLYGON);
    glColor3f(0.0,1.0,0.0);
    glVertex3f( -1.0, -1.0,  1.0);
    glVertex3f( -1.0,  1.0,  1.0);
    glVertex3f( -1.0,  1.0, -1.0);
    glVertex3f( -1.0, -1.0, -1.0);
 glEnd();

 glBegin(GL_POLYGON);
    glColor3f(0.0,1.0,1.0);
    glNormal3f(  0.0, -1.0,  0.0);
    glVertex3f( -1.0, -1.0,  1.0);
    glVertex3f(  1.0, -1.0,  1.0);
    glVertex3f(  1.0, -1.0, -1.0);
    glVertex3f( -1.0, -1.0, -1.0);
 glEnd();

 glBegin(GL_POLYGON);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-1.0, 1.0,-1.0);
    glVertex3f( 1.0, 1.0,-1.0);
    glVertex3f( 1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
 glEnd();

 glBegin(GL_POLYGON);
    glColor3f(1.0,0.0,1.0);
    glVertex3f( 1.0, 1.0, 1.0);
    glVertex3f( 1.0,-1.0, 1.0);
    glVertex3f( 1.0,-1.0,-1.0);
    glVertex3f( 1.0, 1.0,-1.0);
 glEnd();

 glBegin(GL_POLYGON);
    glColor3f(1.0,1.0,0.0);
    glVertex3f(  1.0,  1.0,  1.0);
    glVertex3f( -1.0,  1.0,  1.0);
    glVertex3f( -1.0,  1.0, -1.0);
    glVertex3f(  1.0,  1.0, -1.0);
 glEnd();

 glPopMatrix();    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
 if (SaveDialog1->Execute())
       OpenGLPanel1->SaveOpenGLPanelToFile(SaveDialog1->FileName);   
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
 if (PrintDialog1->Execute()) OpenGLPanel1->PrintOpenGLPanel();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
RotX=(float)TrackBar1->Position;
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar2Change(TObject *Sender)
{
 RotY=(float)TrackBar2->Position;
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------


