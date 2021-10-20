# life3d

Version 1.0a

## What is this?

Yet another implementation in C++ of Conway's 'Game of Life' cellular
simulation, for use with the Linux framebuffer. This one is
slightly unusual, in that it draws a 3D grid, in perspective.
Each cell is represented as a sphere, with a colour that
varies between red and blue, according to the age of the
cell. 

The rendering is done using Don Cross's raytracing demo.
The original code, and mathematical details are here:

http://cosinekitty.com/raytrace/source.html

The ray tracing code is comprehensible, and easy to use, but
does not make use of any hardware rendering. It is therefore not
very fast, particularly when there are large numbers of objects
in the scene. This speed limitation makes it impractical
to simulate a larger cell grid than about 8x8x8 cells -- and
even that requires a fairly hefty computer. Still, this is
real 3D rendering, with shading and shadows so, unless
you use specialized graphics hardware, burning CPU is
the price we have to pay.

To get even remotely useful speed, it is necessary to configure
the C++ with a high degree of optimization. That makes compilation
slow. To help with that problem, I've removed from Don Cross's
original code everything that isn't directly relevant to 
rendering spheres.

Some operational settings can be changed on the command line, but
there's so much that can be tweaked in a program like this, that
I haven't even attempted to add switches for everything.
Later in this document I provide hints about what could usefully
be changed in the source.

## Building

The usual:

    $ make
    $ sudo make install 

## Command-line options

*-d,--delay [seconds]*

Number of seconds to wait before drawing each generation.
Given the amount of math involved, there's probably no
point setting this to anything other than "0" or "1",
and setting it to zero will use 100% CPU. Default
is 1.

*-f,--fbdev [device]*

Framebuffer device. Default is `/dev/fb0` 

*-g,--gens [N]*

Maximum number of generations to run before re-seeding
the cells. Some limit is necessary because it's easy
to get into a situation where the same few patterns
repeats over and over again.

Default is 20.

*-i,--filling [0-1.0]*

Proportional of grid cells that are initially filled at
the start of the simulation. With the default rules,
only extremes of this range have any significant effect.


*-p,--pixels [N]*

Size of the image on the screen, in pixels. The default is
to make the image one quarter of the longest dimension.
Increasing this size will slow things down and use more
CPU.

*-q,--quality*

Anti-aliasing quality, from 1 to 4. Each step increase
is a doubling of the number of anti-aliasing iterations
and, in practice, 1 is probably OK.

*-s,--size*

Grid size. The grid is a cube of the specified size.
Practical values are probably in the range 1-8, 
unless you're running on a supercomputer.

*-v,--version*

Show the version

## Notes

This program probably won't work under X, because the X server
will either disable the framebuffer, or compete for its 
attention. For testing purposes, it should work on a desktop 
system is you switch to a terminal console (e..g, ctrl+alt+F2).

To run `life3d` you'll need read/write access to the framebuffer. 
You can do this by running as `root` or as a user
which is in the same group as the `/dev/fbX` framebuffer
device.

`life3d` will only work with a linear RGB framebuffer. Pixels
are arranged in rows in memory, perhaps with a dead gap between
rows. The program won't work with any other arrangement. Most
Linux displays are of this type, but from comments I've received
on some of my other programs, I'm aware that not all are. Sadly,
there isn't a huge amount I can do about this, unless somebody wants
to send me some hardware to test with.

`life3d` sends the control sequence to disable the flashing 
screen cursor on start-up, and to re-enable it on exit. However,
not all Linux terminals respond to these control sequences.

## Potential hacks 

To edit the game rules (that is, the algorithm that determines
when cells die and new ones are spawned), look in

`Life3D::step()` in `src/life3d.cpp`. 

To change the rendering, including the way colours are assigned,
see `Life3DRunner::render()` in `src/life3drunner.cpp`.


## Legal, etc

The framebuffer and game management parts of `life3d` are copyright 
(c)2020-1 Kevin Boone, and distributed under the
terms of the GNU Public Licence, version 3.0. 
The ray-tracing code is largely derived from the work of Don
Cross, and subject to its own distribution rules. Please see
the source files for licensing details.
 

