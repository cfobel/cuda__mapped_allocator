Introduction
============

The `mapped_allocator` allocator defined in `mapped_allocator.hpp` can be used
to allocate host mapped memory from the NVIDIA CUDA API.  Behind the scenes,
the mapped_allocator uses `cudaHostAlloc` and `cudaFreeHost`.

The demonstration code is based on example from [Dr. Dobb](http://drdobbs.com/architecture-and-design/217500110).
