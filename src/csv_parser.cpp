#include "csv_parser.hpp"
#include <stdio.h>
#include <vector>


size_t CsvParser::typeSize(enum ColumnType t) {
    switch (t) {
        case Type_Int32:   return sizeof(int32_t);
        case Type_Int64:   return sizeof(int64_t);
        case Type_uInt32:  return sizeof(uint32_t);
        case Type_uInt64:  return sizeof(uint64_t);
        case Type_Float32: return sizeof(float);
        case Type_Float64: return sizeof(double);
        default: return 0;
    }
}

CsvParser::CsvParser(
    const std::string& path,
    Arena& arena,
    std::unordered_map<std::string, size_t>& fields
):
    arena_{arena},
    fields_{fields}
{



}

void CsvParser::loadCsv(std::string& path) {
    FILE* file = fopen(path.data(), "r");
    if (file == NULL) {
        printf("loadCsv failed to open the file!");
        return;
    }
    constexpr size_t BUF_SIZE   = 4096;
    constexpr size_t MAX_HEADER = 1024;
    constexpr size_t MAX_FIELD  = 128;

    char headerString[MAX_HEADER];
    if (!fgets(headerString, MAX_HEADER, file)) {
        fclose(file);
        printf("failed to read the header!");
        return;
    }


    std::vector<std::string> headers(fields_.size());
    std::string header = "";
    header.reserve(MAX_FIELD);

    bool hitNewline = false;
    int charIndex = 0;
    for (int i = 0; i < MAX_HEADER && !hitNewline; ++i) {
        char c = headerString[i];
        if (c == '\0') break;

        switch (c) {
            case '\n':
                hitNewline = true;
            case ',':
                if (fields_.find(header) != fields_.end()) headers.push_back(header);
                header.clear();
                charIndex = 0;
                break;
            default:
                header[charIndex] = c;
                charIndex++;
                break;
        };

    }


    char buffer[BUF_SIZE];
    size_t count = 0;

    size_t bytes_read = 0;
    while ((bytes_read = fread(buffer, sizeof(char), BUF_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            if(buffer[i] == '\n') ++count;
        }
    }

}

