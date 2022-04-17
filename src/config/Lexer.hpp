#pragma once

#include <cstddef>
#include <istream>
#include <vector>

#include "../util/SourceLocation.hpp"

namespace Config {

class Token {
public:
    enum class Type {
        Invalid,
        NewLine,
        Number,
        String,
        Identifier,
        Hyphen,
        Add,
        EqualSign,
        SquareBracketLeft,
        SquareBracketRight,
        BracketLeft,
        BracketRight,
        CurlyLeft,
        CurlyRight,
        Comma,
    };

    Token(Type type, Util::SourceLocation start, size_t size, std::string value)
        : m_type(type)
        , m_value(value)
        , m_start(start)
        , m_size(size) { }

    Type type() const { return m_type; }
    std::string value() const { return m_value; }
    Util::SourceLocation location() const { return m_start; }
    size_t size() const { return m_size; }

private:
    Type m_type { Type::Invalid };
    std::string m_value;
    Util::SourceLocation m_start;
    size_t m_size {};
};

class Lexer {
public:
    Lexer(std::istream& input)
        : m_input(input) { }

    std::vector<Token> lex();

private:
    template<class Predicate>
    std::string consume_while(Predicate&& predicate) {
        std::string string;
        char c = m_input.peek();
        while (predicate(c) && !m_input.eof()) {
            consume_one();
            string += c;
            c = m_input.peek();
        }
        return string;
    }

    char consume_one();

    std::istream& m_input;
    Util::SourceLocation m_location;
};

}
