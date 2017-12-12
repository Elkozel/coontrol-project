# coontrol-project

The project aims to connect a user on the internet with an RC car. 

Configuration:

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
