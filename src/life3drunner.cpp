/*==========================================================================
 
  life3drunner.c

  Copyright (c)2021 Kevin Boone
  Distributed under the terms of the GPL v3.0

==========================================================================*/

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "life3drunner.h"
#include "life3d.h"
#include "log.h"
#include "imager.h"

/*==========================================================================
 
  Life3DRunner constructor 

==========================================================================*/
Life3DRunner::Life3DRunner (FrameBuffer *fb, int size, 
    int pixels, double zoom, int q, int gens, int delay,
    double filling)
  {
  this->fb = fb;
  this->size = size;
  this->pixels = pixels;
  this->zoom = zoom;
  this->q = q;
  this->gens = gens;
  this->delay = delay;
  this->filling = filling;
  }


/*==========================================================================
 
  render

  Use Don Cross's ray tracer to render the cubic grid of cells into
  a collection of spheres. There's plenty of scope of tweaking here, 
  but it isn't entirely obvious how to link the various tunable
  parameters together in any kind of automated way. 

==========================================================================*/
void Life3DRunner::render (FrameBuffer *fb, const Life3D &life3D)
  {
  LOG_IN

  using namespace Imager;
  int N = life3D.get_size();

  // Draw on a black (0, 0, 0) background
  Scene scene (Color (0, 0, 0, 7.0e-2));

  for (int x = 0; x < N; x++)
    {
    for (int y = 0; y < N; y++)
      {
      for (int z = 0; z < N; z++)
        {
	int age = life3D.get_age (x, y, z);
        if (age > 0) // Cell is alive
	  {
	  // Sphere radius in scene units. 
	  double r = 5;
	  // The sphere layout in the grid is determined
	  //   entirely by the radius and the number
	  //   of cells
	  double spacing = 2.1 * r;
	  double half_space = spacing / 2;
	  double box = (N + 1) * spacing;
          Sphere* sphere = new Sphere 
	    (Vector (spacing * x - box / 2 + half_space, 
	      spacing * y - box / 2 + spacing, 
	      - spacing * z - box), 
	      r);
	  switch (age)
	    {
	    // Apply colour to the spheres, according to their age
	    // New cells are red, becoming more blue as they age
	    case 1:
              sphere->SetFullMatte (Color (1, 0, 0));
	      break;
	    case 2:
              sphere->SetFullMatte (Color (0.8, 0, 0.2));
	      break;
	    case 3:
              sphere->SetFullMatte (Color (0.6, 0, 0.4));
	      break;
	    case 4:
              sphere->SetFullMatte (Color (0.4, 0, 0.6));
	      break;
	    case 5:
              sphere->SetFullMatte (Color (0.2, 0, 0.8));
	      break;
	    default:
              sphere->SetFullMatte (Color (0, 0, 1));
	    }
          scene.AddSolidObject (sphere);
	  }
        }
      }

    LOG_OUT
    }

  // It's interesting to fiddle with the location of the light sources
  // This first one is a long way to the left, and produces hard shadows...
  scene.AddLightSource (LightSource (Vector (50, 0, 50), Color (0.9, 0.9, 0.9)));
  // This one is front and right, so fills in some of the dark areas
  scene.AddLightSource (LightSource (Vector (-2, 0, 5), Color (0.5, 0.5, 0.5)));

  // Draw the image to the framebuffer
  scene.SaveImage (fb, pixels, pixels, zoom, q);
  }


/*==========================================================================
 
  run

==========================================================================*/
void Life3DRunner::run (void)
  {
  framebuffer_clear (fb);
  Life3D life3D (size, filling);
  srand (time (0));
  life3D.seed();
  int steps = 0;
  while (true)
    {
    log_debug ("Step %d\n", steps);
    render (fb, life3D); 
    life3D.step();
    if (life3D.is_empty())
      {
      // All cells dead -- start with a new random selection
      life3D.seed();
      }
    if (steps >= gens)
      {
      life3D.seed();
      steps = 0;
      }
    sleep (delay);
    steps++;
    }
  }

