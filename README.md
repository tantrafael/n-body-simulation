# N-body simulation

## Thoughts and decisions

### Spatial partitioning
Identifying a variant of the n-body problem with O(n<sup>2</sup>) time complexity, some form of spatial partitioning was necessary to begin with. From what I understand, fixed uniform grids have proven most suitable for uniformly distributed objects of about the same size in a relatively small space. They also have a simple implementation. This led me to choose this grid over other grid types. With all objects constantly moving, my feeling was that reinserting all shapes every frame would be both simpler and faster, rather than conserving them and updating the grid as objects move between cells.

I use a fixed uniform grid of 16x16 cells. Every frame, each shape is inserted into the grid, calculating the grid coordinates from its position. Insertion is made into all of the cells overlapped by the shape's axis aligned bounding box, maximally 4 cells, given the constant shape size and the chosen cell size. This is with collision detection in mind, only having to check for collisions within each cell. The shapes are stored in grid cells, both in unordered buckets, as well as in type separated buckets. The latter for quick access of only wanted shapes during attraction detection.

### Collision detection
Collision detection is made by traversing the grid. For every cell, the stored shapes are being checked for collisions among each other. Each pair of shapes is firstly checked whether their bounding boxes are overlapping, for a quick decision of possible collision (the broad phase). If so, a detailed test is made to determine whether the two shapes are actually overlapping (the narrow phase). In the case of a collision, a check is made against a set of pair ids of already registered collisions. If not found there, the pair is inserted there and a collision is performed. The last step is to prevent making multiple collisions of the same shapes during a frame. The collision-pair register would be helpful to prevent multiple checks in the first place, before the bounding box test. But as it is implemented it performs very slowly, holding and searching all entered pairs in the whole grid. It's even slower than the narrow phase test, so I put it last only because it affected performance the least there.
Since all shapes possibly colliding within the grid cell are included during insertion, we don't have to consider adjacent cells.

### Attraction detection
Attraction detection is also made by traversing the same grid. For every cell, all shape types are gone through, extracting the shapes of each type, together with the shapes of its attracting type from the same and all adjacent cells. Every shape is searched for the closest of its near attractors, and made to follow that target. The grid cell size was chosen to match the given maximum search range for attractions, in order to cover all nearby attractions by looking no further than a cell's adjacent cells. For this purpose, insertion into the grid would optimally be based on shape center points, and only into the corresponding cell. Insertion into multiple cells causes unnecessary checks here. Also, I've noticed that the collision detection performed better with grid divisions of closer to 40, rather than 16 like now. My guess is it would be wise to have two separate grids, one specialized for collisions, and another for attractions.

I also implemented the Verlet neighbor list algorithm, where each shape holds a list of its neighboring attractors within a certain range. The list is being traversed each frame in order to decide which of these neighbors to follow. It is constructed to be reused for a number of frames, before being rebuilt. This is done by selecting attractor neighbors, using the uniform grid, within a radius consisting of the given maximum search range, call it cutoff radius, and an additional “skin”. The skin is set to be thick enough that no unaccounted shape can enter the cutoff sphere during the time between rebuilds. Rebuilding all neighbor lists at the same time turned out to cause unacceptably disturbing freezing every so many frames, so I rebuilt only a certain amount of lists every frame, just enough to have all covered within the necessary amount of frames. Doing this, I ended up using a lot of memory, and it turned out to perform worse than the first approach described above, so I returned to that one.

### Time flies
I found this an extraordinarily interesting project, diving into a lot of collision physics and molecular dynamics. I've been completely immersed, and would love to just keep going. There's a lot more to do for performance as well as memory management. Given more time, I would make two grids, one specialized for collisions, and the other for attractions. I would find a better solution for collision-pair checking. I would try to get more done when inserting shapes into the collision grid, while being rebuilt every frame. I would look over the careless use of dynamic memory. I would get the somewhat unnecessary id out of the shape class. I would refactor, dividing the code into smaller and more manageable pieces. I would put more effort into naming and commenting things. I would implement a vector structure, to get away from the itchingly many code repetitions for x and y. I would do a lot more. I would make it perfect.
