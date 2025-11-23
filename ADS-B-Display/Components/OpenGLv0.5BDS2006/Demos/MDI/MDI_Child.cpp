//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MDI_Child.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OpenGLPanel"
#pragma resource "*.dfm"
TChild *Child;
//---------------------------------------------------------------------------
__fastcall TChild::TChild(TComponent* Owner)
    : TForm(Owner)
{
  RotXv=0.0;
  RotYv=0.0;
  MouseDown=false;
}
//---------------------------------------------------------------------------
void __fastcall TChild::OpenGLPanel1Init(TObject *Sender)
{
 BMPTexture *BMPTex;

 OpenGLPanel1Resize(NULL);

 if (Verticies->Checked)
       glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
 else if (Wireframe->Checked)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
 else if (Solid->Checked)
       glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

 if (Smooth->Checked)
      glShadeModel(GL_SMOOTH);
 else
      glShadeModel(GL_FLAT);

 if(Antialiasing->Checked)
	{
     glEnable(GL_LINE_SMOOTH);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
     glLineWidth(1.5);
	}
 else
    {
     glDisable(GL_LINE_SMOOTH);
     glDisable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
     glLineWidth(1.0);
	}
 if (Texture->Checked) glEnable(GL_TEXTURE_2D);
 else glDisable(GL_TEXTURE_2D);

  glEnable(GL_NORMALIZE);
  glClearDepth( 1.0 );

  GLfloat  Ambient[4];
  Ambient[0]= GetRValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
  Ambient[1]= GetGValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
  Ambient[2]= GetBValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
  Ambient[3]= 1.0;
  GLfloat Diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat Specular[]= {1.0f, 1.0f, 1.0f, 1.0f};


  glLightfv( GL_LIGHT0, GL_AMBIENT, Ambient);
  glLightfv( GL_LIGHT0, GL_DIFFUSE, Diffuse);
  glLightfv( GL_LIGHT0, GL_SPECULAR,Specular);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

  glEnable(GL_LIGHT0);

  if(Lighting->Checked) glEnable(GL_LIGHTING);
  else glDisable(GL_LIGHTING);

  Background[0]=GetRValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;
  Background[1]=GetGValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;;
  Background[2]=GetBValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;;
  Background[3]=1.0;

  OColor[0] = GetRValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
  OColor[1] = GetGValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
  OColor[2] = GetBValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
  OColor[3] = 1.0f;

  Scale[0] = (float)RotX->Position/50.0f;
  Scale[1] = (float)RotY->Position/50.0f;
  Scale[2] = (float)RotZ->Position/50.0f;


  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  if (Animate->Checked) Timer1->Enabled=true;
  else Timer1->Enabled=false;

  BMPTex=OpenGLPanel1->LoadBMPTexture("..\\..\\earth.bmp",true);
  if (BMPTex)
    {
      BMPTex->SetAlpha(128);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glTexImage2D(GL_TEXTURE_2D, 0, BMPTex->GetComponents(),
                   BMPTex->GetWidth(), BMPTex->GetHeight(),0,
                   BMPTex->GetFormat(),BMPTex->GetType(),
                   BMPTex->GetPixels());
    }
  else
    {
     Texture->State=cbGrayed;
     Texture->Checked=false;
     Texture->Enabled=false;
	}

}

//---------------------------------------------------------------------------

void __fastcall TChild::OpenGLPanel1Paint(TObject *Sender)
{
  static GLint  s_vec[]={1,0,0,0};
  static GLint  t_vec[]={0,1,0,0};
  double XYZ[3];

  glClearColor(Background[0],Background[1],Background[2],Background[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();

  glTranslated(0.0,-0.5,-4.0);
  glRotatef(RotXv, 0.0, 1.0, 0.0);
  glRotatef(RotYv, 1.0, 0.0, 0.0);
  glRotatef(-90.0, 1.0, 0.0, 0.0);

  glScalef(Scale[0],Scale[1],Scale[2]);
  glColor4f(OColor[0],OColor[1],OColor[2],OColor[3]);
  GLUquadricObj*	q = gluNewQuadric();
  gluQuadricDrawStyle(q, GLU_FILL);
  gluQuadricNormals(q, GLU_SMOOTH);
  if (Texture->Checked) gluQuadricTexture(q, GL_TRUE);
  gluSphere(q, 1.0, 20, 20);
  gluDeleteQuadric(q);
  glPopMatrix();


  // 3D Text
  glPushMatrix();
  glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGeniv(GL_S,GL_OBJECT_PLANE,s_vec);
  glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGeniv(GL_T,GL_OBJECT_PLANE,t_vec);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  glTranslated(0.0,1.0,-4.0);
  glRotatef(RotXv, 0.0, 1.0, 0.0);
  glRotatef(RotYv, 1.0, 0.0, 0.0);

  glScalef(Scale[0],Scale[1],Scale[2]);
  glScalef(0.75,0.75,0.75);
  OpenGLPanel1->Get3DTextSize("MDI DEMO",XYZ);
  glTranslated(-XYZ[0]/2.0,-XYZ[1]/2.0,-XYZ[2]/2.0);
  OpenGLPanel1->Draw3DText("MDI DEMO");
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glPopMatrix();

  // Raster Text

  if(Lighting->Checked) glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, (double)OpenGLPanel1->Width, (double)OpenGLPanel1->Height,0.0,
          1.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glRasterPos2i(20,20);
  glColor4f(OColor[0],OColor[1],OColor[2],OColor[3]);
  OpenGLPanel1->Draw2DText("Raster Text");
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  if(Lighting->Checked) glEnable(GL_LIGHTING);
}
//---------------------------------------------------------------------------

void __fastcall TChild::OpenGLPanel1Resize(TObject *Sender)
{
	double aspect;
	aspect = (OpenGLPanel1->Height == 0) ?
    (double)OpenGLPanel1->Width : (double)OpenGLPanel1->Width/
                                  (double)OpenGLPanel1->Height;

	glViewport(0,0,OpenGLPanel1->Width,OpenGLPanel1->Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,1,20.0);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);
	glEnable(GL_DEPTH_TEST);

}
//---------------------------------------------------------------------------

void __fastcall TChild::BackgroundColorMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (ColorDialog1->Execute())
    {
     BackgroundColor->Brush->Color = ColorDialog1->Color;
     Background[0] = GetRValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;
     Background[1] = GetGValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;
     Background[2] = GetBValue(ColorToRGB(BackgroundColor->Brush->Color))/255.0f;
     Background[3] = 1.0f;
     OpenGLPanel1->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TChild::LightColorMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (ColorDialog1->Execute())
    {
     GLfloat	Ambient[4];
     LightColor->Brush->Color = ColorDialog1->Color;
     Ambient[0]= GetRValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
     Ambient[1]= GetGValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
     Ambient[2]= GetBValue(ColorToRGB(LightColor->Brush->Color))/255.0f;
     Ambient[3]= 1.0f;
     OpenGLPanel1->MakeOpenGLPanelCurrent();
     glLightfv( GL_LIGHT0, GL_AMBIENT, Ambient);
     OpenGLPanel1->MakeOpenGLPanelNotCurrent();
     OpenGLPanel1->Repaint();
   }

}
//---------------------------------------------------------------------------

void __fastcall TChild::VerticiesClick(TObject *Sender)
{
 if (Verticies->Checked)
    {
      OpenGLPanel1->MakeOpenGLPanelCurrent();
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      OpenGLPanel1->MakeOpenGLPanelNotCurrent();
      OpenGLPanel1->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TChild::WireframeClick(TObject *Sender)
{
 if (Wireframe->Checked)
    {
      OpenGLPanel1->MakeOpenGLPanelCurrent();
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      OpenGLPanel1->MakeOpenGLPanelNotCurrent();
      OpenGLPanel1->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TChild::SolidClick(TObject *Sender)
{
if (Solid->Checked)
   {
    OpenGLPanel1->MakeOpenGLPanelCurrent();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    OpenGLPanel1->MakeOpenGLPanelNotCurrent();
    OpenGLPanel1->Repaint();
   }
}
//---------------------------------------------------------------------------

void __fastcall TChild::LightingClick(TObject *Sender)
{
 OpenGLPanel1->MakeOpenGLPanelCurrent();
 if (Lighting->Checked) glEnable(GL_LIGHTING);
 else glDisable(GL_LIGHTING);
 OpenGLPanel1->MakeOpenGLPanelNotCurrent();
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TChild::SmoothClick(TObject *Sender)
{
  OpenGLPanel1->MakeOpenGLPanelCurrent();
  if (Smooth->Checked)
      glShadeModel(GL_SMOOTH);
  else
      glShadeModel(GL_FLAT);
  OpenGLPanel1->MakeOpenGLPanelNotCurrent();
  OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------


void __fastcall TChild::AntialiasingClick(TObject *Sender)
{
 OpenGLPanel1->MakeOpenGLPanelCurrent();
 if(Antialiasing->Checked)
	{
     glEnable(GL_LINE_SMOOTH);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
     glLineWidth(1.25);
	}
 else
    {
     glDisable(GL_LINE_SMOOTH);
     glDisable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
     glLineWidth(1.0);
	}
  OpenGLPanel1->MakeOpenGLPanelNotCurrent();
  OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TChild::RotXChange(TObject *Sender)
{
 if (Link->Checked)
 {
  Scale[0]= Scale[1]=Scale[2] = (float)RotX->Position/50.0f;
  RotY->Position=RotX->Position;
  RotZ->Position=RotX->Position;
 }
 else Scale[0] = (float)RotX->Position/50.0f;
 OpenGLPanel1->Repaint();

}
//---------------------------------------------------------------------------

void __fastcall TChild::RotYChange(TObject *Sender)
{
 if (Link->Checked)
 {
  Scale[0]= Scale[1]=Scale[2] = (float)RotY->Position/50.0f;
  RotX->Position=RotY->Position;
  RotZ->Position=RotY->Position;
 }
 else Scale[1] = (float)RotY->Position/50.0f;

 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TChild::RotZChange(TObject *Sender)
{
 if (Link->Checked)
 {
  Scale[0]= Scale[1]=Scale[2] = (float)RotZ->Position/50.0f;
  RotX->Position=RotZ->Position;
  RotY->Position=RotZ->Position;
 }
 else Scale[2] = (float)RotZ->Position/50.0f;

 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TChild::AnimateClick(TObject *Sender)
{
  if (Animate->Checked) Timer1->Enabled=true;
  else Timer1->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TChild::Timer1Timer(TObject *Sender)
{
 RotXv += 5.0f;
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------


void __fastcall TChild::OpenGLPanel1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button!=mbLeft) return;
  MouseDown=true;
  LastX=X;
  LastY=Y;
}
//---------------------------------------------------------------------------

void __fastcall TChild::OpenGLPanel1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button!=mbLeft) return;
  MouseDown=false;
}
//---------------------------------------------------------------------------

void __fastcall TChild::OpenGLPanel1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if(MouseDown)
	{
     RotXv += (float)(X - LastX)/5.0f;
     RotYv += (float)(Y - LastY)/5.0f;
     LastX=X;
     LastY=Y;
     OpenGLPanel1->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TChild::FormClose(TObject *Sender, TCloseAction &Action)
{
 Action=caFree;
}
//---------------------------------------------------------------------------

void __fastcall TChild::ObjectColorMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (ColorDialog1->Execute())
    {
     ObjectColor->Brush->Color = ColorDialog1->Color;
     OColor[0] = GetRValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
     OColor[1] = GetGValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
     OColor[2] = GetBValue(ColorToRGB(ObjectColor->Brush->Color))/255.0f;
     OColor[3] = 1.0f;
     OpenGLPanel1->Repaint();
    }
}
//---------------------------------------------------------------------------



void __fastcall TChild::TextureClick(TObject *Sender)
{
 OpenGLPanel1->MakeOpenGLPanelCurrent();
 if (Texture->Checked) glEnable(GL_TEXTURE_2D);
 else glDisable(GL_TEXTURE_2D);
 OpenGLPanel1->MakeOpenGLPanelNotCurrent();
 OpenGLPanel1->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TChild::OpenGLPanel1PreInit(TObject *Sender)
{
 // This does not work as expected; the mode changes fine, but then
 // creation of the GL context fails ???
#ifdef DOTEST
 String Result;
 ShowMessage("DisplayBPP:"+IntToStr(OpenGLPanel1->GetDisplayBPP()));
 if (OpenGLPanel1->SetDisplayBPP(24)) Result="Success";
 else Result="Failure";
 ShowMessage("SetDisplayBPP:"+Result);
 ShowMessage("DisplayBPP now is :"+IntToStr(OpenGLPanel1->GetDisplayBPP()));
 #endif
}
//---------------------------------------------------------------------------

