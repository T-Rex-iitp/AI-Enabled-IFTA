//---------------------------------------------------------------------------
#ifndef OpenGLFont3DH
#define OpenGLFont3DH
//---------------------------------------------------------------------------
// My includes
#include <gl\gl.h>
#include <gl\glu.h>
//---------------------------------------------------------------------------
class OpenGLFont3D
{
       public:
                                OpenGLFont3D()
                                {
                                 GlyphMetrics= NULL;
                                }
                               ~OpenGLFont3D()
                               {
                                delete[] GlyphMetrics;
                                glDeleteLists(ListBase,NumGylph);
                               }
       private:
                    int                   FirstGylph;
                    int                   NumGylph;
                    int                   ListBase;
                    float                 Extrusion;
                    GLYPHMETRICSFLOAT   * GlyphMetrics;
      friend class TOpenGLPanel;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
