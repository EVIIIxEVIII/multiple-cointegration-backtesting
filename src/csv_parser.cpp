#include "csv_parser.hpp"



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
