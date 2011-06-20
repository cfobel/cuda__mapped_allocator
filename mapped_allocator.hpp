#ifndef ___MAPPED_ALLOCATOR__HPP___
#define ___MAPPED_ALLOCATOR__HPP___

#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cuda.h>

template <typename T> class mapped_allocator;

template <> class mapped_allocator<void>
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    // reference to void members are impossible.
    typedef void value_type;
    template <class U> 
        struct rebind { typedef mapped_allocator<U> other; };
};    

namespace cuda_mapped{
    inline void destruct(char *){}
    inline void destruct(wchar_t*){}
    template <typename T> 
        inline void destruct(T *t){t->~T();}
    inline void checkCUDAError(const char *msg) {
        cudaError_t err = cudaGetLastError();
        if(cudaSuccess != err) {
            fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString(err));
            exit(EXIT_FAILURE);
        }                        
    }
} // namespace
    
template <typename T>
class mapped_allocator
{
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> 
        struct rebind { typedef mapped_allocator<U> other; };
    mapped_allocator(){}
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    pointer allocate(size_type size, mapped_allocator<void>::const_pointer hint = 0) {
        int bytes = size * sizeof(T);
        std::cout << "Allocating " << bytes << " bytes." << std::endl;
        pointer a_m; // pointer to host memory
        cudaHostAlloc((void **)&a_m, bytes, cudaHostAllocMapped);
        cuda_mapped::checkCUDAError("cudaHostAllocMapped");
        return a_m;
    }

    template <class U> mapped_allocator(const mapped_allocator<U>&){}
    void deallocate(pointer p, size_type n) {
        cudaFreeHost(p);
    }
    void deallocate(void *p, size_type n) {
        cudaFreeHost(p);
    }
    size_type max_size() const throw() {
        return std::numeric_limits<size_t>::max()/sizeof(T);
    }
    void construct(pointer p, const T& val)
    {
        new(static_cast<void*>(p)) T(val);
    }
    void construct(pointer p)
    {
        new(static_cast<void*>(p)) T();
    }
    void destroy(pointer p){cuda_mapped::destruct(p);}
};


template <typename T, typename U>
inline bool operator==(const mapped_allocator<T>&, const mapped_allocator<U>){return true;}

template <typename T, typename U>
inline bool operator!=(const mapped_allocator<T>&, const mapped_allocator<U>){return false;}


namespace std{
template <class _Tp1, class _Tp2>
inline mapped_allocator<_Tp2>&
__stl_alloc_rebind(mapped_allocator<_Tp1>& __a, const _Tp2*) 
{  
    return (mapped_allocator<_Tp2>&)(__a); 
}


template <class _Tp1, class _Tp2>
inline mapped_allocator<_Tp2>
__stl_alloc_create(const mapped_allocator<_Tp1>&, const _Tp2*) 
{ 
    return mapped_allocator<_Tp2>(); 
}

} // namespace std
// end STLPort

#endif

