#pragma once
namespace mpv{
    struct CudaDeleter {
        CudaDeleter() = default;
        void operator()(void* p)noexcept { cudaFree(p); puts("calling cudaFree"); }
    };
    template<typename T>
    class cuda_uPtr:public uPtr<T,CudaDeleter> {
        public:
        using base=uPtr<T, CudaDeleter>;
        using base::base;
        using base::operator=;
        cudaError_t allocate(size_t size) {
            T* p = nullptr;
            cudaError_t cudaStatus = cudaMalloc((void**)&p, size * sizeof(T));
            if (cudaStatus != cudaSuccess) {
                cudaFree(p);
                //std::cerr << "cudaMalloc error\n";
            }
            else {
                *this = p;
            }
            return cudaStatus;
        }
    };
}
