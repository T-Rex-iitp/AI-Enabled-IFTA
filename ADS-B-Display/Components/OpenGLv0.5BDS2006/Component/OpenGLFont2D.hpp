//---------------------------------------------------------------------------
#ifndef OpenGLFont2DH
#define OpenGLFont2DH
//---------------------------------------------------------------------------
// My includes
#include <gl\gl.h>
#include <gl\glu.h>
//---------------------------------------------------------------------------
class OpenGLFont2D
{
       public:
                                OpenGLFont2D()
                                 {
                                 }
                                ~OpenGLFont2D()
                                 {
                                  glDeleteLists(ListBase,NumGylph);
                                 }
       private:
                    int                   FirstGylph;
                    int                   NumGylph;
                    int                   ListBase;
      friend class TOpenGLPanel;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
