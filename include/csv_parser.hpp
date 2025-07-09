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

class CsvParser {
public:
    CsvParser(
        const std::string& path,
        Arena& arena,
        std::unordered_map<std::string, size_t>& fields
    );


private:
    void loadCsv(std::string& path);
    inline static size_t typeSize(enum ColumnType t);

    Arena& arena_;
    std::unordered_map<std::string, size_t>& fields_;
    std::unordered_map<std::string, void*> parsedContent_;
};

void loadCsv();
