/*******************************************************************************
FILE : graphics_library.c

LAST MODIFIED : 5 May 2005

DESCRIPTION :
Functions for interfacing with the graphics library.
==============================================================================*/
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is cmgui.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#if defined (OPENGL_API)
#  if defined (UNIX)
#    include <dlfcn.h>
#  endif /* defined (UNIX) */
#  include <math.h>
#  include <string.h>
#  include <stdio.h>
#  if defined (MOTIF) /* switch (USER_INTERFACE) */
#    include <X11/Xlib.h>
#    include <GL/glu.h>
#    define GLX_GLXEXT_PROTOTYPES
#    include <GL/glx.h>
#  elif defined (GTK_USER_INTERFACE) /* switch (USER_INTERFACE) */
#    include <gtk/gtk.h>
#    if defined (UNIX)
#      define GLX_GLXEXT_PROTOTYPES
#      include <GL/glx.h>
#      include <GL/glxext.h>
#    endif
#    if ( GTK_MAJOR_VERSION < 2 ) || defined (WIN32_SYSTEM)
/* SAB When compiling GTK with WIN32 currently uses GTK GL AREA for open GL. */
#      define GTK_USE_GTKGLAREA
#    endif /* ( GTK_MAJOR_VERSION < 2 ) || defined (WIN32_SYSTEM)*/
#    if defined (GTK_USE_GTKGLAREA)
#      include <gtkgl/gtkglarea.h>
#    else /* defined (GTK_USE_GTKGLAREA) */
#      include <gtk/gtkgl.h>
#      include <gdk/x11/gdkglx.h>
#    endif /* defined (GTK_USE_GTKGLAREA) */
#  endif /* switch (USER_INTERFACE) */
#endif /* defined (OPENGL_API) */
#include "general/debug.h"
#define GRAPHICS_LIBRARY_C
#include "graphics/graphics_library.h"
#include "three_d_drawing/graphics_buffer.h"
#include "user_interface/message.h"
#include "user_interface/user_interface.h"

/*
Global functions
----------------
*/

int initialize_graphics_library(struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 23 February 2004

DESCRIPTION :
Sets up the default light, material and light model for the graphics library.
==============================================================================*/
{
#if defined(GLX_ARB_get_proc_address)
#if defined (MOTIF)
	Display *display;
#endif
#endif /* defined(GLX_ARB_get_proc_address) */
	int return_code;
	static int initialized=0;

	ENTER(initialize_graphics_library);

	return_code=1;
	USE_PARAMETER(user_interface);
	if (!initialized)
	{
#if defined (OPENGL_API)
		glMatrixMode(GL_MODELVIEW);

#if defined(GLX_ARB_get_proc_address)
#if defined (MOTIF)
		/* Try and load this function while we have the user_interface connection */
		display = User_interface_get_display(user_interface);
		if (GLEXTENSION_UNSURE == GLEXTENSIONFLAG(GLX_ARB_get_proc_address))
		{
			if (query_glx_extension("GLX_ARB_get_proc_address", display,
				DefaultScreen(display)))
			{
				GLEXTENSIONFLAG(GLX_ARB_get_proc_address) = 1;
			}
			else
			{
				GLEXTENSIONFLAG(GLX_ARB_get_proc_address) = 0;
			}
		}
#endif /* defined (MOTIF) */
#endif /* defined(GLX_ARB_get_proc_address) */
#endif
		initialized=1;
	}
	LEAVE;

	return (return_code);
} /* initialize_graphics_library */

#if defined (GTK_USER_INTERFACE)
#if defined (UNIX)
int Graphics_library_initialise_gtkglext_glx_extensions(GdkGLConfig *config)
/*******************************************************************************
LAST MODIFIED : 5 February 2007

DESCRIPTION :
To test GLX extensions requires a connection to the display.
==============================================================================*/
{
	int return_code;

	ENTER(Graphics_library_initialise_gtkglext_glx_extensions);
	return_code=1;
#if defined(GLX_ARB_get_proc_address)
	if (GLEXTENSION_UNSURE == GLEXTENSIONFLAG(GLX_ARB_get_proc_address))
	{
		if (gdk_x11_gl_query_glx_extension(config,
				"GLX_ARB_get_proc_address"))
		{
			GLEXTENSIONFLAG(GLX_ARB_get_proc_address) = 1;
		}
		else
		{
			GLEXTENSIONFLAG(GLX_ARB_get_proc_address) = 0;
		}
	}
#endif /* defined(GLX_ARB_get_proc_address) */
	LEAVE;

	return (return_code);
} /* Graphics_library_initialise_gtkglext_glx_extensions */
#endif /* defined (UNIX) */
#endif /* defined (GTK_USER_INTERFACE) */

int gtMatrix_is_identity(gtMatrix *matrix)
/*******************************************************************************
LAST MODIFIED : 28 November 2001

DESCRIPTION :
Returns true if <matrix> is the 4x4 identity
==============================================================================*/
{
	int return_code;

	ENTER(gtMatrix_is_identity);
	if (matrix)
	{
		return_code = 
			((*matrix)[0][0] == 1.0) &&
			((*matrix)[0][1] == 0.0) &&
			((*matrix)[0][2] == 0.0) &&
			((*matrix)[0][3] == 0.0) &&
			((*matrix)[1][0] == 0.0) &&
			((*matrix)[1][1] == 1.0) &&
			((*matrix)[1][2] == 0.0) &&
			((*matrix)[1][3] == 0.0) &&
			((*matrix)[2][0] == 0.0) &&
			((*matrix)[2][1] == 0.0) &&
			((*matrix)[2][2] == 1.0) &&
			((*matrix)[2][3] == 0.0) &&
			((*matrix)[3][0] == 0.0) &&
			((*matrix)[3][1] == 0.0) &&
			((*matrix)[3][2] == 0.0) &&
			((*matrix)[3][3] == 1.0);
	}
	else
	{
		display_message(ERROR_MESSAGE, "gtMatrix_is_identity.  Missing matrix");
		return_code = 0;
	}
	LEAVE;

	return (return_code);
} /* gtMatrix_is_identity */

int gtMatrix_match(gtMatrix *matrix1, gtMatrix *matrix2)
/*******************************************************************************
LAST MODIFIED : 28 November 2001

DESCRIPTION :
Returns true if <matrix1> and <matrix2> are identical.
==============================================================================*/
{
	int return_code;

	ENTER(gtMatrix_match);
	if (matrix1 && matrix2)
	{
		return_code = 
			((*matrix1)[0][0] == (*matrix2)[0][0]) &&
			((*matrix1)[0][1] == (*matrix2)[0][1]) &&
			((*matrix1)[0][2] == (*matrix2)[0][2]) &&
			((*matrix1)[0][3] == (*matrix2)[0][3]) &&
			((*matrix1)[1][0] == (*matrix2)[1][0]) &&
			((*matrix1)[1][1] == (*matrix2)[1][1]) &&
			((*matrix1)[1][2] == (*matrix2)[1][2]) &&
			((*matrix1)[1][3] == (*matrix2)[1][3]) &&
			((*matrix1)[2][0] == (*matrix2)[2][0]) &&
			((*matrix1)[2][1] == (*matrix2)[2][1]) &&
			((*matrix1)[2][2] == (*matrix2)[2][2]) &&
			((*matrix1)[2][3] == (*matrix2)[2][3]) &&
			((*matrix1)[3][0] == (*matrix2)[3][0]) &&
			((*matrix1)[3][1] == (*matrix2)[3][1]) &&
			((*matrix1)[3][2] == (*matrix2)[3][2]) &&
			((*matrix1)[3][3] == (*matrix2)[3][3]);
	}
	else
	{
		display_message(ERROR_MESSAGE, "gtMatrix_match.  Missing matrices");
		return_code = 0;
	}
	LEAVE;

	return (return_code);
} /* gtMatrix_match */

int gtMatrix_to_euler(gtMatrix matrix, float *euler_angles)
/*******************************************************************************
LAST MODIFIED : 21 November 2002

DESCRIPTION :
Cleaned this up from view/coord_trans.c
Returns <euler_angles> in radians.
==============================================================================*/
{
	int return_code;
#define MATRIX_TO_EULER_TOLERANCE 1.0E-12

	ENTER(matrix_euler_float4);
	if ((fabs(matrix[0][0])>MATRIX_TO_EULER_TOLERANCE) &&
		(fabs(matrix[0][1])>MATRIX_TO_EULER_TOLERANCE))
	{
		euler_angles[0] = atan2(matrix[0][1],matrix[0][0]);
		euler_angles[2] = atan2(matrix[1][2],matrix[2][2]);
		euler_angles[1] = atan2(-matrix[0][2],matrix[0][0]/
			cos(euler_angles[0]));
	}
	else
	{
		if (fabs(matrix[0][0])>MATRIX_TO_EULER_TOLERANCE)
		{
			euler_angles[0] = atan2(matrix[0][1],matrix[0][0]);
			euler_angles[2] = atan2(matrix[1][2],matrix[2][2]);
			euler_angles[1] = atan2(-matrix[0][2],matrix[0][0]/
				cos(euler_angles[0]));
		}
		else
		{
			if (fabs(matrix[0][1])>MATRIX_TO_EULER_TOLERANCE)
			{
				euler_angles[0] = atan2(matrix[0][1],matrix[0][0]);
				euler_angles[2] = atan2(matrix[1][2],matrix[2][2]);
				euler_angles[1] = atan2(-matrix[0][2],matrix[0][1]/
					sin(euler_angles[0]));
			}
			else
			{
				euler_angles[1] = atan2(-matrix[0][2],0); /* get +/-1 */
				euler_angles[0] = 0;
				euler_angles[2] = atan2(-matrix[2][1],
					-matrix[2][0]*matrix[0][2]);
			}
		}
	}
	return_code = 1;

	LEAVE;
	return(return_code);
} /* matrix_euler */

int euler_to_gtMatrix(float *euler_angles, gtMatrix matrix)
/*******************************************************************************
LAST MODIFIED : 21 November 2002

DESCRIPTION :
Cleaned this up from view/coord_trans.c
<euler_angles> are in radians.
==============================================================================*/
{
	double cos_azimuth,cos_elevation,cos_roll,sin_azimuth,sin_elevation,
		sin_roll;
	int return_code;

	ENTER(euler_to_gtMatrix);

	cos_azimuth = cos(euler_angles[0]);
	sin_azimuth = sin(euler_angles[0]);
	cos_elevation = cos(euler_angles[1]);
	sin_elevation = sin(euler_angles[1]);
	cos_roll = cos(euler_angles[2]);
	sin_roll = sin(euler_angles[2]);
	matrix[0][0] = cos_azimuth*cos_elevation;
	matrix[0][1] = sin_azimuth*cos_elevation;
	matrix[0][2] = -sin_elevation;
	matrix[1][0] = cos_azimuth*sin_elevation*sin_roll-
		sin_azimuth*cos_roll;
	matrix[1][1] = sin_azimuth*sin_elevation*sin_roll+
		cos_azimuth*cos_roll;
	matrix[1][2] = cos_elevation*sin_roll;
	matrix[2][0] = cos_azimuth*sin_elevation*cos_roll+
		sin_azimuth*sin_roll;
	matrix[2][1] = sin_azimuth*sin_elevation*cos_roll-
		cos_azimuth*sin_roll;
	matrix[2][2] = cos_elevation*cos_roll;

	/* Populate the 4 x 4 */
	matrix[3][0] = 0.0;
	matrix[3][1] = 0.0;
	matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	matrix[3][3] = 1.0;

	return_code = 1;

	LEAVE;
	return (return_code);
} /* euler_matrix */

int gtMatrix_match_with_tolerance(gtMatrix *matrix1, gtMatrix *matrix2,
	float tolerance)
/*******************************************************************************
LAST MODIFIED : 5 December 2001

DESCRIPTION :
Returns true if <matrix1> and <matrix2> have no components different by
more than <tolerance> times the largest absolute value in either matrix.
==============================================================================*/
{
	float abs_value, difference, max_abs_value, max_difference;
	int i, j, return_code;

	ENTER(gtMatrix_match_with_tolerance);
	if (matrix1 && matrix2 && (0.0 <= tolerance) && (1.0 >= tolerance))
	{
		max_abs_value = 0.0;
		max_difference = 0.0;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if ((abs_value = fabs((*matrix1)[i][j])) > max_abs_value)
				{
					max_abs_value = abs_value;
				}
				if ((abs_value = fabs((*matrix2)[i][j])) > max_abs_value)
				{
					max_abs_value = abs_value;
				}
				if ((difference = fabs((*matrix2)[i][j] - (*matrix1)[i][j])) >
					max_difference)
				{
					max_difference = difference;
				}
			}
		}
		return_code = (max_difference <= (tolerance*max_abs_value));
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"gtMatrix_match_with_tolerance.  Invalid argument(s)");
		return_code = 0;
	}
	LEAVE;

	return (return_code);
} /* gtMatrix_match_with_tolerance */

#if defined (OPENGL_API)
void wrapperLoadCurrentMatrix(gtMatrix *theMatrix)
/*******************************************************************************
LAST MODIFIED : 13 February 1995

DESCRIPTION :
==============================================================================*/
{
	int col,i,row;
	static float vector[16];

	ENTER(wrapperLoadCurrentMatrix);
	i=0;
	for (col=0;col<4;col++)
	{
		for (row=0;row<4;row++)
		{
			vector[i++]=(*theMatrix)[col][row];
		}
	}
	glLoadMatrixf(vector);
	LEAVE;
} /* wrapperLoadCurrentMatrix */

void wrapperMultiplyCurrentMatrix(gtMatrix *theMatrix)
/*******************************************************************************
LAST MODIFIED : 13 February 1995

DESCRIPTION :
==============================================================================*/
{
	int col,i,row;
	static float vector[16];

	ENTER(wrapperMultiplyCurrentMatrix);
	i=0;
	for (col=0;col<4;col++)
	{
		for (row=0;row<4;row++)
		{
			vector[i++]=(*theMatrix)[col][row];
		}
	}
	glMultMatrixf(vector);
	LEAVE;
} /* wrapperMultiplyCurrentMatrix */
#endif /* defined (OPENGL_API) */

#if defined (OPENGL_API)
int query_gl_extension(char *extName)
/*******************************************************************************
LAST MODIFIED : 9 September 1998

DESCRIPTION :
Search for extName in the GL extensions string. Use of strstr() is not sufficient
because extension names can be prefixes of other extension names. Could use
strtok() but the constant string returned by glGetString might be in read-only
memory.
???SAB.  Taken directly from the insight book on OpenGL Extensions
Returns GLEXTENSION_UNSURE if openGL extension string is NULL and therefore
we still are not sure if the extension will be available when the openGL is
initialiased.
==============================================================================*/
{
	char *end, *p;
	int extNameLen, n, return_code;

	/* check arguments */
	if (extName)
	{
		extNameLen=strlen(extName);

		p=(char *)glGetString(GL_EXTENSIONS);
#define DEBUG
#if defined (DEBUG)
		/* For debugging */
		{
			char *vendor;
			char *version;
			char *glu;
			vendor=(char *)glGetString(GL_VENDOR);
			version=(char *)glGetString(GL_VERSION);
			glu=(char *)gluGetString(GLU_EXTENSIONS);
			printf("Vendor %s\n", vendor);
			printf("Version %s\n", version);
			printf("OpenGL %s\n", p);
			printf("GLU %s\n", glu);
		}
#endif /* defined (DEBUG) */
		if (NULL==p)
		{
			/* We still don't know, so return this and allow the calling
				routine to decide what to do */
			return_code=GLEXTENSION_UNSURE;
		}
		else
		{
			end=p+strlen(p);
			return_code=GLEXTENSION_UNAVAILABLE;
			while (p<end)
			{
				n=strcspn(p," ");
				if ((extNameLen==n)&&(strncmp(extName,p,n)==0)) 
				{
					return_code=GLEXTENSION_AVAILABLE;
				}
				p += (n+1);
			}
		}
	}
	else
	{
		return_code=GLEXTENSION_UNAVAILABLE;
	}

	return (return_code);
} /* query_extension */
#endif /* defined (OPENGL_API) */

#if defined (OPENGL_API)
int query_gl_version(int major_version, int minor_version)
/*******************************************************************************
LAST MODIFIED : 16 April 2003

DESCRIPTION :
Returns true if the OpenGL version is at least <major_version>.<minor_version>
Returns GLEXTENSION_UNSURE if openGL extension string is NULL and therefore
we still are not sure if the extension will be available when the openGL is
initialiased.
==============================================================================*/
{
	char *version;
	static int major = 0, minor = 0;
	int return_code;

	return_code=GLEXTENSION_UNAVAILABLE;

	if (!major)
	{
		version=(char *)glGetString(GL_VERSION);

		if (version)
		{
			if (!(2 == sscanf(version, "%d.%d", &major, &minor)))
			{
				major = -1;
				minor = -1;
			}
		}
		else
		{
			/* We still don't know, so return this and allow the calling
				routine to decide what to do */
			return_code=GLEXTENSION_UNSURE;
		}
	}

	if (major > major_version)
	{
		return_code=GLEXTENSION_AVAILABLE;
	}
	else if (major == major_version)
	{
		if (minor >= minor_version)
		{
			return_code=GLEXTENSION_AVAILABLE;
		}
	}

	return (return_code);
} /* query_gl_version */
#endif /* defined (OPENGL_API) */

int Graphics_library_read_pixels(unsigned char *frame_data,
	int width, int height, enum Texture_storage_type storage,
	int front_buffer)
/*******************************************************************************
LAST MODIFIED : 30 June 2004

DESCRIPTION :
Read pixels from the current graphics context into <frame_data> of size <width>
and <height> according to the storage type.  'MakeCurrent' the desired source 
before calling this routine.  If <front_buffer> is 1 then pixels will be read
from the front buffer, otherwise they will be read from the back buffer in a
double buffered context.
==============================================================================*/
{
	int return_code;

	ENTER(Graphics_library_read_pixels);
	if (frame_data && width && height)
	{
#if defined (OPENGL_API)
		/* Make sure we get it from the front for a double buffer,
			has no effect on a single buffer, keep the old read
			buffer so we can set it back after reading */
		if (front_buffer)
		{
			glReadBuffer(GL_FRONT);
		}
		else
		{
			glReadBuffer(GL_BACK);
		}
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		switch(storage)
		{
			case TEXTURE_LUMINANCE:
			{
				glReadPixels(0, 0, width, height, GL_LUMINANCE,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
			case TEXTURE_LUMINANCE_ALPHA:
			{
				glReadPixels(0, 0, width, height, GL_LUMINANCE_ALPHA,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
			case TEXTURE_RGB:
			{
				glReadPixels(0, 0, width, height, GL_RGB,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
			case TEXTURE_BGR:
			{
				glReadPixels(0, 0, width, height, GL_BGR,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
			case TEXTURE_RGBA:
			{
				glReadPixels(0, 0, width, height, GL_RGBA,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
#if defined (GL_ABGR_EXT)
			case TEXTURE_ABGR:
			{
				glReadPixels(0, 0, width, height, GL_ABGR_EXT,
					GL_UNSIGNED_BYTE,frame_data);
				return_code=1;
			} break;
#endif /* defined (GL_ABGR_EXT) */
			default:
			{
				display_message(ERROR_MESSAGE,
					"Graphics_library_read_pixels.  Unsupported or unknown storage type");
				return_code=0;
			} break;
		}
#else /* defined (OPENGL_API) */
		return_code=0;
#endif /* defined (OPENGL_API) */
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Graphics_library_read_pixels.  Invalid arguments");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* Graphics_library_read_pixels */

static void *Graphics_library_get_function_ptr(char *function_name)
/*******************************************************************************
LAST MODIFIED : 20 February 2004

DESCRIPTION :
Finds and loads gl function symbols at runtime.
==============================================================================*/
{
	void *function_ptr;
#if defined (UNIX)
	void *symbol_table;
#endif /* defined (UNIX) */

	ENTER(Graphics_library_get_function_ptr);
	if (function_name)
	{
#if defined (OPENGL_API)
#if defined (WIN32_SYSTEM)
		 function_ptr = (void *) wglGetProcAddress(function_name);
#else /* defined (WIN32_SYSTEM) */
#if defined(GLX_ARB_get_proc_address)
		/* We don't try and load this here because testing for a GLX extension
			requires the Display, it is done in the initialize_graphics_library above */
		if (1 == GLEXTENSIONFLAG(GLX_ARB_get_proc_address))
		{
			function_ptr = (void *) glXGetProcAddressARB((const GLubyte *) function_name);
		}
		else
		{
#endif /* defined(GLX_ARB_get_proc_address) */
#if defined (UNIX)
			symbol_table = dlopen((char *)0, RTLD_LAZY);
			function_ptr = dlsym(symbol_table, function_name);
			dlclose(symbol_table);
#else /* defined (UNIX) */
			function_ptr = NULL;			
#endif /* defined (UNIX) */
#if defined(GLX_ARB_get_proc_address)
		}
#endif /* defined(GLX_ARB_get_proc_address) */
#endif /* defined (WIN32_SYSTEM) */
#else /* defined (OPENGL_API) */
		return_code=0;
#endif /* defined (OPENGL_API) */
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Graphics_library_get_function_ptr.  Missing function name.");
		function_ptr = NULL;
	}

	LEAVE;

	return (function_ptr);
} /* Graphics_library_get_function_ptr */

enum Graphics_library_vendor_id Graphics_library_get_vendor_id()
/*******************************************************************************
LAST MODIFIED : 18 March 2008

DESCRIPTION :
Returns an enumeration which can be used to select for a particular vendor
implementation which is valid for the current OpenGL context.
==============================================================================*/
{
	const char *vendor = (const char *)glGetString(GL_VENDOR);
	enum Graphics_library_vendor_id vendor_id;

	ENTER(Graphics_library_get_vendor_id);

	vendor_id = Graphics_library_vendor_unknown;
	if (vendor)
	{
		switch (vendor[0])
		{
			case 'A':
			{
				if (!strcmp(vendor, "ATI Technologies Inc."))
				{
					vendor_id = Graphics_library_vendor_ati;
				}
			} break;
			case 'B':
			{
				if (!strcmp(vendor, "Brian Paul"))
				{
					vendor_id = Graphics_library_vendor_mesa;
				}
			} break;
			case 'I':
			{
				if (!strcmp(vendor, "Intel"))
				{
					vendor_id = Graphics_library_vendor_intel;
				}
			} break;
			case 'M':
			{
				if (!strcmp(vendor, "Microsoft Corporation"))
				{
					vendor_id = Graphics_library_vendor_microsoft;
				}
			} break;
			case 'N':
			{
				if (!strcmp(vendor, "NVIDIA Corporation"))
				{
					vendor_id = Graphics_library_vendor_nvidia;
				}
			} break;
		}
	}

	LEAVE;

	return (vendor_id);
} /* Graphics_library_get_vendor_id */

int Graphics_library_load_extension(char *extension_name)
/*******************************************************************************
LAST MODIFIED : 2 March 2007

DESCRIPTION :
Attempts to load a single openGL extension.
If the extension symbol and all the functions used in cmgui from that extension are
found then returns GLEXTENSION_AVAILABLE.
If the extension list is defined but this extension is not available then
it returns GLEXTENSION_UNAVAILABLE.
If the extension string is not yet defined then the test is not definitive and
so it returns GLEXTENSION_UNSURE, allowing the calling procedure to react
appropriately.
==============================================================================*/
{
	int return_code;
#if defined (GRAPHICS_LIBRARY_USE_EXTENSION_FUNCTION_HANDLES)
#define GRAPHICS_LIBRARY_ASSIGN_HANDLE(function,type) function ## _handle = (type)
#else /* defined (GRAPHICS_LIBRARY_USE_EXTENSION_FUNCTION_HANDLES) */
#define GRAPHICS_LIBRARY_ASSIGN_HANDLE(function,type)
#endif /* defined (GRAPHICS_LIBRARY_USE_EXTENSION_FUNCTION_HANDLES) */

	ENTER(Graphics_library_load_extension);
	if (extension_name)
	{
#if defined (OPENGL_API)
		if (NULL == extension_name)
		{
			return_code = 0;
		}
#if defined GL_VERSION_1_2
		else if (!strcmp(extension_name, "GL_VERSION_1_2"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_VERSION_1_2))
			{
				return_code = GLEXTENSIONFLAG(GL_VERSION_1_2);
			}
			else
			{
				return_code = query_gl_version(1, 2);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glTexImage3D, PFNGLTEXIMAGE3DPROC)
						Graphics_library_get_function_ptr("glTexImage3D")))
					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_VERSION_1_2) = return_code;
			}			
		}
#endif /* GL_VERSION_1_2 */
#if defined GL_VERSION_1_3
		else if (!strcmp(extension_name, "GL_VERSION_1_3"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_VERSION_1_3))
			{
				return_code = GLEXTENSIONFLAG(GL_VERSION_1_3);
			}
			else
			{
				return_code = query_gl_version(1, 3);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!((GRAPHICS_LIBRARY_ASSIGN_HANDLE(glActiveTexture, PFNGLACTIVETEXTUREPROC)
						Graphics_library_get_function_ptr("glActiveTexture")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glMultiTexCoord3fv, PFNGLMULTITEXCOORD3FVPROC)
							Graphics_library_get_function_ptr("glMultiTexCoord3fv"))))
 					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_VERSION_1_3) = return_code;
			}
		}
#endif /* GL_VERSION_1_3 */
#if defined GL_VERSION_1_4
		else if (!strcmp(extension_name, "GL_VERSION_1_4"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_VERSION_1_4))
			{
				return_code = GLEXTENSIONFLAG(GL_VERSION_1_4);
			}
			else
			{
				return_code = query_gl_version(1, 4);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC)
							Graphics_library_get_function_ptr("glBlendFuncSeparate")))
 					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_VERSION_1_4) = return_code;
			}
		}
#endif /* GL_VERSION_1_4 */
#if defined GL_ARB_depth_texture
		else if (!strcmp(extension_name, "GL_ARB_depth_texture"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_depth_texture))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_depth_texture);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_ARB_depth_texture) = return_code;
			}
		}
#endif /* GL_ARB_depth_texture */
#if defined GL_ARB_fragment_program
		else if (!strcmp(extension_name, "GL_ARB_fragment_program"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_fragment_program))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_fragment_program);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!((GRAPHICS_LIBRARY_ASSIGN_HANDLE(glGenProgramsARB, PFNGLGENPROGRAMSARBPROC)
						Graphics_library_get_function_ptr("glGenProgramsARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glBindProgramARB, PFNGLBINDPROGRAMARBPROC)
						Graphics_library_get_function_ptr("glBindProgramARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramStringARB, PFNGLPROGRAMSTRINGARBPROC)
						Graphics_library_get_function_ptr("glProgramStringARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glDeleteProgramsARB, PFNGLDELETEPROGRAMSARBPROC)
						Graphics_library_get_function_ptr("glDeleteProgramsARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramEnvParameter4fARB, PFNGLPROGRAMENVPARAMETER4FARBPROC)
						Graphics_library_get_function_ptr("glProgramEnvParameter4fARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramEnvParameter4fvARB, PFNGLPROGRAMENVPARAMETER4FVARBPROC)
						Graphics_library_get_function_ptr("glProgramEnvParameter4fvARB"))))
 					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_ARB_fragment_program) = return_code;
			}
		}
#endif /* GL_ARB_fragment_program */
#if defined GL_ARB_fragment_program_shadow
		else if (!strcmp(extension_name, "GL_ARB_fragment_program_shadow"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_fragment_program_shadow))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_fragment_program_shadow);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_ARB_fragment_program_shadow) = return_code;
			}
		}
#endif /* GL_ARB_fragment_program_shadow */
#if defined GL_ARB_texture_compression
		else if (!strcmp(extension_name, "GL_ARB_texture_compression"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_texture_compression))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_texture_compression);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_ARB_texture_compression) = return_code;
			}
		}
#endif /* GL_ARB_texture_compression */
#if defined GL_ARB_texture_non_power_of_two
		else if (!strcmp(extension_name, "GL_ARB_texture_non_power_of_two"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_texture_non_power_of_two))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_texture_non_power_of_two);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_ARB_texture_non_power_of_two) = return_code;
			}
		}
#endif /* GL_ARB_texture_non_power_of_two */
#if defined GL_ARB_texture_rectangle
		else if (!strcmp(extension_name, "GL_ARB_texture_rectangle"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_texture_rectangle))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_texture_rectangle);
			}
			else
			{
				/* These extensions are equivalent so at runtime we will be 
					happy to have the EXT version */
				if ((GLEXTENSION_AVAILABLE == query_gl_extension(extension_name)) || 
					(GLEXTENSION_AVAILABLE == query_gl_extension("GL_EXT_texture_rectangle")))
				{
					return_code = GLEXTENSION_AVAILABLE;
				}
				else
				{
					if ((GLEXTENSION_UNSURE == query_gl_extension(extension_name)) || 
						(GLEXTENSION_UNSURE == query_gl_extension("GL_EXT_texture_rectangle")))
					{
						return_code = GLEXTENSION_UNSURE;
					}
					else
					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_ARB_texture_rectangle) = return_code;
			}
		}
#endif /* GL_ARB_texture_rectangle */
#if defined GL_ARB_vertex_program
		else if (!strcmp(extension_name, "GL_ARB_vertex_program"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_vertex_program))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_vertex_program);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!((GRAPHICS_LIBRARY_ASSIGN_HANDLE(glGenProgramsARB, PFNGLGENPROGRAMSARBPROC)
						Graphics_library_get_function_ptr("glGenProgramsARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glBindProgramARB, PFNGLBINDPROGRAMARBPROC)
						Graphics_library_get_function_ptr("glBindProgramARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramStringARB, PFNGLPROGRAMSTRINGARBPROC)
						Graphics_library_get_function_ptr("glProgramStringARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glDeleteProgramsARB, PFNGLDELETEPROGRAMSARBPROC)
						Graphics_library_get_function_ptr("glDeleteProgramsARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramEnvParameter4fARB, PFNGLPROGRAMENVPARAMETER4FARBPROC)
						Graphics_library_get_function_ptr("glProgramEnvParameter4fARB")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glProgramEnvParameter4fvARB, PFNGLPROGRAMENVPARAMETER4FVARBPROC)
							Graphics_library_get_function_ptr("glProgramEnvParameter4fvARB"))))
 					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_ARB_vertex_program) = return_code;
			}
		}
#endif /* GL_ARB_vertex_program */
#if defined GL_ARB_shadow
		else if (!strcmp(extension_name, "GL_ARB_shadow"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_ARB_shadow))
			{
				return_code = GLEXTENSIONFLAG(GL_ARB_shadow);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_ARB_shadow) = return_code;
			}
		}
#endif /* GL_ARB_shadow */
#if defined GL_EXT_abgr
		else if (!strcmp(extension_name, "GL_EXT_abgr"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_EXT_abgr))
			{
				return_code = GLEXTENSIONFLAG(GL_EXT_abgr);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				GLEXTENSIONFLAG(GL_EXT_abgr) = return_code;
			}
		}
#endif /* GL_EXT_texture3D */
#if defined GL_EXT_texture3D
		else if (!strcmp(extension_name, "GL_EXT_texture3D"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_EXT_texture3D))
			{
				return_code = GLEXTENSIONFLAG(GL_EXT_texture3D);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					/* We are using the non EXT version of these functions as this simplifies
						the code where they are used, and the SGI implementation while currently
						only OpenGL 1.1 supplies both glTexImage3D and glTexImage3DEXT */
					if (!(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glTexImage3D, PFNGLTEXIMAGE3DPROC)
						Graphics_library_get_function_ptr("glTexImage3D")))
					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_EXT_texture3D) = return_code;
			}
		}
#endif /* GL_EXT_texture3D */
#if defined GL_EXT_framebuffer_object
		else if (!strcmp(extension_name, "GL_EXT_framebuffer_object"))
		{
			if (GLEXTENSION_UNSURE != GLEXTENSIONFLAG(GL_EXT_framebuffer_object))
			{
				return_code = GLEXTENSIONFLAG(GL_EXT_framebuffer_object);
			}
			else
			{
				return_code = query_gl_extension(extension_name);
				if (GLEXTENSION_AVAILABLE == return_code)
				{
					if (!((GRAPHICS_LIBRARY_ASSIGN_HANDLE(glGenFramebuffersEXT, PFNGLGENFRAMEBUFFERSEXTPROC)
						Graphics_library_get_function_ptr("glGenFramebuffersEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glBindFramebufferEXT, PFNGLBINDFRAMEBUFFEREXTPROC)
						Graphics_library_get_function_ptr("glBindFramebufferEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glGenRenderbuffersEXT, PFNGLGENRENDERBUFFERSEXTPROC)
						Graphics_library_get_function_ptr("glGenRenderbuffersEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glBindRenderbufferEXT, PFNGLBINDRENDERBUFFEREXTPROC)
						Graphics_library_get_function_ptr("glBindRenderbufferEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glFramebufferRenderbufferEXT, PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)
						Graphics_library_get_function_ptr("glFramebufferRenderbufferEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glFramebufferTexture2DEXT, PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
						Graphics_library_get_function_ptr("glFramebufferTexture2DEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glDeleteFramebuffersEXT, PFNGLDELETEFRAMEBUFFERSEXTPROC)
						Graphics_library_get_function_ptr("glDeleteFramebuffersEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glDeleteRenderbuffersEXT, PFNGLDELETERENDERBUFFERSEXTPROC)
						Graphics_library_get_function_ptr("glDeleteRenderbuffersEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glCheckFramebufferStatusEXT, PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)
							 Graphics_library_get_function_ptr("glCheckFramebufferStatusEXT")) &&
						(GRAPHICS_LIBRARY_ASSIGN_HANDLE(glRenderbufferStorageEXT, PFNGLRENDERBUFFERSTORAGEEXTPROC)
						Graphics_library_get_function_ptr("glRenderbufferStorageEXT"))))
					{
						return_code = GLEXTENSION_UNAVAILABLE;
					}
				}
				GLEXTENSIONFLAG(GL_EXT_framebuffer_object) = return_code;
			}
		}
#endif /* GL_EXT_FRAMEBUFFER_OBJECT */
		else
		{
			display_message(ERROR_MESSAGE,  "Graphics_library_load_extension.  "
				"Extension %s was not compiled in or is unknown.", extension_name);
			return_code = 0;
		}
#else /* defined (OPENGL_API) */
		return_code=0;
#endif /* defined (OPENGL_API) */
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Graphics_library_load_extension.  Missing extension name.");
		return_code = 0;
	}

	LEAVE;

	return (return_code);
} /* Graphics_library_load_extension */

int Graphics_library_load_extensions(char *extensions)
/*******************************************************************************
LAST MODIFIED : 20 February 2004

DESCRIPTION :
Attempts to load the space separated list of extensions.  Returns true if all
the extensions succeed, false if not.
==============================================================================*/
{
	char *extension, *next_extension;
	int return_code;

	ENTER(Graphics_library_load_extensions);
	if (extensions)
	{
		return_code = 1;
		extension = extensions;
		while (return_code && (next_extension = strchr(extension, ' ')))
		{
			*next_extension = 0;
			return_code = (GLEXTENSION_AVAILABLE == 
				Graphics_library_load_extension(extension));
			extension = next_extension + 1;
		}
		if (*extension)
		{
			return_code = (GLEXTENSION_AVAILABLE == 
				Graphics_library_load_extension(extension));
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"Graphics_library_load_extensions.  Missing extension name list.");
		return_code = 0;
	}

	LEAVE;

	return (return_code);
} /* Graphics_library_load_extensions */
