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

struct Value {
    void* data;
    ColumnType type;
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
    CsvParser(
        const std::string& path,
        Arena& arena,
        std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual>& fields
    );


private:
    void loadCsv(const std::string& path);
    inline static size_t typeSize(enum ColumnType t);
    inline static void typeCast(enum ColumnType t, const char* value, void* out);

    Arena& arena_;
    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual>& fields_;
    std::unordered_map<const char*, Value, CStrHash, CStrEqual> parsedContent_;
};

void loadCsv();
