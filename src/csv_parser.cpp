#include "csv_parser.hpp"
#include "profiling/profiler.hpp"
#include "profiling/timer.hpp"

#include <stdio.h>
#include <sys/stat.h>

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

void CsvParser::typeCast(ColumnType t, const char* value, void* out) {
    switch (t) {
        case Type_Int32: *(int32_t*)out = static_cast<int32_t>(std::strtol(value, nullptr, 10));
            break;
        case Type_Int64: *(int64_t*)out = static_cast<int64_t>(std::strtoll(value, nullptr, 10));
            break;
        case Type_uInt32: *(uint32_t*)out = static_cast<uint32_t>(std::strtoul(value, nullptr, 10));
            break;
        case Type_uInt64: *(uint64_t*)out = static_cast<uint64_t>(std::strtoull(value, nullptr, 10));
            break;
        case Type_Float32: *(float*)out = static_cast<float>(std::strtof(value, nullptr));
            break;
        case Type_Float64: *(double*)out = std::strtod(value, nullptr);
            break;
        default:
            fprintf(stderr, "Unknown ColumnType in typeCast\n");
            break;
    }
}


CsvParser::CsvParser(
    const std::string& path,
    Arena& arena,
    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual>& fields
):
    arena_{arena},
    fields_{fields}
{
    loadCsv(path);
}

void CsvParser::loadCsv(const std::string& path) {
    FILE* file = fopen(path.data(), "rb");
    if (file == NULL) {
        printf("loadCsv failed to open the file!");
        return;
    }

    constexpr size_t BUF_SIZE   = 4096 * 10;
    constexpr size_t MAX_HEADER = 1024;
    constexpr size_t MAX_FIELD_SIZE = 64;
    constexpr size_t MAX_FIELDS_NUM = 10;
    constexpr size_t MAX_VALUE_SIZE = 32;

    char headerString[MAX_HEADER];
    if (!fgets(headerString, MAX_HEADER, file)) {
        fclose(file);
        printf("failed to read the header!");
        return;
    }

    char headers[MAX_FIELDS_NUM][MAX_FIELD_SIZE];
    int headersOffsets[MAX_FIELDS_NUM];
    char header[MAX_FIELD_SIZE];
    ColumnType columnTypes[MAX_FIELDS_NUM];

    bool hitNewline = false;
    int charIndex = 0;
    int headerIndex = 0;
    int headerSize = 0;
    int offset = 0;

    for (int i = 0; i < MAX_HEADER && !hitNewline; ++i) {
        char c = headerString[i];
        if (c == '\0') break;

        switch (c) {
            case ',':
            case '\n':
                if(c == '\n') {
                    hitNewline = true;
                    headerSize = i + 1; // one for the new line
                }
                header[charIndex] = '\0';

                if (fields_.find(header) != fields_.end()){
                    strcpy(headers[headerIndex], header);

                    columnTypes[headerIndex] = static_cast<ColumnType>(fields_[header]);
                    headersOffsets[headerIndex] = offset;
                    headerIndex++;
                }

                offset++;
                charIndex = 0;
                break;
            default:
                if (charIndex < MAX_FIELD_SIZE - 1) {
                    header[charIndex++] = c;
                }
                break;
        };

    }

    struct stat Stat;
    stat(path.data(), &Stat);

    char rowString[MAX_FIELDS_NUM * MAX_FIELD_SIZE];
    if (!fgets(rowString, MAX_FIELDS_NUM * MAX_FIELD_SIZE, file)) {
        fclose(file);
        printf("failed to read the first row!");
        return;
    }

    char buffer[BUF_SIZE];
    size_t bytes_read = 0;
    size_t count = (size_t)((f64)Stat.st_size / (f64)strlen(rowString) * 1.1);
    void* valuesStorages[MAX_FIELDS_NUM];

    for (int i = 0; i < headerIndex; ++i) {
        valuesStorages[i] = arena_.allocate(count * typeSize(columnTypes[i]));
    }

    fseek(file, headerSize, SEEK_SET);

    bytes_read = 0;
    offset = 0;
    charIndex = 0;

    char value[MAX_VALUE_SIZE];
    int rowNumer = 0;
    int valueIndex = 0;
    bool copyActive = false;
    int nextTarget = headersOffsets[0];

    while ((bytes_read = fread(buffer, sizeof(char), BUF_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];

            switch(c) {
                case ',':
                case '\n': {
                    if (copyActive) {
                        value[charIndex] = '\0';
                        void* dst = static_cast<char*>(valuesStorages[valueIndex]) +
                                     rowNumer * typeSize(columnTypes[valueIndex]);
                        typeCast(columnTypes[valueIndex], value, dst);
                        ++valueIndex;

                        nextTarget = (valueIndex < headerIndex)
                                     ? headersOffsets[valueIndex]
                                     : -1;     // sentinel: no more columns
                        copyActive = false;
                    }

                    ++offset;
                    charIndex = 0;

                    if (c == '\n') {
                        valueIndex = 0;
                        offset     = 0;
                        nextTarget = headersOffsets[0];
                        ++rowNumer;
                    }
                    break;
                }

                default:
                    value[charIndex++] = c;
                    break;
            }

            if (!copyActive && offset == nextTarget)
                copyActive = true;
        }
    }

    fclose(file);

    for (int i = 0; i < headerIndex; ++i) {
        parsedContent_[headers[i]] = Value{valuesStorages[i], columnTypes[i]};
    }
}

