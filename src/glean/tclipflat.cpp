// BEGIN_COPYRIGHT -*- glean -*-
// 
// Copyright (C) 2009  VMware, Inc. All Rights Reserved.
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the
// Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
// KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL VMWARE BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// END_COPYRIGHT


// Test that the correct provoking vertex is used when a tri/quad/polygon
// is clipped for glShadeModel(GL_FLAT).
//
// Test with glDrawArrays and glBegin/End.  Test GL_CCW and GL_CW winding.
// Back-face polygon culling is enabled so if the winding order of any
// primitive is incorrect, nothing may be drawn.
//
// XXX We should also test with two-sided lighting.
//
// If GL_ARB/EXT_provoking_vertex is supported, that feature is tested as well.
//
// Author: Brian Paul


#include <cassert>
#include <cstring>
#include "tclipflat.h"


namespace GLEAN {

static PFNGLPROVOKINGVERTEXEXTPROC ProvokingVertex_func = NULL;


// Note: all correctly rendered tris/quad/polygons will be green.
// Any other color indicates that the wrong vertex color was used.


// GL_TRIANGLES: provoking vertex = last of tri
static const GLfloat TriVerts[6][5] =
   {
     // R  G  B     X   Y
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 0, 1, 0,    1,  1 }, // PV

      { 0, 0, 1,    1,  1 },
      { 1, 0, 0,   -1,  1 },
      { 0, 1, 0,   -1, -1 } // PV
   };

// GL_TRIANGLES: first provoking vertex
static const GLfloat TriVertsFirstPV[6][5] =
   {
      { 0, 1, 0,    1,  1 }, // PV
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },

      { 0, 1, 0,   -1, -1 }, // PV
      { 0, 0, 1,    1,  1 },
      { 1, 0, 0,   -1,  1 }
   };


// GL_TRIANGLE_STRIP: provoking vertex = last of tri
static const GLfloat TriStripVerts[6][5] =
   {
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 0, 1, 0,   -1,  0 }, // PV
      { 0, 1, 0,    1,  0 }, // PV
      { 0, 1, 0,   -1,  1 }, // PV
      { 0, 1, 0,    1,  1 }  // PV
   };

// GL_TRIANGLE_STRIP: first provoking vertex
static const GLfloat TriStripVertsFirstPV[6][5] =
   {
      { 0, 1, 0,   -1, -1 }, // PV
      { 0, 1, 0,    1, -1 }, // PV
      { 0, 1, 0,   -1,  0 }, // PV
      { 0, 1, 0,    1,  0 }, // PV
      { 1, 0, 0,   -1,  1 },
      { 0, 0, 1,    1,  1 }
   };


// GL_TRIANGLE_FAN: provoking vertex = last of tri
static const GLfloat TriFanVerts[4][5] =
   {
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 0, 1, 0,    1,  1 }, // PV
      { 0, 1, 0,   -1,  1 }  // PV
   };

// GL_TRIANGLE_FAN: first provoking vertex
static const GLfloat TriFanVertsFirstPV[4][5] =
   {
      { 0, 0, 1,    1, -1 },
      { 0, 1, 0,    1,  1 }, // PV
      { 0, 1, 0,   -1,  1 }, // PV
      { 1, 0, 0,   -1, -1 }
   };


// GL_QUADS: provoking vertex = last of quad
static const GLfloat QuadVerts[4][5] =
   {
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 1, 1, 0,    1,  1 },
      { 0, 1, 0,   -1,  1 }  // PV
   };

// GL_QUADS: first provoking vertex
static const GLfloat QuadVertsFirstPV[4][5] =
   {
      { 0, 1, 0,   -1,  1 }, // PV
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 1, 1, 0,    1,  1 }
   };


// GL_QUAD_STRIP: provoking vertex = last of quad
static const GLfloat QuadStripVerts[6][5] =
   {
      { 1, 0, 0,   -1, -1 },
      { 0, 0, 1,    1, -1 },
      { 1, 1, 0,   -1,  0 },
      { 0, 1, 0,    1,  0 }, // PV
      { 1, 1, 0,   -1,  1 },
      { 0, 1, 0,    1,  1 }  // PV
   };

// GL_QUAD_STRIP: first provoking vertex
static const GLfloat QuadStripVertsFirstPV[6][5] =
   {
      { 0, 1, 0,   -1, -1 }, // PV
      { 1, 1, 0,    1, -1 },
      { 0, 1, 0,   -1,  0 }, // PV
      { 1, 0, 0,    1,  0 },
      { 0, 0, 1,   -1,  1 },
      { 1, 0, 0,    1,  1 }
   };


// GL_POLYGON: provoking vertex = first vertex
static const GLfloat PolygonVerts[4][5] =
   {
      { 0, 1, 0,   -1, -1 }, // PV
      { 1, 0, 0,    1, -1 },
      { 0, 0, 1,    1,  1 },
      { 1, 1, 0,   -1,  1 }
   };


#define Elements(array) (sizeof(array) / sizeof(array[0]))


enum draw_mode {
   BEGIN_END,
   DRAW_ARRAYS,
   DRAW_ELEMENTS,
   NUM_DRAW_MODES
};


ClipFlatResult::ClipFlatResult()
{
   pass = false;
}


void
ClipFlatTest::setup(void)
{
   glDisable(GL_DITHER);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.25, 1.25, -1.25, 1.25, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glShadeModel(GL_FLAT);
   glPixelStorei(GL_PACK_ALIGNMENT, 1);

   glFrontFace(GL_CW);
   glCullFace(GL_FRONT);
   glEnable(GL_CULL_FACE);

   if (GLUtils::haveExtension("GL_ARB_provoking_vertex")) {
      ProvokingVertex_func = reinterpret_cast<PFNGLPROVOKINGVERTEXPROC>
         (GLUtils::getProcAddress("glProvokingVertex"));
      provoking_vertex_first = true;
   }
   else if (GLUtils::haveExtension("GL_EXT_provoking_vertex")) {
      ProvokingVertex_func = reinterpret_cast<PFNGLPROVOKINGVERTEXEXTPROC>
         (GLUtils::getProcAddress("glProvokingVertexEXT"));
      provoking_vertex_first = true;
   }

   if (provoking_vertex_first) {
      GLboolean k;
      glGetBooleanv(GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT, &k);
      quads_follows_pv_convention = k;
   }
}


// Draw with glDrawArrays()
void
ClipFlatTest::drawArrays(GLenum mode, const GLfloat *verts, GLuint count)
{
   glColorPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), verts + 0);
   glVertexPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), verts + 3);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);

   glDrawArrays(mode, 0, count);

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
}


// Draw with glDrawElements()
void
ClipFlatTest::drawElements(GLenum mode, const GLfloat *verts, GLuint count)
{
   static const GLuint elements[6] = { 0, 1, 2, 3, 4, 5 };
   glColorPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), verts + 0);
   glVertexPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), verts + 3);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);

   assert(count <= Elements(elements));

   glDrawElements(mode, count, GL_UNSIGNED_INT, elements);

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
}


// Draw with glBegin/End()
void
ClipFlatTest::drawBeginEnd(GLenum mode, const GLfloat *verts, GLuint count)
{
   GLuint i;

   glBegin(mode);
   for (i = 0; i < count; i++) {
      glColor3fv(verts + i * 5);
      glVertex2fv(verts + i * 5 + 3);
   }
   glEnd();
}


// Read pixels and check pixels.  All pixels should be green or black.
// Any other color indicates a failure.
bool
ClipFlatTest::checkResult(Window &w, GLfloat badColor[3])
{
   GLubyte image[windowSize * windowSize * 3];
   GLuint i, j;
   GLboolean anyGreen = GL_FALSE;

   badColor[0] = badColor[1] = badColor[2] = 0.0f;

   glReadPixels(0, 0, windowSize, windowSize,
                GL_RGB, GL_UNSIGNED_BYTE, image);

   w.swap();

   for (i = 0; i < windowSize; i++) {
      for (j = 0; j < windowSize; j++) {
         GLuint k = (i * windowSize + j) * 3;

         if (image[k + 0] == 0 &&
             image[k + 1] == 0 &&
             image[k + 2] == 0) {
            // black - OK
         }
         else if (image[k + 0] == 0 &&
                  image[k + 1] >= 254 &&
                  image[k + 0] == 0) {
            // green - OK
            anyGreen = GL_TRUE;
         }
         else {
            // any other color = failure
            badColor[0] = image[k + 0] / 255.0;
            badColor[1] = image[k + 1] / 255.0;
            badColor[2] = image[k + 2] / 255.0;
            // sleep(10);
            return false;
         }
      }
   }
   return anyGreen;
}


void
ClipFlatTest::reportFailure(GLenum mode, int drawMode, GLuint facing,
                            GLuint fill,
                            const GLfloat badColor[3], GLfloat x, GLfloat y)
{
   const char *m, *d, *f, *p;

   switch (mode) {
   case GL_TRIANGLES:
      m = "GL_TRIANGLES";
      break;
   case GL_TRIANGLE_STRIP:
      m = "GL_TRIANGLE_STRIP";
      break;
   case GL_TRIANGLE_FAN:
      m = "GL_TRIANGLE_FAN";
      break;
   case GL_QUADS:
      m = "GL_QUADS";
      break;
   case GL_QUAD_STRIP:
      m = "GL_QUAD_STRIP";
      break;
   case GL_POLYGON:
      m = "GL_POLYGON";
      break;
   default:
      m = "???";
   }

   switch (drawMode) {
   case BEGIN_END:
      d = "glBegin/End";
      break;
   case DRAW_ARRAYS:
      d = "glDrawArrays";
      break;
   case DRAW_ELEMENTS:
      d = "glDrawElements";
      break;
   default:
      assert(0);
   }

   if (facing == 0)
      f = "GL_CCW";
   else
      f = "GL_CW";

   if (fill == 0)
      p = "GL_FILL";
   else
      p = "GL_LINE";

   env->log << name << ": Failure for "
            << d << "(" << m << "), glFrontFace(" << f
            << "), glPolygonMode(" << p << ")\n";

   env->log << "\tTranslation: " << x << ", " << y << "\n";

   if (testing_first_pv)
      env->log << "\tGL_EXT_provoking_vertex test: GL_FIRST_VERTEX_CONVENTION_EXT mode\n";

   env->log << "\tExpected color (0, 1, 0) but found ("
            << badColor[0] << ", "
            << badColor[1] << ", "
            << badColor[2] << ")\n";
}


// Test drawing/clipping at nine positions of which 8 will be clipped.
bool
ClipFlatTest::testPositions(Window &w, GLenum mode,
                            const GLfloat *verts, GLuint count)
{
   GLfloat x, y;
   GLuint facing, fill;
   int drawMode;

   // Loop over polygon mode: filled vs. outline
   for (fill = 0; fill < 2; fill++) {

      glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_LINE : GL_FILL);

      // Loop over drawing mode: glBegin/End vs glDrawArrays vs glDrawElements
      for (drawMode = 0; drawMode < NUM_DRAW_MODES; drawMode++) {

         // Loop over CW vs. CCW winding (should make no difference)
         for (facing = 0; facing < 2; facing++) {

            if (facing == 0) {
               glFrontFace(GL_CCW);
               glCullFace(GL_BACK);
            }
            else {
               glFrontFace(GL_CW);
               glCullFace(GL_FRONT);
            }

            // Position the geometry at 9 different locations to test
            // clipping against the left, right, bottom and top edges of
            // the window.
            // Only the center location will be unclipped.
            for (y = -1.0; y <= 1.0; y += 1.0) {
               for (x = -1.0; x <= 1.0; x += 1.0) {
                  glPushMatrix();
                  glTranslatef(x, y, 0.0);

                  glClear(GL_COLOR_BUFFER_BIT);

                  switch (drawMode) {
                  case BEGIN_END:
                     drawBeginEnd(mode, verts, count);
                     break;
                  case DRAW_ARRAYS:
                     drawArrays(mode, verts, count);
                     break;
                  case DRAW_ELEMENTS:
                     drawElements(mode, verts, count);
                     break;
                  default:
                     assert(0);
                  }

                  glPopMatrix();

                  GLfloat badColor[3];
                  if (!checkResult(w, badColor)) {
                     reportFailure(mode, drawMode, facing, fill, badColor, x, y);
                     glFlush();
                     //sleep(25);  // enable for debugging
                     return false;
                  }
                  //usleep(50000);  // enable for debugging
               }
            }
         }
      }
   }
   return true;
}


void
ClipFlatTest::runOne(ClipFlatResult &r, Window &w)
{
   setup();

   testing_first_pv = false;
   r.pass = true;

   if (r.pass)
      r.pass = testPositions(w, GL_TRIANGLES,
                             (GLfloat *) TriVerts,
                             Elements(TriVerts));

   if (r.pass)
      r.pass = testPositions(w, GL_TRIANGLE_STRIP,
                             (GLfloat *) TriStripVerts,
                             Elements(TriStripVerts));

   if (r.pass)
      r.pass = testPositions(w, GL_TRIANGLE_FAN,
                             (GLfloat *) TriFanVerts,
                             Elements(TriFanVerts));

   if (r.pass)
      r.pass = testPositions(w, GL_QUADS,
                             (GLfloat *) QuadVerts,
                             Elements(QuadVerts));

   if (r.pass)
      r.pass = testPositions(w, GL_QUAD_STRIP,
                             (GLfloat *) QuadStripVerts,
                             Elements(QuadStripVerts));

   if (r.pass)
      r.pass = testPositions(w, GL_POLYGON,
                             (GLfloat *) PolygonVerts,
                             Elements(PolygonVerts));

   if (provoking_vertex_first) {
      ProvokingVertex_func(GL_FIRST_VERTEX_CONVENTION_EXT);
      testing_first_pv = true;

      if (r.pass)
         r.pass = testPositions(w, GL_TRIANGLES,
                                (GLfloat *) TriVertsFirstPV,
                                Elements(TriVertsFirstPV));

      if (r.pass)
         r.pass = testPositions(w, GL_TRIANGLE_STRIP,
                                (GLfloat *) TriStripVertsFirstPV,
                                Elements(TriStripVertsFirstPV));

      if (r.pass)
         r.pass = testPositions(w, GL_TRIANGLE_FAN,
                                (GLfloat *) TriFanVertsFirstPV,
                                Elements(TriFanVertsFirstPV));

      if (r.pass) {
         if (quads_follows_pv_convention)
            r.pass = testPositions(w, GL_QUADS,
                                   (GLfloat *) QuadVertsFirstPV,
                                   Elements(QuadVertsFirstPV));
         else
            r.pass = testPositions(w, GL_QUADS,
                                   (GLfloat *) QuadVerts,
                                   Elements(QuadVerts));
      }

      if (r.pass) {
         if (quads_follows_pv_convention)
            r.pass = testPositions(w, GL_QUAD_STRIP,
                                   (GLfloat *) QuadStripVertsFirstPV,
                                   Elements(QuadStripVertsFirstPV));
         else
            r.pass = testPositions(w, GL_QUAD_STRIP,
                                   (GLfloat *) QuadStripVerts,
                                   Elements(QuadStripVerts));
      }

      if (r.pass) {
         r.pass = testPositions(w, GL_POLYGON,
                                (GLfloat *) PolygonVerts,
                                Elements(PolygonVerts));
      }
   }
}


void
ClipFlatTest::logOne(ClipFlatResult &r)
{
   logPassFail(r);
   logConcise(r);
}


void
ClipFlatTest::compareOne(ClipFlatResult &oldR,
			     ClipFlatResult &newR)
{
   comparePassFail(oldR, newR);
}


void
ClipFlatResult::putresults(ostream &s) const
{
   if (pass) {
      s << "PASS\n";
   }
   else {
      s << "FAIL\n";
   }
}


bool
ClipFlatResult::getresults(istream &s)
{
   char result[1000];
   s >> result;

   if (strcmp(result, "FAIL") == 0) {
      pass = false;
   }
   else {
      pass = true;
   }
   return s.good();
}


// The test object itself:
ClipFlatTest newTest("clipFlat", "window, rgb",
                     "",
                     "Test clipping with flat shading (provoking vertex).\n");



} // namespace GLEAN


