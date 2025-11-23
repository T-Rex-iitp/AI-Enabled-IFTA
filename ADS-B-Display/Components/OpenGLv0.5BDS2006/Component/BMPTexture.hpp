//---------------------------------------------------------------------------
#ifndef BMPTexureH
#define BMPTexureH
//---------------------------------------------------------------------------
// My includes
#include <gl\gl.h>
#include <gl\glu.h>
//---------------------------------------------------------------------------
class BMPTexture
{
       public:
                                BMPTexture()
                                {
                                 Pixels    = NULL;
                                }
                               ~BMPTexture()   { delete[] Pixels;         }
                     GLint     GetComponents() { return Components;       }
                     GLsizei   GetWidth()      { return Width;            }
                     GLsizei   GetHeight()     { return Height;           }
                     GLenum    GetFormat()     { return Format;           }
                     GLenum    GetType()       { return Type;             }
               const GLvoid  * GetPixels()     { return (GLvoid *)Pixels; }
                     void      SetAlpha(GLubyte Red,
                                        GLubyte Blue,
                                        GLubyte Green,
                                        GLubyte Alpha)
                                        {
                                          if (Components!=4) return;
                                          GLsizei size=Width*Height*Components;
                                          for (GLsizei i=0;i<size;i+=Components)
                                           {
                                            if ((Pixels[i+0]==Red) &&
                                                (Pixels[i+1]==Blue) &&
                                                (Pixels[i+2]==Green))
                                                  Pixels[i+3]=Alpha;
                                            }

                                        }
                     void      SetAlpha(GLubyte Alpha)
                                        {
                                          if (Components!=4) return;
                                          GLsizei  size=Width*Height*Components;
                                          for (GLsizei i=0;i<size;i+=Components)
                                           {
                                            Pixels[i+3]=Alpha;
                                           }
                                        }
       public:
                     GLuint  Name;
       private:
                     GLint    Components;
                     GLsizei  Width;
                     GLsizei  Height;
                     GLenum   Format;
                     GLenum   Type;
                     GLubyte *Pixels;
      friend class TOpenGLPanel;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
