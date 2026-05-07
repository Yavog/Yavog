#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "Parser/Source.hpp"

// I store source files :)
typedef uint32_t SourceId;
struct SourceCollection{
    std::vector<std::shared_ptr<Source>> sources;
public:
    SourceId add(std::shared_ptr<Source> source);
    Source& operator[](SourceId id);

    bool has(SourceId id);
};