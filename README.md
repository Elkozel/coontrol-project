# Important Notice, not that anyone is going to see it

This project has not been maintained for quite a while and I wrote this code in my early years as a programmer.
If you need an example of how to not write code, feel free to use this one. A more mature version of this project is
posted [here](https://github.com/Elkozel/RC-Car). As always, feel free to contribute and propose ideas!

# coontrol-project

The project aims to connect a user on the internet with an RC car. 

Configuration: (For configuration please see raw file.)

+------------------------------------------------------------------------------+
|                                                                              |
|                              +--------+   +--------+ +--------+  +--------+  |
|      +------+                | Motor1 |   | Motor2 | | Motor3 |  | Motor4 |  |
|      | User |                +---^----+   +----^---+ +---^----+  +----^---+  |
|      +-+--^-+                    | +---------+ |         |----------+ |      |
|        |  |                      +-+  L298N  +-+         ++  L298N  +-+      |
|        |  |                        +----^----+            +----^----+        |
|   +----v--+-----+                       |   +--------------+   |             |
|   |             |                       |   |              |   |             |
|   | Web Server  |                       +---+  Car Client  +---+             |
|   |             |                           |              |                 |
|   +----+----+---+                           +----+-----+---+                 |
|        ^    |              +------------+        |     ^                     |
|        |    +------------> |            | <------+     |                     |
|        |                   | Car Server |              |                     |
|        +-------------------+            +--------------+                     |
|                            +------------+                                    |
|                                                                              |
+------------------------------------------------------------------------------+
  
Technical Data: 
  Each Motor is uses 270W on 12V.
  Logic boards use ~1,37 A on full load.
  Overall current 95A. (Hope that a truck battery will be able to supply so much).
