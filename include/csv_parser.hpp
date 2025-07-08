#pragma once

#include <unordered_map>
#include <string>
#include <cstring>

#include "arena.hpp"

enum ColumnType {
    Type_Int32,
    Type_Int64,
    Type_uInt32,
    Type_uInt64,
    Type_Float32,
    Type_Float64,
};

struct CStrHash {
    size_t operator()(const char* s) const {
        return std::hash<std::string>{}(s);
    }
};

struct CStrEqual {
    bool operator()(const char* a, const char* b) const {
        return std::strcmp(a, b) == 0;
    }
};

class CsvParser {
public:
    CsvParser(const std::string& path, Arena& arena);


private:
    void readFile(std::string& path);
    inline static size_t typeSize(enum ColumnType t);

    std::unordered_map<std::string, void*> parsedContent;
    const size_t LINE_BUFFER = 512;
};
