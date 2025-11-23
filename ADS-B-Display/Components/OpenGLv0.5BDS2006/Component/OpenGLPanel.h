//---------------------------------------------------------------------------

#ifndef OpenGLPanelH
#define OpenGLPanelH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <gl\gl.h>
#include <gl\glu.h>
#include "BMPTexture.hpp"
#include "OpenGLFont3D.hpp"
#include "OpenGLFont2D.hpp"
//---------------------------------------------------------------------------
class PACKAGE TOpenGLPanel : public TCustomPanel
{
private:
 //Enumerations
 enum TPFDPixelTypes     {f_PFD_TYPE_RGBA,f_PFD_TYPE_COLORINDEX};
 enum TPFDFlags          {f_PFD_DRAW_TO_WINDOW, f_PFD_DRAW_TO_BITMAP,
                         f_PFD_SUPPORT_GDI, f_PFD_SUPPORT_OPENGL,
                         f_PFD_GENERIC_ACCELERATED, f_PFD_GENERIC_FORMAT,
                         f_PFD_NEED_PALETTE, f_PFD_NEED_SYSTEM_PALETTE,
                         f_PFD_DOUBLEBUFFER,f_PFD_STEREO,
                         f_PFD_SWAP_LAYER_BUFFERS};
 enum TOpenGLSwapBuffers {Auto,Manual};
 enum TOpenGLPrintScale  {pglNone,pglPrintToFit,pglProportional};
 enum TFont3DFormat      {f_WGL_FONT_LINES,f_WGL_FONT_POLYGONS};

 // types
 typedef Set<TPFDFlags, f_PFD_DRAW_TO_WINDOW, f_PFD_SWAP_LAYER_BUFFERS>
                                                                 TPFDFlagsSet;
 // variables
   HDC                   DisplayDeviceContext;
   HGLRC                 GLRenderingContext;
   HPALETTE              Palette;

   TPFDFlagsSet          FdwFlags;
   bool                  DoubleBuffer;
   bool                  AutoSwapbuffers;
   TPFDPixelTypes        FiPixelType;
   BYTE                  SavedPixelType;
   BYTE                  FcColorBits;
   BYTE                  FcDepthBits;
   TOpenGLSwapBuffers    FSwapBuffers;

   TOpenGLPrintScale     FOpenGLPrintScale;
   int                   FOpenGLPixelsPerInch;

   bool                  Font3DEnabled;
   TFont               * Font3DType;
   int                   Font3DFirstGylph;
   int                   Font3DNumGylph;
   float                 Font3DMaxDeviation;
   float                 Font3DExtrusion;
   TFont3DFormat         Font3DFormat;
   int                   SavedFont3DFormat;
   OpenGLFont3D        * Font3DDefault;

   bool                  Font2DEnabled;
   TFont               * Font2DType;
   int                   Font2DFirstGylph;
   int                   Font2DNumGylph;
   OpenGLFont2D        * Font2DDefault;

   // Methods
              bool   __fastcall CreateGLContext(void);
              void   __fastcall DestroyGLContext(void);
              bool   __fastcall ConstructPalette(PIXELFORMATDESCRIPTOR &pfd);
              void   __fastcall Create3DFont(void);
              void   __fastcall Create2DFont(void);

   // Property Medthods
              void   __fastcall SetPFDPixelType(const TPFDPixelTypes Value);
              void   __fastcall SetPFDFlags(const TPFDFlagsSet Value);
              void   __fastcall SetPFDNumColorBits(const BYTE Value);
              void   __fastcall SetPFDNumDepthBits(const BYTE Value);

              void   __fastcall SetOpenGLPrintScale(const TOpenGLPrintScale Value);
              void   __fastcall SetOpenGLPixelsPerInch(const int Value);

              void   __fastcall SetFont3DType(TFont* Value);
              void   __fastcall SetFont3DFormat(const TFont3DFormat Value);
              void   __fastcall SetFont(TFont* Value);

protected:
   // Variables
   TNotifyEvent FOnPaint,FOnResize,FOnInit,FOnPreInit;

   // Inherited or Overidden Methods
   virtual    void   __fastcall CreateParams(TCreateParams &Params);
   virtual    void   __fastcall Paint();
   DYNAMIC    void   __fastcall Resize();
              void   __fastcall WMCreate(TWMCreate &Message);
              void   __fastcall WMPaletteChanged(TWMPaletteChanged &Message);
              void   __fastcall WMQueryNewPalette(TWMQueryNewPalette &Message);
              void   __fastcall WMEraseBkgnd(TWMEraseBkgnd &Message);
              void   __fastcall WMDestroy(TWMDestroy &Message);
public:
                     __fastcall TOpenGLPanel(TComponent* Owner);
              void   __fastcall MakeOpenGLPanelCurrent(void);
              void   __fastcall MakeOpenGLPanelNotCurrent(void);
              void   __fastcall SwapBuffers(void);
              bool   __fastcall PanelShareLists(TOpenGLPanel *WithOpenGLPanel);
              OpenGLFont3D *
                     __fastcall Create3DFont(TFont *Font, int FirstGylph,
                                             int NumGylph,float Deviation,
                                             float Extrusion, int Format);
              void   __fastcall Get3DTextSize(AnsiString Text,GLdouble *XYZ);
              void   __fastcall Get3DTextSize(OpenGLFont3D *Font,
                                              AnsiString Text,GLdouble *XYZ);
              void   __fastcall Draw3DText(AnsiString Text);
              void   __fastcall Draw3DText(OpenGLFont3D * Font,AnsiString Text);
              OpenGLFont2D *
                     __fastcall Create2DFont(TFont *Font, int FirstGylph,
                                             int NumGylph);
              void   __fastcall Draw2DText(AnsiString Text);
              void   __fastcall Draw2DText(OpenGLFont2D * Font,AnsiString Text);

              BMPTexture *
                     __fastcall LoadBMPTexture(AnsiString FileName,
                                               Boolean WithAlpha);
              void   __fastcall PrintOpenGLPanel(void);
              void   __fastcall SaveOpenGLPanelToFile(const String &FileName);
              bool   __fastcall QueryOpenGLExtention(AnsiString ExtentionName);
              int    __fastcall GetDisplayBPP(void);
              bool   __fastcall SetDisplayBPP(int);
__published:
   // Inherited Properties
   __property Alignment;
   __property Align;
   __property DragCursor;
   __property DragMode;
   __property Enabled;
   __property ParentFont;
   __property ParentShowHint;
   __property PopupMenu;
   __property ShowHint;
   __property TabOrder;
   __property TabStop;
   __property Visible;

   // Added Properites
   __property TPFDPixelTypes     PixelType = {read= FiPixelType,
                                 write= SetPFDPixelType,
                                 default=f_PFD_TYPE_RGBA};

   __property TPFDFlagsSet       PFDFlags = {read=FdwFlags,
                                 write=SetPFDFlags,nodefault};

   __property BYTE               cColorBits = {read= FcColorBits,
                                 write=SetPFDNumColorBits, default=32};

   __property BYTE               cDepthBits = {read= FcDepthBits,
                                 write=SetPFDNumDepthBits, default=32};

   __property TOpenGLSwapBuffers GLSwapBuffers ={read=FSwapBuffers,
                                 write=FSwapBuffers, default=Auto};

   __property int                PixelsPerInch = {read=FOpenGLPixelsPerInch,
                                 write= SetOpenGLPixelsPerInch,default=96};

   __property TOpenGLPrintScale  PrintScale = {read= FOpenGLPrintScale,
                                 write=SetOpenGLPrintScale,
                                 default=pglProportional};

   __property bool               Font3D_Enabled = {read=Font3DEnabled,
                                 write=Font3DEnabled, default=false};

   __property TFont              * Font3D_Type = {read=Font3DType,
                                 write=SetFont3DType};

   __property int                Font3D_FirstGylph = {read=Font3DFirstGylph,
                                 write=Font3DFirstGylph, default=32};

   __property int                Font3D_NumGylph = {read=Font3DNumGylph,
                                 write=Font3DNumGylph, default=96};

   __property float              Font3D_MaxDeviation = {read=Font3DMaxDeviation,
                                 write=Font3DMaxDeviation};

   __property float              Font3D_Extrustion = {read=Font3DExtrusion,
                                 write=Font3DExtrusion};

   __property TFont3DFormat      Font3D_Format = {read=Font3DFormat,
                                 write=SetFont3DFormat,
                                 default=f_WGL_FONT_POLYGONS};

   __property bool               Font2D_Enabled = {read=Font2DEnabled,
                                 write=Font2DEnabled, default=false};

   __property TFont              * Font2D_Type = {read=Font2DType,
                                 write=SetFont};

   __property int                Font2D_FirstGylph = {read=Font2DFirstGylph,
                                 write=Font2DFirstGylph, default=32};

   __property int                Font2D_NumGylph = {read=Font2DNumGylph,
                                 write=Font2DNumGylph, default=96};


   //  Inherited Events
   __property OnClick;
   __property OnDblClick;
   __property OnDragDrop;
   __property OnDragOver;
   __property OnEndDrag;
   __property OnEnter;
   __property OnExit;
   __property OnMouseDown;
   __property OnMouseMove;
   __property OnMouseUp;
   __property OnStartDrag;

   // Added Events
      // Use OnInit to initalize your OpenGL State
   __property TNotifyEvent OnInit    = {read=FOnInit,write=FOnInit};
     // Warning::OnPreInit is intended for changing Display BPP Modes only!!!
   __property TNotifyEvent OnPreInit = {read=FOnPreInit,write=FOnPreInit};

   // Overidden Events
   __property TNotifyEvent OnResize = {read=FOnResize,write=FOnResize};
   __property TNotifyEvent OnPaint  = {read=FOnPaint,write=FOnPaint};

   // Message Maps
   BEGIN_MESSAGE_MAP
      MESSAGE_HANDLER(WM_CREATE,TWMCreate,WMCreate)
      MESSAGE_HANDLER(WM_DESTROY,TWMDestroy,WMDestroy)
      MESSAGE_HANDLER(WM_PALETTECHANGED,TWMPaletteChanged,WMPaletteChanged)
      MESSAGE_HANDLER(WM_QUERYNEWPALETTE,TWMQueryNewPalette,WMQueryNewPalette)
      MESSAGE_HANDLER(WM_ERASEBKGND,TWMEraseBkgnd,WMEraseBkgnd)
   END_MESSAGE_MAP(TCustomPanel)
};
//---------------------------------------------------------------------------
#endif
