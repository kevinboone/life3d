/*==========================================================================
 
  main.c

  Copyright (c)2021 Kevin Boone
  Distributed under the terms of the GPL v3.0

==========================================================================*/

#include <iostream>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include "framebuffer.h"
#include "log.h"
#include "life3d.h"
#include "life3drunner.h"

/*==========================================================================
 
  show_version 

==========================================================================*/
void show_version (void)
  {
  printf (NAME " version " VERSION ": ");
  printf ("Perspective 3D version of Conway's Game of Life for\n");
  printf ("the Linux framebuffer. Copyright (C)2021 Kevin Boone and others.\n");
  }

/*==========================================================================
 
  show_help

==========================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options]\n");
  printf (" -d,--delay [seconds]  delay between generations (1)\n");
  printf (" -f,--fbdev [device]   framebuffer device (/dev/fb0)\n");
  printf (" -g,--gens [N]         maximum number of generations (20)\n");
  printf (" -i,--filling [0-1.0]  Proportion of cells initially seeded\n");
  printf (" -p,--pixels [N]       image size in pixels (quarter screen)\n");
  printf (" -q,--quality [1-4]    anti-aliasing quality (1)\n");
  printf (" -s,--size [N]         grid size (6)\n");
  printf ("\n");
  }

/*======================================================================

  quit_signal 

======================================================================*/
void quit_signal (int dummy)
  {
  // Show the cursor
  fputs ("\e[?25h", stdout); 
  fflush (stdout);
  exit (0);
  }

/*==========================================================================
 
  main 

==========================================================================*/
int main (int argc, char **argv)
  {
  // N is the size of the cell array -- a cube N x N x N cells
  int N = 6;
  // zoom is the zoom factor for the renderer. The more cells in the grid,
  //   the smaller the zoom will have to be, to see them all
  double zoom = 1;
  // Size of the grid to render on screen
  int pixels = 0;
  // Framebuffer device. Note that user must have permissions to write it
  char *fbdev = strdup ("/dev/fb0");
  // Anti-aliasing quality, 1-4
  int q = 1;
  // Maximum number of steps before repopulating the grid
  int gens = 20;
  // Sleep between generations 
  int delay = 1;
  // Proportion of cells alive in initial seeding
  double filling = 0.5;

  bool version = false;
  bool help = false;
  bool carry_on = true;

  static struct option long_options[] =
    {
      {"delay", required_argument, NULL, 'd'},
      {"fbdev", required_argument, NULL, 'f'},
      {"filling", required_argument, NULL, 'i'},
      {"gens", required_argument, NULL, 'g'},
      {"help", no_argument, NULL, 'h'},
      {"pixels", required_argument, NULL, 'p'},
      {"quality", required_argument, NULL, 'q'},
      {"size", required_argument, NULL, 's'},
      {"version", no_argument, NULL, 'v'},
      {0, 0, 0, 0}
    };

   int opt;
   while (carry_on)
     {
     int option_index = 0;
     opt = getopt_long (argc, argv, "hvf:p:q:g:d:s:i:", long_options, &option_index);

     if (opt == -1) break;

     switch (opt)
       {
       case 0:
	 printf ("hello\n");
         break;
       case 'v': 
	 version = true; 
	 break;
       case 'h': 
	 help = true; 
	 break;
       case 'f': 
	 fbdev = strdup (optarg);
	 break;
       case 'p': 
	 pixels = atoi (optarg);
	 break;
       case 'q': 
	 q = atoi (optarg);
	 break;
       case 'g': 
	 gens = atoi (optarg);
	 break;
       case 'd': 
	 delay = atoi (optarg);
	 break;
       case 's': 
	 N = atoi (optarg);
	 break;
       case 'i': 
	 filling = atof (optarg);
	 break;
       default:
         carry_on = false; 
       }
    }

  if (carry_on)
    {
    if (version)
      {
      show_version(); 
      carry_on = false;
      }
    else if (help)
      {
      show_help(); 
      carry_on = false;
      }
    }

  if (carry_on)
    {
    if (q > 4 || q < 1)
      {
      log_error ("'quality' argument must be in range 1-4\n");
      carry_on = false;
      }
    }

  if (carry_on)
    {
    if (filling < 0.0 || filling >= 1.0)
      {
      log_error ("'filling' argument must be in range 0.0-1.0\n");
      carry_on = false;
      }
    }
  
  if (carry_on)
    {
    FrameBuffer *fb = framebuffer_create (fbdev);
    char *error = NULL;
    framebuffer_init (fb, &error);
    
    if (error == 0)
      {
      signal (SIGQUIT, quit_signal);
      signal (SIGTERM, quit_signal);
      signal (SIGHUP, quit_signal);
      signal (SIGINT, quit_signal);
      srand (time (NULL));
      
      // Hide cursor
      fputs("\e[?25l", stdout);
      fflush (stdout);

      // FB initialized OK. We can get to work
      int fb_width = framebuffer_get_width (fb);
      int fb_height = framebuffer_get_height (fb);
      
      // Work out the largest FB dimension -- usually the width
      int max = fb_width;
      if (fb_height > fb_width) max = fb_height;

      // If no value for pixels, make the display one quarter the longest
      //   screen dimension
      if (pixels == 0)
	pixels = max / 4;

      // In any case, don't let the pixel size by larger than the screen
      if (pixels > max) pixels = max;


      Life3DRunner runner (fb, N, pixels, zoom, q, gens, delay, filling);
      runner.run();
      }
    else
      log_error (error);

    framebuffer_destroy (fb);
    }

  free (fbdev);

  return 0;
  }

