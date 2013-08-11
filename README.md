Graph plotter
=============

This program is Copyright (C) 2005-2013 Ryan Lothian. All rights reserved.


Purpose 
-------
This program plots 2d graphs, with some nice antialiasing on the lines.

Please note this code is old. It was written in 2006 and the only updates
in 2013 were to get it to compile. It's buggy and mostly untested. 

I've uploaded it to github so it doesn't get lost, and in case it's helpful 
to someone else.

Visit www.ryanlothian.com for some more recent projects.


License 
-------
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option) 
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 

Dependencies
------------

  * g++               -- to compile C++
  * libgd-xpm-dev     -- used by the program image writing
  * python            -- to run the build script


How to use
----------
To compile and run regression tests:

  python build.py
  
To run:

  bin/gdplotgraph out.png 2048 testscript.txt
