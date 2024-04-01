/*{{{
  ZLIB LICENSE

  Copyright (C) 1996-1998, Justin Frankel
  Copyright (C) 1998-2000, Nullsoft, Inc.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Justin Frankel
  justin@nullsoft.com
}}}*/
#ifndef PLUSH_H
#define PLUSH_H

#include <stdbool.h>
#include <stdarg.h>
#include <inttypes.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PL_API
	#define PL_API
#endif

/* Maximum children per object */
#ifndef PL_MAX_CHILDREN
	#define PL_MAX_CHILDREN (16)
#endif

/* Maximum lights per scene -- if you exceed this, they will be ignored */
#ifndef PL_MAX_LIGHTS
	#define PL_MAX_LIGHTS (32)
#endif

/* Maximum number of triangles per scene -- if you exceed this, entire
objects will be ignored. You can increase this if you need it. It takes
approximately 8*PL_MAX_TRIANGLES bytes of memory. i.e. the default of
16384 consumes 128kbytes of memory. not really a big deal,
*/
#ifndef PL_MAX_TRIANGLES
	#define PL_MAX_TRIANGLES (16384)
#endif

#ifndef NUM_CLIP_PLANES
	#define NUM_CLIP_PLANES 5
#endif

#ifndef PL_COB_MAX_LINELENGTH
	#define PL_COB_MAX_LINELENGTH 1024
#endif

#ifndef PL_DEFAULT_FONT_TAB_SIZE
	#define PL_DEFAULT_FONT_TAB_SIZE 4
#endif

#define PL_DEFAULT_FONT_SIZE 16
#define PL_DEFAULT_FONT_WIDTH 8

#ifndef PL_PI
	#ifdef M_PI
		#define PL_PI M_PI
	#else
		#define PL_PI 3.14159265359
	#endif
#endif

/* Utility min() and max() functions */
#define plMin(x,y) (( ( x ) > ( y ) ? ( y ) : ( x )))
#define plMax(x,y) (( ( x ) < ( y ) ? ( y ) : ( x )))

/*
** Shade modes. Used with plMat.ShadeType
** Note that (PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE) and
** (PL_SHADE_FLAT|PL_SHADE_FLAT_DISTANCE) are valid shading modes.
*/
#define PL_SHADE_NONE (1)
#define PL_SHADE_FLAT (2)
#define PL_SHADE_FLAT_DISTANCE (4)
#define PL_SHADE_GOURAUD (8)
#define PL_SHADE_GOURAUD_DISTANCE (16)

/*
** Light modes. Used with plLight.Type or plLightSet().
** Note that PL_LIGHT_POINT_ANGLE assumes no falloff and uses the angle between
** the light and the point, PL_LIGHT_POINT_DISTANCE has falloff with proportion
** to distance**2 (see plLightSet() for setting it), PL_LIGHT_POINT does both.
*/
#define PL_LIGHT_NONE (0x0)
#define PL_LIGHT_VECTOR (0x1)
#define PL_LIGHT_POINT (0x2|0x4)
#define PL_LIGHT_POINT_DISTANCE (0x2)
#define PL_LIGHT_POINT_ANGLE (0x4)

/* Used internally; PL_FILL_* are stored in plMat._st. */
#define PL_FILL_SOLID (0x0)
#define PL_FILL_TEXTURE (0x1)
#define PL_FILL_ENVIRONMENT (0x2)
#define PL_FILL_TRANSPARENT (0x4)

#define PL_TEXENV_ADD (0)
#define PL_TEXENV_MUL (1)
#define PL_TEXENV_AVG (2)
#define PL_TEXENV_TEXMINUSENV (3)
#define PL_TEXENV_ENVMINUSTEX (4)
#define PL_TEXENV_MIN (5)
#define PL_TEXENV_MAX (6)

#define PUTFACE_SORT() \
  i0 = 0; i1 = 1; i2 = 2; \
  if (TriFace->Scry[0] > TriFace->Scry[1]) { \
     i0 = 1; i1 = 0; \
  } \
  if (TriFace->Scry[i0] > TriFace->Scry[2]) { \
     i2 ^= i0; i0 ^= i2; i2 ^= i0; \
  } \
  if (TriFace->Scry[i1] > TriFace->Scry[i2]) { \
     i2 ^= i1; i1 ^= i2; i2 ^= i1; \
  }

#define PUTFACE_SORT_ENV() \
  PUTFACE_SORT(); \
  MappingU1=TriFace->eMappingU[i0]*Texture->uScale*\
            TriFace->Material->EnvScaling;\
  MappingV1=TriFace->eMappingV[i0]*Texture->vScale*\
            TriFace->Material->EnvScaling;\
  MappingU2=TriFace->eMappingU[i1]*Texture->uScale*\
            TriFace->Material->EnvScaling;\
  MappingV2=TriFace->eMappingV[i1]*Texture->vScale*\
            TriFace->Material->EnvScaling;\
  MappingU3=TriFace->eMappingU[i2]*Texture->uScale*\
            TriFace->Material->EnvScaling;\
  MappingV3=TriFace->eMappingV[i2]*Texture->vScale*\
            TriFace->Material->EnvScaling;

#define PUTFACE_SORT_TEX() \
  PUTFACE_SORT(); \
  MappingU1=TriFace->MappingU[i0]*Texture->uScale*\
            TriFace->Material->TexScaling;\
  MappingV1=TriFace->MappingV[i0]*Texture->vScale*\
            TriFace->Material->TexScaling;\
  MappingU2=TriFace->MappingU[i1]*Texture->uScale*\
            TriFace->Material->TexScaling;\
  MappingV2=TriFace->MappingV[i1]*Texture->vScale*\
            TriFace->Material->TexScaling;\
  MappingU3=TriFace->MappingU[i2]*Texture->uScale*\
            TriFace->Material->TexScaling;\
  MappingV3=TriFace->MappingV[i2]*Texture->vScale*\
            TriFace->Material->TexScaling;

typedef float pl_ZBuffer;              /* z-buffer type (must be float) */
typedef float pl_Float;                /* General floating point */
typedef float pl_IEEEFloat32;          /* IEEE 32 bit floating point */
typedef signed long int pl_sInt32;     /* signed 32 bit integer */
typedef unsigned long int pl_uInt32;   /* unsigned 32 bit integer */
typedef signed short int pl_sInt16;    /* signed 16 bit integer */
typedef unsigned short int pl_uInt16;  /* unsigned 16 bit integer */
typedef signed int pl_sInt;            /* signed optimal integer */
typedef unsigned int pl_uInt;          /* unsigned optimal integer */
typedef int pl_Bool;                   /* boolean */
typedef unsigned char pl_uChar;        /* unsigned 8 bit integer */
typedef signed char pl_sChar;          /* signed 8 bit integer */

/*
** Texture type. Read textures with plReadPCXTex(), and assign them to
** plMat.Environment or plMat.Texture.
*/
typedef struct _pl_Texture {
  pl_uChar *Data;            /* Texture data */
  pl_uChar *PaletteData;     /* Palette data (NumColors bytes) */
  pl_uChar Width, Height;    /* Log2 of dimensions */
  pl_uInt iWidth, iHeight;   /* Integer dimensions */
  pl_Float uScale, vScale;   /* Scaling (usually 2**Width, 2**Height) */
  pl_uInt NumColors;         /* Number of colors used in texture */
} pl_Texture;

typedef struct _pl_Cam pl_Cam;
typedef struct _pl_Face pl_Face;

/*
** Material type. Create materials with plMatCreate().
*/
typedef struct _pl_Mat {
  pl_sInt Ambient[3];          /* RGB of surface (0-255 is a good range) */
  pl_sInt Diffuse[3];          /* RGB of diffuse (0-255 is a good range) */
  pl_sInt Specular[3];         /* RGB of "specular" highlights (0-255) */
  pl_uInt Shininess;           /* Shininess of material. 1 is dullest */
  pl_Float FadeDist;           /* For distance fading, distance at
                                  which intensity is 0 */
  pl_uChar ShadeType;          /* Shade type: PL_SHADE_* */
  pl_uChar Transparent;        /* Transparency index (0 = none), 4 = alot
                                  Note: transparencies disable textures */
  pl_uChar PerspectiveCorrect; /* Correct textures every n pixels */
  pl_Texture *Texture;         /* Texture map (see pl_Texture) above */
  pl_Texture *Environment;     /* Environment map (ditto) */
  pl_Float TexScaling;         /* Texture map scaling */
  pl_Float EnvScaling;         /* Environment map scaling */
  pl_uChar TexEnvMode;         /* TexEnv combining mode (PL_TEXENV_*) */
  pl_Bool zBufferable;         /* Can this material be zbuffered? */
  pl_uInt NumGradients;        /* Desired number of gradients to be used */
                 /* The following are used mostly internally */
  pl_uInt _ColorsUsed;         /* Number of colors actually used */
  pl_uChar _st, _ft;           /* The shadetype and filltype */
  pl_uInt _tsfact;             /* Translucent shading factor */
  pl_uInt16 *_AddTable;        /* Shading/Translucent/etc table */
  pl_uChar *_ReMapTable;       /* Table to remap colors to palette */
  pl_uChar *_RequestedColors;  /* _ColorsUsed colors, desired colors */
  void (*_PutFace)(pl_Cam *cam, pl_Face *TriFace); /* Function that renders the triangle with this material */
} pl_Mat;

/*
** Vertex, used within pl_Obj
*/
typedef struct _pl_Vertex {
  pl_Float x, y, z;              /* Vertex coordinate (objectspace) */
  pl_Float xformedx, xformedy, xformedz;
                                 /* Transformed vertex
                                    coordinate (cameraspace) */
  pl_Float nx, ny, nz;           /* Unit vertex normal (objectspace) */
  pl_Float xformednx, xformedny, xformednz;
                                 /* Transformed unit vertex normal
                                    (cameraspace) */
} pl_Vertex;

/*
** Face
*/
struct _pl_Face {
  pl_Vertex *Vertices[3];      /* Vertices of triangle */
  pl_Float nx, ny, nz;         /* Normal of triangle (object space) */
  pl_Mat *Material;            /* Material of triangle */
  pl_sInt32 Scrx[3], Scry[3];  /* Projected screen coordinates
                                  (12.20 fixed point) */
  pl_Float Scrz[3];            /* Projected 1/Z coordinates */
  pl_sInt32 MappingU[3], MappingV[3];
                               /* 16.16 Texture mapping coordinates */
  pl_sInt32 eMappingU[3], eMappingV[3];
                               /* 16.16 Environment map coordinates */
  pl_Float fShade;             /* Flat intensity */
  pl_Float sLighting;          /* Face static lighting. Should usually be 0.0 */
  pl_Float Shades[3];          /* Vertex intensity */
  pl_Float vsLighting[3];      /* Vertex static lighting. Should be 0.0 */
};

/*
** Object
*/
typedef struct _pl_Obj {
  pl_uInt32 NumVertices;              /* Number of vertices */
  pl_uInt32 NumFaces;                 /* Number of faces */
  pl_Vertex *Vertices;                /* Array of vertices */
  pl_Face *Faces;                     /* Array of faces */
  struct _pl_Obj *Children[PL_MAX_CHILDREN];
                                      /* Children */
  pl_Bool BackfaceCull;               /* Are backfacing polys drawn? */
  pl_Bool BackfaceIllumination;       /* Illuminated by lights behind them? */
  pl_Bool GenMatrix;                  /* Generate Matrix from the following
                                         if set */
  pl_Float Xp, Yp, Zp, Xa, Ya, Za;    /* Position and rotation of object:
                                         Note: rotations are around
                                         X then Y then Z. Measured in degrees */
  pl_Float Matrix[16];                /* Transformation matrix */
  pl_Float RotMatrix[16];             /* Rotation only matrix (for normals) */
} pl_Obj;

/*
** Spline type. See plSpline*().
*/
typedef struct _pl_Spline {
  pl_Float *keys;              /* Key data, keyWidth*numKeys */
  pl_sInt keyWidth;            /* Number of floats per key */
  pl_sInt numKeys;             /* Number of keys */
  pl_Float cont;               /* Continuity. Should be -1.0 -> 1.0 */
  pl_Float bias;               /* Bias. -1.0 -> 1.0 */
  pl_Float tens;               /* Tension. -1.0 -> 1.0 */
} pl_Spline;

/*
** Light type. See plLight*().
*/
typedef struct _pl_Light {
  pl_uChar Type;               /* Type of light: PL_LIGHT_* */
  pl_Float Xp, Yp, Zp;         /* If Type=PL_LIGHT_POINT*,
                                  this is Position (PL_LIGHT_POINT_*),
                                  otherwise if PL_LIGHT_VECTOR,
                                  Unit vector */
  pl_Float Intensity;           /* Intensity. 0.0 is off, 1.0 is full */
  pl_Float HalfDistSquared;     /* Distance squared at which
                                   PL_LIGHT_POINT_DISTANCE is 50% */
} pl_Light;

/*
** Camera Type.
*/
struct _pl_Cam {
  pl_Float Fov;                  /* FOV in degrees valid range is 1-179 */
  pl_Float AspectRatio;          /* Aspect ratio (usually 1.0) */
  pl_sChar Sort;                 /* Sort polygons, -1 f-t-b, 1 b-t-f, 0 no */
  pl_Float ClipBack;             /* Far clipping ( < 0.0 is none) */
  pl_sInt ClipTop, ClipLeft;     /* Screen Clipping */
  pl_sInt ClipBottom, ClipRight;
  pl_uInt ScreenWidth, ScreenHeight; /* Screen dimensions */
  pl_sInt CenterX, CenterY;      /* Center of screen */
  pl_Float X, Y, Z;              /* Camera position in worldspace */
  pl_Float Pitch, Pan, Roll;     /* Camera angle in degrees in worldspace */
  pl_uChar *frameBuffer;         /* Framebuffer (ScreenWidth*ScreenHeight) */
  pl_ZBuffer *zBuffer;           /* Z Buffer (NULL if none) */
};

#ifdef PLUSH_IMPLEMENTATION
PL_API pl_sChar plVersionString[] = "Plush 3D Version 1.2.0";
PL_API pl_sChar plCopyrightString[] = "Copyright (C) 1996-2000, Justin Frankel and Nullsoft";
PL_API pl_uChar plText_DefaultFont[256*16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 60, 66, 129, 231, 165, 153, 129, 153, 66, 60, 0, 0, 0, 0,
  0, 0, 60, 126, 255, 153, 219, 231, 255, 231, 126, 60, 0, 0, 0, 0,
  0, 0, 0, 0, 102, 255, 255, 255, 255, 126, 60, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 60, 126, 255, 126, 60, 24, 0, 0, 0, 0, 0,
  0, 0, 0, 24, 60, 60, 90, 255, 255, 90, 24, 60, 0, 0, 0, 0,
  0, 0, 0, 24, 60, 126, 255, 255, 255, 90, 24, 60, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 24, 60, 60, 24, 0, 0, 0, 0, 0, 0,
  255,255,255, 255, 255, 255, 231, 195, 195, 231, 255, 255, 255, 255, 255, 255,
  0, 0, 0, 0, 0, 60, 102, 66, 66, 102, 60, 0, 0, 0, 0, 0,
  255,255,255, 255, 255, 195, 153, 189, 189, 153, 195, 255, 255, 255, 255, 255,
  0, 0, 15, 7, 13, 24, 62, 99, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 126, 195, 195, 195, 126, 24, 24, 30, 120, 24, 0, 0, 0, 0,
  0, 0, 8, 12, 14, 11, 8, 8, 8, 120, 248, 112, 0, 0, 0, 0,
  0, 16, 24, 28, 22, 26, 22, 18, 114, 242, 98, 14, 30, 12, 0, 0,
  0, 0, 24, 24, 219, 126, 60, 255, 60, 126, 219, 24, 24, 0, 0, 0,
  0, 0, 96, 112, 120, 124, 126, 126, 124, 120, 112, 96, 0, 0, 0, 0,
  0, 0, 6, 14, 30, 62, 126, 126, 62, 30, 14, 6, 0, 0, 0, 0,
  0, 0, 16, 56, 124, 254, 56, 56, 56, 56, 254, 124, 56, 16, 0, 0,
  0, 0, 102, 102, 102, 102, 102, 102, 102, 0, 102, 102, 0, 0, 0, 0,
  0, 0, 63, 123, 219, 219, 219, 127, 59, 27, 27, 27, 0, 0, 0, 0,
  0, 31, 48, 120, 220, 206, 231, 115, 59, 30, 12, 24, 48, 224, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 0, 0, 0, 0,
  0, 0, 16, 56, 124, 254, 56, 56, 56, 254, 124, 56, 16, 0, 254, 0,
  0, 0, 16, 56, 124, 254, 56, 56, 56, 56, 56, 56, 56, 56, 0, 0,
  0, 0, 56, 56, 56, 56, 56, 56, 56, 56, 254, 124, 56, 16, 0, 0,
  0, 0, 0, 0, 0, 8, 12, 254, 255, 254, 12, 8, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 16, 48, 127, 255, 127, 48, 16, 0, 0, 0, 0,
  0, 0, 204, 102, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 36, 102, 255, 255, 102, 36, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 60, 60, 126, 126, 255, 255, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 255, 255, 126, 126, 60, 60, 24, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 24, 24, 24, 24, 24, 24, 24, 0, 24, 24, 0, 0, 0, 0,
  0, 0, 51, 102, 204, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 51, 51, 255, 102, 102, 102, 102, 255, 204, 204, 0, 0, 0, 0,
  0, 24, 126, 219, 216, 120, 28, 30, 27, 219, 219, 126, 24, 0, 0, 0,
  0, 0, 96, 209, 179, 102, 12, 24, 54, 109, 203, 6, 0, 0, 0, 0,
  0, 0, 28, 54, 102, 60, 56, 108, 199, 198, 110, 59, 0, 0, 0, 0,
  0, 0, 12, 24, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 12, 24, 48, 48, 96, 96, 96, 96, 48, 48, 24, 12, 0, 0, 0,
  0, 48, 24, 12, 12, 6, 6, 6, 6, 12, 12, 24, 48, 0, 0, 0,
  0, 0, 102, 60, 255, 60, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 24, 24, 126, 24, 24, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 24, 48, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 0,
  0, 0, 6, 4, 12, 8, 24, 16, 48, 32, 96, 64, 192, 0, 0, 0,
  0, 0, 62, 99, 195, 195, 195, 207, 219, 243, 198, 124, 0, 0, 0, 0,
  0, 0, 12, 28, 60, 108, 12, 12, 12, 12, 12, 12, 0, 0, 0, 0,
  0, 0, 62, 99, 195, 3, 6, 12, 24, 48, 99, 255, 0, 0, 0, 0,
  0, 0, 255, 198, 12, 24, 62, 3, 3, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 6, 14, 30, 54, 102, 199, 222, 246, 6, 6, 0, 0, 0, 0,
  0, 0, 31, 240, 192, 220, 246, 3, 3, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 60, 102, 198, 192, 220, 246, 198, 198, 204, 120, 0, 0, 0, 0,
  0, 0, 255, 195, 6, 12, 12, 24, 24, 48, 48, 48, 0, 0, 0, 0,
  0, 0, 60, 102, 198, 108, 62, 99, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 60, 102, 198, 198, 222, 118, 6, 198, 204, 120, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 24, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 24, 0, 0, 0, 24, 24, 48, 0, 0, 0, 0,
  0, 0, 6, 12, 24, 48, 96, 96, 48, 24, 12, 6, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 96, 48, 24, 12, 6, 6, 12, 24, 48, 96, 0, 0, 0, 0,
  0, 0, 62, 99, 198, 12, 24, 48, 48, 0, 48, 48, 0, 0, 0, 0,
  0, 0, 30, 51, 111, 219, 219, 219, 222, 216, 198, 220, 112, 0, 0, 0,
  0, 0, 24, 24, 60, 36, 102, 110, 122, 227, 195, 195, 0, 0, 0, 0,
  0, 0, 30, 51, 227, 198, 220, 247, 195, 198, 220, 240, 0, 0, 0, 0,
  0, 0, 30, 51, 96, 192, 192, 192, 192, 192, 99, 62, 0, 0, 0, 0,
  0, 0, 252, 198, 195, 195, 195, 195, 195, 198, 220, 240, 0, 0, 0, 0,
  0, 0, 30, 240, 192, 192, 220, 240, 192, 192, 222, 240, 0, 0, 0, 0,
  0, 0, 30, 240, 192, 192, 220, 240, 192, 192, 192, 192, 0, 0, 0, 0,
  0, 0, 62, 99, 192, 192, 192, 207, 195, 195, 102, 60, 0, 0, 0, 0,
  0, 0, 195, 195, 195, 195, 207, 251, 195, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 28, 56, 24, 24, 24, 24, 24, 24, 28, 56, 0, 0, 0, 0,
  0, 0, 3, 3, 3, 3, 3, 3, 195, 195, 99, 62, 0, 0, 0, 0,
  0, 0, 195, 198, 220, 240, 224, 240, 216, 204, 198, 195, 0, 0, 0, 0,
  0, 0, 192, 192, 192, 192, 192, 192, 192, 192, 222, 240, 0, 0, 0, 0,
  0, 0, 195, 195, 231, 239, 251, 211, 195, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 195, 195, 227, 243, 211, 219, 207, 199, 195, 195, 0, 0, 0, 0,
  0, 0, 62, 99, 195, 195, 195, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 30, 51, 227, 195, 198, 220, 240, 192, 192, 192, 0, 0, 0, 0,
  0, 0, 62, 99, 195, 195, 195, 195, 243, 222, 204, 124, 6, 3, 0, 0,
  0, 0, 30, 51, 227, 195, 198, 252, 216, 204, 198, 195, 0, 0, 0, 0,
  0, 0, 126, 195, 192, 112, 28, 6, 3, 195, 195, 126, 0, 0, 0, 0,
  0, 0, 15, 248, 24, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 0, 195, 195, 195, 195, 195, 195, 198, 198, 204, 120, 0, 0, 0, 0,
  0, 0, 195, 195, 195, 195, 102, 102, 124, 56, 48, 48, 0, 0, 0, 0,
  0, 0, 195, 195, 195, 195, 219, 219, 219, 255, 231, 195, 0, 0, 0, 0,
  0, 0, 195, 195, 102, 102, 60, 60, 102, 102, 195, 195, 0, 0, 0, 0,
  0, 0, 195, 195, 102, 102, 60, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 0, 31, 246, 4, 12, 24, 16, 48, 32, 111, 248, 0, 0, 0, 0,
  0, 62, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 62, 0, 0, 0,
  0, 0, 192, 64, 96, 32, 48, 16, 24, 8, 12, 4, 6, 0, 0, 0,
  0, 124, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 124, 0, 0, 0,
  0, 24, 60, 102, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0,
  0, 0, 48, 24, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  0, 0, 192, 192, 192, 220, 246, 195, 195, 198, 220, 240, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 30, 51, 96, 192, 192, 195, 126, 0, 0, 0, 0,
  0, 0, 3, 3, 3, 31, 115, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 62, 99, 206, 248, 192, 195, 126, 0, 0, 0, 0,
  0, 0, 30, 51, 48, 48, 60, 240, 48, 48, 48, 48, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 63, 99, 195, 199, 207, 219, 115, 3, 195, 126, 0,
  0, 0, 192, 192, 192, 206, 219, 243, 227, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 24, 24, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 0, 12, 12, 0, 12, 12, 12, 12, 12, 12, 12, 204, 204, 120, 0,
  0, 0, 192, 192, 192, 198, 204, 216, 248, 236, 198, 195, 0, 0, 0, 0,
  0, 0, 56, 24, 24, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 230, 219, 219, 219, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 206, 219, 243, 227, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 206, 219, 243, 227, 195, 198, 252, 192, 192, 192, 0,
  0, 0, 0, 0, 0, 115, 219, 207, 199, 195, 99, 63, 3, 3, 3, 0,
  0, 0, 0, 0, 0, 206, 219, 243, 224, 192, 192, 192, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 126, 195, 112, 30, 3, 195, 126, 0, 0, 0, 0,
  0, 0, 16, 48, 48, 60, 240, 48, 48, 54, 60, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 195, 195, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 195, 195, 195, 102, 108, 56, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 195, 195, 195, 219, 219, 255, 195, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 195, 102, 60, 24, 60, 102, 195, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 195, 195, 195, 195, 195, 102, 62, 12, 216, 112, 0,
  0, 0, 0, 0, 0, 255, 6, 12, 24, 48, 96, 255, 0, 0, 0, 0,
  0, 14, 24, 24, 24, 24, 112, 112, 24, 24, 24, 24, 14, 0, 0, 0,
  0, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0,
  0, 112, 24, 24, 24, 24, 14, 14, 24, 24, 24, 24, 112, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 118, 220, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 16, 56, 108, 198, 198, 198, 254, 0, 0, 0, 0, 0,
  0, 0, 30, 51, 96, 192, 192, 192, 192, 192, 99, 62, 12, 24, 240, 0,
  0, 0, 102, 102, 0, 195, 195, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 6, 12, 24, 0, 62, 99, 206, 248, 192, 195, 126, 0, 0, 0, 0,
  12, 30, 51, 96, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  0, 0, 54, 54, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  0, 48, 24, 12, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  28, 54, 54, 28, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 30, 51, 96, 192, 192, 195, 126, 12, 24, 240, 0,
  24, 60, 102, 192, 0, 62, 99, 206, 248, 192, 195, 126, 0, 0, 0, 0,
  0, 0, 102, 102, 0, 62, 99, 206, 248, 192, 195, 126, 0, 0, 0, 0,
  0, 96, 48, 24, 0, 62, 99, 206, 248, 192, 195, 126, 0, 0, 0, 0,
  0, 0, 102, 102, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  24, 60, 102, 192, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 96, 48, 24, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  102, 102, 24, 24, 60, 36, 102, 102, 126, 195, 195, 195, 0, 0, 0, 0,
  24, 36, 36, 24, 60, 36, 102, 102, 126, 195, 195, 195, 0, 0, 0, 0,
  24, 48, 96, 30, 240, 192, 222, 240, 192, 192, 222, 240, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 126, 219, 59, 126, 220, 219, 110, 0, 0, 0, 0,
  0, 0, 63, 62, 60, 108, 111, 110, 124, 204, 207, 206, 0, 0, 0, 0,
  24, 60, 102, 192, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 0, 102, 102, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 96, 48, 24, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  24, 60, 102, 192, 0, 195, 195, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 96, 48, 24, 0, 195, 195, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 0, 102, 102, 0, 195, 195, 195, 195, 195, 102, 62, 12, 216, 112, 0,
  102, 0, 62, 99, 195, 195, 195, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  102, 0, 195, 195, 195, 195, 195, 195, 198, 198, 204, 120, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 30, 59, 104, 200, 200, 203, 126, 8, 8, 0, 0,
  0, 0, 60, 102, 96, 248, 96, 248, 96, 96, 99, 126, 0, 0, 0, 0,
  0, 0, 195, 195, 102, 102, 60, 24, 126, 24, 126, 24, 24, 0, 0, 0,
  0, 0, 30, 51, 227, 195, 198, 220, 248, 204, 222, 204, 15, 14, 4, 0,
  0, 0, 30, 51, 48, 48, 60, 240, 48, 48, 48, 48, 224, 0, 0, 0,
  0, 6, 12, 24, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 0, 0,
  0, 6, 12, 24, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 6, 12, 24, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 0, 0,
  0, 12, 24, 48, 0, 195, 195, 195, 199, 207, 219, 115, 0, 0, 0, 0,
  0, 3, 118, 220, 0, 206, 219, 243, 227, 195, 195, 195, 0, 0, 0, 0,
  3, 118, 220, 0, 195, 227, 243, 211, 219, 207, 199, 195, 0, 0, 0, 0,
  0, 0, 62, 99, 31, 115, 195, 207, 123, 0, 0, 255, 0, 0, 0, 0,
  0, 0, 62, 99, 195, 195, 195, 198, 124, 0, 0, 255, 0, 0, 0, 0,
  0, 0, 12, 12, 0, 12, 12, 24, 48, 99, 198, 124, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 254, 192, 192, 192, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 254, 6, 6, 6, 0, 0, 0, 0, 0, 0,
  0, 96, 225, 99, 102, 108, 24, 48, 96, 206, 155, 6, 13, 31, 0, 0,
  0, 96, 225, 99, 102, 108, 24, 48, 102, 206, 151, 62, 6, 6, 0, 0,
  0, 0, 24, 24, 0, 24, 24, 24, 24, 24, 24, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 3, 54, 108, 216, 216, 108, 54, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 108, 54, 27, 27, 54, 108, 192, 0, 0, 0, 0,
  130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16,
  0, 149, 0, 169, 0, 149, 0, 169, 0, 149, 0, 169, 0, 149, 0, 169,
  146, 73, 146, 73, 146, 73, 146, 73, 146, 73, 146, 73, 146, 73, 146, 73,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 248, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 248, 248, 248, 24, 24, 24, 24, 24, 24, 24,
  60, 60, 60, 60, 60, 60, 60, 60, 252, 60, 60, 60, 60, 60, 60, 60,
  0, 0, 0, 0, 0, 0, 0, 0, 252, 60, 60, 60, 60, 60, 60, 60,
  0, 0, 0, 0, 0, 0, 248, 248, 248, 24, 24, 24, 24, 24, 24, 24,
  60, 60, 60, 60, 60, 60, 252, 252, 252, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
  0, 0, 0, 0, 0, 0, 252, 252, 252, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 252, 252, 252, 0, 0, 0, 0, 0, 0, 0,
  60, 60, 60, 60, 60, 60, 60, 60, 252, 0, 0, 0, 0, 0, 0, 0,
  24, 24, 24, 24, 24, 24, 248, 248, 248, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 248, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 31, 0, 0, 0, 0, 0, 0, 0,
  24, 24, 24, 24, 24, 24, 24, 24, 255, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 255, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 31, 24, 24, 24, 24, 24, 24, 24,
  0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0,
  24, 24, 24, 24, 24, 24, 24, 24, 255, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 31, 31, 31, 24, 24, 24, 24, 24, 24, 24,
  60, 60, 60, 60, 60, 60, 60, 60, 63, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 63, 63, 63, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 255, 255, 255, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 63, 63, 63, 60, 60, 60, 60, 60, 60, 60,
  0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0,
  60, 60, 60, 60, 60, 60, 255, 255, 255, 60, 60, 60, 60, 60, 60, 60,
  24, 24, 24, 24, 24, 24, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0,
  60, 60, 60, 60, 60, 60, 60, 60, 255, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 255, 255, 255, 24, 24, 24, 24, 24, 24, 24,
  0, 0, 0, 0, 0, 0, 0, 0, 255, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 60, 60, 63, 0, 0, 0, 0, 0, 0, 0,
  24, 24, 24, 24, 24, 24, 31, 31, 31, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 31, 31, 31, 24, 24, 24, 24, 24, 24, 24,
  0, 0, 0, 0, 0, 0, 0, 0, 63, 60, 60, 60, 60, 60, 60, 60,
  60, 60, 60, 60, 60, 60, 60, 60, 255, 60, 60, 60, 60, 60, 60, 60,
  24, 24, 24, 24, 24, 24, 255, 255, 255, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 248, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 31, 24, 24, 24, 24, 24, 24, 24,
  255, 255, 255,255,255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255,
  240, 240, 240, 240,240,240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 118, 204, 204, 204, 222, 115, 0, 0, 0, 0,
  0, 0, 30, 51, 227, 194, 204, 194, 195, 195, 206, 216, 192, 192, 0, 0,
  0, 0, 31, 243, 195, 192, 192, 192, 192, 192, 192, 192, 0, 0, 0, 0,
  0, 0, 0, 0, 3, 126, 230, 102, 102, 102, 102, 68, 0, 0, 0, 0,
  0, 0, 31, 240, 96, 48, 24, 48, 96, 192, 223, 240, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 127, 240, 216, 216, 216, 216, 112, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 99, 99, 99, 99, 103, 111, 123, 96, 96, 192, 0,
  0, 0, 0, 0, 111, 184, 48, 48, 48, 48, 48, 48, 0, 0, 0, 0,
  0, 0, 24, 24, 126, 219, 219, 219, 219, 126, 24, 24, 0, 0, 0, 0,
  0, 0, 0, 60, 102, 195, 195, 255, 195, 195, 102, 60, 0, 0, 0, 0,
  0, 0, 60, 102, 195, 195, 195, 195, 102, 36, 165, 231, 0, 0, 0, 0,
  0, 7, 28, 48, 24, 12, 62, 102, 198, 198, 204, 120, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 110, 219, 219, 219, 118, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 2, 4, 124, 206, 214, 230, 124, 64, 128, 0, 0, 0, 0,
  0, 0, 30, 48, 96, 96, 126, 96, 96, 96, 48, 30, 0, 0, 0, 0,
  0, 0, 0, 126, 195, 195, 195, 195, 195, 195, 195, 195, 0, 0, 0, 0,
  0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 24, 126, 24, 24, 0, 0, 126, 0, 0, 0, 0,
  0, 0, 0, 48, 24, 12, 6, 12, 24, 48, 0, 126, 0, 0, 0, 0,
  0, 0, 0, 12, 24, 48, 96, 48, 24, 12, 0, 126, 0, 0, 0, 0,
  0, 0, 0, 14, 27, 27, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 216, 216, 112, 0, 0, 0, 0,
  0, 0, 0, 0, 24, 24, 0, 255, 0, 24, 24, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 118, 220, 0, 118, 220, 0, 0, 0, 0, 0, 0,
  0, 0, 60, 102, 102, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 3, 2, 6, 4, 12, 8, 216, 80, 112, 32, 0, 0, 0, 0,
  0, 0, 220, 246, 230, 198, 198, 198, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 120, 204, 24, 48, 100, 252, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
PL_API pl_uInt32 plRender_TriStats[4];
#else
extern pl_sChar plVersionString[];      /* Version string */
extern pl_sChar plCopyrightString[];    /* Copyright string */

extern pl_uChar plText_DefaultFont[256*16]; /* Default 8x16 font for plText* */
extern pl_uInt32 plRender_TriStats[4]; /* Three different triangle counts from
                                          the last plRender() block:
                                          0: initial tris
                                          1: tris after culling
                                          2: final polys after real clipping
                                          3: final tris after tesselation
                                       */
#endif

/******************************************************************************
** Material Functions (mat.c)
******************************************************************************/

/*
  plMatCreate() creates a material.
  Parameters:
    none
  Returns:
    a pointer to the material on success, 0 on failure
*/
PL_API pl_Mat *plMatCreate();

/*
  plMatDelete() deletes a material that was created with plMatCreate().
  Parameters:
    m: a pointer to the material to be deleted
  Returns:
    nothing
*/
PL_API void plMatDelete(pl_Mat *m);

/*
  plMatInit() initializes a material that was created with plMatCreate().
  Parameters:
    m: a pointer to the material to be intialized
  Returns:
    nothing
  Notes:
    you *must* do this before calling plMatMapToPal() or plMatMakeOptPal().
*/
PL_API void plMatInit(pl_Mat *m);

/*
  plMatMapToPal() maps a material that was created with plMatCreate() and
    initialized with plMatInit() to a palette.
  Parameters:
    mat: material to map
    pal: a 768 byte array of unsigned chars, each 3 being a rgb triplet
         (0-255, *not* the cheesy vga 0-63)
    pstart: starting offset to use colors of, usually 0
    pend: ending offset to use colors of, usually 255
  Returns:
    nothing
  Notes:
    Mapping a material with > 2000 colors can take up to a second or two.
      Be careful, and go easy on plMat.NumGradients ;)
*/
PL_API void plMatMapToPal(pl_Mat *m, pl_uChar *pal, pl_sInt pstart, pl_sInt pend);


/*
  plMatMakeOptPal() makes an almost optimal palette from materials
    created with plMatCreate() and initialized with plMatInit().
  Paramters:
    p: palette to create
    pstart: first color entry to use
    pend: last color entry to use
    materials: an array of pointers to materials to generate the palette from
    nmats: number of materials
  Returns:
    nothing
*/
PL_API void plMatMakeOptPal(pl_uChar *p, pl_sInt pstart,
                     pl_sInt pend, pl_Mat **materials, pl_sInt nmats);


/******************************************************************************
** Object Functions (obj.c)
******************************************************************************/

/*
  plObjCreate() allocates an object
  Paramters:
    np: Number of vertices in object
    nf: Number of faces in object
  Returns:
    a pointer to the object on success, 0 on failure
*/
PL_API pl_Obj *plObjCreate(pl_uInt32 np, pl_uInt32 nf);

/*
  plObjDelete() frees an object and all of it's subobjects
    that was allocated with plObjCreate();
  Paramters:
    o: object to delete
  Returns:
    nothing
*/
PL_API void plObjDelete(pl_Obj *o);

/*
  plObjClone() creates an exact but independent duplicate of an object and
    all of it's subobjects
  Paramters:
    o: the object to clone
  Returns:
    a pointer to the new object on success, 0 on failure
*/
PL_API pl_Obj *plObjClone(pl_Obj *o);

/*
  plObjScale() scales an object, and all of it's subobjects.
  Paramters:
    o: a pointer to the object to scale
    s: the scaling factor
  Returns:
    a pointer to o.
  Notes: This scales it slowly, by going through each vertex and scaling it's
    position. Avoid doing this in realtime.
*/
PL_API pl_Obj *plObjScale(pl_Obj *o, pl_Float s);

/*
  plObjStretch() stretches an object, and all of it's subobjects
  Parameters:
    o: a pointer to the object to stretch
    x,y,z: the x y and z stretch factors
  Returns:
    a pointer to o.
  Notes: same as plObjScale(). Note that the normals are preserved.
*/
PL_API pl_Obj *plObjStretch(pl_Obj *o, pl_Float x, pl_Float y, pl_Float z);

/*
   plObjTranslate() translates an object
   Parameters:
     o: a pointer to the object to translate
     x,y,z: translation in object space
   Returns:
     a pointer to o
   Notes: same has plObjScale().
*/
PL_API pl_Obj *plObjTranslate(pl_Obj *o, pl_Float x, pl_Float y, pl_Float z);

/*
  plObjFlipNormals() flips all vertex and face normals of and object
    and allo of it's subobjects.
  Parameters:
    o: a pointer to the object to flip normals of
  Returns:
    a pointer to o
  Notes:
    Not especially fast.
    A call to plObjFlipNormals() or plObjCalcNormals() will restore the normals
*/
PL_API pl_Obj *plObjFlipNormals(pl_Obj *o);

/*
  plObjSetMat() sets the material of all faces in an object.
  Paramters:
    o: the object to set the material of
    m: the material to set it to
    th: "transcend hierarchy". If set, it will set the
        material of all subobjects too.
  Returns:
    nothing
*/
PL_API void plObjSetMat(pl_Obj *o, pl_Mat *m, pl_Bool th);

/*
   plObjCalcNormals() calculates all face and vertex normals for an object
     and all subobjects.
   Paramters:
     obj: the object
   Returns:
     nothing
*/
PL_API void plObjCalcNormals(pl_Obj *obj);

/******************************************************************************
** Frustum Clipping Functions (clip.c)
******************************************************************************/

/*
  plClipSetFrustum() sets up the clipping frustum.
  Parameters:
    cam: a camera allocated with plCamCreate().
  Returns:
    nothing
  Notes:
    Sets up the internal structures.
    DO NOT CALL THIS ROUTINE FROM WITHIN A plRender*() block.
*/
PL_API void plClipSetFrustum(pl_Cam *cam);

/*
  plClipRenderFace() renders a face and clips it to the frustum initialized
    with plClipSetFrustum().
  Parameters:
    face: the face to render
  Returns:
    nothing
  Notes: this is used internally by plRender*(), so be careful. Kinda slow too.
*/
PL_API void plClipRenderFace(pl_Face *face);

/*
  plClipNeeded() decides whether the face is in the frustum, intersecting
    the frustum, or completely out of the frustum craeted with
    plClipSetFrustum().
  Parameters:
    face: the face to check
  Returns:
    0: the face is out of the frustum, no drawing necessary
    1: the face is intersecting the frustum, splitting and drawing necessary
  Notes: this is used internally by plRender*(), so be careful. Kinda slow too.
*/
PL_API pl_sInt plClipNeeded(pl_Face *face);

/******************************************************************************
** Light Handling Routines (light.c)
******************************************************************************/

/*
  plLightCreate() creates a new light
  Parameters:
    none
  Returns:
    a pointer to the light
*/
PL_API pl_Light *plLightCreate();

/*
  plLightSet() sets up a light allocated with plLightCreate()
  Parameters:
    light: the light to set up
    mode: the mode of the light (PL_LIGHT_*)
    x,y,z: either the position of the light (PL_LIGHT_POINT*) or the angle
           in degrees of the light (PL_LIGHT_VECTOR)
    intensity: the intensity of the light (0.0-1.0)
    halfDist: the distance at which PL_LIGHT_POINT_DISTANCE is 1/2 intensity
  Returns:
    a pointer to light.
*/
PL_API pl_Light *plLightSet(pl_Light *light, pl_uChar mode, pl_Float x, pl_Float y,
                     pl_Float z, pl_Float intensity, pl_Float halfDist);

/*
  plLightDelete() frees a light allocated with plLightCreate().
  Paramters:
    l: light to delete
  Returns:
    nothing
*/
PL_API void plLightDelete(pl_Light *l);

/* PUT ME SOMEWHERE */
/*
** plTexDelete() frees all memory associated with "t"
*/
PL_API void plTexDelete(pl_Texture *t);


/******************************************************************************
** Camera Handling Routines (cam.c)
******************************************************************************/

/*
  plCamCreate() allocates a new camera
  Parameters:
    sw: screen width
    sh: screen height
    ar: aspect ratio (usually 1.0)
    fov: field of view (usually 45-120)
    fb: pointer to framebuffer
    zb: pointer to Z buffer (or NULL)
  Returns:
    a pointer to the newly allocated camera
*/
PL_API pl_Cam *plCamCreate(pl_uInt sw, pl_uInt sh, pl_Float ar, pl_Float fov,
                    pl_uChar *fb, pl_ZBuffer *zb);

/*
  plCamSetTarget() sets the target of a camera allocated with plCamCreate().
  Parameters:
    c: the camera to set the target of
    x,y,z: the worldspace coordinate of the target
  Returns:
    nothing
  Notes:
    Sets the pitch and pan of the camera. Does not touch the roll.
*/
PL_API void plCamSetTarget(pl_Cam *c, pl_Float x, pl_Float y, pl_Float z);

/*
   plCamDelete() frees all memory associated with a camera excluding
     framebuffers and Z buffers
   Paramters:
     c: camera to free
   Returns:
     nothing
*/
PL_API void plCamDelete(pl_Cam *c);

/******************************************************************************
** Easy Rendering Interface (render.c)
******************************************************************************/

/*
 plRenderBegin() begins the rendering process.
   Parameters:
     Camera: camera to use for rendering
   Returns:
     nothing
   Notes:
     Only one rendering process can occur at a time.
     Uses plClip*(), so don't use them within or around a plRender() block.
*/
PL_API void plRenderBegin(pl_Cam *Camera);

/*
   plRenderLight() adds a light to the scene.
   Parameters:
     light: light to add to scene
   Returns:
     nothing
   Notes: Any objects rendered before will be unaffected by this.
*/
PL_API void plRenderLight(pl_Light *light);

/*
   plRenderObj() adds an object and all of it's subobjects to the scene.
   Parameters:
     obj: object to render
   Returns:
     nothing
   Notes: if Camera->Sort is zero, objects are rendered in the order that
     they are added to the scene.
*/
PL_API void plRenderObj(pl_Obj *obj);

/*
   plRenderEnd() actually does the rendering, and closes the rendering process
   Paramters:
     none
   Returns:
     nothing
*/
PL_API void plRenderEnd();

/******************************************************************************
** Object Primitives Code (make.c)
******************************************************************************/

/*
  plMakePlane() makes a plane centered at the origin facing up the y axis.
  Parameters:
    w: width of the plane (along the x axis)
    d: depth of the plane (along the z axis)
    res: resolution of plane, i.e. subdivisions
    m: material to use
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakePlane(pl_Float w, pl_Float d, pl_uInt res, pl_Mat *m);

/*
  plMakeBox() makes a box centered at the origin
  Parameters:
    w: width of the box (x axis)
    d: depth of the box (z axis)
    h: height of the box (y axis)
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakeBox(pl_Float w, pl_Float d, pl_Float h, pl_Mat *m);

/*
  plMakeCone() makes a cone centered at the origin
  Parameters:
    r: radius of the cone (x-z axis)
    h: height of the cone (y axis)
    div: division of cone (>=3)
    cap: close the big end?
    m: material to use
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakeCone(pl_Float r, pl_Float h, pl_uInt div, pl_Bool cap, pl_Mat *m);

/*
  plMakeCylinder() makes a cylinder centered at the origin
  Parameters:
    r: radius of the cylinder (x-z axis)
    h: height of the cylinder (y axis)
    divr: division of of cylinder (around the circle) (>=3)
    captop: close the top
    capbottom: close the bottom
    m: material to use
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakeCylinder(pl_Float r, pl_Float h, pl_uInt divr, pl_Bool captop,
                       pl_Bool capbottom, pl_Mat *m);

/*
  plMakeSphere() makes a sphere centered at the origin.
  Parameters:
    r: radius of the sphere
    divr: division of the sphere (around the y axis) (>=3)
    divh: division of the sphere (around the x,z axis) (>=3)
    m: material to use
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakeSphere(pl_Float r, pl_uInt divr, pl_uInt divh, pl_Mat *m);

/*
  plMakeTorus() makes a torus centered at the origin
  Parameters:
    r1: inner radius of the torus
    r2: outer radius of the torus
    divrot: division of the torus (around the y axis) (>=3)
    divrad: division of the radius of the torus (x>=3)
    m: material to use
  Returns:
    pointer to object created.
*/
PL_API pl_Obj *plMakeTorus(pl_Float r1, pl_Float r2, pl_uInt divrot,
                    pl_uInt divrad, pl_Mat *m);

/******************************************************************************
** File Readers (read_*.c)
******************************************************************************/

/*
  plRead3DSObj() reads a 3DS object
  Parameters:
    fn: filename of object to read
    m: material to assign it
  Returns:
    pointer to object
  Notes:
    This reader organizes multiple objects like so:
      1) the first object is returned
      2) the second object is the first's first child
      3) the third object is the second's first child
      4) etc
*/
PL_API pl_Obj *plRead3DSObj(char *fn, pl_Mat *m);

/*
  plReadCOBObj() reads an ascii .COB object
  Parameters:
    fn: filename of object to read
    mat: material to assign it
  Returns:
    pointer to object
  Notes:
    This is Caligari's ASCII object format.
    This reader doesn't handle multiple objects. It just reads the first one.
    Polygons with lots of sides are not always tesselated correctly. Just
      use the "Tesselate" button from within truespace to improve the results.
*/
PL_API pl_Obj *plReadCOBObj(char *fn, pl_Mat *mat);

/*
  plReadJAWObj() reads a .JAW object.
  Parameters:
    fn: filename of object to read
    m: material to assign it
  Returns:
    pointer to object
  Notes:
    For information on the .JAW format, please see the jaw3D homepage,
      http://www.tc.umn.edu/nlhome/g346/kari0022/jaw3d/
*/
PL_API pl_Obj *plReadJAWObj(char *fn, pl_Mat *m);

/*
  plReadPCXTex() reads a 8bpp PCX texture
  Parameters:
    fn: filename of texture to read
    rescale: will rescale image if not whole log2 dimensions (USE THIS)
    optimize: will optimize colors (USE THIS TOO)
  Returns:
    pointer to texture
  Notes:
    The PCX must be a 8bpp zSoft version 5 PCX. The texture's palette will
      be optimized, and the texture might be scaled up so that it's dimensions
      will be a nice power of two.
*/
PL_API pl_Texture *plReadPCXTex(char *fn, pl_Bool rescale, pl_Bool optimize);

/******************************************************************************
** Math Code (math.c)
******************************************************************************/

/*
  plMatrixRotate() generates a rotation matrix
  Parameters:
    matrix: an array of 16 pl_Floats that is a 4x4 matrix
    m: the axis to rotate around, 1=X, 2=Y, 3=Z.
    Deg: the angle in degrees to rotate
  Returns:
    nothing
*/
PL_API void plMatrixRotate(pl_Float matrix[], pl_uChar m, pl_Float Deg);

/*
  plMatrixTranslate() generates a translation matrix
  Parameters:
    m: the matrix (see plMatrixRotate for more info)
    x,y,z: the translation coordinates
  Returns:
    nothing
*/
PL_API void plMatrixTranslate(pl_Float m[], pl_Float x, pl_Float y, pl_Float z);

/*
  plMatrixMultiply() multiplies two matrices
  Parameters:
    dest: destination matrix will be multipled by src
    src: source matrix
  Returns:
    nothing
  Notes:
    this is the same as dest = dest*src (since the order *does* matter);
*/
PL_API void plMatrixMultiply(pl_Float *dest, pl_Float src[]);

/*
   plMatrixApply() applies a matrix.
  Parameters:
    m: matrix to apply
    x,y,z: input coordinate
    outx,outy,outz: pointers to output coords.
  Returns:
    nothing
  Notes:
    applies the matrix to the 3d point to produce the transformed 3d point
*/
PL_API void plMatrixApply(pl_Float *m, pl_Float x, pl_Float y, pl_Float z,
                   pl_Float *outx, pl_Float *outy, pl_Float *outz);

/*
  plNormalizeVector() makes a vector a unit vector
  Parameters:
    x,y,z: pointers to the vector
  Returns:
    nothing
*/
PL_API void plNormalizeVector(pl_Float *x, pl_Float *y, pl_Float *z);

/*
  plDotProduct() returns the dot product of two vectors
  Parameters:
    x1,y1,z1: the first vector
    x2,y2,z2: the second vector
  Returns:
    the dot product of the two vectors
*/
PL_API pl_Float plDotProduct(pl_Float x1, pl_Float y1, pl_Float z1,
                      pl_Float x2, pl_Float y2, pl_Float z2);

/******************************************************************************
** Spline Interpolation (spline.c)
******************************************************************************/

/*
  plSplineInit() initializes a spline
  Parameters:
    s: the spline
  Returns:
    nothing
  Notes:
    Intializes the spline. Do this once, or when you change any of the settings
*/
PL_API void plSplineInit(pl_Spline *s);

/*
  plSplineGetPoint() gets a point on the spline
  Parameters:
    s: spline
    frame: time into spline. 0.0 is start, 1.0 is second key point, etc.
    out: a pointer to an array of s->keyWidth floats that will be filled in.
  Returns:
    nothing
*/
PL_API void plSplineGetPoint(pl_Spline *s, pl_Float frame, pl_Float *out);

/******************************************************************************
** 8xX  Bitmapped Text
******************************************************************************/
/*
  plTextSetFont() sets the font to be used by the plText*() functions.
    Parameters:
      font: a pointer to a 8xX bitmapped font
      height: the height of the font (X)
    Returns:
      nothing
*/

PL_API void plTextSetFont(pl_uChar *font, pl_uChar height);

/*
  plTextPutChar() puts a character to a camera
  Parameters:
    cam: The camera. If the camera has a zBuffer, it will be used.
    x: the x screen position of the left of the text
    y: the y screen position of the top of the text
    z: the depth of the text (used when cam->zBuffer is set)
    color: the color to make the text
    c: the character to put. Special characters such as '\n' aren't handled.
  Returns:
    nothing
*/

PL_API void plTextPutChar(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                   pl_uChar color, pl_uChar c);

/*
  plTextPutString() puts an array of characters to a camera
  Parameters:
    cam: The camera. If the camera has a zBuffer, it will be used.
    x: the x screen position of the left of the text
    y: the y screen position of the top of the text
    z: the depth of the text (used when cam->zBuffer is set)
    color: the color to make the text
    string:
      the characters to put. '\n' and '\t' are handled as one would expect
  Returns:
    nothing
*/
PL_API void plTextPutStr(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                  pl_uChar color, pl_sChar *string);

/*
  plTextPrintf() is printf() for graphics
  Parameters:
    cam: The camera. If the camera has a zBuffer, it will be used.
    x: the x screen position of the left of the text
    y: the y screen position of the top of the text
    z: the depth of the text (used when cam->zBuffer is set)
    color: the color to make the text
    format:
      the characters to put, with printf() formatting codes.
      '\n' and '\t' are handled as one would expect
    ...: any additional parameters specified by format
  Returns:
    nothing
*/
PL_API void plTextPrintf(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                  pl_uChar color, pl_sChar *format, ...);

/******************************************************************************
** Built-in Rasterizers
******************************************************************************/

PL_API void plPF_SolidF(pl_Cam *, pl_Face *);
PL_API void plPF_SolidG(pl_Cam *, pl_Face *);
PL_API void plPF_TexF(pl_Cam *, pl_Face *);
PL_API void plPF_TexG(pl_Cam *, pl_Face *);
PL_API void plPF_TexEnv(pl_Cam *, pl_Face *);
PL_API void plPF_PTexF(pl_Cam *, pl_Face *);
PL_API void plPF_PTexG(pl_Cam *, pl_Face *);
PL_API void plPF_TransF(pl_Cam *, pl_Face *);
PL_API void plPF_TransG(pl_Cam *, pl_Face *);

#ifdef __cplusplus
}
#endif

#ifdef PLUSH_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PL_API void plCamDelete(pl_Cam *c) {
  if (c) free(c);
}

PL_API void plCamSetTarget(pl_Cam *c, pl_Float x, pl_Float y, pl_Float z) {
  double dx, dy, dz;
  dx = x - c->X;
  dy = y - c->Y;
  dz = z - c->Z;
  c->Roll = 0;
  if (dz > 0.0001f) {
    c->Pan = (pl_Float) (-atan(dx/dz)*(180.0/PL_PI));
    dz /= cos(c->Pan*(PL_PI/180.0));
    c->Pitch = (pl_Float) (atan(dy/dz)*(180.0/PL_PI));
  } else if (dz < -0.0001f) {
    c->Pan = (pl_Float) (180.0-atan(dx/dz)*(180.0/PL_PI));
    dz /= cos((c->Pan-180.0f)*(PL_PI/180.0));
    c->Pitch = (pl_Float) (-atan(dy/dz)*(180.0/PL_PI));
  } else {
    c->Pan = 0.0f;
    c->Pitch = -90.0f;
  }
}

PL_API pl_Cam *plCamCreate(pl_uInt sw, pl_uInt sh, pl_Float ar, pl_Float fov,
                    pl_uChar *fb, pl_ZBuffer *zb) {
  pl_Cam *c;
  c = malloc(sizeof(pl_Cam));
  if (!c) return 0;
  memset(c,0,sizeof(pl_Cam));
  c->Fov = fov;
  c->AspectRatio = ar;
  c->ClipRight = c->ScreenWidth = sw;
  c->ClipBottom = c->ScreenHeight = sh;
  c->CenterX = sw>>1;
  c->CenterY = sh>>1;
  c->ClipBack = 8.0e30f;
  c->frameBuffer = fb;
  c->zBuffer = zb;
  c->Sort = 1;
  if (zb) c->Sort = 0;
  return (c);
}

typedef struct
{
  pl_Vertex newVertices[8];
  double Shades[8];
  double MappingU[8];
  double MappingV[8];
  double eMappingU[8];
  double eMappingV[8];
} _clipInfo;

static _clipInfo m_cl[2];

static double m_clipPlanes[NUM_CLIP_PLANES][4];
static pl_Cam *m_cam;
static pl_sInt32 m_cx, m_cy;
static double m_fov;
static double m_adj_asp;

static void _FindNormal(double x2, double x3,
                        double y2, double y3,
                        double zv,
                        double *res);

 /* Returns: 0 if nothing gets in,  1 or 2 if pout1 & pout2 get in */
static pl_uInt _ClipToPlane(pl_uInt numVerts, double *plane);

PL_API void plClipSetFrustum(pl_Cam *cam) {
  m_adj_asp = 1.0 / cam->AspectRatio;
  m_fov = plMin(plMax(cam->Fov,1.0),179.0);
  m_fov = (1.0/tan(m_fov*(PL_PI/360.0)))*(double) (cam->ClipRight-cam->ClipLeft);
  m_cx = cam->CenterX<<20;
  m_cy = cam->CenterY<<20;
  m_cam = cam;
  memset(m_clipPlanes,0,sizeof(m_clipPlanes));

  /* Back */
  m_clipPlanes[0][2] = -1.0;
  m_clipPlanes[0][3] = -cam->ClipBack;

  /* Left */
  m_clipPlanes[1][3] = 0.00000001;
  if (cam->ClipLeft == cam->CenterX) {
    m_clipPlanes[1][0] = 1.0;
  }
  else _FindNormal(-100,-100,
                100, -100,
                m_fov*-100.0/(cam->ClipLeft-cam->CenterX),
                m_clipPlanes[1]);
  if (cam->ClipLeft > cam->CenterX) {
    m_clipPlanes[1][0] = -m_clipPlanes[1][0];
    m_clipPlanes[1][1] = -m_clipPlanes[1][1];
    m_clipPlanes[1][2] = -m_clipPlanes[1][2];
  }

  /* Right */
  m_clipPlanes[2][3] = 0.00000001;
  if (cam->ClipRight == cam->CenterX) {
    m_clipPlanes[2][0] = -1.0;
  }
  else _FindNormal(100,100,
                -100, 100,
                m_fov*100.0/(cam->ClipRight-cam->CenterX),
                m_clipPlanes[2]);
  if (cam->ClipRight < cam->CenterX) {
    m_clipPlanes[2][0] = -m_clipPlanes[2][0];
    m_clipPlanes[2][1] = -m_clipPlanes[2][1];
    m_clipPlanes[2][2] = -m_clipPlanes[2][2];
  }
  /* Top */
  m_clipPlanes[3][3] = 0.00000001;
  if (cam->ClipTop == cam->CenterY) {
    m_clipPlanes[3][1] = -1.0;
  } else _FindNormal(100, -100,
                100, 100,
                m_fov*m_adj_asp*100.0/(cam->CenterY-cam->ClipTop),
                m_clipPlanes[3]);
  if (cam->ClipTop > cam->CenterY) {
    m_clipPlanes[3][0] = -m_clipPlanes[3][0];
    m_clipPlanes[3][1] = -m_clipPlanes[3][1];
    m_clipPlanes[3][2] = -m_clipPlanes[3][2];
  }

  /* Bottom */
  m_clipPlanes[4][3] = 0.00000001;
  if (cam->ClipBottom == cam->CenterY) {
    m_clipPlanes[4][1] = 1.0;
  } else _FindNormal(-100, 100,
                -100, -100,
                m_fov*m_adj_asp*-100.0/(cam->CenterY-cam->ClipBottom),
                m_clipPlanes[4]);
  if (cam->ClipBottom < cam->CenterY) {
    m_clipPlanes[4][0] = -m_clipPlanes[4][0];
    m_clipPlanes[4][1] = -m_clipPlanes[4][1];
    m_clipPlanes[4][2] = -m_clipPlanes[4][2];
  }
}

PL_API void plClipRenderFace(pl_Face *face) {
  pl_uInt k, a, w, numVerts, q;
  double tmp, tmp2;
  pl_Face newface;

  for (a = 0; a < 3; a ++) {
    m_cl[0].newVertices[a] = *(face->Vertices[a]);
    m_cl[0].Shades[a] = face->Shades[a];
    m_cl[0].MappingU[a] = face->MappingU[a];
    m_cl[0].MappingV[a] = face->MappingV[a];
    m_cl[0].eMappingU[a] = face->eMappingU[a];
    m_cl[0].eMappingV[a] = face->eMappingV[a];
  }

  numVerts = 3;
  q = 0;
  a = (m_clipPlanes[0][3] < 0.0 ? 0 : 1);
  while (a < NUM_CLIP_PLANES && numVerts > 2)
  {
    numVerts = _ClipToPlane(numVerts, m_clipPlanes[a]);
    memcpy(&m_cl[0],&m_cl[1],sizeof(m_cl)/2);
    a++;
  }
  if (numVerts > 2) {
    memcpy(&newface,face,sizeof(pl_Face));
    for (k = 2; k < numVerts; k ++) {
      newface.fShade = plMax(0,plMin(face->fShade,1));
      for (a = 0; a < 3; a ++) {
        if (a == 0) w = 0;
        else w = a+(k-2);
        newface.Vertices[a] = m_cl[0].newVertices+w;
        newface.Shades[a] = (pl_Float) m_cl[0].Shades[w];
        newface.MappingU[a] = (pl_sInt32)m_cl[0].MappingU[w];
        newface.MappingV[a] = (pl_sInt32)m_cl[0].MappingV[w];
        newface.eMappingU[a] = (pl_sInt32)m_cl[0].eMappingU[w];
        newface.eMappingV[a] = (pl_sInt32)m_cl[0].eMappingV[w];
        newface.Scrz[a] = 1.0f/newface.Vertices[a]->xformedz;
        tmp2 = m_fov * newface.Scrz[a];
        tmp = tmp2*newface.Vertices[a]->xformedx;
        tmp2 *= newface.Vertices[a]->xformedy;
        newface.Scrx[a] = m_cx + ((pl_sInt32)((tmp*(float) (1<<20))));
        newface.Scry[a] = m_cy - ((pl_sInt32)((tmp2*m_adj_asp*(float) (1<<20))));
      }
      newface.Material->_PutFace(m_cam,&newface);
      plRender_TriStats[3] ++;
    }
    plRender_TriStats[2] ++;
  }
}

PL_API pl_sInt plClipNeeded(pl_Face *face) {
  double dr,dl,db,dt;
  double f;
  dr = (m_cam->ClipRight-m_cam->CenterX);
  dl = (m_cam->ClipLeft-m_cam->CenterX);
  db = (m_cam->ClipBottom-m_cam->CenterY);
  dt = (m_cam->ClipTop-m_cam->CenterY);
  f = m_fov*m_adj_asp;
  return ((m_cam->ClipBack <= 0.0 ||
           face->Vertices[0]->xformedz <= m_cam->ClipBack ||
           face->Vertices[1]->xformedz <= m_cam->ClipBack ||
           face->Vertices[2]->xformedz <= m_cam->ClipBack) &&
          (face->Vertices[0]->xformedz >= 0 ||
           face->Vertices[1]->xformedz >= 0 ||
           face->Vertices[2]->xformedz >= 0) &&
          (face->Vertices[0]->xformedx*m_fov<=dr*face->Vertices[0]->xformedz ||
           face->Vertices[1]->xformedx*m_fov<=dr*face->Vertices[1]->xformedz ||
           face->Vertices[2]->xformedx*m_fov<=dr*face->Vertices[2]->xformedz) &&
          (face->Vertices[0]->xformedx*m_fov>=dl*face->Vertices[0]->xformedz ||
           face->Vertices[1]->xformedx*m_fov>=dl*face->Vertices[1]->xformedz ||
           face->Vertices[2]->xformedx*m_fov>=dl*face->Vertices[2]->xformedz) &&
          (face->Vertices[0]->xformedy*f<=db*face->Vertices[0]->xformedz ||
           face->Vertices[1]->xformedy*f<=db*face->Vertices[1]->xformedz ||
           face->Vertices[2]->xformedy*f<=db*face->Vertices[2]->xformedz) &&
          (face->Vertices[0]->xformedy*f>=dt*face->Vertices[0]->xformedz ||
           face->Vertices[1]->xformedy*f>=dt*face->Vertices[1]->xformedz ||
           face->Vertices[2]->xformedy*f>=dt*face->Vertices[2]->xformedz));
}

static void _FindNormal(double x2, double x3,double y2, double y3,
                        double zv, double *res) {
  res[0] = zv*(y2-y3);
  res[1] = zv*(x3-x2);
  res[2] = x2*y3 - y2*x3;
}

/* Returns: 0 if nothing gets in,  1 or 2 if pout1 & pout2 get in */
static pl_uInt _ClipToPlane(pl_uInt numVerts, double *plane)
{
  pl_uInt i, nextvert, curin, nextin;
  double curdot, nextdot, scale;
  pl_uInt invert, outvert;
  invert = 0;
  outvert = 0;
  curdot = m_cl[0].newVertices[0].xformedx*plane[0] +
           m_cl[0].newVertices[0].xformedy*plane[1] +
           m_cl[0].newVertices[0].xformedz*plane[2];
  curin = (curdot >= plane[3]);

  for (i=0 ; i < numVerts; i++) {
    nextvert = (i + 1) % numVerts;
    if (curin) {
      m_cl[1].Shades[outvert] = m_cl[0].Shades[invert];
      m_cl[1].MappingU[outvert] = m_cl[0].MappingU[invert];
      m_cl[1].MappingV[outvert] = m_cl[0].MappingV[invert];
      m_cl[1].eMappingU[outvert] = m_cl[0].eMappingU[invert];
      m_cl[1].eMappingV[outvert] = m_cl[0].eMappingV[invert];
      m_cl[1].newVertices[outvert++] = m_cl[0].newVertices[invert];
    }
    nextdot = m_cl[0].newVertices[nextvert].xformedx*plane[0] +
              m_cl[0].newVertices[nextvert].xformedy*plane[1] +
              m_cl[0].newVertices[nextvert].xformedz*plane[2];
    nextin = (nextdot >= plane[3]);
    if (curin != nextin) {
      scale = (plane[3] - curdot) / (nextdot - curdot);
      m_cl[1].newVertices[outvert].xformedx = (pl_Float) (m_cl[0].newVertices[invert].xformedx +
           (m_cl[0].newVertices[nextvert].xformedx - m_cl[0].newVertices[invert].xformedx)
             * scale);
      m_cl[1].newVertices[outvert].xformedy = (pl_Float) (m_cl[0].newVertices[invert].xformedy +
           (m_cl[0].newVertices[nextvert].xformedy - m_cl[0].newVertices[invert].xformedy)
             * scale);
      m_cl[1].newVertices[outvert].xformedz = (pl_Float) (m_cl[0].newVertices[invert].xformedz +
           (m_cl[0].newVertices[nextvert].xformedz - m_cl[0].newVertices[invert].xformedz)
             * scale);
      m_cl[1].Shades[outvert] = m_cl[0].Shades[invert] +
                        (m_cl[0].Shades[nextvert] - m_cl[0].Shades[invert]) * scale;
      m_cl[1].MappingU[outvert] = m_cl[0].MappingU[invert] +
           (m_cl[0].MappingU[nextvert] - m_cl[0].MappingU[invert]) * scale;
      m_cl[1].MappingV[outvert] = m_cl[0].MappingV[invert] +
           (m_cl[0].MappingV[nextvert] - m_cl[0].MappingV[invert]) * scale;
      m_cl[1].eMappingU[outvert] = m_cl[0].eMappingU[invert] +
           (m_cl[0].eMappingU[nextvert] - m_cl[0].eMappingU[invert]) * scale;
      m_cl[1].eMappingV[outvert] = m_cl[0].eMappingV[invert] +
           (m_cl[0].eMappingV[nextvert] - m_cl[0].eMappingV[invert]) * scale;
      outvert++;
    }
    curdot = nextdot;
    curin = nextin;
    invert++;
  }
  return outvert;
}

PL_API pl_Light *plLightSet(pl_Light *light, pl_uChar mode, pl_Float x, pl_Float y,
                     pl_Float z, pl_Float intensity, pl_Float halfDist) {
  pl_Float m[16], m2[16];
  light->Type = mode;
  light->Intensity = intensity;
  light->HalfDistSquared = halfDist*halfDist;
  switch (mode) {
    case PL_LIGHT_VECTOR:
      plMatrixRotate(m,1,x);
      plMatrixRotate(m2,2,y);
      plMatrixMultiply(m,m2);
      plMatrixRotate(m2,3,z);
      plMatrixMultiply(m,m2);
      plMatrixApply(m,0.0,0.0,-1.0,&light->Xp, &light->Yp, &light->Zp);
    break;
    case PL_LIGHT_POINT_ANGLE:
    case PL_LIGHT_POINT_DISTANCE:
    case PL_LIGHT_POINT:
      light->Xp = x;
      light->Yp = y;
      light->Zp = z;
    break;
  }
  return light;
}

PL_API pl_Light *plLightCreate() {
  pl_Light *l;
  l = malloc(sizeof(pl_Light));
  if (!l) return 0;
  memset(l,0,sizeof(pl_Light));
  return (l);
}

PL_API void plLightDelete(pl_Light *l) {
  if (l) free(l);
}

PL_API pl_Obj *plMakeTorus(pl_Float r1, pl_Float r2, pl_uInt divrot, pl_uInt divrad,
                    pl_Mat *m) {
  pl_Obj *o;
  pl_Vertex *v;
  pl_Face *f;
  pl_uInt x, y;
  double ravg, rt, a, da, al, dal;
  pl_sInt32 U,V,dU,dV;
  if (divrot < 3) divrot = 3;
  if (divrad < 3) divrad = 3;
  ravg = (r1+r2)*0.5;
  rt = (r2-r1)*0.5;
  o = plObjCreate(divrad*divrot,divrad*divrot*2);
  if (!o) return 0;
  v = o->Vertices;
  a = 0.0;
  da = 2*PL_PI/divrot;
  for (y = 0; y < divrot; y ++) {
    al = 0.0;
    dal = 2*PL_PI/divrad;
    for (x = 0; x < divrad; x ++) {
      v->x = (pl_Float) (cos((double) a)*(ravg + cos((double) al)*rt));
      v->z = (pl_Float) (sin((double) a)*(ravg + cos((double) al)*rt));
      v->y = (pl_Float) (sin((double) al)*rt);
      v++;
      al += dal;
    }
    a += da;
  }
  v = o->Vertices;
  f = o->Faces;
  dV = 65535/divrad;
  dU = 65535/divrot;
  U = 0;
  for (y = 0; y < divrot; y ++) {
    V = -32768;
    for (x = 0; x < divrad; x ++) {
      f->Vertices[0] = v+x+y*divrad;
      f->MappingU[0] = U;
      f->MappingV[0] = V;
      f->Vertices[1] = v+(x+1==divrad?0:x+1)+y*divrad;
      f->MappingU[1] = U;
      f->MappingV[1] = V+dV;
      f->Vertices[2] = v+x+(y+1==divrot?0:(y+1)*divrad);
      f->MappingU[2] = U+dU;
      f->MappingV[2] = V;
      f->Material = m;
      f++;
      f->Vertices[0] = v+x+(y+1==divrot?0:(y+1)*divrad);
      f->MappingU[0] = U+dU;
      f->MappingV[0] = V;
      f->Vertices[1] = v+(x+1==divrad?0:x+1)+y*divrad;
      f->MappingU[1] = U;
      f->MappingV[1] = V+dV;
      f->Vertices[2] = v+(x+1==divrad?0:x+1)+(y+1==divrot?0:(y+1)*divrad);
      f->MappingU[2] = U+dU;
      f->MappingV[2] = V+dV;
      f->Material = m;
      f++;
      V += dV;
    }
    U += dU;
  }
  plObjCalcNormals(o);
  return (o);
}

PL_API pl_Obj *plMakeSphere(pl_Float r, pl_uInt divr, pl_uInt divh, pl_Mat *m) {
  pl_Obj *o;
  pl_Vertex *v;
  pl_Face *f;
  pl_uInt x, y;
  double a, da, yp, ya, yda, yf;
  pl_sInt32 U,V,dU,dV;
  if (divh < 3) divh = 3;
  if (divr < 3) divr = 3;
  o = plObjCreate(2+(divh-2)*(divr),2*divr+(divh-3)*divr*2);
  if (!o) return 0;
  v = o->Vertices;
  v->x = v->z = 0.0; v->y = r; v++;
  v->x = v->z = 0.0; v->y = -r; v++;
  ya = 0.0;
  yda = PL_PI/(divh-1);
  da = (PL_PI*2.0)/divr;
  for (y = 0; y < divh - 2; y ++) {
    ya += yda;
    yp = cos((double) ya)*r;
    yf = sin((double) ya)*r;
    a = 0.0;
    for (x = 0; x < divr; x ++) {
      v->y = (pl_Float) yp;
      v->x = (pl_Float) (cos((double) a)*yf);
      v->z = (pl_Float) (sin((double) a)*yf);
      v++;
      a += da;
    }
  }
  f = o->Faces;
  v = o->Vertices + 2;
  a = 0.0;
  U = 0;
  dU = 65535/divr;
  dV = V = 65535/divh;
  for (x = 0; x < divr; x ++) {
    f->Vertices[0] = o->Vertices;
    f->Vertices[1] = v + (x+1==divr ? 0 : x+1);
    f->Vertices[2] = v + x;
    f->MappingU[0] = U;
    f->MappingV[0] = 0;
    f->MappingU[1] = U+dU;
    f->MappingV[1] = V;
    f->MappingU[2] = U;
    f->MappingV[2] = V;
    f->Material = m;
    f++;
    U += dU;
  }
  da = 1.0/(divr+1);
  v = o->Vertices + 2;
  for (x = 0; x < (divh-3); x ++) {
    U = 0;
    for (y = 0; y < divr; y ++) {
      f->Vertices[0] = v+y;
      f->Vertices[1] = v+divr+(y+1==divr?0:y+1);
      f->Vertices[2] = v+y+divr;
      f->MappingU[0] = U;
      f->MappingV[0] = V;
      f->MappingU[1] = U+dU;
      f->MappingV[1] = V+dV;
      f->MappingU[2] = U;
      f->MappingV[2] = V+dV;
      f->Material = m; f++;
      f->Vertices[0] = v+y;
      f->Vertices[1] = v+(y+1==divr?0:y+1);
      f->Vertices[2] = v+(y+1==divr?0:y+1)+divr;
      f->MappingU[0] = U;
      f->MappingV[0] = V;
      f->MappingU[1] = U+dU;
      f->MappingV[1] = V;
      f->MappingU[2] = U+dU;
      f->MappingV[2] = V+dV;
      f->Material = m; f++;
      U += dU;
    }
    V += dV;
    v += divr;
  }
  v = o->Vertices + o->NumVertices - divr;
  U = 0;
  for (x = 0; x < divr; x ++) {
    f->Vertices[0] = o->Vertices + 1;
    f->Vertices[1] = v + x;
    f->Vertices[2] = v + (x+1==divr ? 0 : x+1);
    f->MappingU[0] = U;
    f->MappingV[0] = 65535;
    f->MappingU[1] = U;
    f->MappingV[1] = V;
    f->MappingU[2] = U+dU;
    f->MappingV[2] = V;
    f->Material = m;
    f++;
    U += dU;
  }
  plObjCalcNormals(o);
  return (o);
}

PL_API pl_Obj *plMakeCylinder(pl_Float r, pl_Float h, pl_uInt divr, pl_Bool captop,
                       pl_Bool capbottom, pl_Mat *m) {
  pl_Obj *o;
  pl_Vertex *v, *topverts, *bottomverts, *topcapvert=0, *bottomcapvert=0;
  pl_Face *f;
  pl_uInt32 i;
  double a, da;
  if (divr < 3) divr = 3;
  o = plObjCreate(divr*2+((divr==3)?0:(captop?1:0)+(capbottom?1:0)),
                  divr*2+(divr==3 ? (captop ? 1 : 0) + (capbottom ? 1 : 0) :
                  (captop ? divr : 0) + (capbottom ? divr : 0)));
  if (!o) return 0;
  a = 0.0;
  da = (2.0*PL_PI)/divr;
  v = o->Vertices;
  topverts = v;
  for (i = 0; i < divr; i ++) {
    v->y = h/2.0f;
    v->x = (pl_Float) (r*cos((double) a));
    v->z = (pl_Float)(r*sin(a));
    v->xformedx = (pl_Float) (32768.0 + (32768.0*cos((double) a))); // temp
    v->xformedy = (pl_Float) (32768.0 + (32768.0*sin((double) a))); // use xf
    v++;
    a += da;
  }
  bottomverts = v;
  a = 0.0;
  for (i = 0; i < divr; i ++) {
    v->y = -h/2.0f;
    v->x = (pl_Float) (r*cos((double) a));
    v->z = (pl_Float) (r*sin(a));
    v->xformedx = (pl_Float) (32768.0 + (32768.0*cos((double) a)));
    v->xformedy = (pl_Float) (32768.0 + (32768.0*sin((double) a)));
    v++; a += da;
  }
  if (captop && divr != 3) {
    topcapvert = v;
    v->y = h / 2.0f;
    v->x = v->z = 0.0f;
    v++;
  }
  if (capbottom && divr != 3) {
    bottomcapvert = v;
    v->y = -h / 2.0f;
    v->x = v->z = 0.0f;
    v++;
  }
  f = o->Faces;
  for (i = 0; i < divr; i ++) {
    f->Vertices[0] = bottomverts + i;
    f->Vertices[1] = topverts + i;
    f->Vertices[2] = bottomverts + (i == divr-1 ? 0 : i+1);
    f->MappingV[0] = f->MappingV[2] = 65535; f->MappingV[1] = 0;
    f->MappingU[0] = f->MappingU[1] = (i<<16)/divr;
    f->MappingU[2] = ((i+1)<<16)/divr;
    f->Material = m; f++;
    f->Vertices[0] = bottomverts + (i == divr-1 ? 0 : i+1);
    f->Vertices[1] = topverts + i;
    f->Vertices[2] = topverts + (i == divr-1 ? 0 : i+1);
    f->MappingV[1] = f->MappingV[2] = 0; f->MappingV[0] = 65535;
    f->MappingU[0] = f->MappingU[2] = ((i+1)<<16)/divr;
    f->MappingU[1] = (i<<16)/divr;
    f->Material = m; f++;
  }
  if (captop) {
    if (divr == 3) {
      f->Vertices[0] = topverts + 0;
      f->Vertices[1] = topverts + 2;
      f->Vertices[2] = topverts + 1;
      f->MappingU[0] = (pl_sInt32) topverts[0].xformedx;
      f->MappingV[0] = (pl_sInt32) topverts[0].xformedy;
      f->MappingU[1] = (pl_sInt32) topverts[1].xformedx;
      f->MappingV[1] = (pl_sInt32) topverts[1].xformedy;
      f->MappingU[2] = (pl_sInt32) topverts[2].xformedx;
      f->MappingV[2] = (pl_sInt32) topverts[2].xformedy;
      f->Material = m; f++;
    } else {
      for (i = 0; i < divr; i ++) {
        f->Vertices[0] = topverts + (i == divr-1 ? 0 : i + 1);
        f->Vertices[1] = topverts + i;
        f->Vertices[2] = topcapvert;
        f->MappingU[0] = (pl_sInt32) topverts[(i==divr-1?0:i+1)].xformedx;
        f->MappingV[0] = (pl_sInt32) topverts[(i==divr-1?0:i+1)].xformedy;
        f->MappingU[1] = (pl_sInt32) topverts[i].xformedx;
        f->MappingV[1] = (pl_sInt32) topverts[i].xformedy;
        f->MappingU[2] = f->MappingV[2] = 32768;
        f->Material = m; f++;
      }
    }
  }
  if (capbottom) {
    if (divr == 3) {
      f->Vertices[0] = bottomverts + 0;
      f->Vertices[1] = bottomverts + 1;
      f->Vertices[2] = bottomverts + 2;
      f->MappingU[0] = (pl_sInt32) bottomverts[0].xformedx;
      f->MappingV[0] = (pl_sInt32) bottomverts[0].xformedy;
      f->MappingU[1] = (pl_sInt32) bottomverts[1].xformedx;
      f->MappingV[1] = (pl_sInt32) bottomverts[1].xformedy;
      f->MappingU[2] = (pl_sInt32) bottomverts[2].xformedx;
      f->MappingV[2] = (pl_sInt32) bottomverts[2].xformedy;
      f->Material = m; f++;
    } else {
      for (i = 0; i < divr; i ++) {
        f->Vertices[0] = bottomverts + i;
        f->Vertices[1] = bottomverts + (i == divr-1 ? 0 : i + 1);
        f->Vertices[2] = bottomcapvert;
        f->MappingU[0] = (pl_sInt32) bottomverts[i].xformedx;
        f->MappingV[0] = (pl_sInt32) bottomverts[i].xformedy;
        f->MappingU[1] = (pl_sInt32) bottomverts[(i==divr-1?0:i+1)].xformedx;
        f->MappingV[1] = (pl_sInt32) bottomverts[(i==divr-1?0:i+1)].xformedy;
        f->MappingU[2] = f->MappingV[2] = 32768;
        f->Material = m; f++;
      }
    }
  }
  plObjCalcNormals(o);
  return (o);
}

PL_API pl_Obj *plMakeCone(pl_Float r, pl_Float h, pl_uInt div,
                   pl_Bool cap, pl_Mat *m) {
  pl_Obj *o;
  pl_Vertex *v;
  pl_Face *f;
  pl_uInt32 i;
  double a, da;
  if (div < 3) div = 3;
  o = plObjCreate(div + (div == 3 ? 1 : (cap ? 2 : 1)),
                  div + (div == 3 ? 1 : (cap ? div : 0)));
  if (!o) return 0;
  v = o->Vertices;
  v->x = v->z = 0; v->y = h/2;
  v->xformedx = 1<<15;
  v->xformedy = 1<<15;
  v++;
  a = 0.0;
  da = (2.0*PL_PI)/div;
  for (i = 1; i <= div; i ++) {
    v->y = h/-2.0f;
    v->x = (pl_Float) (r*cos((double) a));
    v->z = (pl_Float) (r*sin((double) a));
    v->xformedx = (pl_Float) (32768.0 + (cos((double) a)*32768.0));
    v->xformedy = (pl_Float) (32768.0 + (sin((double) a)*32768.0));
    a += da;
    v++;
  }
  if (cap && div != 3) {
    v->y = h / -2.0f;
    v->x = v->z = 0.0f;
    v->xformedx = (pl_Float) (1<<15);
    v->xformedy = (pl_Float) (1<<15);
    v++;
  }
  f = o->Faces;
  for (i = 1; i <= div; i ++) {
    f->Vertices[0] = o->Vertices;
    f->Vertices[1] = o->Vertices + (i == div ? 1 : i + 1);
    f->Vertices[2] = o->Vertices + i;
    f->MappingU[0] = (pl_sInt32) o->Vertices[0].xformedx;
    f->MappingV[0] = (pl_sInt32) o->Vertices[0].xformedy;
    f->MappingU[1] = (pl_sInt32) o->Vertices[(i==div?1:i+1)].xformedx;
    f->MappingV[1] = (pl_sInt32) o->Vertices[(i==div?1:i+1)].xformedy;
    f->MappingU[2] = (pl_sInt32) o->Vertices[i].xformedx;
    f->MappingV[2] = (pl_sInt32) o->Vertices[i].xformedy;
    f->Material = m;
    f++;
  }
  if (cap) {
    if (div == 3) {
      f->Vertices[0] = o->Vertices + 1;
      f->Vertices[1] = o->Vertices + 2;
      f->Vertices[2] = o->Vertices + 3;
      f->MappingU[0] = (pl_sInt32) o->Vertices[1].xformedx;
      f->MappingV[0] = (pl_sInt32) o->Vertices[1].xformedy;
      f->MappingU[1] = (pl_sInt32) o->Vertices[2].xformedx;
      f->MappingV[1] = (pl_sInt32) o->Vertices[2].xformedy;
      f->MappingU[2] = (pl_sInt32) o->Vertices[3].xformedx;
      f->MappingV[2] = (pl_sInt32) o->Vertices[3].xformedy;
      f->Material = m;
      f++;
    } else {
      for (i = 1; i <= div; i ++) {
        f->Vertices[0] = o->Vertices + div + 1;
        f->Vertices[1] = o->Vertices + i;
        f->Vertices[2] = o->Vertices + (i==div ? 1 : i+1);
        f->MappingU[0] = (pl_sInt32) o->Vertices[div+1].xformedx;
        f->MappingV[0] = (pl_sInt32) o->Vertices[div+1].xformedy;
        f->MappingU[1] = (pl_sInt32) o->Vertices[i].xformedx;
        f->MappingV[1] = (pl_sInt32) o->Vertices[i].xformedy;
        f->MappingU[2] = (pl_sInt32) o->Vertices[i==div?1:i+1].xformedx;
        f->MappingV[2] = (pl_sInt32) o->Vertices[i==div?1:i+1].xformedy;
        f->Material = m;
        f++;
      }
    }
  }
  plObjCalcNormals(o);
  return (o);
}

static pl_uChar verts[6*6] = {
  0,4,1, 1,4,5, 0,1,2, 3,2,1, 2,3,6, 3,7,6,
  6,7,4, 4,7,5, 1,7,3, 7,1,5, 2,6,0, 4,0,6
};
static pl_uChar map[24*2*3] = {
  1,0, 1,1, 0,0, 0,0, 1,1, 0,1,
  0,0, 1,0, 0,1, 1,1, 0,1, 1,0,
  0,0, 1,0, 0,1, 1,0, 1,1, 0,1,
  0,0, 1,0, 0,1, 0,1, 1,0, 1,1,
  1,0, 0,1, 0,0, 0,1, 1,0, 1,1,
  1,0, 1,1, 0,0, 0,1, 0,0, 1,1
};

PL_API pl_Obj *plMakeBox(pl_Float w, pl_Float d, pl_Float h, pl_Mat *m) {
  pl_uChar *mm = map;
  pl_uChar *vv = verts;
  pl_Obj *o;
  pl_Vertex *v;
  pl_Face *f;
  pl_uInt x;
  o = plObjCreate(8,12);
  if (!o) return 0;
  v = o->Vertices;
  v->x = -w/2; v->y = h/2; v->z = d/2; v++;
  v->x = w/2; v->y = h/2; v->z = d/2; v++;
  v->x = -w/2; v->y = h/2; v->z = -d/2; v++;
  v->x = w/2; v->y = h/2; v->z = -d/2; v++;
  v->x = -w/2; v->y = -h/2; v->z = d/2; v++;
  v->x = w/2; v->y = -h/2; v->z = d/2; v++;
  v->x = -w/2; v->y = -h/2; v->z = -d/2; v++;
  v->x = w/2; v->y = -h/2; v->z = -d/2; v++;
  f = o->Faces;
  for (x = 0; x < 12; x ++) {
    f->Vertices[0] = o->Vertices + *vv++;
    f->Vertices[1] = o->Vertices + *vv++;
    f->Vertices[2] = o->Vertices + *vv++;
    f->MappingU[0] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->MappingV[0] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->MappingU[1] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->MappingV[1] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->MappingU[2] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->MappingV[2] = (pl_sInt32) ((double)*mm++ * 65535.0);
    f->Material = m;
    f++;
  }

  plObjCalcNormals(o);
  return (o);
}

PL_API pl_Obj *plMakePlane(pl_Float w, pl_Float d, pl_uInt res, pl_Mat *m) {
  pl_Obj *o;
  pl_Vertex *v;
  pl_Face *f;
  pl_uInt x, y;
  o = plObjCreate((res+1)*(res+1),res*res*2);
  if (!o) return 0;
  v = o->Vertices;
  for (y = 0; y <= res; y ++) {
    for (x = 0; x <= res; x ++) {
      v->y = 0;
      v->x = ((x*w)/res) - w/2;
      v->z = ((y*d)/res) - d/2;
      v++;
    }
  }
  f = o->Faces;
  for (y = 0; y < res; y ++) {
    for (x = 0; x < res; x ++) {
      f->Vertices[0] = o->Vertices + x+(y*(res+1));
      f->MappingU[0] = (x<<16)/res;
      f->MappingV[0] = (y<<16)/res;
      f->Vertices[2] = o->Vertices + x+1+(y*(res+1));
      f->MappingU[2] = ((x+1)<<16)/res;
      f->MappingV[2] = (y<<16)/res;
      f->Vertices[1] = o->Vertices + x+((y+1)*(res+1));
      f->MappingU[1] = (x<<16)/res;
      f->MappingV[1] = ((y+1)<<16)/res;
      f->Material = m;
      f++;
      f->Vertices[0] = o->Vertices + x+((y+1)*(res+1));
      f->MappingU[0] = (x<<16)/res;
      f->MappingV[0] = ((y+1)<<16)/res;
      f->Vertices[2] = o->Vertices + x+1+(y*(res+1));
      f->MappingU[2] = ((x+1)<<16)/res;
      f->MappingV[2] = (y<<16)/res;
      f->Vertices[1] = o->Vertices + x+1+((y+1)*(res+1));
      f->MappingU[1] = ((x+1)<<16)/res;
      f->MappingV[1] = ((y+1)<<16)/res;
      f->Material = m;
      f++;
    }
  }
  plObjCalcNormals(o);
  return (o);
}

static void _plGenerateSinglePalette(pl_Mat *);
static void _plGeneratePhongPalette(pl_Mat *);
static void _plGenerateTextureEnvPalette(pl_Mat *);
static void _plGenerateTexturePalette(pl_Mat *, pl_Texture *);
static void _plGeneratePhongTexturePalette(pl_Mat *, pl_Texture *);
static void _plGeneratePhongTransparentPalette(pl_Mat *m);
static void  _plGenerateTransparentPalette(pl_Mat *);
static void _plSetMaterialPutFace(pl_Mat *m);
static void _plMatSetupTransparent(pl_Mat *m, pl_uChar *pal);

PL_API pl_Mat *plMatCreate() {
  pl_Mat *m;
  m = (pl_Mat *) malloc(sizeof(pl_Mat));
  if (!m) return 0;
  memset(m,0,sizeof(pl_Mat));
  m->EnvScaling = 1.0f;
  m->TexScaling = 1.0f;
  m->Ambient[0] = m->Ambient[1] = m->Ambient[2] = 0;
  m->Diffuse[0] = m->Diffuse[1] = m->Diffuse[2] = 128;
  m->Specular[0] = m->Specular[1] = m->Specular[2] = 128;
  m->Shininess = 4;
  m->NumGradients = 32;
  m->FadeDist = 1000.0;
  m->zBufferable = 1;
  return m;
}

PL_API void plMatDelete(pl_Mat *m) {
  if (m) {
    if (m->_ReMapTable) free(m->_ReMapTable);
    if (m->_RequestedColors) free(m->_RequestedColors);
    if (m->_AddTable) free(m->_AddTable);
    free(m);
  }
}

PL_API void plMatInit(pl_Mat *m) {
  if (m->Shininess < 1) m->Shininess = 1;
  m->_ft = ((m->Environment ? PL_FILL_ENVIRONMENT : 0) |
           (m->Texture ? PL_FILL_TEXTURE : 0));
  m->_st = m->ShadeType;

  if (m->Transparent) m->_ft = PL_FILL_TRANSPARENT;

  if (m->_ft == (PL_FILL_TEXTURE|PL_FILL_ENVIRONMENT))
    m->_st = PL_SHADE_NONE;

  if (m->_ft == PL_FILL_SOLID) {
    if (m->_st == PL_SHADE_NONE) _plGenerateSinglePalette(m);
    else _plGeneratePhongPalette(m);
  } else if (m->_ft == PL_FILL_TEXTURE) {
    if (m->_st == PL_SHADE_NONE)
      _plGenerateTexturePalette(m,m->Texture);
    else _plGeneratePhongTexturePalette(m,m->Texture);
  } else if (m->_ft == PL_FILL_ENVIRONMENT) {
    if (m->_st == PL_SHADE_NONE)
      _plGenerateTexturePalette(m,m->Environment);
    else _plGeneratePhongTexturePalette(m,m->Environment);
  } else if (m->_ft == (PL_FILL_ENVIRONMENT|PL_FILL_TEXTURE))
    _plGenerateTextureEnvPalette(m);
  else if (m->_ft == PL_FILL_TRANSPARENT) {
    if (m->_st == PL_SHADE_NONE) _plGenerateTransparentPalette(m);
    else _plGeneratePhongTransparentPalette(m);
  }
  _plSetMaterialPutFace(m);
}

static void _plMatSetupTransparent(pl_Mat *m, pl_uChar *pal) {
  pl_uInt x, intensity;
  if (m->Transparent)
  {
    if (m->_AddTable) free(m->_AddTable);
    m->_AddTable = (pl_uInt16 *) malloc(256*sizeof(pl_uInt16));
    for (x = 0; x < 256; x ++) {
      intensity = *pal++;
      intensity += *pal++;
      intensity += *pal++;
      m->_AddTable[x] = ((intensity*(m->_ColorsUsed-m->_tsfact))/768);
    }
  }
}

PL_API void plMatMapToPal(pl_Mat *m, pl_uChar *pal, pl_sInt pstart, pl_sInt pend) {
  pl_sInt32 j, r, g, b, bestdiff, r2, g2, b2;
  pl_sInt bestpos,k;
  pl_uInt32 i;
  pl_uChar *p;
  if (!m->_RequestedColors) plMatInit(m);
  if (!m->_RequestedColors) return;
  if (m->_ReMapTable) free(m->_ReMapTable);
  m->_ReMapTable = (pl_uChar *) malloc(m->_ColorsUsed);
  for (i = 0; i < m->_ColorsUsed; i ++) {
    bestdiff = 1000000000;
    bestpos = pstart;
    r = m->_RequestedColors[i*3];
    g = m->_RequestedColors[i*3+1];
    b = m->_RequestedColors[i*3+2];
    p = pal + pstart*3;
    for (k = pstart; k <= (pl_sInt)pend; k ++) {
      r2 = p[0] - r;
      g2 = p[1] - g;
      b2 = p[2] - b;
      p += 3;
      j = r2*r2+g2*g2+b2*b2;
      if (j < bestdiff) {
        bestdiff = j;
        bestpos = k;
      }
    }
    m->_ReMapTable[i] = bestpos;
  }
  _plMatSetupTransparent(m,pal);
}

static void _plGenerateSinglePalette(pl_Mat *m) {
  m->_ColorsUsed = 1;
  if (m->_RequestedColors) free(m->_RequestedColors);
  m->_RequestedColors = (pl_uChar *) malloc(3);
  m->_RequestedColors[0] = plMin(plMax(m->Ambient[0],0),255);
  m->_RequestedColors[1] = plMin(plMax(m->Ambient[1],0),255);
  m->_RequestedColors[2] = plMin(plMax(m->Ambient[2],0),255);
}

static void _plGeneratePhongPalette(pl_Mat *m) {
  pl_uInt i = m->NumGradients, x;
  pl_sInt c;
  pl_uChar *pal;
  double a, da, ca, cb;
  m->_ColorsUsed = m->NumGradients;
  if (m->_RequestedColors) free(m->_RequestedColors);
  pal =  m->_RequestedColors = (pl_uChar *) malloc(m->_ColorsUsed*3);
  a = PL_PI/2.0;

  if (m->NumGradients > 1) da = -PL_PI/((m->NumGradients-1)<<1);
  else da=0.0;

  do {
    if (m->NumGradients == 1) ca = 1;
    else {
      ca = cos((double) a);
      a += da;
    }
    cb = pow((double) ca, (double) m->Shininess);
    for (x = 0; x < 3; x ++) {
      c = (pl_sInt) ((cb*m->Specular[x])+(ca*m->Diffuse[x])+m->Ambient[x]);
      *(pal++) = plMax(0,plMin(c,255));
    }
  } while (--i);
}

static void _plGenerateTextureEnvPalette(pl_Mat *m) {
  pl_sInt c;
  pl_uInt whichlevel,whichindex;
  pl_uChar *texpal, *envpal, *pal;
  m->_ColorsUsed = m->Texture->NumColors*m->Environment->NumColors;
  if (m->_RequestedColors) free(m->_RequestedColors);
  pal = m->_RequestedColors = (pl_uChar *) malloc(m->_ColorsUsed*3);
  envpal = m->Environment->PaletteData;
  if (m->_AddTable) free(m->_AddTable);
  m->_AddTable = (pl_uInt16 *) malloc(m->Environment->NumColors*sizeof(pl_uInt16));
  for (whichlevel = 0; whichlevel < m->Environment->NumColors; whichlevel++) {
    texpal = m->Texture->PaletteData;
    switch (m->TexEnvMode)
    {
      case PL_TEXENV_MUL: // multiply
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) * (pl_sInt) envpal[0])>>8);
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) * (pl_sInt) envpal[1])>>8);
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) * (pl_sInt) envpal[2])>>8);
        }
      break;
      case PL_TEXENV_AVG: // average
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) + (pl_sInt) envpal[0])>>1);
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) + (pl_sInt) envpal[1])>>1);
          *pal++ = (pl_uChar) (((pl_sInt) (*texpal++) + (pl_sInt) envpal[2])>>1);
        }
      break;
      case PL_TEXENV_TEXMINUSENV: // tex-env
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          c = (pl_sInt) (*texpal++) - (pl_sInt) envpal[0]; *pal++ = plMax(0,plMin(255,c));
          c = (pl_sInt) (*texpal++) - (pl_sInt) envpal[1]; *pal++ = plMax(0,plMin(255,c));
          c = (pl_sInt) (*texpal++) - (pl_sInt) envpal[2]; *pal++ = plMax(0,plMin(255,c));
        }
      break;
      case PL_TEXENV_ENVMINUSTEX: // env-tex
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          c = -(pl_sInt) (*texpal++) - (pl_sInt) envpal[0]; *pal++ = plMax(0,plMin(255,c));
          c = -(pl_sInt) (*texpal++) - (pl_sInt) envpal[1]; *pal++ = plMax(0,plMin(255,c));
          c = -(pl_sInt) (*texpal++) - (pl_sInt) envpal[2]; *pal++ = plMax(0,plMin(255,c));
        }
      break;
      case PL_TEXENV_MIN:
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          *pal++ = plMin(texpal[0],envpal[0]);
          *pal++ = plMin(texpal[1],envpal[1]);
          *pal++ = plMin(texpal[2],envpal[2]);
          texpal+=3;
        }
      break;
      case PL_TEXENV_MAX:
      break;
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          *pal++ = plMax(texpal[0],envpal[0]);
          *pal++ = plMax(texpal[1],envpal[1]);
          *pal++ = plMax(texpal[2],envpal[2]);
          texpal+=3;
        }
      default: // add
        for (whichindex = 0; whichindex < m->Texture->NumColors; whichindex++) {
          c = (pl_sInt) (*texpal++) + (pl_sInt) envpal[0]; *pal++ = plMax(0,plMin(255,c));
          c = (pl_sInt) (*texpal++) + (pl_sInt) envpal[1]; *pal++ = plMax(0,plMin(255,c));
          c = (pl_sInt) (*texpal++) + (pl_sInt) envpal[2]; *pal++ = plMax(0,plMin(255,c));
        }
      break;
    }
    envpal += 3;
    m->_AddTable[whichlevel] = whichlevel*m->Texture->NumColors;
  }
}

static void _plGenerateTexturePalette(pl_Mat *m, pl_Texture *t) {
  pl_uChar *ppal, *pal;
  pl_sInt c, i, x;
  m->_ColorsUsed = t->NumColors;
  if (m->_RequestedColors) free(m->_RequestedColors);
  pal = m->_RequestedColors = (pl_uChar *) malloc(m->_ColorsUsed*3);
  ppal = t->PaletteData;
  i = t->NumColors;
  do {
    for (x = 0; x < 3; x ++) {
      c = m->Ambient[x] + *ppal++;
      *(pal++) = plMax(0,plMin(c,255));
    }
  } while (--i);
}

static void _plGeneratePhongTexturePalette(pl_Mat *m, pl_Texture *t) {
  double a, ca, da, cb;
  pl_uInt16 *addtable;
  pl_uChar *ppal, *pal;
  pl_sInt c, i, i2, x;
  pl_uInt num_shades;

  if (t->NumColors) num_shades = (m->NumGradients / t->NumColors);
  else num_shades=1;

  if (!num_shades) num_shades = 1;
  m->_ColorsUsed = num_shades*t->NumColors;
  if (m->_RequestedColors) free(m->_RequestedColors);
  pal = m->_RequestedColors = (pl_uChar *) malloc(m->_ColorsUsed*3);
  a = PL_PI/2.0;
  if (num_shades>1) da = (-PL_PI/2.0)/(num_shades-1);
  else da=0.0;
  i2 = num_shades;
  do {
    ppal = t->PaletteData;
    ca = cos((double) a);
    a += da;
    cb = pow(ca, (double) m->Shininess);
    i = t->NumColors;
    do {
      for (x = 0; x < 3; x ++) {
        c = (pl_sInt) ((cb*m->Specular[x])+(ca*m->Diffuse[x])+m->Ambient[x] + *ppal++);
        *(pal++) = plMax(0,plMin(c,255));
      }
    } while (--i);
  } while (--i2);
  ca = 0;
  if (m->_AddTable) free(m->_AddTable);
  m->_AddTable = (pl_uInt16 *) malloc(256*sizeof(pl_uInt16));
  addtable = m->_AddTable;
  i = 256;
  do {
    a = sin(ca) * num_shades;
    ca += PL_PI/512.0;
    *addtable++ = ((pl_sInt) a)*t->NumColors;
  } while (--i);
}

static void _plGeneratePhongTransparentPalette(pl_Mat *m) {
  m->_tsfact = (pl_sInt) (m->NumGradients*(1.0/(1+m->Transparent)));
  _plGeneratePhongPalette(m);
}

static void  _plGenerateTransparentPalette(pl_Mat *m) {
  m->_tsfact = 0;
  _plGeneratePhongPalette(m);
}

static void _plSetMaterialPutFace(pl_Mat *m) {
  m->_PutFace = 0;
  switch (m->_ft) {
    case PL_FILL_TRANSPARENT: switch(m->_st) {
      case PL_SHADE_NONE: case PL_SHADE_FLAT:
      case PL_SHADE_FLAT_DISTANCE: case PL_SHADE_FLAT_DISTANCE|PL_SHADE_FLAT:
        m->_PutFace = plPF_TransF;
      break;
      case PL_SHADE_GOURAUD: case PL_SHADE_GOURAUD_DISTANCE:
      case PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE:
        m->_PutFace = plPF_TransG;
      break;
    }
    break;
    case PL_FILL_SOLID: switch(m->_st) {
      case PL_SHADE_NONE: case PL_SHADE_FLAT:
      case PL_SHADE_FLAT_DISTANCE: case PL_SHADE_FLAT_DISTANCE|PL_SHADE_FLAT:
        m->_PutFace = plPF_SolidF;
      break;
      case PL_SHADE_GOURAUD: case PL_SHADE_GOURAUD_DISTANCE:
      case PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE:
        m->_PutFace = plPF_SolidG;
      break;
    }
    break;
    case PL_FILL_ENVIRONMENT:
    case PL_FILL_TEXTURE:
      if (m->PerspectiveCorrect) switch (m->_st) {
        case PL_SHADE_NONE: case PL_SHADE_FLAT:
        case PL_SHADE_FLAT_DISTANCE: case PL_SHADE_FLAT_DISTANCE|PL_SHADE_FLAT:
          m->_PutFace = plPF_PTexF;
        break;
        case PL_SHADE_GOURAUD: case PL_SHADE_GOURAUD_DISTANCE:
        case PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE:
          m->_PutFace = plPF_PTexG;
        break;
      }
      else switch (m->_st) {
        case PL_SHADE_NONE: case PL_SHADE_FLAT:
        case PL_SHADE_FLAT_DISTANCE: case PL_SHADE_FLAT_DISTANCE|PL_SHADE_FLAT:
          m->_PutFace = plPF_TexF;
        break;
        case PL_SHADE_GOURAUD: case PL_SHADE_GOURAUD_DISTANCE:
        case PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE:
          m->_PutFace = plPF_TexG;
        break;
      }
    break;
    case PL_FILL_TEXTURE|PL_FILL_ENVIRONMENT:
      m->_PutFace = plPF_TexEnv;
    break;
  }
}

typedef struct __ct {
  pl_uChar r,g,b;
  pl_Bool visited;
  struct __ct *next;
} _ct;

static int mdist(_ct *a, _ct *b) {
  return ((a->r-b->r)*(a->r-b->r)+(a->g-b->g)*(a->g-b->g)+(a->b-b->b)*(a->b-b->b));
}

PL_API void plMatMakeOptPal(pl_uChar *p, pl_sInt pstart,
                     pl_sInt pend, pl_Mat **materials, pl_sInt nmats) {
  pl_uChar *allColors = 0;
  pl_sInt numColors = 0, nc, x;
  pl_sInt len = pend + 1 - pstart;
  pl_sInt32 current, newnext, bestdist, thisdist;
  _ct *colorBlock, *best, *cp;

  for (x = 0; x < nmats; x ++) {
    if (materials[x]) {
      if (!materials[x]->_RequestedColors) plMatInit(materials[x]);
      if (materials[x]->_RequestedColors) numColors+=materials[x]->_ColorsUsed;
    }
  }
  if (!numColors) return;

  allColors=(pl_uChar*)malloc(numColors*3);
  numColors=0;

  for (x = 0; x < nmats; x ++) {
    if (materials[x]) {
      if (materials[x]->_RequestedColors)
        memcpy(allColors + (numColors*3), materials[x]->_RequestedColors,
             materials[x]->_ColorsUsed*3);
      numColors += materials[x]->_ColorsUsed;
    }
  }

  if (numColors <= len) {
    memcpy(p+pstart*3,allColors,numColors*3);
    free(allColors);
    return;
  }

  colorBlock = (_ct *) malloc(sizeof(_ct)*numColors);
  for (x = 0; x < numColors; x++) {
    colorBlock[x].r = allColors[x*3];
    colorBlock[x].g = allColors[x*3+1];
    colorBlock[x].b = allColors[x*3+2];
    colorBlock[x].visited = 0;
    colorBlock[x].next = 0;
  }
  free(allColors);

  /* Build a list, starting at color 0 */
  current = 0;
  nc = numColors;
  do {
    newnext = -1;
    bestdist = 300000000;
    colorBlock[current].visited = 1;
    for (x = 0; x < nc; x ++) {
      if (!colorBlock[x].visited) {
        thisdist = mdist(colorBlock + x, colorBlock + current);
        if (thisdist < 5) { colorBlock[x].visited = 1; numColors--; }
        else if (thisdist < bestdist) { bestdist = thisdist; newnext = x; }
      }
    }
    if (newnext != -1) {
      colorBlock[current].next = colorBlock + newnext;
      current = newnext;
    }
  } while (newnext != -1);
  colorBlock[current].next = 0; /* terminate the list */

  /* we now have a linked list starting at colorBlock, which is each one and
     it's closest neighbor */

  while (numColors > len) {
    bestdist = mdist(colorBlock,colorBlock->next);
    for (best = cp = colorBlock; cp->next; cp = cp->next) {
      if (bestdist > (thisdist = mdist(cp,cp->next))) {
        best = cp;
        bestdist = thisdist;
      }
    }
    best->r = ((int) best->r + (int) best->next->r)>>1;
    best->g = ((int) best->g + (int) best->next->g)>>1;
    best->b = ((int) best->b + (int) best->next->b)>>1;
    best->next = best->next->next;
    numColors--;
  }
  x = pstart*3;
  for (cp = colorBlock; cp; cp = cp->next) {
    p[x++] = cp->r;
    p[x++] = cp->g;
    p[x++] = cp->b;
  }
  free(colorBlock);
}

PL_API void plMatrixRotate(pl_Float matrix[], pl_uChar m, pl_Float Deg) {
  pl_uChar m1, m2;
  double c,s;
  double d= Deg * PL_PI / 180.0;
  memset(matrix,0,sizeof(pl_Float)*16);
  matrix[((m-1)<<2)+m-1] = matrix[15] = 1.0;
  m1 = (m % 3);
  m2 = ((m1+1) % 3);
  c = cos(d); s = sin(d);
  matrix[(m1<<2)+m1]=(pl_Float)c; matrix[(m1<<2)+m2]=(pl_Float)s;
  matrix[(m2<<2)+m2]=(pl_Float)c; matrix[(m2<<2)+m1]=(pl_Float)-s;
}

PL_API void plMatrixTranslate(pl_Float m[], pl_Float x, pl_Float y, pl_Float z) {
  memset(m,0,sizeof(pl_Float)*16);
  m[0] = m[4+1] = m[8+2] = m[12+3] = 1.0;
  m[0+3] = x; m[4+3] = y; m[8+3] = z;
}

PL_API void plMatrixMultiply(pl_Float *dest, pl_Float src[]) {
  pl_Float temp[16];
  pl_uInt i;
  memcpy(temp,dest,sizeof(pl_Float)*16);
  for (i = 0; i < 16; i += 4) {
    *dest++ = src[i+0]*temp[(0<<2)+0]+src[i+1]*temp[(1<<2)+0]+
              src[i+2]*temp[(2<<2)+0]+src[i+3]*temp[(3<<2)+0];
    *dest++ = src[i+0]*temp[(0<<2)+1]+src[i+1]*temp[(1<<2)+1]+
              src[i+2]*temp[(2<<2)+1]+src[i+3]*temp[(3<<2)+1];
    *dest++ = src[i+0]*temp[(0<<2)+2]+src[i+1]*temp[(1<<2)+2]+
              src[i+2]*temp[(2<<2)+2]+src[i+3]*temp[(3<<2)+2];
    *dest++ = src[i+0]*temp[(0<<2)+3]+src[i+1]*temp[(1<<2)+3]+
              src[i+2]*temp[(2<<2)+3]+src[i+3]*temp[(3<<2)+3];
  }
}

PL_API void plMatrixApply(pl_Float *m, pl_Float x, pl_Float y, pl_Float z,
                   pl_Float *outx, pl_Float *outy, pl_Float *outz) {
  *outx = x*m[0] + y*m[1] + z*m[2] + m[3];
  *outy	= x*m[4] + y*m[5] + z*m[6] + m[7];
  *outz = x*m[8] + y*m[9] + z*m[10] + m[11];
}

PL_API pl_Float plDotProduct(pl_Float x1, pl_Float y1, pl_Float z1,
                      pl_Float x2, pl_Float y2, pl_Float z2) {
  return ((x1*x2)+(y1*y2)+(z1*z2));
}

PL_API void plNormalizeVector(pl_Float *x, pl_Float *y, pl_Float *z) {
  double length;
  length = (*x)*(*x)+(*y)*(*y)+(*z)*(*z);
  if (length > 0.0000000001) {
    pl_Float t = (pl_Float)sqrt(length);
    *x /= t;
    *y /= t;
    *z /= t;
  } else *x = *y = *z = 0.0;
}

PL_API pl_Obj *plObjScale(pl_Obj *o, pl_Float s) {
  pl_uInt32 i = o->NumVertices;
  pl_Vertex *v = o->Vertices;
  while (i--) {
    v->x *= s; v->y *= s; v->z *= s; v++;
  }
  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (o->Children[i]) plObjScale(o->Children[i],s);
  return o;
}

PL_API pl_Obj *plObjStretch(pl_Obj *o, pl_Float x, pl_Float y, pl_Float z) {
  pl_uInt32 i = o->NumVertices;
  pl_Vertex *v = o->Vertices;
  while (i--) {
    v->x *= x; v->y *= y; v->z *= z; v++;
  }
  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (o->Children[i]) plObjStretch(o->Children[i],x,y,z);
  return o;
}

PL_API pl_Obj *plObjTranslate(pl_Obj *o, pl_Float x, pl_Float y, pl_Float z) {
  pl_uInt32 i = o->NumVertices;
  pl_Vertex *v = o->Vertices;
  while (i--) {
    v->x += x; v->y += y; v->z += z; v++;
  }
  return o;
}

PL_API pl_Obj *plObjFlipNormals(pl_Obj *o) {
  pl_uInt32 i = o->NumVertices;
  pl_Vertex *v = o->Vertices;
  pl_Face *f = o->Faces;
  while (i--) {
    v->nx = - v->nx; v->ny = - v->ny; v->nz = - v->nz; v++;
  }
  i = o->NumFaces;
  while (i--) {
    f->nx = - f->nx; f->ny = - f->ny; f->nz = - f->nz;
    f++;
  }
  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (o->Children[i]) plObjFlipNormals(o->Children[i]);
  return o;
}

PL_API void plObjDelete(pl_Obj *o) {
  pl_uInt i;
  if (o) {
    for (i = 0; i < PL_MAX_CHILDREN; i ++)
      if (o->Children[i]) plObjDelete(o->Children[i]);
    if (o->Vertices) free(o->Vertices);
    if (o->Faces) free(o->Faces);
    free(o);
  }
}

PL_API pl_Obj *plObjCreate(pl_uInt32 nv, pl_uInt32 nf) {
  pl_Obj *o;
  if (!(o = (pl_Obj *) malloc(sizeof(pl_Obj)))) return 0;
  memset(o,0,sizeof(pl_Obj));
  o->GenMatrix = 1;
  o->BackfaceCull = 1;
  o->NumVertices = nv;
  o->NumFaces = nf;
  if (nv && !(o->Vertices=(pl_Vertex *) malloc(sizeof(pl_Vertex)*nv))) {
    free(o);
    return 0;
  }
  if (nf && !(o->Faces = (pl_Face *) malloc(sizeof(pl_Face)*nf))) {
    free(o->Vertices);
    free(o);
    return 0;
  }
  memset(o->Vertices,0,sizeof(pl_Vertex)*nv);
  memset(o->Faces,0,sizeof(pl_Face)*nf);
  return o;
}

PL_API pl_Obj *plObjClone(pl_Obj *o) {
  pl_Face *iff, *of;
  pl_uInt32 i;
  pl_Obj *out;
  if (!(out = plObjCreate(o->NumVertices,o->NumFaces))) return 0;
  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (o->Children[i]) out->Children[i] = plObjClone(o->Children[i]);
  out->Xa = o->Xa; out->Ya = o->Ya; out->Za = o->Za;
  out->Xp = o->Xp; out->Yp = o->Yp; out->Zp = o->Zp;
  out->BackfaceCull = o->BackfaceCull;
  out->BackfaceIllumination = o->BackfaceIllumination;
  out->GenMatrix = o->GenMatrix;
  memcpy(out->Vertices, o->Vertices, sizeof(pl_Vertex) * o->NumVertices);
  iff = o->Faces;
  of = out->Faces;
  i = out->NumFaces;
  while (i--) {
    of->Vertices[0] = (pl_Vertex *)
      out->Vertices + (iff->Vertices[0] - o->Vertices);
    of->Vertices[1] = (pl_Vertex *)
      out->Vertices + (iff->Vertices[1] - o->Vertices);
    of->Vertices[2] = (pl_Vertex *)
      out->Vertices + (iff->Vertices[2] - o->Vertices);
    of->MappingU[0] = iff->MappingU[0];
    of->MappingV[0] = iff->MappingV[0];
    of->MappingU[1] = iff->MappingU[1];
    of->MappingV[1] = iff->MappingV[1];
    of->MappingU[2] = iff->MappingU[2];
    of->MappingV[2] = iff->MappingV[2];
    of->nx = iff->nx;
    of->ny = iff->ny;
    of->nz = iff->nz;
    of->Material = iff->Material;
    of++;
    iff++;
  }
  return out;
}

PL_API void plObjSetMat(pl_Obj *o, pl_Mat *m, pl_Bool th) {
  pl_sInt32 i = o->NumFaces;
  pl_Face *f = o->Faces;
  while (i--) (f++)->Material = m;
  if (th) for (i = 0; i < PL_MAX_CHILDREN; i++)
    if (o->Children[i]) plObjSetMat(o->Children[i],m,th);
}

PL_API void plObjCalcNormals(pl_Obj *obj) {
  pl_uInt32 i;
  pl_Vertex *v = obj->Vertices;
  pl_Face *f = obj->Faces;
  double x1, x2, y1, y2, z1, z2;
  i = obj->NumVertices;
  while (i--) {
    v->nx = 0.0; v->ny = 0.0; v->nz = 0.0;
    v++;
  }
  i = obj->NumFaces;
  while (i--) {
    x1 = f->Vertices[0]->x-f->Vertices[1]->x;
    x2 = f->Vertices[0]->x-f->Vertices[2]->x;
    y1 = f->Vertices[0]->y-f->Vertices[1]->y;
    y2 = f->Vertices[0]->y-f->Vertices[2]->y;
    z1 = f->Vertices[0]->z-f->Vertices[1]->z;
    z2 = f->Vertices[0]->z-f->Vertices[2]->z;
    f->nx = (pl_Float) (y1*z2 - z1*y2);
    f->ny = (pl_Float) (z1*x2 - x1*z2);
    f->nz = (pl_Float) (x1*y2 - y1*x2);
    plNormalizeVector(&f->nx, &f->ny, &f->nz);
    f->Vertices[0]->nx += f->nx;
    f->Vertices[0]->ny += f->ny;
    f->Vertices[0]->nz += f->nz;
    f->Vertices[1]->nx += f->nx;
    f->Vertices[1]->ny += f->ny;
    f->Vertices[1]->nz += f->nz;
    f->Vertices[2]->nx += f->nx;
    f->Vertices[2]->ny += f->ny;
    f->Vertices[2]->nz += f->nz;
    f++;
  }
  v = obj->Vertices;
  i = obj->NumVertices;
  do {
    plNormalizeVector(&v->nx, &v->ny, &v->nz);
    v++;
  } while (--i);
  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (obj->Children[i]) plObjCalcNormals(obj->Children[i]);
}

PL_API void plPF_PTexF(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_uChar *remap = TriFace->Material->_ReMapTable;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_Float MappingU1, MappingU2, MappingU3;
  pl_Float MappingV1, MappingV2, MappingV3;
  pl_sInt32 MappingU_AND, MappingV_AND;
  pl_uChar *texture;
  pl_uChar vshift;
  pl_uInt16 bc;
  pl_Texture *Texture;
  pl_sInt32 iShade;

  pl_uChar nm, nmb;
  pl_sInt n;
  pl_Float U1,V1,U2,V2,dU1=0,dU2=0,dV1=0,dV2=0,dUL=0,dVL=0,UL,VL;
  pl_sInt32 iUL, iVL, idUL=0, idVL=0, iULnext, iVLnext;

  pl_sInt32 scrwidth = cam->ScreenWidth;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, Xlen;
  pl_ZBuffer Z1, dZ1=0, dZ2=0, Z2, dZL=0, ZL, pZL, pdZL;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_uChar stat;

  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;

  if (TriFace->Material->Environment) Texture = TriFace->Material->Environment;
  else Texture = TriFace->Material->Texture;

  if (!Texture) return;
  texture = Texture->Data;
  iShade = (pl_sInt32)(TriFace->fShade*256.0);
  if (iShade < 0) iShade=0;
  if (iShade > 255) iShade=255;

  if (!TriFace->Material->_AddTable) bc=0;
  else bc = TriFace->Material->_AddTable[iShade];
  nm = TriFace->Material->PerspectiveCorrect;
  nmb = 0; while (nm) { nmb++; nm >>= 1; }
  nmb = plMin(6,nmb);
  nm = 1<<nmb;
  MappingV_AND = ((1<<Texture->Height)-1)<<Texture->Width;
  MappingU_AND = (1<<Texture->Width)-1;
  vshift = 16 - Texture->Width;

  if (TriFace->Material->Environment) {
    PUTFACE_SORT_ENV();
  } else {
    PUTFACE_SORT_TEX();
  }

  MappingU1 *= TriFace->Scrz[i0]/65536.0f;
  MappingV1 *= TriFace->Scrz[i0]/65536.0f;
  MappingU2 *= TriFace->Scrz[i1]/65536.0f;
  MappingV2 *= TriFace->Scrz[i1]/65536.0f;
  MappingU3 *= TriFace->Scrz[i2]/65536.0f;
  MappingV3 *= TriFace->Scrz[i2]/65536.0f;

  U1 = U2 = MappingU1;
  V1 = V2 = MappingV1;
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2-Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
    dU2 = (MappingU3 - U1) / dY;
    dV2 = (MappingV3 - V1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    dU1 = (MappingU2 - U1) / dY;
    dV1 = (MappingV2 - V1) / dY;
    if (dX2 < dX1) {
      XL1 = dX2; dX2 = dX1; dX1 = XL1;
      dUL = dU1; dU1 = dU2; dU2 = dUL;
      dVL = dV1; dV1 = dV2; dV2 = dVL;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      U2 = MappingU2;
      V2 = MappingV2;
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      U1 = MappingU2;
      V1 = MappingV2;
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  XL1 = ((dX1-dX2)*dY+(1<<19))>>20;
  if (XL1) {
    dUL = ((dU1-dU2)*dY)/XL1;
    dVL = ((dV1-dV2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dUL = (U2-U1)/XL1;
      dVL = (V2-V1)/XL1;
      dZL = (Z2-Z1)/XL1;
    }
  }

  pdZL = dZL * nm;
  dUL *= nm;
  dVL *= nm;

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2-((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1)/dY;
        dV1 = (MappingV3 - V1) / dY;
        dU1 = (MappingU3 - U1) / dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    Xlen = ((X2+(1<<19))>>20) - XL1;
    if (Xlen > 0) {
      register pl_Float t;
      pZL = ZL = Z1;
      UL = U1;
      VL = V1;
      gmem += XL1;
      zbuf += XL1;
      XL1 += Xlen-scrwidth;
      t = 65536.0f/ZL;
      iUL = iULnext = ((pl_sInt32) (UL*t));
      iVL = iVLnext = ((pl_sInt32) (VL*t));
      do {
        UL += dUL;
        VL += dVL;
        iUL = iULnext;
        iVL = iVLnext;
        pZL += pdZL;
        t = 65536.0f/pZL;
        iULnext = ((pl_sInt32) (UL*t));
        iVLnext = ((pl_sInt32) (VL*t));
        idUL = (iULnext - iUL)>>nmb;
        idVL = (iVLnext - iVL)>>nmb;
        n = nm;
        Xlen -= n;
        if (Xlen < 0) n += Xlen;
        if (zb) do {
            if (*zbuf < ZL) {
              *zbuf = ZL;
              *gmem = remap[bc + texture[((iUL>>16)&MappingU_AND) +
                                   ((iVL>>vshift)&MappingV_AND)]];
            }
            zbuf++;
            gmem++;
            ZL += dZL;
            iUL += idUL;
            iVL += idVL;
          } while (--n);
        else do {
            *gmem++ = remap[bc + texture[((iUL>>16)&MappingU_AND) +
                                   ((iVL>>vshift)&MappingV_AND)]];
            iUL += idUL;
            iVL += idVL;
          } while (--n);
      } while (Xlen > 0);
      gmem -= XL1;
      zbuf -= XL1;
    } else {
      zbuf += cam->ScreenWidth;
      gmem += cam->ScreenWidth;
    }
    Z1 += dZ1;
    U1 += dU1;
    V1 += dV1;
    X1 += dX1;
    X2 += dX2;
    Y0++;
  }
}

PL_API void plPF_PTexG(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_Float MappingU1, MappingU2, MappingU3;
  pl_Float MappingV1, MappingV2, MappingV3;

  pl_Texture *Texture;
  pl_Bool zb = (cam->zBuffer&&TriFace->Material->zBufferable) ? 1 : 0;

  pl_uChar nm, nmb;
  pl_uInt n;
  pl_sInt32 MappingU_AND, MappingV_AND;
  pl_uChar vshift;
  pl_uChar *texture;
  pl_uInt16 *addtable;
  pl_uChar *remap = TriFace->Material->_ReMapTable;
  pl_sInt32 iUL, iVL, idUL, idVL, iULnext, iVLnext;
  pl_Float U2,V2,dU2=0,dV2=0,dUL=0,dVL=0,UL,VL;
  pl_sInt32 XL1, Xlen;
  pl_sInt32 C2, dC2=0, CL, dCL=0;
  pl_Float ZL, Z2, dZ2=0, dZL=0, pdZL, pZL;

  pl_sInt32 Y2, dY;
  pl_uChar stat;

  /* Cache line */
  pl_sInt32 Y0,Y1;
  pl_sInt32 C1, dC1=0, X2, dX2=0, X1, dX1=0;

  /* Cache line */
  pl_Float dU1=0, U1, dZ1=0, Z1, V1, dV1=0;
  pl_sInt32 scrwidth = cam->ScreenWidth;
  pl_uChar *gmem = cam->frameBuffer;
  pl_ZBuffer *zbuf = cam->zBuffer;

  if (TriFace->Material->Environment) Texture = TriFace->Material->Environment;
  else Texture = TriFace->Material->Texture;

  if (!Texture) return;
  texture = Texture->Data;
  addtable = TriFace->Material->_AddTable;
  if (!addtable) return;

  nm = TriFace->Material->PerspectiveCorrect;
  nmb = 0; while (nm) { nmb++; nm >>= 1; }
  nmb = plMin(6,nmb);
  nm = 1<<nmb;
  MappingV_AND = ((1<<Texture->Height)-1)<<Texture->Width;
  MappingU_AND = (1<<Texture->Width)-1;
  vshift = 16 - Texture->Width;

  if (TriFace->Material->Environment) {
    PUTFACE_SORT_ENV();
  } else {
    PUTFACE_SORT_TEX();
  }

  MappingU1 *= TriFace->Scrz[i0]/65536.0f;
  MappingV1 *= TriFace->Scrz[i0]/65536.0f;
  MappingU2 *= TriFace->Scrz[i1]/65536.0f;
  MappingV2 *= TriFace->Scrz[i1]/65536.0f;
  MappingU3 *= TriFace->Scrz[i2]/65536.0f;
  MappingV3 *= TriFace->Scrz[i2]/65536.0f;
  TriFace->Shades[0] *= 65536.0f;
  TriFace->Shades[1] *= 65536.0f;
  TriFace->Shades[2] *= 65536.0f;

  C1 = C2 = (pl_sInt32) TriFace->Shades[i0];
  U1 = U2 = MappingU1;
  V1 = V2 = MappingV1;
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2-Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
    dC2 = (pl_sInt32) ((TriFace->Shades[i2] - C1) / dY);
    dU2 = (MappingU3 - U1) / dY;
    dV2 = (MappingV3 - V1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    dC1 = (pl_sInt32) ((TriFace->Shades[i1] - C1) / dY);
    dU1 = (MappingU2 - U1) / dY;
    dV1 = (MappingV2 - V1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dUL = dU1; dU1 = dU2; dU2 = dUL;
      dVL = dV1; dV1 = dV2; dV2 = dVL;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      dCL = dC1; dC1 = dC2; dC2 = dCL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      C2 = (pl_sInt32)TriFace->Shades[i1];
      U2 = MappingU2;
      V2 = MappingV2;
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      C1 = (pl_sInt32)TriFace->Shades[i1];
      U1 = MappingU2;
      V1 = MappingV2;
      stat = 1|8;
    }
  }

  gmem += (Y0 * scrwidth);
  zbuf += (Y0 * scrwidth);

  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dUL = ((dU1-dU2)*dY)/XL1;
    dVL = ((dV1-dV2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
    dCL = ((dC1-dC2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dUL = (U2-U1)/XL1;
      dVL = (V2-V1)/XL1;
      dZL = (Z2-Z1)/XL1;
      dCL = (C2-C1)/XL1;
    }
  }

  pdZL = dZL * nm;
  dUL *= nm;
  dVL *= nm;
  Y1 -= Y0;
  Y0 = Y2-Y0;
  while (Y0--) {
    if (!Y1--) {
      dY = Y2-((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1)/dY;
        dC1 = (pl_sInt32)((TriFace->Shades[i2]-C1)/dY);
        dV1 = (MappingV3 - V1) / dY;
        dU1 = (MappingU3 - U1) / dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    Xlen = ((X2+(1<<19))>>20) - XL1;
    if (Xlen > 0) {
      register pl_Float t;
      CL = C1;
      pZL = ZL = Z1;
      UL = U1;
      VL = V1;
      gmem += XL1;
      zbuf += XL1;
      XL1 += Xlen-scrwidth;
      t = 65536.0f / ZL;
      iUL = iULnext = ((pl_sInt32) (UL*t));
      iVL = iVLnext = ((pl_sInt32) (VL*t));
      do {
        UL += dUL;
        VL += dVL;
        iUL = iULnext;
        iVL = iVLnext;
        pZL += pdZL;
        t = 65536.0f/pZL;
        iULnext = ((pl_sInt32) (UL*t));
        iVLnext = ((pl_sInt32) (VL*t));
        idUL = (iULnext - iUL)>>nmb;
        idVL = (iVLnext - iVL)>>nmb;
        n = nm;
        Xlen -= n;
        if (Xlen < 0) n += Xlen;
        if (zb) do {
            if (*zbuf < ZL) {
              int av;
              if (CL < 0) av=addtable[0];
              else if (CL > (255<<8)) av=addtable[255];
              else av=addtable[CL>>8];
              *zbuf = ZL;
              *gmem = remap[av +
                      texture[((iUL>>16)&MappingU_AND) +
                              ((iVL>>vshift)&MappingV_AND)]];
            }
            zbuf++;
            gmem++;
            ZL += dZL;
            CL += dCL;
            iUL += idUL;
            iVL += idVL;
          } while (--n);
        else do {
            int av;
            if (CL < 0) av=addtable[0];
            else if (CL > (255<<8)) av=addtable[255];
            else av=addtable[CL>>8];
            *gmem++ = remap[av +
                      texture[((iUL>>16)&MappingU_AND) +
                              ((iVL>>vshift)&MappingV_AND)]];
            CL += dCL;
            iUL += idUL;
            iVL += idVL;
          } while (--n);
      } while (Xlen > 0);
      gmem -= XL1;
      zbuf -= XL1;
    } else {
      zbuf += scrwidth;
      gmem += scrwidth;
    }
    Z1 += dZ1;
    U1 += dU1;
    V1 += dV1;
    X1 += dX1;
    X2 += dX2;
    C1 += dC1;
  }
}

PL_API void plPF_SolidF(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;

  pl_uChar *gmem = cam->frameBuffer;
  pl_ZBuffer *zbuf = cam->zBuffer;

  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_ZBuffer dZL=0, dZ1=0, dZ2=0, Z1, ZL, Z2, Z3;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_uChar stat;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;
  pl_uChar bc;
  pl_sInt32 shade;

  PUTFACE_SORT();

  shade=(pl_sInt32) (TriFace->fShade*(TriFace->Material->_ColorsUsed-1));
  if (shade < 0) shade=0;
  if (shade > (pl_sInt32) TriFace->Material->_ColorsUsed-1) shade=TriFace->Material->_ColorsUsed-1;
  bc=TriFace->Material->_ReMapTable[shade];

  X2 = X1 = TriFace->Scrx[i0];
  Z1 = TriFace->Scrz[i0];
  Z2 = TriFace->Scrz[i1];
  Z3 = TriFace->Scrz[i2];
  Y0 = (TriFace->Scry[i0]+(1<<19)) >> 20;
  Y1 = (TriFace->Scry[i1]+(1<<19)) >> 20;
  Y2 = (TriFace->Scry[i2]+(1<<19)) >> 20;

  dY = Y2-Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (Z3 - Z1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (Z2 - Z1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
    Z2 = Z1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      ZL = Z1; Z1 = Z2; Z2 = ZL;
      stat = 1|8;
    }
  }

  if (zb) {
    XL1 = ((dX1-dX2)*dY+(1<<19))>>20;
    if (XL1) dZL = ((dZ1-dZ2)*dY)/XL1;
    else {
      XL1 = (X2-X1+(1<<19))>>20;
      if (zb && XL1) dZL = (Z2-Z1)/XL1;
      else dZL = 0.0;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (Z3-Z1)/dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    XL2 -= XL1;
    if (XL2 > 0) {
      zbuf += XL1;
      gmem += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            *zbuf = ZL;
            *gmem = bc;
          }
          gmem++;
          zbuf++;
          ZL += dZL;
        } while (--XL2);
      else do *gmem++ = bc; while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    gmem += cam->ScreenWidth;
    zbuf += cam->ScreenWidth;
    Z1 += dZ1;
    X1 += dX1;
    X2 += dX2;
    Y0++;
  }
}

PL_API void plPF_SolidG(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_uChar *remap = TriFace->Material->_ReMapTable;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_ZBuffer dZL=0, dZ1=0, dZ2=0, Z1, Z2, ZL, Z3;
  pl_sInt32 dX1=0, dX2=0, X1, X2, XL1, XL2;
  pl_sInt32 C1, C2, dC1=0, dC2=0, dCL=0, CL, C3;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_uChar stat;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;

  pl_Float nc = (TriFace->Material->_ColorsUsed-1)*65536.0f;
  pl_sInt32 maxColor=((TriFace->Material->_ColorsUsed-1)<<16);
  pl_sInt32 maxColorNonShift=TriFace->Material->_ColorsUsed-1;

  PUTFACE_SORT();


  C1 = (pl_sInt32) (TriFace->Shades[i0]*nc);
  C2 = (pl_sInt32) (TriFace->Shades[i1]*nc);
  C3 = (pl_sInt32) (TriFace->Shades[i2]*nc);
  X2 = X1 = TriFace->Scrx[i0];
  Z1 = TriFace->Scrz[i0];
  Z2 = TriFace->Scrz[i1];
  Z3 = TriFace->Scrz[i2];

  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dC2 = (C3 - C1) / dY;
    dZ2 = (Z3 - Z1) / dY;
  }
  dY = Y1 - Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dC1 = (C2 - C1) / dY;
    dZ1 = (Z2 - Z1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dC2 ^= dC1; dC1 ^= dC2; dC2 ^= dC1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
    Z2 = Z1;
    C2 = C1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      stat = 2|4;
    } else {
      X1 = C1; C1 = C2; C2 = X1;
      ZL = Z1; Z1 = Z2; Z2 = ZL;
      X1 = TriFace->Scrx[i1];
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dCL = ((dC1-dC2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dCL = (C2-C1)/XL1;
      dZL = (Z2-Z1)/XL1;
    }
  }

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        dZ1 = (Z3-Z1)/dY;
        dC1 = (C3-C1) / dY;
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
      }
    }
    CL = C1;
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    XL2 -= XL1;
    if (XL2 > 0) {
      gmem += XL1;
      zbuf += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            *zbuf = ZL;
            if (CL >= maxColor) *gmem=remap[maxColorNonShift];
            else if (CL > 0) *gmem = remap[CL>>16];
            else *gmem = remap[0];
          }
          gmem++;
          zbuf++;
          ZL += dZL;
          CL += dCL;
        } while (--XL2);
      else do {
          if (CL >= maxColor) *gmem++=remap[maxColorNonShift];
          else if (CL > 0) *gmem++ = remap[CL>>16];
          else *gmem++ = remap[0];
          CL += dCL;
        } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    gmem += cam->ScreenWidth;
    zbuf += cam->ScreenWidth;
    X1 += dX1;
    X2 += dX2;
    C1 += dC1;
    Z1 += dZ1;
    Y0++;
  }
}

PL_API void plPF_TexEnv(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_uChar *remap;
  pl_ZBuffer *zbuf = cam->zBuffer;

  pl_sInt32 MappingU1, MappingU2, MappingU3;
  pl_sInt32 MappingV1, MappingV2, MappingV3;
  pl_sInt32 MappingU_AND, MappingV_AND;
  pl_sInt32 eMappingU1, eMappingU2, eMappingU3;
  pl_sInt32 eMappingV1, eMappingV2, eMappingV3;
  pl_sInt32 eMappingU_AND, eMappingV_AND;

  pl_uChar *texture, *environment;
  pl_uChar vshift;
  pl_uChar evshift;
  pl_uInt16 *addtable;
  pl_Texture *Texture, *Environment;
  pl_uChar stat;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;

  pl_sInt32 U1, V1, U2, V2, dU1=0, dU2=0, dV1=0, dV2=0, dUL=0, dVL=0, UL, VL;
  pl_sInt32 eU1, eV1, eU2, eV2, edU1=0, edU2=0, edV1=0,
            edV2=0, edUL=0, edVL=0, eUL, eVL;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_Float Z1, ZL, dZ1=0, dZ2=0, dZL=0, Z2;
  pl_sInt32 Y1, Y2, Y0, dY;

  Environment = TriFace->Material->Environment;
  Texture = TriFace->Material->Texture;

  if (!Texture || !Environment) return;
  texture = Texture->Data;
  environment = Environment->Data;
  addtable = TriFace->Material->_AddTable;
  remap = TriFace->Material->_ReMapTable;

  MappingV_AND = ((1<<Texture->Height)-1)<<Texture->Width;
  MappingU_AND = (1<<Texture->Width)-1;
  vshift = 16 - Texture->Width;
  eMappingV_AND = ((1<<Environment->Height)-1)<<Environment->Width;
  eMappingU_AND = (1<<Environment->Width)-1;
  evshift = 16 - Environment->Width;

  PUTFACE_SORT_TEX();

  eMappingU1=(pl_sInt32) (TriFace->eMappingU[i0]*Environment->uScale*TriFace->Material->EnvScaling);
  eMappingV1=(pl_sInt32) (TriFace->eMappingV[i0]*Environment->vScale*TriFace->Material->EnvScaling);
  eMappingU2=(pl_sInt32) (TriFace->eMappingU[i1]*Environment->uScale*TriFace->Material->EnvScaling);
  eMappingV2=(pl_sInt32) (TriFace->eMappingV[i1]*Environment->vScale*TriFace->Material->EnvScaling);
  eMappingU3=(pl_sInt32) (TriFace->eMappingU[i2]*Environment->uScale*TriFace->Material->EnvScaling);
  eMappingV3=(pl_sInt32) (TriFace->eMappingV[i2]*Environment->vScale*TriFace->Material->EnvScaling);

  U1 = U2 = MappingU1;
  V1 = V2 = MappingV1;
  eU1 = eU2 = eMappingU1;
  eV1 = eV2 = eMappingV1;
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dU2 = (MappingU3 - U1) / dY;
    dV2 = (MappingV3 - V1) / dY;
    edU2 = (eMappingU3 - eU1) / dY;
    edV2 = (eMappingV3 - eV1) / dY;
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dU1 = (MappingU2 - U1) / dY;
    dV1 = (MappingV2 - V1) / dY;
    edU1 = (eMappingU2 - eU1) / dY;
    edV1 = (eMappingV2 - eV1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dU2 ^= dU1; dU1 ^= dU2; dU2 ^= dU1;
      dV2 ^= dV1; dV1 ^= dV2; dV2 ^= dV1;
      edU2 ^= edU1; edU1 ^= edU2; edU2 ^= edU1;
      edV2 ^= edV1; edV1 ^= edV2; edV2 ^= edV1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      U2 = MappingU2;
      V2 = MappingV2;
      eU2 = eMappingU2;
      eV2 = eMappingV2;
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      U1 = MappingU2;
      V1 = MappingV2;
      eU1 = eMappingU2;
      eV1 = eMappingV2;
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dUL = ((dU1-dU2)*dY)/XL1;
    dVL = ((dV1-dV2)*dY)/XL1;
    edUL = ((edU1-edU2)*dY)/XL1;
    edVL = ((edV1-edV2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      edUL = (eU2-eU1)/XL1;
      edVL = (eV2-eV1)/XL1;
      dUL = (U2-U1)/XL1;
      dVL = (V2-V1)/XL1;
      dZL = (Z2-Z1)/XL1;
    }
  }

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1)/dY;
        dV1 = (MappingV3 - V1) / dY;
        dU1 = (MappingU3 - U1) / dY;
        edV1 = (eMappingV3 - eV1) / dY;
        edU1 = (eMappingU3 - eU1) / dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    UL = U1;
    VL = V1;
    eUL = eU1;
    eVL = eV1;
    if ((XL2-XL1) > 0) {
      XL2 -= XL1;
      gmem += XL1;
      zbuf += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            *zbuf = ZL;
            *gmem = remap[addtable[environment[
                ((eUL>>16)&eMappingU_AND)+((eVL>>evshift)&eMappingV_AND)]] +
                            texture[((UL>>16)&MappingU_AND) +
                                    ((VL>>vshift)&MappingV_AND)]];
          }
          zbuf++;
          gmem++;
          ZL += dZL;
          UL += dUL;
          VL += dVL;
          eUL += edUL;
          eVL += edVL;
        } while (--XL2);
      else do {
          *gmem++ = remap[addtable[environment[
              ((eUL>>16)&eMappingU_AND)+((eVL>>evshift)&eMappingV_AND)]] +
                          texture[((UL>>16)&MappingU_AND) +
                                  ((VL>>vshift)&MappingV_AND)]];
          UL += dUL;
          VL += dVL;
          eUL += edUL;
          eVL += edVL;
        } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    zbuf += cam->ScreenWidth;
    gmem += cam->ScreenWidth;
    Z1 += dZ1;
    X1 += dX1;
    X2 += dX2;
    U1 += dU1;
    V1 += dV1;
    eU1 += edU1;
    eV1 += edV1;
    Y0++;
  }
}

PL_API void plPF_TexF(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_sInt32 MappingU1, MappingU2, MappingU3;
  pl_sInt32 MappingV1, MappingV2, MappingV3;
  pl_sInt32 MappingU_AND, MappingV_AND;
  pl_uChar *texture;
  pl_uChar vshift;
  pl_uInt bc;
  pl_uChar *remap;
  pl_Texture *Texture;
  pl_uChar stat;

  pl_ZBuffer Z1, ZL, dZ1=0, dZL=0, Z2, dZ2=0;
  pl_sInt32 dU1=0, dV1=0, dU2=0, dV2=0, U1, V1, U2, V2;
  pl_sInt32 dUL=0, dVL=0, UL, VL;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;
  pl_sInt shade;

  if (TriFace->Material->Environment) Texture = TriFace->Material->Environment;
  else Texture = TriFace->Material->Texture;

  if (!Texture) return;
  remap = TriFace->Material->_ReMapTable;
  if (TriFace->Material->_AddTable)
  {
    shade=(pl_sInt)(TriFace->fShade*255.0f);
    if (shade < 0) shade=0;
    if (shade > 255) shade=255;
    bc = TriFace->Material->_AddTable[shade];
  }
  else bc=0;
  texture = Texture->Data;
  vshift = 16 - Texture->Width;
  MappingV_AND = ((1<<Texture->Height)-1)<<Texture->Width;
  MappingU_AND = (1<<Texture->Width)-1;

  if (TriFace->Material->Environment) {
    PUTFACE_SORT_ENV();
  } else {
    PUTFACE_SORT_TEX();
  }

  U1 = U2 = MappingU1;
  V1 = V2 = MappingV1;
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dV2 = (MappingV3 - V1) / dY;
    dU2 = (MappingU3 - U1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    dU1 = (MappingU2 - U1) / dY;
    dV1 = (MappingV2 - V1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dU2 ^= dU1; dU1 ^= dU2; dU2 ^= dU1;
      dV2 ^= dV1; dV1 ^= dV2; dV2 ^= dV1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      U2 = MappingU2;
      V2 = MappingV2;
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      U1 = MappingU2;
      V1 = MappingV2;
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dUL = ((dU1-dU2)*dY)/XL1;
    dVL = ((dV1-dV2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dUL = (U2-U1)/XL1;
      dVL = (V2-V1)/XL1;
      dZL = (Z2-Z1)/XL1;
    }
  }

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1) / dY;
        dV1 = (MappingV3 - V1) / dY;
        dU1 = (MappingU3 - U1) / dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    UL = U1;
    VL = V1;
    if ((XL2-XL1) > 0) {
      XL2 -= XL1;
      gmem += XL1;
      zbuf += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            *zbuf = ZL;
            *gmem = remap[bc + texture[((UL >> 16)&MappingU_AND) +
                                ((VL>>vshift)&MappingV_AND)]];
          }
          zbuf++;
          gmem++;
          ZL += dZL;
          UL += dUL;
          VL += dVL;
        } while (--XL2);
      else do {
          *gmem++ = remap[bc + texture[((UL >> 16)&MappingU_AND) +
                                ((VL>>vshift)&MappingV_AND)]];
          UL += dUL;
          VL += dVL;
        } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    zbuf += cam->ScreenWidth;
    gmem += cam->ScreenWidth;
    X1 += dX1;
    X2 += dX2;
    U1 += dU1;
    V1 += dV1;
    Z1 += dZ1;
    Y0++;
  }
}

PL_API void plPF_TexG(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_sInt32 MappingU1, MappingU2, MappingU3;
  pl_sInt32 MappingV1, MappingV2, MappingV3;
  pl_sInt32 MappingU_AND, MappingV_AND;
  pl_uChar *texture;
  pl_uChar *remap;
  pl_uChar vshift;
  pl_uInt16 *addtable;
  pl_Texture *Texture;

  pl_sInt32 U1, V1, U2, V2, dU1=0, dU2=0, dV1=0, dV2=0, dUL=0, dVL=0, UL, VL;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_sInt32 C1, C2, dC1=0, dC2=0, CL, dCL=0;
  pl_ZBuffer Z1, ZL, dZ1=0, dZ2=0, dZL=0, Z2;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_uChar stat;

  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;

  if (TriFace->Material->Environment) Texture = TriFace->Material->Environment;
  else Texture = TriFace->Material->Texture;

  if (!Texture) return;
  remap = TriFace->Material->_ReMapTable;
  texture = Texture->Data;
  addtable = TriFace->Material->_AddTable;
  vshift = 16 - Texture->Width;
  MappingV_AND = ((1<<Texture->Height)-1)<<Texture->Width;
  MappingU_AND = (1<<Texture->Width)-1;

  if (TriFace->Material->Environment) {
    PUTFACE_SORT_ENV();
  } else {
    PUTFACE_SORT_TEX();
  }

  C1 = C2 = TriFace->Shades[i0]*65535.0f;
  U1 = U2 = MappingU1;
  V1 = V2 = MappingV1;
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
    dC2 = (TriFace->Shades[i2]*65535.0f - C1) / dY;
    dU2 = (MappingU3 - U1) / dY;
    dV2 = (MappingV3 - V1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    dC1 = (TriFace->Shades[i1]*65535.0f - C1) / dY;
    dU1 = (MappingU2 - U1) / dY;
    dV1 = (MappingV2 - V1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dU2 ^= dU1; dU1 ^= dU2; dU2 ^= dU1;
      dV2 ^= dV1; dV1 ^= dV2; dV2 ^= dV1;
      dC2 ^= dC1; dC1 ^= dC2; dC2 ^= dC1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      C2 = TriFace->Shades[i1]*65535.0f;
      U2 = MappingU2;
      V2 = MappingV2;
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      C1 = TriFace->Shades[i1]*65535.0f;
      U1 = MappingU2;
      V1 = MappingV2;
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);

  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dUL = ((dU1-dU2)*dY)/XL1;
    dVL = ((dV1-dV2)*dY)/XL1;
    if (zb) dZL = ((dZ1-dZ2)*dY)/XL1;
    dCL = ((dC1-dC2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dUL = (U2-U1)/XL1;
      dVL = (V2-V1)/XL1;
      if (zb) dZL = (Z2-Z1)/XL1;
      dCL = (C2-C1)/(XL1);
    }
  }
  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1)/dY;
        dV1 = (MappingV3 - V1) / dY;
        dU1 = (MappingU3 - U1) / dY;
        dC1 = (TriFace->Shades[i2]*65535.0f-C1)/dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    CL = C1;
    ZL = Z1;
    UL = U1;
    VL = V1;
    if ((XL2-XL1) > 0) {
      XL2 -= XL1;
      gmem += XL1;
      zbuf += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            int av;
            if (CL < 0) av=addtable[0];
            else if (CL > (255<<8)) av=addtable[255];
            else av=addtable[CL>>8];
            *zbuf = ZL;
            *gmem = remap[av +
                            texture[((UL>>16)&MappingU_AND) +
                                    ((VL>>vshift)&MappingV_AND)]];
          }
          zbuf++;
          gmem++;
          ZL += dZL;
          CL += dCL;
          UL += dUL;
          VL += dVL;
        } while (--XL2);
      else do {
          int av;
          if (CL < 0) av=addtable[0];
          else if (CL > (255<<8)) av=addtable[255];
          else av=addtable[CL>>8];
          *gmem++ = remap[av +
                          texture[((UL>>16)&MappingU_AND) +
                                  ((VL>>vshift)&MappingV_AND)]];
          CL += dCL;
          UL += dUL;
          VL += dVL;
        } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    zbuf += cam->ScreenWidth;
    gmem += cam->ScreenWidth;
    Z1 += dZ1;
    X1 += dX1;
    X2 += dX2;
    C1 += dC1;
    U1 += dU1;
    V1 += dV1;
    Y0++;
  }
}

PL_API void plPF_TransF(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_uChar *remap = TriFace->Material->_ReMapTable;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_ZBuffer Z1, ZL, dZ1=0, dZL=0, dZ2=0, Z2;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_uInt16 *lookuptable = TriFace->Material->_AddTable;
  pl_uChar stat;
  pl_sInt32 bc = (pl_sInt32) TriFace->fShade*TriFace->Material->_tsfact;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;

  PUTFACE_SORT();

  if (bc < 0) bc=0;
  if (bc > (pl_sInt32) TriFace->Material->_tsfact-1) bc=TriFace->Material->_tsfact-1;
  remap+=bc;

  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      stat= 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      stat= 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);
  if (zb) {
    XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
    if (XL1) dZL = ((dZ1-dZ2)*dY)/XL1;
    else {
      XL1 = ((X2-X1+(1<<19))>>20);
      if (XL1) dZL = (Z2-Z1)/XL1;
    }
  }

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]- Z1)/dY;
      }
    }
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    if ((XL2-XL1) > 0) {
      XL2 -= XL1;
      zbuf += XL1;
      gmem += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            *zbuf = ZL;
            *gmem = remap[lookuptable[*gmem]];
          }
          gmem++;
          zbuf++;
          ZL += dZL;
        } while (--XL2);
      else do { *gmem = remap[lookuptable[*gmem]]; gmem++; } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    gmem += cam->ScreenWidth;
    zbuf += cam->ScreenWidth;
    Z1 += dZ1;
    X1 += dX1;
    X2 += dX2;
    Y0 ++;
  }
}

PL_API void plPF_TransG(pl_Cam *cam, pl_Face *TriFace) {
  pl_uChar i0, i1, i2;
  pl_uChar *gmem = cam->frameBuffer;
  pl_uChar *remap = TriFace->Material->_ReMapTable;
  pl_ZBuffer *zbuf = cam->zBuffer;
  pl_sInt32 X1, X2, dX1=0, dX2=0, XL1, XL2;
  pl_ZBuffer Z1, ZL, dZ1=0, dZL=0, dZ2=0, Z2;
  pl_sInt32 dC1=0, dCL=0, CL, C1, C2, dC2=0;
  pl_sInt32 Y1, Y2, Y0, dY;
  pl_Float nc = (TriFace->Material->_tsfact*65536.0f);
  pl_uInt16 *lookuptable = TriFace->Material->_AddTable;
  pl_Bool zb = (zbuf&&TriFace->Material->zBufferable) ? 1 : 0;
  pl_uChar stat;

  pl_sInt32 maxColor=((TriFace->Material->_tsfact-1)<<16);
  pl_sInt32 maxColorNonShift=TriFace->Material->_tsfact-1;

  PUTFACE_SORT();

  C1 = C2 = (pl_sInt32) (TriFace->Shades[i0]*nc);
  X2 = X1 = TriFace->Scrx[i0];
  Z2 = Z1 = TriFace->Scrz[i0];
  Y0 = (TriFace->Scry[i0]+(1<<19))>>20;
  Y1 = (TriFace->Scry[i1]+(1<<19))>>20;
  Y2 = (TriFace->Scry[i2]+(1<<19))>>20;

  dY = Y2 - Y0;
  if (dY) {
    dX2 = (TriFace->Scrx[i2] - X1) / dY;
    dC2 = (pl_sInt32) ((TriFace->Shades[i2]*nc - C1) / dY);
    dZ2 = (TriFace->Scrz[i2] - Z1) / dY;
  }
  dY = Y1-Y0;
  if (dY) {
    dX1 = (TriFace->Scrx[i1] - X1) / dY;
    dZ1 = (TriFace->Scrz[i1] - Z1) / dY;
    dC1 = (pl_sInt32) ((TriFace->Shades[i1]*nc - C1) / dY);
    if (dX2 < dX1) {
      dX2 ^= dX1; dX1 ^= dX2; dX2 ^= dX1;
      dC2 ^= dC1; dC1 ^= dC2; dC2 ^= dC1;
      dZL = dZ1; dZ1 = dZ2; dZ2 = dZL;
      stat = 2;
    } else stat = 1;
  } else {
    if (TriFace->Scrx[i1] > X1) {
      X2 = TriFace->Scrx[i1];
      Z2 = TriFace->Scrz[i1];
      C2 = (pl_sInt32) (TriFace->Shades[i1]*nc);
      stat = 2|4;
    } else {
      X1 = TriFace->Scrx[i1];
      Z1 = TriFace->Scrz[i1];
      C1 = (pl_sInt32) (TriFace->Shades[i1]*nc);
      stat = 1|8;
    }
  }

  gmem += (Y0 * cam->ScreenWidth);
  zbuf += (Y0 * cam->ScreenWidth);
  XL1 = (((dX1-dX2)*dY+(1<<19))>>20);
  if (XL1) {
    dCL = ((dC1-dC2)*dY)/XL1;
    dZL = ((dZ1-dZ2)*dY)/XL1;
  } else {
    XL1 = ((X2-X1+(1<<19))>>20);
    if (XL1) {
      dCL = (C2-C1)/XL1;
      dZL = (Z2-Z1)/XL1;
    }
  }

  while (Y0 < Y2) {
    if (Y0 == Y1) {
      dY = Y2 - ((TriFace->Scry[i1]+(1<<19))>>20);
      if (dY) {
        if (stat & 1) {
          X1 = TriFace->Scrx[i1];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 2) {
          X2 = TriFace->Scrx[i1];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i1])/dY;
        }
        if (stat & 4) {
          X1 = TriFace->Scrx[i0];
          dX1 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        if (stat & 8) {
          X2 = TriFace->Scrx[i0];
          dX2 = (TriFace->Scrx[i2]-TriFace->Scrx[i0])/dY;
        }
        dZ1 = (TriFace->Scrz[i2]-Z1)/dY;
        dC1 = (pl_sInt32) ((TriFace->Shades[i2]*nc - C1) / dY);
      }
    }
    CL = C1;
    XL1 = (X1+(1<<19))>>20;
    XL2 = (X2+(1<<19))>>20;
    ZL = Z1;
    if ((XL2-XL1) > 0) {
      XL2 -= XL1;
      zbuf += XL1;
      gmem += XL1;
      XL1 += XL2;
      if (zb) do {
          if (*zbuf < ZL) {
            int av;
            if (CL >= maxColor) av=maxColorNonShift;
            else if (CL > 0) av=CL>>16;
            else av=0;
            *zbuf = ZL;
            *gmem = remap[av + lookuptable[*gmem]];
          }
          gmem++;
          CL += dCL;
          zbuf++;
          ZL += dZL;
        } while (--XL2);
      else do {
          int av;
          if (CL >= maxColor) av=maxColorNonShift;
          else if (CL > 0) av=CL>>16;
          else av=0;
          *gmem = remap[av + lookuptable[*gmem]];
		  gmem++;
          CL += dCL;
        } while (--XL2);
      gmem -= XL1;
      zbuf -= XL1;
    }
    gmem += cam->ScreenWidth;
    zbuf += cam->ScreenWidth;
    Z1 += dZ1;
    X1 += dX1;
    X2 += dX2;
    C1 += dC1;
    Y0++;
  }
}

/* Can't find another place to put this... */
PL_API void plTexDelete(pl_Texture *t) {
  if (t) {
    if (t->Data) free(t->Data);
    if (t->PaletteData) free(t->PaletteData);
    free(t);
  }
}

typedef struct {
    pl_uInt16 id;
    void (*func)(FILE *f, pl_uInt32 p);
} _pl_3DSChunk;

static pl_Obj *obj;
static pl_Obj *bobj;
static pl_Obj *lobj;
static pl_sInt16 currentobj;
static pl_Mat *_m;

static pl_Float _pl3DSReadFloat(FILE *f);
static pl_uInt32 _pl3DSReadDWord(FILE *f);
static pl_uInt16 _pl3DSReadWord(FILE *f);
static void _pl3DSChunkReader(FILE *f, pl_uInt32 p);
static void _pl3DSRGBFReader(FILE *f, pl_uInt32 p);
static void _pl3DSRGBBReader(FILE *f, pl_uInt32 p);
static void _pl3DSASCIIZReader(FILE *f, pl_uInt32 p, char *as);
static void _pl3DSObjBlockReader(FILE *f, pl_uInt32 p);
static void _pl3DSTriMeshReader(FILE *f, pl_uInt32 p);
static void _pl3DSVertListReader(FILE *f, pl_uInt32 p);
static void _pl3DSFaceListReader(FILE *f, pl_uInt32 p);
static void _pl3DSFaceMatReader(FILE *f, pl_uInt32 p);
static void MapListReader(FILE *f, pl_uInt32 p);
static pl_sInt16 _pl3DSFindChunk(pl_uInt16 id);

static _pl_3DSChunk _pl3DSChunkNames[] = {
    {0x4D4D,NULL}, /* Main */
    {0x3D3D,NULL}, /* Object Mesh */
    {0x4000,_pl3DSObjBlockReader},
    {0x4100,_pl3DSTriMeshReader},
    {0x4110,_pl3DSVertListReader},
    {0x4120,_pl3DSFaceListReader},
    {0x4130,_pl3DSFaceMatReader},
    {0x4140,MapListReader},
    {0xAFFF,NULL}, /* Material */
    {0xA010,NULL}, /* Ambient */
    {0xA020,NULL}, /* Diff */
    {0xA030,NULL}, /* Specular */
    {0xA200,NULL}, /* Texture */
    {0x0010,_pl3DSRGBFReader},
    {0x0011,_pl3DSRGBBReader},
};

PL_API pl_Obj *plRead3DSObj(char *fn, pl_Mat *m) {
  FILE *f;
  pl_uInt32 p;
  _m = m;
  obj = bobj = lobj = 0;
  currentobj = 0;
  f = fopen(fn, "rb");
  if (!f) return 0;
  fseek(f, 0, 2);
  p = ftell(f);
  rewind(f);
  _pl3DSChunkReader(f, p);
  fclose(f);
  return bobj;
}

static pl_Float _pl3DSReadFloat(FILE *f) {
  pl_uInt32 *i;
  pl_IEEEFloat32 c;
  i = (pl_uInt32 *) &c;
  *i = _pl3DSReadDWord(f);
  return ((pl_Float) c);
}

static pl_uInt32 _pl3DSReadDWord(FILE *f) {
  pl_uInt32 r;
  r = fgetc(f);
  r |= fgetc(f)<<8;
  r |= fgetc(f)<<16;
  r |= fgetc(f)<<24;
  return r;
}

static pl_uInt16 _pl3DSReadWord(FILE *f) {
  pl_uInt16 r;
  r = fgetc(f);
  r |= fgetc(f)<<8;
  return r;
}

static void _pl3DSRGBFReader(FILE *f, pl_uInt32 p) {
  pl_Float c[3];
  c[0] = _pl3DSReadFloat(f);
  c[1] = _pl3DSReadFloat(f);
  c[2] = _pl3DSReadFloat(f);
}

static void _pl3DSRGBBReader(FILE *f, pl_uInt32 p) {
  unsigned char c[3];
  if (fread(&c, sizeof(c), 1, f) != 1) return;
}

static void _pl3DSASCIIZReader(FILE *f, pl_uInt32 p, char *as) {
  char c;
  if (!as) while ((c = fgetc(f)) != EOF && c != '\0');
  else {
    while ((c = fgetc(f)) != EOF && c != '\0') *as++ = c;
    *as = 0;
  }
}

static void _pl3DSObjBlockReader(FILE *f, pl_uInt32 p) {
  _pl3DSASCIIZReader(f,p,0);
  _pl3DSChunkReader(f, p);
}

static void _pl3DSTriMeshReader(FILE *f, pl_uInt32 p) {
  pl_uInt32 i;
  pl_Face *face;
  obj = plObjCreate(0,0);
  _pl3DSChunkReader(f, p);
  i = obj->NumFaces;
  face = obj->Faces;
  while (i--) {
    face->Vertices[0] = obj->Vertices + (uintptr_t) face->Vertices[0];
    face->Vertices[1] = obj->Vertices + (uintptr_t) face->Vertices[1];
    face->Vertices[2] = obj->Vertices + (uintptr_t) face->Vertices[2];
    face->MappingU[0] = face->Vertices[0]->xformedx;
    face->MappingV[0] = face->Vertices[0]->xformedy;
    face->MappingU[1] = face->Vertices[1]->xformedx;
    face->MappingV[1] = face->Vertices[1]->xformedy;
    face->MappingU[2] = face->Vertices[2]->xformedx;
    face->MappingV[2] = face->Vertices[2]->xformedy;
    face++;
  }
  plObjCalcNormals(obj);
  if (currentobj == 0) {
    currentobj = 1;
    lobj = bobj = obj;
  } else {
    lobj->Children[0] = obj;
    lobj = obj;
  }
}

static void _pl3DSVertListReader(FILE *f, pl_uInt32 p) {
  pl_uInt16 nv;
  pl_Vertex *v;
  nv = _pl3DSReadWord(f);
  obj->NumVertices = nv;
  v = obj->Vertices = (pl_Vertex *) calloc(sizeof(pl_Vertex)*nv,1);
  while (nv--) {
    v->x = _pl3DSReadFloat(f);
    v->y = _pl3DSReadFloat(f);
    v->z = _pl3DSReadFloat(f);
    if (feof(f)) return;
    v++;
  }
}

static void _pl3DSFaceListReader(FILE *f, pl_uInt32 p) {
  pl_uInt16 nv;
  pl_uInt16 c[3];
  pl_uInt16 flags;
  pl_Face *face;

  nv = _pl3DSReadWord(f);
  obj->NumFaces = nv;
  face = obj->Faces = (pl_Face *) calloc(sizeof(pl_Face)*nv,1);
  while (nv--) {
    c[0] = _pl3DSReadWord(f);
    c[1] = _pl3DSReadWord(f);
    c[2] = _pl3DSReadWord(f);
    flags = _pl3DSReadWord(f);
    if (feof(f)) return;
    face->Vertices[0] = (pl_Vertex *) (uintptr_t) c[0];
    face->Vertices[1] = (pl_Vertex *) (uintptr_t) c[1];
    face->Vertices[2] = (pl_Vertex *) (uintptr_t) c[2];
    face->Material = _m;
    face++;
  }
  _pl3DSChunkReader(f, p);
}

static void _pl3DSFaceMatReader(FILE *f, pl_uInt32 p) {
  pl_uInt16 n, nf;

  _pl3DSASCIIZReader(f, p,0);

  n = _pl3DSReadWord(f);
  while (n--) {
    nf = _pl3DSReadWord(f);
  }
}

static void MapListReader(FILE *f, pl_uInt32 p) {
  pl_uInt16 nv;
  pl_Float c[2];
  pl_Vertex *v;
  nv = _pl3DSReadWord(f);
  v = obj->Vertices;
  if (nv == obj->NumVertices) while (nv--) {
    c[0] = _pl3DSReadFloat(f);
    c[1] = _pl3DSReadFloat(f);
    if (feof(f)) return;
    v->xformedx = (pl_sInt32) (c[0]*65536.0);
    v->xformedy = (pl_sInt32) (c[1]*65536.0);
    v++;
  }
}

static pl_sInt16 _pl3DSFindChunk(pl_uInt16 id) {
  pl_sInt16 i;
  for (i = 0; i < sizeof(_pl3DSChunkNames)/sizeof(_pl3DSChunkNames[0]); i++)
    if (id == _pl3DSChunkNames[i].id) return i;
  return -1;
}

static void _pl3DSChunkReader(FILE *f, pl_uInt32 p) {
  pl_uInt32 hlen;
  pl_uInt16 hid;
  pl_sInt16 n;
  pl_uInt32 pc;

  while (ftell(f) < (int)p) {
    pc = ftell(f);
    hid = _pl3DSReadWord(f); if (feof(f)) return;
    hlen = _pl3DSReadDWord(f); if (feof(f)) return;
    if (hlen == 0) return;
    n = _pl3DSFindChunk(hid);
    if (n < 0) fseek(f, pc + hlen, 0);
    else {
      pc += hlen;
      if (_pl3DSChunkNames[n].func != NULL) _pl3DSChunkNames[n].func(f, pc);
      else _pl3DSChunkReader(f, pc);
      fseek(f, pc, 0);
    }
    if (ferror(f)) break;
  }
}

PL_API pl_Obj *plReadCOBObj(char *fn, pl_Mat *mat) {
  FILE *fp = fopen(fn,"rt");
  long int p1,m1,p2,m2,p3,m3;
  char temp_string[PL_COB_MAX_LINELENGTH];
  float TransMatrix[4][4];
  pl_Obj *obj;
  pl_sInt32 x,i2;
  long int numVertices, numMappingVertices, numFaces, i;
  pl_sInt32 *MappingVertices = 0;
  if (!fp) return 0;

  fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  if (memcmp("Caligari",temp_string,8)) { fclose(fp); return 0; }

  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("Transform",temp_string,9));
  if (feof(fp)) { fclose(fp); return 0; }
  fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  sscanf(temp_string,"%f %f %f %f",
   &TransMatrix[0][0],&TransMatrix[0][1],&TransMatrix[0][2],&TransMatrix[0][3]);
  fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  sscanf(temp_string,"%f %f %f %f",
   &TransMatrix[1][0],&TransMatrix[1][1],&TransMatrix[1][2],&TransMatrix[1][3]);
  fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  sscanf(temp_string,"%f %f %f %f",
   &TransMatrix[2][0],&TransMatrix[2][1],&TransMatrix[2][2],&TransMatrix[2][3]);
  fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  sscanf(temp_string,"%f %f %f %f",
   &TransMatrix[3][0],&TransMatrix[3][1],&TransMatrix[3][2],&TransMatrix[3][3]);

  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("World Vertices",temp_string,12));
  if (feof(fp) ||  sscanf(temp_string,"World Vertices %ld",&numVertices) != 1)
    { fclose(fp); return 0; }

  rewind(fp);
  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("Texture Vertices",temp_string,16));
  if (feof(fp) ||
      sscanf(temp_string,"Texture Vertices %ld",&numMappingVertices) != 1) {
    fclose(fp); return 0;
  }

  rewind(fp);
  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("Faces",temp_string,5));
  if (feof(fp) || sscanf(temp_string,"Faces %ld",&numFaces) != 1) {
    fclose(fp); return 0;
  }
  for (x = numFaces; x; x--) {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
    if (feof(fp) || sscanf(temp_string+4," verts %ld",&i) != 1 || i < 3) {
      fclose(fp);
      return 0;
    }
    numFaces += i-3;
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  }
  obj = plObjCreate(numVertices,numFaces);
  if (!obj) { fclose(fp); return 0; }
  rewind(fp);
  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("World Vertices",temp_string,12));
  if (feof(fp)) { plObjDelete(obj); fclose(fp); return 0; }
  for (x = 0; x < numVertices; x ++) {
    float xp, yp, zp;
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
    if (feof(fp) ||
        sscanf(temp_string,"%f %f %f", &xp, &yp, &zp) != 3) {
      plObjDelete(obj); fclose(fp); return 0;
    }
    obj->Vertices[x].x = (TransMatrix[0][0]*xp+TransMatrix[0][1]*yp+
                          TransMatrix[0][2]*zp+TransMatrix[0][3]);
    obj->Vertices[x].y = (TransMatrix[1][0]*xp+TransMatrix[1][1]*yp+
                          TransMatrix[1][2]*zp+TransMatrix[1][3]);
    obj->Vertices[x].z = (TransMatrix[2][0]*xp+TransMatrix[2][1]*yp+
                          TransMatrix[2][2]*zp+TransMatrix[2][3]);
  }
  rewind(fp);
  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("Texture Vertices",temp_string,16));
  if (!feof(fp)) {
    MappingVertices = (pl_sInt32 *)
      malloc(sizeof(pl_sInt32) * numMappingVertices * 2);
    if (MappingVertices) {
      for (x = 0; x < numMappingVertices; x ++) {
        float p1, p2;
        fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
        if (feof(fp) || sscanf(temp_string,"%f %f", &p1, &p2) != 2) {
          free(MappingVertices); plObjDelete(obj); fclose(fp); return 0;
        }
        MappingVertices[x*2] = (pl_sInt32) (p1*65536.0);
        MappingVertices[x*2+1] = (pl_sInt32) (p2*65536.0);
      }
    }
  }
  rewind(fp);
  do {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
  } while (!feof(fp) && memcmp("Faces",temp_string,5));
  if (feof(fp)) {
    if (MappingVertices) free(MappingVertices);
    plObjDelete(obj); fclose(fp); return 0;
  }
  for (x = 0; x < numFaces; x ++) {
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
    sscanf(temp_string+4," verts %ld",&i);
    fgets(temp_string,PL_COB_MAX_LINELENGTH,fp);
    if (i == 3) {
      if (feof(fp) || sscanf(temp_string,"<%ld,%ld> <%ld,%ld> <%ld,%ld>",
                             &p3,&m3,&p2,&m2,&p1,&m1) != 6) {
        if (MappingVertices) free(MappingVertices);
        plObjDelete(obj); fclose(fp); return 0;
      }
      obj->Faces[x].Vertices[0] = obj->Vertices + p1;
      obj->Faces[x].Vertices[1] = obj->Vertices + p2;
      obj->Faces[x].Vertices[2] = obj->Vertices + p3;
      if (MappingVertices) {
        obj->Faces[x].MappingU[0] = MappingVertices[m1*2];
        obj->Faces[x].MappingV[0] = MappingVertices[m1*2+1];
        obj->Faces[x].MappingU[1] = MappingVertices[m2*2];
        obj->Faces[x].MappingV[1] = MappingVertices[m2*2+1];
        obj->Faces[x].MappingU[2] = MappingVertices[m3*2];
        obj->Faces[x].MappingV[2] = MappingVertices[m3*2+1];
      }
      obj->Faces[x].Material = mat;
    } else {
      long int p[16],m[16];
      if (feof(fp)) {
        if (MappingVertices) free(MappingVertices);
        plObjDelete(obj); fclose(fp); return 0;
      }
      sscanf(temp_string,
         "<%ld,%ld> <%ld,%ld> <%ld,%ld> <%ld,%ld> "
         "<%ld,%ld> <%ld,%ld> <%ld,%ld> <%ld,%ld> "
         "<%ld,%ld> <%ld,%ld> <%ld,%ld> <%ld,%ld> "
         "<%ld,%ld> <%ld,%ld> <%ld,%ld> <%ld,%ld> ",
          p+0,m+0,p+1,m+1,p+2,m+2,p+3,m+3,
          p+4,m+4,p+5,m+5,p+6,m+6,p+7,m+7,
          p+8,m+8,p+9,m+9,p+10,m+10,p+11,m+11,
          p+12,m+12,p+13,m+13,p+14,m+14,p+15,m+15);
      for (i2 = 1; i2 < (i-1); i2 ++) {
        obj->Faces[x].Vertices[0] = obj->Vertices + p[0];
        obj->Faces[x].Vertices[1] = obj->Vertices + p[i2+1];
        obj->Faces[x].Vertices[2] = obj->Vertices + p[i2];
        if (MappingVertices) {
          obj->Faces[x].MappingU[0] = MappingVertices[m[0]*2];
          obj->Faces[x].MappingV[0] = MappingVertices[m[0]*2+1];
          obj->Faces[x].MappingU[1] = MappingVertices[m[i2+1]*2];
          obj->Faces[x].MappingV[1] = MappingVertices[m[i2+1]*2+1];
          obj->Faces[x].MappingU[2] = MappingVertices[m[i2]*2];
          obj->Faces[x].MappingV[2] = MappingVertices[m[i2]*2+1];
        }
        obj->Faces[x].Material = mat;
        x++;
      }
      x--;
    }
  }
  obj->BackfaceCull = 1;
  if (MappingVertices) free(MappingVertices);
  plObjCalcNormals(obj);
  fclose(fp);
  return obj;
}

/*******************************************************************************
 Notes on .JAW files:
   This is a file format created by Jawed Karim for Jaw3D
     (http://jaw3d.home.ml.org).
          -- updated 11/6/00 - www.jawed.com
   It is very simple, and lets one easily create ones own models using only
   a text editor. The format is pretty simple:
     The first line must be "Light: (x,y,z)" where x,y, and z are the x y and
        z components of the lightsource vector (I think ;)
     A series of lines, numbered 0 to n, in the format of
        "i: x y z", where i is the vertex number (which should be listed in
        order, and x y and z are the coordinates of that vertex.
     A series of lines, having the format "tri a, b, c" where a b and c are
        the vertices that the face uses. It is unclear at this time which
        way the vertices are listed (ccw or cw), so just make em consistent
        and you can always use plFlipObjectNormals() on the loaded object.
   That is it! (I told ya it was simple).
******************************************************************************/
PL_API pl_Obj *plReadJAWObj(char *filename, pl_Mat *m) {
  FILE *jawfile;
  pl_Obj *obj;
  pl_uInt32 i;
  pl_sInt crap;
  char line[256];
  pl_uInt32 total_points = 0, total_polys = 0;
  if ((jawfile = fopen(filename, "r")) == NULL) return 0;
  fgets(line, 256, jawfile); /* Ignores lightsource info */
  while (fgets(line, 256, jawfile) != NULL)
    if (strstr(line, ":") != NULL) total_points++;

  rewind(jawfile); fgets(line, 256, jawfile);
  while (fgets(line, 256, jawfile) != NULL)
    if (strstr(line, "tri") != NULL) total_polys++;

  rewind(jawfile); fgets(line, 256, jawfile);
  obj = plObjCreate(total_points,total_polys);

  i = 0;
  while (fgets(line, 256, jawfile) != NULL) if (strstr(line, ":") != NULL) {
    float x, y, z;
    sscanf(line, "%d: %f %f %f",&crap,&x,&y,&z);
    obj->Vertices[i].x = (pl_Float) x;
    obj->Vertices[i].y = (pl_Float) y;
    obj->Vertices[i].z = (pl_Float) z;
    i++;
  }
  rewind(jawfile); fgets(line, 256, jawfile);
  i = 0;
  while (fgets(line, 256, jawfile) != NULL) if (strstr(line, "tri") != NULL) {
    pl_uInt32 a,b,c;
    sscanf(line, "tri %ld, %ld, %ld", &a, &b, &c);
    obj->Faces[i].Vertices[0] = obj->Vertices + a;
    obj->Faces[i].Vertices[1] = obj->Vertices + c;
    obj->Faces[i].Vertices[2] = obj->Vertices + b;
    obj->Faces[i].Material = m;
    i++;
  }
  fclose(jawfile);
  plObjCalcNormals(obj);
  return obj;
}

static pl_uInt _plHiBit(pl_uInt16);
static pl_uInt _plOptimizeImage(pl_uChar *, pl_uChar *, pl_uInt32);
static pl_sInt _plReadPCX(char *filename, pl_uInt16 *width, pl_uInt16 *height,
                          pl_uChar **pal, pl_uChar **data);
static void _plRescaleImage(pl_uChar *in, pl_uChar *out, pl_uInt inx,
                            pl_uInt iny, pl_uInt outx, pl_uInt outy);

PL_API pl_Texture *plReadPCXTex(char *fn, pl_Bool rescale, pl_Bool optimize) {
  pl_uChar *data, *pal;
  pl_uInt16 x, y;
  pl_Texture *t;
  if (_plReadPCX(fn,&x,&y,&pal,&data) < 0) return 0;
  t = (pl_Texture *) malloc(sizeof(pl_Texture));
  if (!t) return 0;
  t->Width = _plHiBit(x);
  t->Height = _plHiBit(y);
  if (rescale && (1 << t->Width != x || 1 << t->Height != y)) {
    pl_uChar nx, ny, *newdata;
    nx = t->Width;
    if ((1 << t->Width) != x) nx++;
    ny = t->Height;
    if ((1 << t->Height) != y) ny++;
    newdata = (pl_uChar *) malloc((1<<nx)*(1<<ny));
    if (!newdata) {
      free(t);
      free(data);
      free(pal);
      return 0;
    }
    _plRescaleImage(data,newdata,x,y,1<<nx,1<<ny);
    free(data);
    data = newdata;
    t->Width = nx;
    t->Height = ny;
    x = 1<<nx; y = 1<<ny;
  }
  t->iWidth = x;
  t->iHeight = y;
  t->uScale = (pl_Float) (1<<t->Width);
  t->vScale = (pl_Float) (1<<t->Height);
  if (optimize) t->NumColors = _plOptimizeImage(pal, data,x*y);
  else t->NumColors = 256;
  t->Data = data;
  t->PaletteData = pal;
  return t;
}


static pl_uInt _plHiBit(pl_uInt16 x) {
  pl_uInt i = 16, mask = 1<<15;
  while (mask) {
    if (x & mask) return i;
    mask >>= 1; i--;
  }
  return 0;
}

static pl_uInt _plOptimizeImage(pl_uChar *pal, pl_uChar *data, pl_uInt32 len) {
  pl_uChar colors[256], *dd = data;
  pl_uChar remap[256];
  pl_sInt32 lastused, firstunused;
  pl_uInt32 x;
  memset(colors,0,256);
  for (x = 0; x < len; x ++) colors[(pl_uInt) *dd++] = 1;
  lastused = -1;
  for (x = 0; x < 256; x ++) remap[x] = (pl_uChar)x;
  lastused = 255;
  firstunused = 0;
  for (;;) {
    while (firstunused < 256 && colors[firstunused]) firstunused++;
    if (firstunused > 255) break;
    while (lastused >= 0 && !colors[lastused]) lastused--;
    if (lastused < 0) break;
	if (lastused <= firstunused) break;
    pal[firstunused*3] = pal[lastused*3];
    pal[firstunused*3+1] = pal[lastused*3+1];
    pal[firstunused*3+2] = pal[lastused*3+2];
    colors[lastused] = 0;
    colors[firstunused] = 1;
	  remap[lastused] = (pl_uChar) firstunused;
  }
  x = len;
  while (x--) { *data = remap[(pl_uInt) *data]; data++; };
  return (lastused+1);
}

static pl_sInt _plReadPCX(char *filename, pl_uInt16 *width, pl_uInt16 *height,
                            pl_uChar **pal, pl_uChar **data) {
  pl_uInt16 sx, sy, ex, ey;
  FILE *fp = fopen(filename,"rb");
  pl_uChar *data2;
  if (!fp) return -1;
  fgetc(fp);
  if (fgetc(fp) != 5) { fclose(fp); return -2; }
  if (fgetc(fp) != 1) { fclose(fp); return -2; }
  if (fgetc(fp) != 8) { fclose(fp); return -3; }
  sx = fgetc(fp); sx |= fgetc(fp)<<8;
  sy = fgetc(fp); sy |= fgetc(fp)<<8;
  ex = fgetc(fp); ex |= fgetc(fp)<<8;
  ey = fgetc(fp); ey |= fgetc(fp)<<8;
  *width = ex - sx + 1;
  *height = ey - sy + 1;
  fseek(fp,128,SEEK_SET);
  if (feof(fp)) { fclose(fp); return -4; }
  *data = (pl_uChar *) malloc((*width) * (*height));
  if (!*data) { fclose(fp); return -128; }
  sx = *height;
  data2 = *data;
  do {
    int xpos = 0;
    do {
      char c = fgetc(fp);
      if ((c & 192) == 192) {
        char oc = fgetc(fp);
        c &= ~192;
        do {
          *(data2++) = oc;
          xpos++;
        } while (--c && xpos < *width);
      } else {
        *(data2++) = c;
        xpos++;
      }
    } while (xpos < *width);
  } while (--sx);
  if (feof(fp)) { fclose(fp); free(*data); return -5; }
  fseek(fp,-769,SEEK_END);
  if (fgetc(fp) != 12) { fclose(fp); free(*data); return -6; }
  *pal = (pl_uChar *) malloc(768);
  if (!*pal) { fclose(fp); free(*data); return -7; }
  fread(*pal,3,256,fp);
  fclose(fp);
  return 0;
}

static void _plRescaleImage(pl_uChar *in, pl_uChar *out, pl_uInt inx,
                            pl_uInt iny, pl_uInt outx, pl_uInt outy) {
  pl_uInt x;
  pl_uInt32 X, dX, dY, Y;
  dX = (inx<<16) / outx;
  dY = (iny<<16) / outy;
  Y = 0;
  do {
    pl_uChar *ptr = in + inx*(Y>>16);
    X = 0;
    Y += dY;
    x = outx;
    do {
      *out++ = ptr[X>>16];
      X += dX;
    } while (--x);
  } while (--outy);
}

typedef struct {
  pl_Float zd;
  pl_Face *face;
} _faceInfo;

typedef struct {
  pl_Light *light;
  pl_Float l[3];
} _lightInfo;

#define MACRO_plMatrixApply(m,x,y,z,outx,outy,outz) \
      ( outx ) = ( x )*( m )[0] + ( y )*( m )[1] + ( z )*( m )[2] + ( m )[3];\
      ( outy ) = ( x )*( m )[4] + ( y )*( m )[5] + ( z )*( m )[6] + ( m )[7];\
      ( outz ) = ( x )*( m )[8] + ( y )*( m )[9] + ( z )*( m )[10] + ( m )[11]

#define MACRO_plDotProduct(x1,y1,z1,x2,y2,z2) \
      ((( x1 )*( x2 ))+(( y1 )*( y2 ))+(( z1 )*( z2 )))

#define MACRO_plNormalizeVector(x,y,z) { \
  register double length; \
  length = ( x )*( x )+( y )*( y )+( z )*( z ); \
  if (length > 0.0000000001) { \
    pl_Float l = (pl_Float) sqrt(length); \
    ( x ) /= l; \
    ( y ) /= l; \
    ( z ) /= l; \
  } \
}

static pl_uInt32 _numfaces;
static _faceInfo _faces[PL_MAX_TRIANGLES];

static pl_Float _cMatrix[16];
static pl_uInt32 _numlights;
static _lightInfo _lights[PL_MAX_LIGHTS];
static pl_Cam *_cam;
static void _RenderObj(pl_Obj *, pl_Float *, pl_Float *);
static void _sift_down(int L, int U, int dir);
static void _hsort(_faceInfo *base, int nel, int dir);

PL_API void plRenderBegin(pl_Cam *Camera) {
  pl_Float tempMatrix[16];
  memset(plRender_TriStats,0,sizeof(plRender_TriStats));
  _cam = Camera;
  _numlights = 0;
  _numfaces = 0;
  plMatrixRotate(_cMatrix,2,-Camera->Pan);
  plMatrixRotate(tempMatrix,1,-Camera->Pitch);
  plMatrixMultiply(_cMatrix,tempMatrix);
  plMatrixRotate(tempMatrix,3,-Camera->Roll);
  plMatrixMultiply(_cMatrix,tempMatrix);
  plClipSetFrustum(_cam);
}

PL_API void plRenderLight(pl_Light *light) {
  pl_Float *pl, xp, yp, zp;
  if (light->Type == PL_LIGHT_NONE || _numlights >= PL_MAX_LIGHTS) return;
  pl = _lights[_numlights].l;
  if (light->Type == PL_LIGHT_VECTOR) {
    xp = light->Xp;
    yp = light->Yp;
    zp = light->Zp;
    MACRO_plMatrixApply(_cMatrix,xp,yp,zp,pl[0],pl[1],pl[2]);
  } else if (light->Type & PL_LIGHT_POINT) {
    xp = light->Xp-_cam->X;
    yp = light->Yp-_cam->Y;
    zp = light->Zp-_cam->Z;
    MACRO_plMatrixApply(_cMatrix,xp,yp,zp,pl[0],pl[1],pl[2]);
  }
  _lights[_numlights++].light = light;
}

static void _RenderObj(pl_Obj *obj, pl_Float *bmatrix, pl_Float *bnmatrix) {
  pl_uInt32 i, x, facepos;
  pl_Float nx = 0.0, ny = 0.0, nz = 0.0;
  double tmp, tmp2;
  pl_Float oMatrix[16], nMatrix[16], tempMatrix[16];

  pl_Vertex *vertex;
  pl_Face *face;
  pl_Light *light;

  if (obj->GenMatrix) {
    plMatrixRotate(nMatrix,1,obj->Xa);
    plMatrixRotate(tempMatrix,2,obj->Ya);
    plMatrixMultiply(nMatrix,tempMatrix);
    plMatrixRotate(tempMatrix,3,obj->Za);
    plMatrixMultiply(nMatrix,tempMatrix);
    memcpy(oMatrix,nMatrix,sizeof(pl_Float)*16);
  } else memcpy(nMatrix,obj->RotMatrix,sizeof(pl_Float)*16);

  if (bnmatrix) plMatrixMultiply(nMatrix,bnmatrix);

  if (obj->GenMatrix) {
    plMatrixTranslate(tempMatrix, obj->Xp, obj->Yp, obj->Zp);
    plMatrixMultiply(oMatrix,tempMatrix);
  } else memcpy(oMatrix,obj->Matrix,sizeof(pl_Float)*16);
  if (bmatrix) plMatrixMultiply(oMatrix,bmatrix);

  for (i = 0; i < PL_MAX_CHILDREN; i ++)
    if (obj->Children[i]) _RenderObj(obj->Children[i],oMatrix,nMatrix);
  if (!obj->NumFaces || !obj->NumVertices) return;

  plMatrixTranslate(tempMatrix, -_cam->X, -_cam->Y, -_cam->Z);
  plMatrixMultiply(oMatrix,tempMatrix);
  plMatrixMultiply(oMatrix,_cMatrix);
  plMatrixMultiply(nMatrix,_cMatrix);

  x = obj->NumVertices;
  vertex = obj->Vertices;

  do {
    MACRO_plMatrixApply(oMatrix,vertex->x,vertex->y,vertex->z,
                  vertex->xformedx, vertex->xformedy, vertex->xformedz);
    MACRO_plMatrixApply(nMatrix,vertex->nx,vertex->ny,vertex->nz,
                  vertex->xformednx,vertex->xformedny,vertex->xformednz);
    vertex++;
  } while (--x);

  face = obj->Faces;
  facepos = _numfaces;

  if (_numfaces + obj->NumFaces >= PL_MAX_TRIANGLES) // exceeded maximum face coutn
  {
    return;
  }

  plRender_TriStats[0] += obj->NumFaces;
  _numfaces += obj->NumFaces;
  x = obj->NumFaces;

  do {
    if (obj->BackfaceCull || face->Material->_st & PL_SHADE_FLAT)
    {
      MACRO_plMatrixApply(nMatrix,face->nx,face->ny,face->nz,nx,ny,nz);
    }
    if (!obj->BackfaceCull || (MACRO_plDotProduct(nx,ny,nz,
        face->Vertices[0]->xformedx, face->Vertices[0]->xformedy,
        face->Vertices[0]->xformedz) < 0.0000001)) {
      if (plClipNeeded(face)) {
        if (face->Material->_st & (PL_SHADE_FLAT|PL_SHADE_FLAT_DISTANCE)) {
          tmp = face->sLighting;
          if (face->Material->_st & PL_SHADE_FLAT) {
            for (i = 0; i < _numlights; i ++) {
              tmp2 = 0.0;
              light = _lights[i].light;
              if (light->Type & PL_LIGHT_POINT_ANGLE) {
                double nx2 = _lights[i].l[0] - face->Vertices[0]->xformedx;
                double ny2 = _lights[i].l[1] - face->Vertices[0]->xformedy;
                double nz2 = _lights[i].l[2] - face->Vertices[0]->xformedz;
                MACRO_plNormalizeVector(nx2,ny2,nz2);
                tmp2 = MACRO_plDotProduct(nx,ny,nz,nx2,ny2,nz2)*light->Intensity;
              }
              if (light->Type & PL_LIGHT_POINT_DISTANCE) {
                double nx2 = _lights[i].l[0] - face->Vertices[0]->xformedx;
                double ny2 = _lights[i].l[1] - face->Vertices[0]->xformedy;
                double nz2 = _lights[i].l[2] - face->Vertices[0]->xformedz;
                if (light->Type & PL_LIGHT_POINT_ANGLE) {
                   nx2 = (1.0 - 0.5*((nx2*nx2+ny2*ny2+nz2*nz2)/
                           light->HalfDistSquared));
                  tmp2 *= plMax(0,plMin(1.0,nx2))*light->Intensity;
                } else {
                  tmp2 = (1.0 - 0.5*((nx2*nx2+ny2*ny2+nz2*nz2)/
                    light->HalfDistSquared));
                  tmp2 = plMax(0,plMin(1.0,tmp2))*light->Intensity;
                }
              }
              if (light->Type == PL_LIGHT_VECTOR)
                tmp2 = MACRO_plDotProduct(nx,ny,nz,_lights[i].l[0],_lights[i].l[1],_lights[i].l[2])
                  * light->Intensity;
              if (tmp2 > 0.0) tmp += tmp2;
              else if (obj->BackfaceIllumination) tmp -= tmp2;
            } /* End of light loop */
          } /* End of flat shading if */
          if (face->Material->_st & PL_SHADE_FLAT_DISTANCE)
            tmp += 1.0-(face->Vertices[0]->xformedz+face->Vertices[1]->xformedz+
                        face->Vertices[2]->xformedz) /
                       (face->Material->FadeDist*3.0);
          face->fShade = (pl_Float) tmp;
        } else face->fShade = 0.0; /* End of flatmask lighting if */
        if (face->Material->_ft & PL_FILL_ENVIRONMENT) {
          face->eMappingU[0] = 32768 + (pl_sInt32) (face->Vertices[0]->xformednx*32768.0);
          face->eMappingV[0] = 32768 - (pl_sInt32) (face->Vertices[0]->xformedny*32768.0);
          face->eMappingU[1] = 32768 + (pl_sInt32) (face->Vertices[1]->xformednx*32768.0);
          face->eMappingV[1] = 32768 - (pl_sInt32) (face->Vertices[1]->xformedny*32768.0);
          face->eMappingU[2] = 32768 + (pl_sInt32) (face->Vertices[2]->xformednx*32768.0);
          face->eMappingV[2] = 32768 - (pl_sInt32) (face->Vertices[2]->xformedny*32768.0);
        }
        if (face->Material->_st &(PL_SHADE_GOURAUD|PL_SHADE_GOURAUD_DISTANCE)) {
          register pl_uChar a;
          for (a = 0; a < 3; a ++) {
            tmp = face->vsLighting[a];
            if (face->Material->_st & PL_SHADE_GOURAUD) {
              for (i = 0; i < _numlights ; i++) {
                tmp2 = 0.0;
                light = _lights[i].light;
                if (light->Type & PL_LIGHT_POINT_ANGLE) {
                  nx = _lights[i].l[0] - face->Vertices[a]->xformedx;
                  ny = _lights[i].l[1] - face->Vertices[a]->xformedy;
                  nz = _lights[i].l[2] - face->Vertices[a]->xformedz;
                  MACRO_plNormalizeVector(nx,ny,nz);
                  tmp2 = MACRO_plDotProduct(face->Vertices[a]->xformednx,
                                      face->Vertices[a]->xformedny,
                                      face->Vertices[a]->xformednz,
                                      nx,ny,nz) * light->Intensity;
                }
                if (light->Type & PL_LIGHT_POINT_DISTANCE) {
                  double nx2 = _lights[i].l[0] - face->Vertices[a]->xformedx;
                  double ny2 = _lights[i].l[1] - face->Vertices[a]->xformedy;
                  double nz2 = _lights[i].l[2] - face->Vertices[a]->xformedz;
                  if (light->Type & PL_LIGHT_POINT_ANGLE) {
                     double t= (1.0 - 0.5*((nx2*nx2+ny2*ny2+nz2*nz2)/light->HalfDistSquared));
                     tmp2 *= plMax(0,plMin(1.0,t))*light->Intensity;
                  } else {
                    tmp2 = (1.0 - 0.5*((nx2*nx2+ny2*ny2+nz2*nz2)/light->HalfDistSquared));
                    tmp2 = plMax(0,plMin(1.0,tmp2))*light->Intensity;
                  }
                }
                if (light->Type == PL_LIGHT_VECTOR)
                  tmp2 = MACRO_plDotProduct(face->Vertices[a]->xformednx,
                                      face->Vertices[a]->xformedny,
                                      face->Vertices[a]->xformednz,
                                      _lights[i].l[0],_lights[i].l[1],_lights[i].l[2])
                                        * light->Intensity;
                if (tmp2 > 0.0) tmp += tmp2;
                else if (obj->BackfaceIllumination) tmp -= tmp2;
              } /* End of light loop */
            } /* End of gouraud shading if */
            if (face->Material->_st & PL_SHADE_GOURAUD_DISTANCE)
              tmp += 1.0-face->Vertices[a]->xformedz/face->Material->FadeDist;
            face->Shades[a] = (pl_Float) tmp;
          } /* End of vertex loop for */
        } /* End of gouraud shading mask if */
        _faces[facepos].zd = face->Vertices[0]->xformedz+
        face->Vertices[1]->xformedz+face->Vertices[2]->xformedz;
        _faces[facepos++].face = face;
        plRender_TriStats[1] ++;
      } /* Is it in our area Check */
    } /* Backface Check */
    _numfaces = facepos;
    face++;
  } while (--x); /* Face loop */
}

PL_API void plRenderObj(pl_Obj *obj) {
  _RenderObj(obj,0,0);
}

PL_API void plRenderEnd() {
  _faceInfo *f;
  if (_cam->Sort > 0) _hsort(_faces,_numfaces,0);
  else if (_cam->Sort < 0) _hsort(_faces,_numfaces,1);
  f = _faces;
  while (_numfaces--) {
    if (f->face->Material && f->face->Material->_PutFace)
    {
      plClipRenderFace(f->face);
    }
    f++;
  }
  _numfaces=0;
  _numlights = 0;
}

static _faceInfo *Base, tmp;

static void _hsort(_faceInfo *base, int nel, int dir) {
  static int i;
  Base=base-1;
  for (i=nel/2; i>0; i--) _sift_down(i,nel,dir);
  for (i=nel; i>1; ) {
    tmp = base[0]; base[0] = Base[i]; Base[i] = tmp;
    _sift_down(1,i-=1,dir);
  }
}

#define _pl_Comp(x,y) (( x ).zd < ( y ).zd ? 1 : 0)

static void _sift_down(int L, int U, int dir) {
  static int c;
  while (1) {
    c=L+L;
    if (c>U) break;
    if ( (c < U) && dir^_pl_Comp(Base[c+1],Base[c])) c++;
    if (dir^_pl_Comp(Base[L],Base[c])) return;
    tmp = Base[L]; Base[L] = Base[c]; Base[c] = tmp;
    L=c;
  }
}
#undef _pl_Comp

PL_API void plSplineGetPoint(pl_Spline *s, pl_Float frame, pl_Float *out) {
  pl_sInt32 i, i_1, i0, i1, i2;
  pl_Float time1,time2,time3;
  pl_Float t1,t2,t3,t4,u1,u2,u3,u4,v1,v2,v3;
  pl_Float a,b,c,d;

  pl_Float *keys = s->keys;

  a = (1-s->tens)*(1+s->cont)*(1+s->bias);
  b = (1-s->tens)*(1-s->cont)*(1-s->bias);
  c = (1-s->tens)*(1-s->cont)*(1+s->bias);
  d = (1-s->tens)*(1+s->cont)*(1-s->bias);
  v1 = t1 = -a / 2.0; u1 = a;
  u2 = (-6-2*a+2*b+c)/2.0; v2 = (a-b)/2.0; t2 = (4+a-b-c) / 2.0;
  t3 = (-4+b+c-d) / 2.0;
  u3 = (6-2*b-c+d)/2.0;
  v3 = b/2.0;
  t4 = d/2.0; u4 = -t4;

  i0 = (pl_uInt) frame;
  i_1 = i0 - 1;
  while (i_1 < 0) i_1 += s->numKeys;
  i1 = i0 + 1;
  while (i1 >= s->numKeys) i1 -= s->numKeys;
  i2 = i0 + 2;
  while (i2 >= s->numKeys) i2 -= s->numKeys;
  time1 = frame - (pl_Float) ((pl_uInt) frame);
  time2 = time1*time1;
  time3 = time2*time1;
  i0 *= s->keyWidth;
  i1 *= s->keyWidth;
  i2 *= s->keyWidth;
  i_1 *= s->keyWidth;
  for (i = 0; i < s->keyWidth; i ++) {
    a = t1*keys[i+i_1]+t2*keys[i+i0]+t3*keys[i+i1]+t4*keys[i+i2];
    b = u1*keys[i+i_1]+u2*keys[i+i0]+u3*keys[i+i1]+u4*keys[i+i2];
    c = v1*keys[i+i_1]+v2*keys[i+i0]+v3*keys[i+i1];
    *out++ = a*time3 + b*time2 + c*time1 + keys[i+i0];
  }
}

static pl_uChar font_height = PL_DEFAULT_FONT_SIZE;

static pl_uChar *current_font = plText_DefaultFont;

PL_API void plTextSetFont(pl_uChar *font, pl_uChar height) {
  current_font = font;
  font_height = height;
}

PL_API void plTextPutChar(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                   pl_uChar color, pl_uChar c) {
  pl_uChar *font = current_font + (c*font_height);
  pl_sInt offset = x+(y*cam->ScreenWidth);
  pl_ZBuffer zz = (pl_ZBuffer) (1.0/z);
  pl_sInt xx = x, a;
  pl_uChar len = font_height;
  pl_uChar ch;
  pl_uChar *outmem;
  pl_ZBuffer *zbuffer;
  if (y+font_height < cam->ClipTop || y >= cam->ClipBottom) return;
  if (y < cam->ClipTop) {
    font += (cam->ClipTop-y);
    offset += (cam->ClipTop-y)*cam->ScreenWidth;
    len -= (cam->ClipTop-y);
    y = cam->ClipTop;
  }
  if (y+font_height >= cam->ClipBottom) {
    len = cam->ClipBottom-y;
  }
  if (len > 0) {
    if (cam->zBuffer && z != 0.0) do {
      outmem = cam->frameBuffer + offset;
      zbuffer = cam->zBuffer + offset;
      offset += cam->ScreenWidth;
      xx = x;
      ch = *font++;
      a = 128;
      while (a) {
        if (xx >= cam->ClipRight) break;
        if (xx++ >= cam->ClipLeft)
          if (ch & a)
            if (zz > *zbuffer) {
              *zbuffer = zz;
              *outmem = color;
            }
        zbuffer++;
        outmem++;
        a >>= 1;
      }
      if (a) break;
    } while (--len);
    else do {
      outmem = cam->frameBuffer + offset;
      offset += cam->ScreenWidth;
      xx = x;
      ch = *font++;
      a = 128;
      while (a) {
        if (xx >= cam->ClipRight) break;
        if (xx++ >= cam->ClipLeft) if (ch & a) *outmem = color;
        outmem++;
        a >>= 1;
      }
      if (a) break;
    } while (--len);
  }
}

PL_API void plTextPutStr(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                  pl_uChar color, pl_sChar *string) {
  pl_sInt xx = x;
  while (*string) {
    switch (*string) {
      case '\n': y += font_height; xx = x; break;
      case ' ': xx += PL_DEFAULT_FONT_WIDTH; break;
      case '\r': break;
      case '\t': xx += PL_DEFAULT_FONT_WIDTH * PL_DEFAULT_FONT_TAB_SIZE; break;
      default:
        plTextPutChar(cam,xx,y,z,color,(pl_uChar) *string);
        xx += PL_DEFAULT_FONT_WIDTH;
      break;
    }
    string++;
  }
}

PL_API void plTextPrintf(pl_Cam *cam, pl_sInt x, pl_sInt y, pl_Float z,
                  pl_uChar color, pl_sChar *format, ...) {
  va_list arglist;
  pl_sChar str[256];
  va_start(arglist, format);
  vsprintf((char *)str, (char *) format,arglist);
  va_end(arglist);
  plTextPutStr(cam,x,y,z,color,str);
}
#endif

#endif
/* vim: set ts=4 sw=4 sts=4 noet: */
