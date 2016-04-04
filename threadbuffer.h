#ifndef __THREADBUFFER_H
#define __THREADBUFFER_H

#include <omp.h>
#include <vector>

template <class T>
class ThreadBuffer
{
    public:
        ThreadBuffer() {
            buffer.resize(omp_get_max_threads());
            for (auto &elem: buffer) elem = T();
        }
        ThreadBuffer(const T &value) {
            buffer.resize(omp_get_max_threads());
            Init(value);
        }
        void Init(const T &value) { 
            for (auto &elem: buffer) elem = value;
        }
        T& Get() { return buffer[omp_get_thread_num()]; }
        T* begin() { return &buffer.front(); }
        T* end() { return &buffer.back(); }

    private:
        std::vector<T> buffer;
};

#endif
