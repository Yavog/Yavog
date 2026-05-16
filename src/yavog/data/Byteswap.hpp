#pragma once
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstring>

template<class T>
T byteswap(const T& t){
    assert(0 && "unreachable");
    return 0;
}
template<class T>
requires(sizeof(T)==1)
T byteswap(const T& t){
    return t;
}
template<class T>
requires(sizeof(T)==2)
T byteswap(const T& t){
    return std::bit_cast<T>(__builtin_bswap16(std::bit_cast<uint16_t>(t)));
}
template<class T>
requires(sizeof(T)==4)
T byteswap(const T& t){
    return std::bit_cast<T>(__builtin_bswap32(std::bit_cast<uint32_t>(t)));
}
template<class T>
requires(sizeof(T)==8)
T byteswap(const T& t){
    return std::bit_cast<T>(__builtin_bswap64(std::bit_cast<uint64_t>(t)));
}
