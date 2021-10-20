/*============================================================================

  life3d.cpp

  Copyright (c)2020-1 Kevin Boone, GPL v3.0

  A class for managing the lifecycles of cells in a cubic grid

============================================================================*/

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include "life3d.h"

/*===========================================================================

  Life3D constructor

===========================================================================*/
Life3D::Life3D (int size, double filling)
  {
  cells = (int *)malloc (size * size * size * sizeof (int));
  // Precompute this, to speed up some array indexing operations
  size_squared = size * size;
  this->size = size;
  this->filling = filling;
  }

/*===========================================================================

  Life3D destructor

===========================================================================*/
Life3D::~Life3D (void)
  {
  free (cells);
  }

/*===========================================================================

  Life3D::seed

===========================================================================*/
void Life3D::seed (void)
  {
  int l = size_squared * size;

  for (int i = 0; i < l; i++)
    {
    double r = rand() / (double) RAND_MAX;
    if (r > (1.0 - filling))
      {
      cells[i] = 1; 
      }
    else
      cells[i] = 0; 
    }
  }

/*===========================================================================

  Life3D::is_alive

===========================================================================*/
bool Life3D::is_alive (int x, int y, int z) const
  {
  return cells [x * size_squared + y * size + z];
  }

/*===========================================================================

  Life3D::get_age

===========================================================================*/
int Life3D::get_age (int x, int y, int z) const
  {
  return cells [x * size_squared + y * size + z];
  }

/*===========================================================================

  Life3D::die

===========================================================================*/
void Life3D::die (int x, int y, int z) 
  {
  cells [x * size_squared + y * size + z] = 0;
  }

/*===========================================================================

  Life3D::spawn

===========================================================================*/
void Life3D::spawn (int x, int y, int z) 
  {
  cells [x * size_squared + y * size + z] = 1;
  }

/*===========================================================================

  Life3D::constrain

===========================================================================*/
int Life3D::constrain (int n) const
  {
  if (n > size) return 0;
  if (n < 0) return size - 1;
  return n;
  }

/*===========================================================================

  Life3D::increment age

===========================================================================*/
void Life3D::increment_age (int x, int y, int z) 
  {
  cells [x * size_squared + y * size + z]++;
  }

/*===========================================================================

  Life3D::neighbours

===========================================================================*/
int Life3D::neighbours (int x, int y, int z) const
  {
  int c = 0;

  int xup = constrain (x + 1); 
  int xdown = constrain (x - 1);
  int yup = constrain (y + 1);
  int ydown = constrain (y - 1);
  int zup = constrain (z + 1);
  int zdown = constrain (z - 1);

  c += is_alive (xup, y, z);
  c += is_alive (xdown, y, z);
  c += is_alive (xup, yup, z);
  c += is_alive (xdown, yup, z);
  c += is_alive (xup, ydown, z);
  c += is_alive (xdown, ydown, z);
  c += is_alive (x, yup, z);
  c += is_alive (x, ydown, z);

  c += is_alive (xup, y, zup);
  c += is_alive (xdown, y, zup);
  c += is_alive (xup, yup, zup);
  c += is_alive (xdown, yup, zup);
  c += is_alive (xup, ydown, zup);
  c += is_alive (xdown, ydown, zup);
  c += is_alive (x, yup, zup);
  c += is_alive (x, ydown, zup);
  c += is_alive (x, y, zup);

  c += is_alive (xup, y, zdown);
  c += is_alive (xdown, y, zdown);
  c += is_alive (xup, yup, zdown);
  c += is_alive (xdown, yup, zdown);
  c += is_alive (xup, ydown, zdown);
  c += is_alive (xdown, ydown, zdown);
  c += is_alive (x, yup, zdown);
  c += is_alive (x, ydown, zdown);
  c += is_alive (x, y, zdown);

  return c;
  }

/*===========================================================================

  Life3D::step

  This is a good place to edit the game rules. At present, the rules are
  the same as those for the 2D version, except with different neighbour
  counts, to account for the larger number of potential neighbours.

===========================================================================*/
void Life3D::step (void)
  {
  for (int x = 0; x < size; x++)
    {
    for (int y = 0; y < size; y++)
      {
      for (int z = 0; z < size; z++)
        {
	int n = neighbours (x, y, z);
	if (is_alive (x, y, z))
	  {
	  increment_age (x, y, z);
	  // There is a cell in this position. Work out whether it
	  //   will die in this generation
	  if (n < 5 || n > 7)
	    {
	    die (x, y, z);
	    }
	  }
	else
	  {
	  // No cell in this position yet. Work out whether one will spawn
	  //   in this generation
	  if (n == 4 || n == 5)
	    {
	    spawn (x, y, z);
	    }
	  }
        }
      }
    }
  }

/*===========================================================================

  Life3D::is_empty

===========================================================================*/
bool Life3D::is_empty (void) const
  {
  int l = size_squared * size;
  for (int i = 0; i < l; i++)
    if (cells[i]) return false;
  return true;
  }

