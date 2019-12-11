# Rope

This is sample usage of Rope. 
Rope implementation based on Treap data structure, but uses COW-semantics.
This mean that any node is immutable and can share either itself or child resources.
Hence, we can store any change in total time cost of update! 
Moreover, it uses `std::shared_ptr` for dynamic memory management, so you can be pretty sure that there's no leaks.

## TODO 

[ ] Create `std::basic_string` like outerior class
[ ] Introduce `iterator` for this structure
[ ] Support `RandomAccessIterator` with `O(log n)` total cost for shifting
