/*
 * "$Id$"
 *
 *   Test/profiling program for dithering code.
 *
 *   Copyright 1997-2000 Michael Sweet (mike@easysw.com)
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
 */

#include "print.h"


/*
 * NOTE: writing of 2-bit dither images is currently broken due to the
 * separated planes generated by the dither functions.
 */


/*
 * Definitions for dither test...
 */

#define IMAGE_WIDTH	5760	/* 8in * 720dpi */
#define IMAGE_HEIGHT	720	/* 4in * 720dpi */
#define BUFFER_SIZE	IMAGE_WIDTH

#define IMAGE_MIXED	0	/* Mix of line types */
#define IMAGE_WHITE	1	/* All white image */
#define IMAGE_BLACK	2	/* All black image */
#define IMAGE_COLOR	3	/* All color image */
#define IMAGE_RANDOM	4	/* All random image */

#define DITHER_GRAY	0	/* Dither grayscale pixels */
#define DITHER_COLOR	1	/* Dither color pixels */
#define DITHER_PHOTO	2	/* Dither photo pixels */


/*
 * Globals...
 */

int		image_type = IMAGE_MIXED;
int		dither_type = DITHER_COLOR;
int		dither_bits = 1;
unsigned short	white_line[IMAGE_WIDTH * 3],
		black_line[IMAGE_WIDTH * 3],
		color_line[IMAGE_WIDTH * 3],
		random_line[IMAGE_WIDTH * 3];


simple_dither_range_t normal_1bit_ranges[] =
{
  { 1.0,  0x1, 1, 1 }
};

simple_dither_range_t normal_2bit_ranges[] =
{
  { 0.45,  0x1, 1, 1 },
  { 0.68,  0x2, 1, 2 },
  { 1.0,   0x3, 1, 3 }
};

simple_dither_range_t photo_1bit_ranges[] =
{
  { 0.33, 0x1, 0, 1 },
  { 1.0,  0x1, 1, 1 }
};

simple_dither_range_t photo_2bit_ranges[] =
{
  { 0.15,  0x1, 0, 1 },
  { 0.227, 0x2, 0, 2 },
  { 0.45,  0x1, 1, 1 },
  { 0.68,  0x2, 1, 2 },
  { 1.0,   0x3, 1, 3 }
};



void image_init(void);
void image_get_row(unsigned short *data, int row);
void write_gray(FILE *fp, unsigned char *black);
void write_color(FILE *fp, unsigned char *cyan, unsigned char *magenta,
                 unsigned char *yellow, unsigned char *black);
void write_photo(FILE *fp, unsigned char *cyan, unsigned char *lcyan,
                 unsigned char *magenta, unsigned char *lmagenta,
                 unsigned char *yellow, unsigned char *black);


/*
 * 'main()' - Test dithering code for performance measurement.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line arguments */
     char *argv[])			/* I - Command-line arguments */
{
  int		i, j;			/* Looping vars */
  unsigned char	black[BUFFER_SIZE],	/* Black bitmap data */
		cyan[BUFFER_SIZE],	/* Cyan bitmap data */
		magenta[BUFFER_SIZE],	/* Magenta bitmap data */
		lcyan[BUFFER_SIZE],	/* Light cyan bitmap data */
		lmagenta[BUFFER_SIZE],	/* Light magenta bitmap data */
		yellow[BUFFER_SIZE];	/* Yellow bitmap data */
  void		*dither;		/* Dither data */
  convert_t	colorfunc;		/* Color conversion function... */
  unsigned short rgb[IMAGE_WIDTH * 3],	/* RGB buffer */
		gray[IMAGE_WIDTH];	/* Grayscale buffer */
  FILE		*fp;			/* PPM/PGM output file */
  char		filename[1024];		/* Name of file */
  time_t	start, end;		/* Start and end times */
  vars_t	v;			/* Dither variables */
  static char	*dither_types[] =	/* Different dithering modes */
		{
		  "gray",
		  "color",
		  "photo"
		};
  static char	*image_types[] =	/* Different image types */
		{
		  "mixed",
		  "white",
		  "black",
		  "color",
		  "random"
		};


 /*
  * Get command-line args...
  */

  for (i = 1; i < argc; i ++)
  {
    if (strcmp(argv[i], "1-bit") == 0)
    {
      dither_bits = 1;
      continue;
    }

    if (strcmp(argv[i], "2-bit") == 0)
    {
      dither_bits = 2;
      continue;
    }

    for (j = 0; j < 3; j ++)
      if (strcmp(argv[i], dither_types[j]) == 0)
        break;

    if (j < 3)
    {
      dither_type = j;
      continue;
    }

    for (j = 0; j < 5; j ++)
      if (strcmp(argv[i], image_types[j]) == 0)
        break;

    if (j < 3)
    {
      image_type = j;
      continue;
    }

    printf("Unknown option \"%s\" ignored!\n", argv[i]);
  }

 /*
  * Setup the image and color functions...
  */

  image_init();

 /*
  * Output the page...
  */

  memset(&v, 0, sizeof(v));
  strcpy(v.dither_algorithm, "Adaptive Hybrid");

  dither = init_dither(IMAGE_WIDTH, IMAGE_WIDTH, 1, 1, &v);

  dither_set_black_levels(dither, 1.0, 1.0, 1.0);

  if (dither_type == DITHER_PHOTO)
    dither_set_black_lower(dither, 0.4 / dither_bits + 0.1);
  else
    dither_set_black_lower(dither, 0.25 / dither_bits);

  dither_set_black_upper(dither, 0.5);

  if (dither_bits == 2)
  {
    if (dither_type == DITHER_PHOTO)
      dither_set_adaptive_divisor(dither, 8);
    else
      dither_set_adaptive_divisor(dither, 16);
  }  
  else
    dither_set_adaptive_divisor(dither, 4);

  switch (dither_type)
  {
    case DITHER_GRAY :
        switch (dither_bits)
	{
	  case 1 :
              dither_set_k_ranges(dither, 1, normal_1bit_ranges, 1.0);
	      break;
	  case 2 :
	      dither_set_transition(dither, 0.5);
              dither_set_k_ranges(dither, 3, normal_2bit_ranges, 1.0);
	      break;
       }
       break;
    case DITHER_COLOR :
        switch (dither_bits)
	{
	  case 1 :
              dither_set_c_ranges(dither, 1, normal_1bit_ranges, 1.0);
              dither_set_m_ranges(dither, 1, normal_1bit_ranges, 1.0);
              dither_set_y_ranges(dither, 1, normal_1bit_ranges, 1.0);
              dither_set_k_ranges(dither, 1, normal_1bit_ranges, 1.0);
	      break;
	  case 2 :
	      dither_set_transition(dither, 0.5);
              dither_set_c_ranges(dither, 3, normal_2bit_ranges, 1.0);
              dither_set_m_ranges(dither, 3, normal_2bit_ranges, 1.0);
              dither_set_y_ranges(dither, 3, normal_2bit_ranges, 1.0);
              dither_set_k_ranges(dither, 3, normal_2bit_ranges, 1.0);
	      break;
       }
       break;
    case DITHER_PHOTO :
        switch (dither_bits)
	{
	  case 1 :
              dither_set_c_ranges(dither, 2, photo_1bit_ranges, 1.0);
              dither_set_m_ranges(dither, 2, photo_1bit_ranges, 1.0);
              dither_set_y_ranges(dither, 1, normal_1bit_ranges, 1.0);
              dither_set_k_ranges(dither, 1, normal_1bit_ranges, 1.0);
	      break;
	  case 2 :
	      dither_set_transition(dither, 0.7);
              dither_set_c_ranges(dither, 5, photo_2bit_ranges, 1.0);
              dither_set_m_ranges(dither, 5, photo_2bit_ranges, 1.0);
              dither_set_y_ranges(dither, 3, normal_2bit_ranges, 1.0);
              dither_set_k_ranges(dither, 3, normal_2bit_ranges, 1.0);
	      break;
       }
       break;
  }

  dither_set_ink_spread(dither, 12 + dither_bits);
  dither_set_density(dither, 1.0);

 /*
  * Open the PPM/PGM file...
  */

  sprintf(filename, "%s-%s-%dbit.%s", image_types[image_type],
          dither_types[dither_type], dither_bits,
	  dither_type == DITHER_GRAY ? "pgm" : "ppm");

  if ((fp = fopen(filename, "wb")) != NULL)
  {
    puts(filename);
    if (dither_type == DITHER_GRAY)
      fputs("P5\n", fp);
    else
      fputs("P6\n", fp);

    fprintf(fp, "%d\n%d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  }
  else
    perror(filename);

 /*
  * Now dither the "page"...
  */

  start = time(NULL);

  for (i = 0; i < IMAGE_HEIGHT; i ++)
  {
    printf("\rProcessing row %d...", i);
    fflush(stdout);

    switch (dither_type)
    {
      case DITHER_GRAY :
          image_get_row(gray, i);
          dither_black(gray, i, dither, black, 0);
	  write_gray(fp, black);
	  break;
      case DITHER_COLOR :
          image_get_row(rgb, i);
	  dither_cmyk(rgb, i, dither, cyan, 0, magenta, 0,
		      yellow, 0, black, 0);
	  write_color(fp, cyan, magenta, yellow, black);
	  break;
      case DITHER_PHOTO :
          image_get_row(rgb, i);
	  dither_cmyk(rgb, i, dither, cyan, lcyan, magenta, lmagenta,
		      yellow, 0, black, 0);
	  write_photo(fp, cyan, lcyan, magenta, lmagenta, yellow, black);
	  break;
    }
  }

  end = time(NULL);

  free_dither(dither);

  if (fp != NULL)
    fclose(fp);

  printf("\rTotal dither time for %d pixels is %d seconds, or %.2f pixels/sec.\n",
         IMAGE_WIDTH * IMAGE_HEIGHT, end - start,
	 (float)(IMAGE_WIDTH * IMAGE_HEIGHT) / (float)(end - start));
}


void
image_get_row(unsigned short *data,
              int            row)
{
  unsigned short *src;


  switch (image_type)
  {
    case IMAGE_MIXED :
        switch ((row / 100) & 3)
	{
	  case 0 :
              src = white_line;
	      break;
	  case 1 :
              src = color_line;
	      break;
	  case 2 :
              src = black_line;
	      break;
	  case 3 :
              src = random_line;
	      break;
	}
        break;
    case IMAGE_WHITE :
        src = white_line;
        break;
    case IMAGE_BLACK :
        src = black_line;
        break;
    case IMAGE_COLOR :
        src = color_line;
        break;
    case IMAGE_RANDOM :
        src = random_line;
        break;
  }

  if (dither_type == DITHER_GRAY)
    memcpy(data, src, IMAGE_WIDTH * 2);
  else
    memcpy(data, src, IMAGE_WIDTH * 6);
}


void
image_init(void)
{
  int			i, j;
  unsigned short	*cptr,
			*rptr;


 /*
  * Set the white and black line data...
  */

  memset(white_line, 255, sizeof(white_line));
  memset(black_line, 0, sizeof(black_line));

 /*
  * Fill in the color and random data...
  */

  for (i = IMAGE_WIDTH, cptr = color_line, rptr = random_line; i > 0; i --)
  {
   /*
    * Do 64 color or grayscale blocks over the line...
    */

    j = i / (IMAGE_WIDTH / 64);

    if (dither_type == DITHER_GRAY)
      *cptr++ = 65535 * j / 63;
    else
    {
      *cptr++ = 65535 * (j >> 4) / 3;
      *cptr++ = 65535 * ((j >> 2) & 3) / 3;
      *cptr++ = 65535 * (j & 3) / 3;
    }

   /*
    * Do random colors over the line...
    */

    *rptr++ = 65535 * (rand() & 255) / 255;
    if (dither_type != DITHER_GRAY)
    {
      *rptr++ = 65535 * (rand() & 255) / 255;
      *rptr++ = 65535 * (rand() & 255) / 255;
    }
  }
}


void
write_gray(FILE          *fp,
           unsigned char *black)
{
  int		count,
		offset;
  unsigned char	byte,
		bit,
		shift;


  if (dither_bits == 1)
  {
    for (count = IMAGE_WIDTH, byte = *black++, bit = 128; count > 0; count --)
    {
      if (byte & bit)
        putc(0, fp);
      else
        putc(255, fp);

      if (bit > 1)
        bit >>= 1;
      else
      {
        byte = *black++;
	bit  = 128;
      }
    }
  }
  else
  {
    for (count = IMAGE_WIDTH, byte = *black++, shift = 6; count > 0; count --)
    {
      putc(255 - 255 * ((byte >> shift) & 3) / 3, fp);

      if (shift > 0)
        shift -= 2;
      else
      {
        byte  = *black++;
	shift = 6;
      }
    }
  }
}


void
write_color(FILE          *fp,
            unsigned char *cyan,
	    unsigned char *magenta,
            unsigned char *yellow,
	    unsigned char *black)
{
  int		count;
  unsigned char	cbyte,
                mbyte,
                ybyte,
                kbyte,
		bit,
		shift;
  int		r, g, b, k;


  if (dither_bits == 1)
  {
    for (count = IMAGE_WIDTH, cbyte = *cyan++, mbyte = *magenta++,
             ybyte = *yellow++, kbyte = *black++, bit = 128;
	 count > 0;
	 count --)
    {
      if (kbyte & bit)
      {
        putc(0, fp);
        putc(0, fp);
        putc(0, fp);
      }
      else
      {
	if (cbyte & bit)
          putc(0, fp);
	else
          putc(255, fp);

	if (mbyte & bit)
          putc(0, fp);
	else
          putc(255, fp);

	if (ybyte & bit)
          putc(0, fp);
	else
          putc(255, fp);
      }

      if (bit > 1)
        bit >>= 1;
      else
      {
        cbyte = *cyan++;
        mbyte = *magenta++;
        ybyte = *yellow++;
        kbyte = *black++;
	bit   = 128;
      }
    }
  }
  else
  {
    for (count = IMAGE_WIDTH, cbyte = *cyan++, mbyte = *magenta++,
             ybyte = *yellow++, kbyte = *black++, shift = 6;
	 count > 0;
	 count --)
    {
      k = 255 * ((kbyte >> shift) & 3) / 3;
      r = 255 - 255 * ((cbyte >> shift) & 3) / 3 - k;
      g = 255 - 255 * ((mbyte >> shift) & 3) / 3 - k;
      b = 255 - 255 * ((ybyte >> shift) & 3) / 3 - k;

      if (r < 0)
        putc(0, fp);
      else
        putc(r, fp);

      if (g < 0)
        putc(0, fp);
      else
        putc(g, fp);

      if (b < 0)
        putc(0, fp);
      else
        putc(b, fp);

      if (shift > 0)
        shift -= 2;
      else
      {
        cbyte = *cyan++;
        mbyte = *magenta++;
        ybyte = *yellow++;
        kbyte = *black++;
	shift = 6;
      }
    }
  }
}


void
write_photo(FILE          *fp,
            unsigned char *cyan,
            unsigned char *lcyan,
            unsigned char *magenta,
	    unsigned char *lmagenta,
            unsigned char *yellow,
            unsigned char *black)
{
  int		count;
  unsigned char	cbyte,
		lcbyte,
                mbyte,
                lmbyte,
                ybyte,
                kbyte,
		bit,
		shift;
  int		r, g, b, k;


  if (dither_bits == 1)
  {
    for (count = IMAGE_WIDTH, cbyte = *cyan++, lcbyte = *lcyan++,
             mbyte = *magenta++, lmbyte = *lmagenta++,
             ybyte = *yellow++, kbyte = *black++, bit = 128;
	 count > 0;
	 count --)
    {
      if (kbyte & bit)
      {
        putc(0, fp);
        putc(0, fp);
        putc(0, fp);
      }
      else
      {
	if (cbyte & bit)
          putc(0, fp);
	else if (lcbyte & bit)
          putc(127, fp);
	else
          putc(255, fp);

	if (mbyte & bit)
          putc(0, fp);
	else if (lmbyte & bit)
          putc(127, fp);
	else
          putc(255, fp);

	if (ybyte & bit)
          putc(0, fp);
	else
          putc(255, fp);
      }

      if (bit > 1)
        bit >>= 1;
      else
      {
        cbyte  = *cyan++;
        lcbyte = *lcyan++;
        mbyte  = *magenta++;
        lmbyte = *lmagenta++;
        ybyte  = *yellow++;
        kbyte  = *black++;
	bit    = 128;
      }
    }
  }
  else
  {
    for (count = IMAGE_WIDTH, cbyte = *cyan++, mbyte = *magenta++,
             ybyte = *yellow++, kbyte = *black++, shift = 6;
	 count > 0;
	 count --)
    {
      k = 255 * ((kbyte >> shift) & 3) / 3;
      r = 255 - 255 * ((cbyte >> shift) & 3) / 3 -
          127 * ((lcbyte >> shift) & 3) / 3 - k;
      g = 255 - 255 * ((mbyte >> shift) & 3) / 3 -
          127 * ((lmbyte >> shift) & 3) / 3 - k;
      b = 255 - 255 * ((ybyte >> shift) & 3) / 3 - k;

      if (r < 0)
        putc(0, fp);
      else
        putc(r, fp);

      if (g < 0)
        putc(0, fp);
      else
        putc(g, fp);

      if (b < 0)
        putc(0, fp);
      else
        putc(b, fp);

      if (shift > 0)
        shift -= 2;
      else
      {
        cbyte  = *cyan++;
        lcbyte = *lcyan++;
        mbyte  = *magenta++;
        lmbyte = *lmagenta++;
        ybyte  = *yellow++;
        kbyte  = *black++;
	shift  = 6;
      }
    }
  }
}


/*
 * End of "$Id$".
 */
