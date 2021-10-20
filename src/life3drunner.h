/*============================================================================

  life3drunner.h

  Copyright (c)2020-1 Kevin Boone, GPL v3.0

============================================================================*/
#pragma once

#include "life3d.h"
#include "framebuffer.h"

class Life3DRunner
  {
  public:

  /** Construct a Life3DRunner object. Arguments:
       fb -- an initialized framebuffer
       size -- number of cells each of the x, y, and z directions
       pixels -- width and height of the output image
       zoom -- not used; should be 1.0
       q - anti-aliasing quality, 1-4 (probably 1)
       g - gens -- maximum number of generations before restarting
       delay -- seconds between drawing each generation
       filling -- proportion of cells initially alive
  */
  Life3DRunner (FrameBuffer *fb, int size, int pixels, double zoom, int q,
                  int gens, int delay, double filling);

  /** Run the game. Execution continues indefinitely, until ctrl+c */
  void run (void);

  protected:

  void render (FrameBuffer *fb, const Life3D &life3D);

  FrameBuffer *fb;
  int size;
  int pixels;
  double zoom;
  int q;
  int gens;
  int delay;
  double filling;
  };


