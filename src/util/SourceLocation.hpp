#pragma once

#include <string>

namespace Util {

struct SourceLocation {
    size_t index {};
    size_t line { 1 };
    size_t column { 1 };
};

struct SourceRange {
    SourceLocation location;
    size_t size;
};

struct ParserError {
    SourceRange range;
    std::string message;
};

}
