## Input/Output

* A function is provided to run the VM until output becomes available and/or input is about to be requested. This allows external devices (like the hull-painting robot) to interface with the program.

## Memory

* VM memory consists of an associative array of "chunks", which are int64_t arrays of a certain size
* When accessing an index that falls within a chunk range which hasn't been initialized, that chunk is allocated and a pointer to it is added to the array
* Associative array is unsorted and searched in-order, because we generally don't expect there to be enough elements for it to matter

## Misc.

* Build is an Ant script because I'm too lazy to refresh myself on CMake at the moment