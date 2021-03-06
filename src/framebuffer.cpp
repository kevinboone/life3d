/*============================================================================

  framebuffer.c

  Copyright (c)2020-1 Kevin Boone, GPL v3.0

  Note that this was originally a C, not C++, module; hence the rather
  unidiomatic usage.

============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "defs.h" 
#include "log.h" 
#include "framebuffer.h" 

#define max(a, b) ((a) > (b) ? (a) : (b))

struct _FrameBuffer
  {
  int fd;
  int w;
  int h;
  int fb_data_size;
  BYTE *fb_data;
  char *fbdev;
  int fb_bytes;
  int line_length;
  int stride;
  int slop;
  BOOL linear;
  }; 


/*==========================================================================
  framebuffer_create
*==========================================================================*/
FrameBuffer *framebuffer_create (const char *fbdev)
  {
  LOG_IN
  FrameBuffer *self = (FrameBuffer *)malloc (sizeof (FrameBuffer));
  self->fbdev = strdup (fbdev);
  self->fd = -1;
  self->fb_data = NULL;
  self->fb_data_size = 0;
  LOG_OUT 
  return self;
  }


/*==========================================================================
  framebuffer_init
*==========================================================================*/
BOOL framebuffer_init (FrameBuffer *self, char **error)
  {
  LOG_IN
  BOOL ret = FALSE;
  log_debug ("Opening framebuffer %s", self->fbdev);
  self->fd = open (self->fbdev, O_RDWR);
  if (self->fd >= 0)
    {
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    ioctl (self->fd, FBIOGET_FSCREENINFO, &finfo);
    ioctl (self->fd, FBIOGET_VSCREENINFO, &vinfo);

    log_debug ("fb_init: xres %d", vinfo.xres); 
    log_debug ("fb_init: yres %d", vinfo.yres); 
    log_debug ("fb_init: bpp %d",  vinfo.bits_per_pixel); 
    log_debug ("fb_init: line_length %d",  finfo.line_length); 

    self->line_length = finfo.line_length; 
    self->w = vinfo.xres;
    self->h = vinfo.yres;
    int fb_bpp = vinfo.bits_per_pixel;
    int fb_bytes = fb_bpp / 8;
    self->fb_bytes = fb_bytes;
    self->fb_data_size = self->w * self->h * fb_bytes;
    self->stride = max (self->line_length, self->w * self->fb_bytes);
    self->slop = self->stride - (self->w * self->fb_bytes);

    if (self->line_length == self->w * self->fb_bytes)
      self->linear = TRUE;
    else
      self->linear = FALSE;

    self->fb_data = (BYTE *)mmap (0, self->fb_data_size, 
	     PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, (off_t)0);

    ret = TRUE;
    }
  else
    {
    if (error)
      asprintf (error, "Can't open framebuffer: %s", strerror (errno));
    }
  LOG_OUT 
  return ret;
  }


/*==========================================================================
  framebuffer_deinit
*==========================================================================*/
void framebuffer_deinit (FrameBuffer *self)
  {
  LOG_IN
  log_debug ("Closing framebuffer %s", self->fbdev);
  if (self)
    {
    if (self->fb_data) 
      {
      munmap (self->fb_data, self->fb_data_size);
      self->fb_data = NULL;
      }
    if (self->fd != -1)
      {
      close (self->fd);
      self->fd = -1;
      }
    }
  LOG_OUT
  }


/*==========================================================================
  framebuffer_set_pixel
*==========================================================================*/
void framebuffer_set_pixel (FrameBuffer *self, int x, int y, 
      BYTE r, BYTE g, BYTE b)
  {
  if (x > 0 && x < self->w && y > 0 && y < self->h)
    {
    int index32 = (y * self->w + x) * self->fb_bytes + y * self->slop;
    self->fb_data [index32++] = b;
    self->fb_data [index32++] = g;
    self->fb_data [index32++] = r;
    self->fb_data [index32] = 0;
    }
  }

/*==========================================================================
  framebuffer_destroy
*==========================================================================*/
void framebuffer_destroy (FrameBuffer *self)
  {
  LOG_IN
  framebuffer_deinit (self);
  if (self)
    {
    if (self->fbdev) free (self->fbdev);
    free (self); 
    }
  LOG_OUT
  }

/*==========================================================================
  framebuffer_get_width
*==========================================================================*/
int framebuffer_get_width (const FrameBuffer *self)
  {
  return self->w;
  }

/*==========================================================================
  framebuffer_get_height
*==========================================================================*/
int framebuffer_get_height (const FrameBuffer *self)
  {
  return self->h;
  }

/*==========================================================================
  framebuffer_get_pixel
*==========================================================================*/
void framebuffer_get_pixel (const FrameBuffer *self, 
                      int x, int y, BYTE *r, BYTE *g, BYTE *b)
  {
  if (x > 0 && x < self->w && y > 0 && y < self->h)
    {
    int index32 = (y * self->w + x) * self->fb_bytes + (y * self->slop);
    *b = self->fb_data [index32++];
    *g = self->fb_data [index32++];
    *r = self->fb_data [index32];
    }
  else
    {
    *r = 0;
    *g = 0;
    *b = 0;
    }
  }

/*==========================================================================
  framebuffer_get_data
*==========================================================================*/
BYTE *framebuffer_get_data (FrameBuffer *self)
  {
  return self->fb_data;
  }


/*==========================================================================
  framebuffer_is_linear

  Returns true if the framebuffer memory is organized into contiguous
  lines, with no gaps
*==========================================================================*/
BOOL framebuffer_is_linear (FrameBuffer *self)
  {
  return self->linear;
  }


/*==========================================================================
  framebuffer_clear

  Sets the whole framebuffer black
*==========================================================================*/
void framebuffer_clear (FrameBuffer *self)
  {
  memset (self->fb_data, 0, self->stride * self->h);
  }



