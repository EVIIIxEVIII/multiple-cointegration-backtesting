#include "arena.hpp"

#include <stdexcept>

Arena::Arena(size_t size): size_(size) {
    start_ = (void*) malloc(size * sizeof(char));
    if (start_ == NULL) {
        throw std::runtime_error("Failed to allocate memory for the arena");
    }
}


void* Arena::allocate(size_t size) {
    if (offset_ + size > size_) {
        printf("WARNING: Memory arena failed to allocate memory. Requested size is too large.");
        return NULL;
    }

    void* ptr = (char*)start_ + offset_ + size;
    offset_ += size;

    return ptr;
}


