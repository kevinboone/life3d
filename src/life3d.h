/*============================================================================

  life3d.h

  Copyright (c)2020-1 Kevin Boone, GPL v3.0

  A class for managing the lifecycles of cells in a cubic grid

============================================================================*/
#pragma once

class Life3D
  {
  public:

  Life3D (int size, double filling);
  ~Life3D (void);

  /** Intialize the grid with cells of zero age (i.e., empty) or
      age 1. */
  void seed (void);

  /** Returns true if a cell is alive, that is, if its age is not zero. */
  bool is_alive (int x, int y, int z) const;

  /** Returns the age of a cell. A zero return means there is no cell
      at the given location. */
  int get_age (int x, int y, int z) const;
  
  /** Returns true if there are now no live cells in the grid. */
  bool is_empty (void) const;

  /** Count the number of neighbours, that is, the number of live
      cells (age > 0) for a given location. */
  int neighbours (int x, int y, int z) const;

  /** Compute the new cell layout from the present one. */
  void step (void);

  /* Return the dimension of the cell grid, as given in the
     constructor. */
  int get_size (void) const { return size; }
  
  protected:

  void die (int x, int y, int z);
  void spawn (int x, int y, int z);
  int constrain (int n) const;
  void increment_age (int x, int y, int z);


  int *cells;
  int size;
  int size_squared; // Precompute this for speed
  double filling;
  };

