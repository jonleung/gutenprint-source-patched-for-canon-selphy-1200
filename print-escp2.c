/*
 * "$Id$"
 *
 *   Print plug-in EPSON ESC/P2 driver for the GIMP.
 *
 *   Copyright 1997-1999 Michael Sweet (mike@easysw.com) and
 *	Robert Krawitz (rlk@alum.mit.edu)
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Contents:
 *
 *   escp2_parameters()     - Return the parameter values for the given
 *                            parameter.
 *   escp2_imageable_area() - Return the imageable area of the page.
 *   escp2_print()          - Print an image to an EPSON printer.
 *   escp2_write()          - Send 6-color ESC/P2 graphics using TIFF packbits compression.
 *
 * Revision History:
 *
 *   $Log$
 *   Revision 1.13  1999/10/26 23:36:51  rlk
 *   Comment out all remaining 16-bit code, and rename 16-bit functions to "standard" names
 *
 *   Revision 1.12  1999/10/26 02:10:30  rlk
 *   Mostly fix save/load
 *
 *   Move all gimp, glib, gtk stuff into print.c (take it out of everything else).
 *   This should help port it to more general purposes later.
 *
 *   Revision 1.11  1999/10/25 23:31:59  rlk
 *   16-bit clean
 *
 *   Revision 1.10  1999/10/25 00:16:12  rlk
 *   Comment
 *
 *   Revision 1.9  1999/10/21 01:27:37  rlk
 *   More progress toward full 16-bit rendering
 *
 *   Revision 1.8  1999/10/19 02:04:59  rlk
 *   Merge all of the single-level print_cmyk functions
 *
 *   Revision 1.7  1999/10/18 01:37:19  rlk
 *   Add Stylus Photo 700 and switch to printer capabilities
 *
 *   Revision 1.6  1999/10/17 23:44:07  rlk
 *   16-bit everything (untested)
 *
 *   Revision 1.5  1999/10/14 01:59:59  rlk
 *   Saturation
 *
 *   Revision 1.4  1999/10/03 23:57:20  rlk
 *   Various improvements
 *
 *   Revision 1.3  1999/09/15 02:53:58  rlk
 *   Remove some stuff that seems to have no effect
 *
 *   Revision 1.2  1999/09/12 00:12:24  rlk
 *   Current best stuff
 *
 *   Revision 1.11  1999/05/29 16:35:26  yosh
 *   * configure.in
 *   * Makefile.am: removed tips files, AC_SUBST GIMP_PLUGINS and
 *   GIMP_MODULES so you can easily skip those parts of the build
 *
 *   * acinclude.m4
 *   * config.sub
 *   * config.guess
 *   * ltconfig
 *   * ltmain.sh: libtool 1.3.2
 *
 *   * app/fileops.c: shuffle #includes to avoid warning about MIN and
 *   MAX
 *
 *   [ The following is a big i18n patch from David Monniaux
 *     <david.monniaux@ens.fr> ]
 *
 *   * tips/gimp_conseils.fr.txt
 *   * tips/gimp_tips.txt
 *   * tips/Makefile.am
 *   * configure.in: moved tips to separate dir
 *
 *   * po-plugins: new dir for plug-in translation files
 *
 *   * configure.in: add po-plugins dir and POTFILES processing
 *
 *   * app/boundary.c
 *   * app/brightness_contrast.c
 *   * app/by_color_select.c
 *   * app/color_balance.c
 *   * app/convert.c
 *   * app/curves.c
 *   * app/free_select.c
 *   * app/gdisplay.c
 *   * app/gimpimage.c
 *   * app/gimpunit.c
 *   * app/gradient.c
 *   * app/gradient_select.c
 *   * app/install.c
 *   * app/session.c: various i18n tweaks
 *
 *   * app/tips_dialog.c: localize tips filename
 *
 *   * libgimp/gimpunit.c
 *   * libgimp/gimpunitmenu.c: #include "config.h"
 *
 *   * plug-ins/CEL
 *   * plug-ins/CML_explorer
 *   * plug-ins/Lighting
 *   * plug-ins/apply_lens
 *   * plug-ins/autostretch_hsv
 *   * plug-ins/blur
 *   * plug-ins/bmp
 *   * plug-ins/borderaverage
 *   * plug-ins/bumpmap
 *   * plug-ins/bz2
 *   * plug-ins/checkerboard
 *   * plug-ins/colorify
 *   * plug-ins/compose
 *   * plug-ins/convmatrix
 *   * plug-ins/cubism
 *   * plug-ins/depthmerge
 *   * plug-ins/destripe
 *   * plug-ins/gif
 *   * plug-ins/gifload
 *   * plug-ins/jpeg
 *   * plug-ins/mail
 *   * plug-ins/oilify
 *   * plug-ins/png
 *   * plug-ins/print
 *   * plug-ins/ps
 *   * plug-ins/xbm
 *   * plug-ins/xpm
 *   * plug-ins/xwd: plug-in i18n stuff
 *
 *   -Yosh
 *
 *   Revision 1.10  1998/08/28 23:01:44  yosh
 *   * acconfig.h
 *   * configure.in
 *   * app/main.c: added check for putenv and #ifdefed it's usage since NeXTStep is
 *   lame
 *
 *   * libgimp/gimp.c
 *   * app/main.c
 *   * app/plug_in.c: conditionally compile shared mem stuff so platforms without
 *   it can still work
 *
 *   * plug-ins/CEL/CEL.c
 *   * plug-ins/palette/palette.c
 *   * plug-ins/print/print-escp2.c
 *   * plug-ins/print/print-pcl.c
 *   * plug-ins/print/print-ps.c: s/strdup/g_strdup/ for portability
 *
 *   -Yosh
 *
 *   Revision 1.9  1998/05/17 07:16:45  yosh
 *   0.99.31 fun
 *
 *   updated print plugin
 *
 *   -Yosh
 *
 *   Revision 1.11  1998/05/15  21:01:51  mike
 *   Updated image positioning code (invert top and center left/top independently)
 *
 *   Revision 1.10  1998/05/08  21:18:34  mike
 *   Now enable microweaving in 720 DPI mode.
 *
 *   Revision 1.9  1998/05/08  20:49:43  mike
 *   Updated to support media size, imageable area, and parameter functions.
 *   Added support for scaling modes - scale by percent or scale by PPI.
 *
 *   Revision 1.8  1998/01/21  21:33:47  mike
 *   Updated copyright.
 *
 *   Revision 1.7  1997/11/12  15:57:48  mike
 *   Minor changes for clean compiles under Digital UNIX.
 *
 *   Revision 1.7  1997/11/12  15:57:48  mike
 *   Minor changes for clean compiles under Digital UNIX.
 *
 *   Revision 1.6  1997/07/30  20:33:05  mike
 *   Final changes for 1.1 release.
 *
 *   Revision 1.6  1997/07/30  20:33:05  mike
 *   Final changes for 1.1 release.
 *
 *   Revision 1.5  1997/07/30  18:47:39  mike
 *   Added scaling, orientation, and offset options.
 *
 *   Revision 1.4  1997/07/15  20:57:11  mike
 *   Updated ESC 800/1520/3000 output code to use vertical spacing of 5 instead of 40.
 *
 *   Revision 1.3  1997/07/03  13:21:15  mike
 *   Updated documentation for 1.0 release.
 *
 *   Revision 1.2  1997/07/03  13:03:57  mike
 *   Added horizontal offset to try to center image.
 *   Got rid of initial vertical positioning since the top margin is
 *   now set properly.
 *
 *   Revision 1.2  1997/07/03  13:03:57  mike
 *   Added horizontal offset to try to center image.
 *   Got rid of initial vertical positioning since the top margin is
 *   now set properly.
 *
 *   Revision 1.1  1997/07/02  13:51:53  mike
 *   Initial revision
 */

/*
 * Stylus Photo EX added by Robert Krawitz <rlk@alum.mit.edu> August 30, 1999
 */

#include "print.h"

/*
 * Local functions...
 */

static void	escp2_write(FILE *, unsigned char *, int, int, int, int, int, int, int);

#define MODEL_PAPER_SIZE_MASK	0x3
#define MODEL_PAPER_SMALL 	0x0
#define MODEL_PAPER_LARGE 	0x1
#define MODEL_PAPER_1200	0x2

#define MODEL_IMAGEABLE_MASK	0xc
#define MODEL_IMAGEABLE_DEFAULT	0x0
#define MODEL_IMAGEABLE_PHOTO	0x4
#define MODEL_IMAGEABLE_600	0x8

#define MODEL_INIT_MASK		0xf0
#define MODEL_INIT_COLOR	0x00
#define MODEL_INIT_PRO		0x10
#define MODEL_INIT_1500		0x20
#define MODEL_INIT_600		0x30
#define MODEL_INIT_PHOTO	0x40

#define MODEL_HASBLACK_MASK	0x100
#define MODEL_HASBLACK_YES	0x000
#define MODEL_HASBLACK_NO	0x100

#define MODEL_6COLOR_MASK	0x200
#define MODEL_6COLOR_NO		0x000
#define MODEL_6COLOR_YES	0x200

#define MODEL_720DPI_MODE_MASK	0xc00
#define MODEL_720DPI_DEFAULT	0x000
#define MODEL_720DPI_600	0x400
#define MODEL_720DPI_PHOTO	0x400 /* 0x800 for experimental stuff */

int model_capabilities[] =
{
  /* Stylus Color */
  (MODEL_PAPER_SMALL | MODEL_IMAGEABLE_DEFAULT | MODEL_INIT_COLOR
   | MODEL_HASBLACK_YES | MODEL_6COLOR_NO | MODEL_720DPI_DEFAULT),
  /* Stylus Color Pro/Pro XL/400/500 */
  (MODEL_PAPER_SMALL | MODEL_IMAGEABLE_DEFAULT | MODEL_INIT_PRO
   | MODEL_HASBLACK_YES | MODEL_6COLOR_NO | MODEL_720DPI_DEFAULT),
  /* Stylus Color 1500 */
  (MODEL_PAPER_LARGE | MODEL_IMAGEABLE_DEFAULT | MODEL_INIT_1500
   | MODEL_HASBLACK_NO | MODEL_6COLOR_NO | MODEL_720DPI_DEFAULT),
  /* Stylus Color 600 */
  (MODEL_PAPER_SMALL | MODEL_IMAGEABLE_600 | MODEL_INIT_600
   | MODEL_HASBLACK_YES | MODEL_6COLOR_NO | MODEL_720DPI_600),
  /* Stylus Color 800 */
  (MODEL_PAPER_SMALL | MODEL_IMAGEABLE_600 | MODEL_INIT_600
   | MODEL_HASBLACK_YES | MODEL_6COLOR_NO | MODEL_720DPI_DEFAULT),
  /* Stylus Color 1520/3000 */
  (MODEL_PAPER_LARGE | MODEL_IMAGEABLE_600 | MODEL_INIT_600
   | MODEL_HASBLACK_YES | MODEL_6COLOR_NO | MODEL_720DPI_DEFAULT),
  /* Stylus Photo 700 */
  (MODEL_PAPER_SMALL | MODEL_IMAGEABLE_PHOTO | MODEL_INIT_PHOTO
   | MODEL_HASBLACK_YES | MODEL_6COLOR_YES | MODEL_720DPI_PHOTO),
  /* Stylus Photo EX */
  (MODEL_PAPER_LARGE | MODEL_IMAGEABLE_PHOTO | MODEL_INIT_PHOTO
   | MODEL_HASBLACK_YES | MODEL_6COLOR_YES | MODEL_720DPI_PHOTO),
};

static int
escp2_has_cap(int model, int featureset, int class)
{
  return ((model_capabilities[model] & featureset) == class);
}

static int
escp2_cap(int model, int featureset)
{
  return (model_capabilities[model] & featureset);
}

/*
 * 'escp2_parameters()' - Return the parameter values for the given parameter.
 */

char **					/* O - Parameter values */
escp2_parameters(int  model,		/* I - Printer model */
                 char *ppd_file,	/* I - PPD file (not used) */
                 char *name,		/* I - Name of parameter */
                 int  *count)		/* O - Number of values */
{
  int		i;
  char		**p,
		**valptrs;
  static char	*media_sizes[] =
		{
		  ("Letter"),
		  ("Legal"),
		  ("A4"),
		  ("Tabloid"),
		  ("A3"),
		  ("12x18")
		};
  static char	*resolutions[] =
		{
		  ("360 DPI"),
		  ("720 DPI")
		};


  if (count == NULL)
    return (NULL);

  *count = 0;

  if (name == NULL)
    return (NULL);

  if (strcmp(name, "PageSize") == 0)
  {
    if (escp2_has_cap(model, MODEL_PAPER_SIZE_MASK, MODEL_PAPER_LARGE))
      *count = 6;
    else
      *count = 3;

    p = media_sizes;
  }
  else if (strcmp(name, "Resolution") == 0)
  {
    *count = 2;
    p = resolutions;
  }
  else
    return (NULL);

  valptrs = malloc(*count * sizeof(char *));
  for (i = 0; i < *count; i ++)
    {
      /* strdup doesn't appear to be POSIX... */
      valptrs[i] = malloc(strlen(p[i]) + 1);
      strcpy(valptrs[i], p[i]);
    }

  return (valptrs);
}


/*
 * 'escp2_imageable_area()' - Return the imageable area of the page.
 */

void
escp2_imageable_area(int  model,	/* I - Printer model */
                     char *ppd_file,	/* I - PPD file (not used) */
                     char *media_size,	/* I - Media size */
                     int  *left,	/* O - Left position in points */
                     int  *right,	/* O - Right position in points */
                     int  *bottom,	/* O - Bottom position in points */
                     int  *top)		/* O - Top position in points */
{
  int	width, length;			/* Size of page */


  default_media_size(model, ppd_file, media_size, &width, &length);

  switch (escp2_cap(model, MODEL_IMAGEABLE_MASK))
  {
  case MODEL_IMAGEABLE_PHOTO:
        *left   = 9;
        *right  = width - 9;
        *top    = length;
        *bottom = 49;
        break;

  case MODEL_IMAGEABLE_600:
        *left   = 8;
        *right  = width - 9;
        *top    = length - 32;
        *bottom = 40;
        break;

  case MODEL_IMAGEABLE_DEFAULT:
  default:
        *left   = 14;
        *right  = width - 14;
        *top    = length - 14;
        *bottom = 40;
        break;
  }
}


/*
 * 'escp2_print()' - Print an image to an EPSON printer.
 */

void
escp2_print(int       model,		/* I - Model */
            char      *ppd_file,	/* I - PPD file (not used) */
            char      *resolution,	/* I - Resolution */
            char      *media_size,	/* I - Media size */
            char      *media_type,	/* I - Media type */
            char      *media_source,	/* I - Media source */
            int       output_type,	/* I - Output type (color/grayscale) */
            int       orientation,	/* I - Orientation of image */
            float     scaling,		/* I - Scaling of image */
            int       left,		/* I - Left offset of image (points) */
            int       top,		/* I - Top offset of image (points) */
            int       copies,		/* I - Number of copies */
            FILE      *prn,		/* I - File to print to */
	    Image     image,		/* I - Image to print */
            unsigned char    *cmap,	/* I - Colormap (for indexed images) */
	    lut_t     *lut,		/* I - Brightness lookup table (16-bit) */
	    float     saturation	/* I - Saturation */
	    )
{
  int		x, y;		/* Looping vars */
  int		xdpi, ydpi;	/* Resolution */
  int		n;		/* Output number */
  unsigned short *out;	/* Output pixels (16-bit) */
  unsigned char	*in,		/* Input pixels */
		*black,		/* Black bitmap data */
		*cyan,		/* Cyan bitmap data */
		*magenta,	/* Magenta bitmap data */
		*lcyan,		/* Light cyan bitmap data */
		*lmagenta,	/* Light magenta bitmap data */
		*yellow;	/* Yellow bitmap data */
  int		page_left,	/* Left margin of page */
		page_right,	/* Right margin of page */
		page_top,	/* Top of page */
		page_bottom,	/* Bottom of page */
		page_width,	/* Width of page */
		page_height,	/* Height of page */
		page_length,	/* True length of page */
		out_width,	/* Width of image on page */
		out_height,	/* Height of image on page */
		out_bpp,	/* Output bytes per pixel */
		temp_width,	/* Temporary width of image on page */
		temp_height,	/* Temporary height of image on page */
		landscape,	/* True if we rotate the output 90 degrees */
		length,		/* Length of raster data */
		errdiv,		/* Error dividend */
		errmod,		/* Error modulus */
		errval,		/* Current error value */
		errline,	/* Current raster line */
		errlast;	/* Last raster line loaded */
  convert_t	colorfunc = 0;	/* Color conversion function... */
  int           image_height,
                image_width,
                image_bpp;

 /*
  * Setup a read-only pixel region for the entire image...
  */

  Image_init(image);
  image_height = Image_height(image);
  image_width = Image_width(image);
  image_bpp = Image_bpp(image);

 /*
  * Choose the correct color conversion function...
  */

  if (image_bpp < 3 && cmap == NULL)
    output_type = OUTPUT_GRAY;		/* Force grayscale output */

  if (output_type == OUTPUT_COLOR)
  {
    out_bpp = 3;

    if (image_bpp >= 3)
      colorfunc = rgb_to_rgb;
    else
      colorfunc = indexed_to_rgb;
  }
  else
  {
    out_bpp = 1;

    if (image_bpp >= 3)
      colorfunc = rgb_to_gray;
    else if (cmap == NULL)
      colorfunc = gray_to_gray;
    else
      colorfunc = indexed_to_gray;
  }

 /*
  * Figure out the output resolution...
  */

  xdpi = ydpi = atoi(resolution);

 /*
  * Compute the output size...
  */

  landscape   = 0;
  escp2_imageable_area(model, ppd_file, media_size, &page_left, &page_right,
                       &page_bottom, &page_top);

  page_width  = page_right - page_left;
  page_height = page_top - page_bottom;

  default_media_size(model, ppd_file, media_size, &n, &page_length);

 /*
  * Portrait width/height...
  */

  if (scaling < 0.0)
  {
   /*
    * Scale to pixels per inch...
    */

    out_width  = image_width * -72.0 / scaling;
    out_height = image_height * -72.0 / scaling;
  }
  else
  {
   /*
    * Scale by percent...
    */

    out_width  = page_width * scaling / 100.0;
    out_height = out_width * image_height / image_width;
    if (out_height > page_height)
    {
      out_height = page_height * scaling / 100.0;
      out_width  = out_height * image_width / image_height;
    }
  }

 /*
  * Landscape width/height...
  */

  if (scaling < 0.0)
  {
   /*
    * Scale to pixels per inch...
    */

    temp_width  = image_height * -72.0 / scaling;
    temp_height = image_width * -72.0 / scaling;
  }
  else
  {
   /*
    * Scale by percent...
    */

    temp_width  = page_width * scaling / 100.0;
    temp_height = temp_width * image_width / image_height;
    if (temp_height > page_height)
    {
      temp_height = page_height;
      temp_width  = temp_height * image_height / image_width;
    }
  }

 /*
  * See which orientation has the greatest area (or if we need to rotate the
  * image to fit it on the page...)
  */

  if (orientation == ORIENT_AUTO)
  {
    if (scaling < 0.0)
    {
      if ((out_width > page_width && out_height < page_width) ||
          (out_height > page_height && out_width < page_height))
	orientation = ORIENT_LANDSCAPE;
      else
	orientation = ORIENT_PORTRAIT;
    }
    else
    {
      if ((temp_width * temp_height) > (out_width * out_height))
	orientation = ORIENT_LANDSCAPE;
      else
	orientation = ORIENT_PORTRAIT;
    }
  }

  if (orientation == ORIENT_LANDSCAPE)
  {
    out_width  = temp_width;
    out_height = temp_height;
    landscape  = 1;

   /*
    * Swap left/top offsets...
    */

    x    = top;
    top  = left;
    left = x;
  }

  if (left < 0)
    left = (page_width - out_width) / 2 + page_left;

  if (top < 0)
    top  = (page_height + out_height) / 2 + page_bottom;
  else
    top = page_height - top + page_bottom;

 /*
  * Let the user know what we're doing...
  */

  Image_progress_init(image);

 /*
  * Send ESC/P2 initialization commands...
  */

  fputs("\033@", prn); 				/* ESC/P2 reset */

#if 0
  if (escp2_has_cap(model, MODEL_INIT_MASK, MODEL_INIT_PHOTO))
    {
      fwrite("\033@", 2, 1, prn);
      fwrite("\033(R\010\000\000REMOTE1PM\002\000\000\000SN\003\000\000\000\003MS\010\000\000\000\010\000\364\013x\017\033\000\000\000", 42, 1, prn);
    }
#endif
  fwrite("\033(G\001\000\001", 6, 1, prn);	/* Enter graphics mode */
  switch (ydpi)					/* Set line feed increment */
  {
    case 180 :
        fwrite("\033(U\001\000\024", 6, 1, prn);
        break;

    case 360 :
        fwrite("\033(U\001\000\012", 6, 1, prn);
        break;

    case 720 :
        fwrite("\033(U\001\000\005", 6, 1, prn);
        break;
  }

  switch (escp2_cap(model, MODEL_INIT_MASK)) /* Printer specific initialization */
  {
    case MODEL_INIT_COLOR : /* ESC */
        if (output_type == OUTPUT_COLOR && ydpi > 360)
      	  fwrite("\033(i\001\000\001", 6, 1, prn);	/* Microweave mode on */
        break;

    case MODEL_INIT_PRO : /* ESC Pro, Pro XL, 400, 500 */
        fwrite("\033(e\002\000\000\001", 7, 1, prn);	/* Small dots */

        if (ydpi > 360)
      	  fwrite("\033(i\001\000\001", 6, 1, prn);	/* Microweave mode on */
        break;

    case MODEL_INIT_1500 : /* ESC 1500 */
        fwrite("\033(e\002\000\000\001", 7, 1, prn);	/* Small dots */

        if (ydpi > 360)
      	  fwrite("\033(i\001\000\001", 6, 1, prn);	/* Microweave mode on */
        break;

    case MODEL_INIT_600 : /* ESC 600, 800, 1520, 3000 */
	if (output_type == OUTPUT_GRAY)
	  fwrite("\033(K\002\000\000\001", 7, 1, prn);	/* Fast black printing */
	else
	  fwrite("\033(K\002\000\000\002", 7, 1, prn);	/* Color printing */

        fwrite("\033(e\002\000\000\002", 7, 1, prn);	/* Small dots */

        if (ydpi > 360)
      	  fwrite("\033(i\001\000\001", 6, 1, prn);	/* Microweave mode on */
        break;

    case MODEL_INIT_PHOTO:
        if (ydpi > 360)
	  {
#if 1
	    fwrite("\033U\000", 3, 1, prn); /* Unidirectional */
	    fwrite("\033(i\001\000\001", 6, 1, prn);	/* Microweave mode on */
	    fwrite("\033\0311", 3, 1, prn); /* ??? */
	    fwrite("\033(e\002\000\000\004", 7, 1, prn);	/* Micro dots */
#else
	    fwrite("\033(i\001\000\000", 6, 1, prn); /* Microweave off */
	    fwrite("\033(e\002\000\000\004", 7, 1, prn);	/* Super micro dots */
#endif
	  }
	else
	  fwrite("\033(e\002\000\000\003", 7, 1, prn);	/* Whatever dots */
        break;

#if 0
	if (output_type == OUTPUT_GRAY)
	  fwrite("\033(K\002\000\000\001", 7, 1, prn);	/* Fast black printing */
	else
	  fwrite("\033(K\002\000\000\002", 7, 1, prn);	/* Color printing */
#endif
  }

  fwrite("\033(C\002\000", 5, 1, prn);		/* Page length */
  n = ydpi * page_length / 72;
  putc(n & 255, prn);
  putc(n >> 8, prn);

  fwrite("\033(c\004\000", 5, 1, prn);		/* Top/bottom margins */
  n = ydpi * (page_length - page_top) / 72;
  putc(n & 255, prn);
  putc(n >> 8, prn);
  n = ydpi * (page_length - page_bottom) / 72;
  putc(n & 255, prn);
  putc(n >> 8, prn);

  fwrite("\033(V\002\000", 5, 1, prn);		/* Absolute vertical position */
  n = ydpi * (page_length - top) / 72;
  putc(n & 255, prn);
  putc(n >> 8, prn);

 /*
  * Convert image size to printer resolution...
  */

  out_width  = xdpi * out_width / 72;
  out_height = ydpi * out_height / 72;

  left = ydpi * (left - page_left) / 72;

 /*
  * Allocate memory for the raster data...
  */

  length = (out_width + 7) / 8;

  if (output_type == OUTPUT_GRAY)
  {
    black   = malloc(length);
    cyan    = NULL;
    magenta = NULL;
    lcyan    = NULL;
    lmagenta = NULL;
    yellow  = NULL;
  }
  else
  {
    cyan    = malloc(length);
    magenta = malloc(length);
    yellow  = malloc(length);
  
    if (escp2_has_cap(model, MODEL_HASBLACK_MASK, MODEL_HASBLACK_YES))
      black = malloc(length);
    else
      black = NULL;
    if (escp2_has_cap(model, MODEL_6COLOR_MASK, MODEL_6COLOR_YES)) {
      lcyan = malloc(length);
      lmagenta = malloc(length);
    } else {
      lcyan = NULL;
      lmagenta = NULL;
    }
  }
    
 /*
  * Output the page, rotating as necessary...
  */

  if (landscape)
  {
    in  = malloc(image_height * image_bpp);
    out = malloc(image_height * out_bpp * 2);

    errdiv  = image_width / out_height;
    errmod  = image_width % out_height;
    errval  = 0;
    errlast = -1;
    errline  = image_width - 1;
    
    for (x = 0; x < out_height; x ++)
    {
#ifdef DEBUG
      printf("escp2_print: x = %d, line = %d, val = %d, mod = %d, height = %d\n",
             x, errline, errval, errmod, out_height);
#endif /* DEBUG */

      if ((x & 255) == 0)
	Image_note_progress(image, x, out_height);

      if (errline != errlast)
      {
        errlast = errline;
	Image_get_col(image, in, errline);
      }

      (*colorfunc)(in, out, image_height, image_bpp, lut, cmap,
		   saturation);

      if (output_type == OUTPUT_GRAY)
      {
        dither_black(out, x, image_height, out_width, black);
        escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
      }
      else if (escp2_has_cap(model, MODEL_6COLOR_MASK, MODEL_6COLOR_YES))
      {
        dither_cmyk(out, x, image_height, out_width, cyan, lcyan,
		      magenta, lmagenta, yellow, 0, black);

	escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
        escp2_write(prn, cyan, length, 0, 2, ydpi, model, out_width, left);
        escp2_write(prn, magenta, length, 0, 1, ydpi, model, out_width, left);
        escp2_write(prn, yellow, length, 0, 4, ydpi, model, out_width, left);
        escp2_write(prn, lcyan, length, 1, 2, ydpi, model, out_width, left);
        escp2_write(prn, lmagenta, length, 1, 1, ydpi, model, out_width, left);
      }
      else
      {
        dither_cmyk(out, x, image_height, out_width, cyan, 0, magenta, 0,
		      yellow, 0, black);

        escp2_write(prn, cyan, length, 0, 2, ydpi, model, out_width, left);
        escp2_write(prn, magenta, length, 0, 1, ydpi, model, out_width, left);
        escp2_write(prn, yellow, length, 0, 4, ydpi, model, out_width, left);
        if (black != NULL)
          escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
      }

      fwrite("\033(v\002\000\001\000", 7, 1, prn);	/* Feed one line */

      errval += errmod;
      errline -= errdiv;
      if (errval >= out_height)
      {
        errval -= out_height;
        errline --;
      }
    }
  }
  else
  {
    in  = malloc(image_width * image_bpp);
    out = malloc(image_width * out_bpp * 2);

    errdiv  = image_height / out_height;
    errmod  = image_height % out_height;
    errval  = 0;
    errlast = -1;
    errline  = 0;
    
    for (y = 0; y < out_height; y ++)
    {
#ifdef DEBUG
      printf("escp2_print: y = %d, line = %d, val = %d, mod = %d, height = %d\n",
             y, errline, errval, errmod, out_height);
#endif /* DEBUG */

      if ((y & 255) == 0)
	Image_note_progress(image, y, out_height);

      if (errline != errlast)
      {
        errlast = errline;
	Image_get_row(image, in, errline);
      }

      (*colorfunc)(in, out, image_width, image_bpp, lut, cmap,
		   saturation);

      if (output_type == OUTPUT_GRAY)
      {
        dither_black(out, y, image_width, out_width, black);
        escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
      }
      else if (escp2_has_cap(model, MODEL_6COLOR_MASK, MODEL_6COLOR_YES))
      {
        dither_cmyk(out, y, image_width, out_width, cyan, lcyan,
		      magenta, lmagenta, yellow, 0, black);

        escp2_write(prn, lcyan, length, 1, 2, ydpi, model, out_width, left);
        escp2_write(prn, lmagenta, length, 1, 1, ydpi, model, out_width, left);
        escp2_write(prn, yellow, length, 0, 4, ydpi, model, out_width, left);
        escp2_write(prn, cyan, length, 0, 2, ydpi, model, out_width, left);
        escp2_write(prn, magenta, length, 0, 1, ydpi, model, out_width, left);
	escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
      }
      else
      {
        dither_cmyk(out, y, image_width, out_width, cyan, 0, magenta, 0,
		      yellow, 0, black);

        escp2_write(prn, cyan, length, 0, 2, ydpi, model, out_width, left);
        escp2_write(prn, magenta, length, 0, 1, ydpi, model, out_width, left);
        escp2_write(prn, yellow, length, 0, 4, ydpi, model, out_width, left);
        if (black != NULL)
          escp2_write(prn, black, length, 0, 0, ydpi, model, out_width, left);
      }

      fwrite("\033(v\002\000\001\000", 7, 1, prn);	/* Feed one line */

      errval += errmod;
      errline += errdiv;
      if (errval >= out_height)
      {
        errval -= out_height;
        errline ++;
      }
    }
  }

 /*
  * Cleanup...
  */

  free(in);
  free(out);

  if (black != NULL)
    free(black);
  if (cyan != NULL)
  {
    free(cyan);
    free(magenta);
    free(yellow);
  }

  putc('\014', prn);			/* Eject page */
  fputs("\033@", prn);			/* ESC/P2 reset */
}


/*
 * 'escp2_write()' - Send ESC/P2 graphics using TIFF packbits compression.
 */

void
escp2_write(FILE          *prn,	/* I - Print file or command */
	     unsigned char *line,	/* I - Output bitmap data */
	     int           length,	/* I - Length of bitmap data */
	     int	   density,     /* I - 0 for dark, 1 for light */
	     int           plane,	/* I - Which color */
	     int           ydpi,	/* I - Vertical resolution */
	     int           model,	/* I - Printer model */
	     int           width,	/* I - Printed width */
	     int           offset)	/* I - Offset from left side */
{
  unsigned char	comp_buf[1536],		/* Compression buffer */
		*comp_ptr,		/* Current slot in buffer */
		*start,			/* Start of compressed data */
		repeat;			/* Repeating char */
  int		count,			/* Count of compressed bytes */
		tcount;			/* Temporary count < 128 */
  static int    last_density = 0;       /* Last density printed */
  static int	last_plane = 0;		/* Last color plane printed */


 /*
  * Don't send blank lines...
  */

  if (line[0] == 0 && memcmp(line, line + 1, length - 1) == 0)
    return;

 /*
  * Compress using TIFF "packbits" run-length encoding...
  */

  comp_ptr = comp_buf;

  while (length > 0)
  {
   /*
    * Get a run of non-repeated chars...
    */

    start  = line;
    line   += 2;
    length -= 2;

    while (length > 0 && (line[-2] != line[-1] || line[-1] != line[0]))
    {
      line ++;
      length --;
    }

    line   -= 2;
    length += 2;

   /*
    * Output the non-repeated sequences (max 128 at a time).
    */

    count = line - start;
    while (count > 0)
    {
      tcount = count > 128 ? 128 : count;

      comp_ptr[0] = tcount - 1;
      memcpy(comp_ptr + 1, start, tcount);

      comp_ptr += tcount + 1;
      start    += tcount;
      count    -= tcount;
    }

    if (length <= 0)
      break;

   /*
    * Find the repeated sequences...
    */

    start  = line;
    repeat = line[0];

    line ++;
    length --;

    while (length > 0 && *line == repeat)
    {
      line ++;
      length --;
    }

   /*
    * Output the repeated sequences (max 128 at a time).
    */

    count = line - start;
    while (count > 0)
    {
      tcount = count > 128 ? 128 : count;

      comp_ptr[0] = 1 - tcount;
      comp_ptr[1] = repeat;

      comp_ptr += 2;
      count    -= tcount;
    }
  }

 /*
  * Set the print head position.
  */

  putc('\r', prn);
 /*
  * Set the color if necessary...
  */

  if (last_plane != plane || last_density != density)
  {
    last_plane = plane;
    last_density = density;
    if (escp2_has_cap(model, MODEL_6COLOR_MASK, MODEL_6COLOR_YES))
      fprintf(prn, "\033(r\002%c%c%c", 0, density, plane);
    else
      fprintf(prn, "\033r%c", plane);
  }

  if (escp2_has_cap(model, MODEL_6COLOR_MASK, MODEL_6COLOR_YES))
    fprintf(prn, "\033(\\%c%c%c%c%c%c", 4, 0, 160, 5,
	    (offset * 1440 / ydpi) & 255, (offset * 1440 / ydpi) >> 8);
  else
    fprintf(prn, "\033\\%c%c", offset & 255, offset >> 8);

 /*
  * Send a line of raster graphics...
  */

  switch (ydpi)				/* Raster graphics header */
  {
    case 180 :
        fwrite("\033.\001\024\024\001", 6, 1, prn);
        break;
    case 360 :
        fwrite("\033.\001\012\012\001", 6, 1, prn);
        break;
    case 720 :
        if (escp2_has_cap(model, MODEL_720DPI_MODE_MASK, MODEL_720DPI_600))
          fwrite("\033.\001\050\005\001", 6, 1, prn);
#if 0
        else if (model == 7)
          fwrite("\033.\000\050\005\040", 6, 1, prn);
#endif
	else
          fwrite("\033.\001\005\005\001", 6, 1, prn);
        break;
  }

  putc(width & 255, prn);		/* Width of raster line in pixels */
  putc(width >> 8, prn);

  fwrite(comp_buf, comp_ptr - comp_buf, 1, prn);
}

/*
 * End of "$Id$".
 */
