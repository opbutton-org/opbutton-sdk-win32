#include "OpButton/OpButtonJson.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace OpButton {

// ── Object operations ──

OpButtonJson& OpButtonJson::put(const OpButtonString& key, const OpButtonJson& value) {
    if (m_type != TYPE_OBJECT) {
        m_type = TYPE_OBJECT;
    }
    m_object[key] = value;
    return *this;
}

bool OpButtonJson::has(const OpButtonString& key) const {
    return m_type == TYPE_OBJECT && m_object.find(key) != m_object.end();
}

OpButtonJson OpButtonJson::get(const OpButtonString& key) const {
    if (m_type != TYPE_OBJECT) return OpButtonJson();
    auto it = m_object.find(key);
    return it != m_object.end() ? it->second : OpButtonJson();
}

OpButtonString OpButtonJson::optString(const OpButtonString& key, const OpButtonString& def) const {
    auto v = get(key);
    return v.m_type == TYPE_STRING ? v.m_string : def;
}

int OpButtonJson::optInt(const OpButtonString& key, int def) const {
    auto v = get(key);
    if (v.m_type == TYPE_INT) return static_cast<int>(v.m_int);
    if (v.m_type == TYPE_DOUBLE) return static_cast<int>(v.m_double);
    return def;
}

bool OpButtonJson::optBool(const OpButtonString& key, bool def) const {
    auto v = get(key);
    return v.m_type == TYPE_BOOL ? v.m_bool : def;
}

double OpButtonJson::optDouble(const OpButtonString& key, double def) const {
    auto v = get(key);
    if (v.m_type == TYPE_DOUBLE) return v.m_double;
    if (v.m_type == TYPE_INT) return static_cast<double>(v.m_int);
    return def;
}

OpButtonJson OpButtonJson::optObject(const OpButtonString& key) const {
    auto v = get(key);
    return v.m_type == TYPE_OBJECT ? v : OpButtonJson::object();
}

OpButtonJson OpButtonJson::optArray(const OpButtonString& key) const {
    auto v = get(key);
    return v.m_type == TYPE_ARRAY ? v : OpButtonJson::array();
}

int OpButtonJson::objectSize() const {
    return m_type == TYPE_OBJECT ? static_cast<int>(m_object.size()) : 0;
}

// ── Array operations ──

OpButtonJson& OpButtonJson::add(const OpButtonJson& value) {
    if (m_type != TYPE_ARRAY) {
        m_type = TYPE_ARRAY;
    }
    m_array.push_back(value);
    return *this;
}

int OpButtonJson::length() const {
    return m_type == TYPE_ARRAY ? static_cast<int>(m_array.size()) : 0;
}

OpButtonJson OpButtonJson::at(int index) const {
    if (m_type != TYPE_ARRAY || index < 0 || index >= static_cast<int>(m_array.size())) {
        return OpButtonJson();
    }
    return m_array[index];
}

// ── Value access ──

bool OpButtonJson::asBool() const {
    if (m_type == TYPE_BOOL) return m_bool;
    if (m_type == TYPE_INT) return m_int != 0;
    return false;
}

int OpButtonJson::asInt() const {
    if (m_type == TYPE_INT) return static_cast<int>(m_int);
    if (m_type == TYPE_DOUBLE) return static_cast<int>(m_double);
    return 0;
}

int64_t OpButtonJson::asInt64() const {
    if (m_type == TYPE_INT) return m_int;
    if (m_type == TYPE_DOUBLE) return static_cast<int64_t>(m_double);
    return 0;
}

double OpButtonJson::asDouble() const {
    if (m_type == TYPE_DOUBLE) return m_double;
    if (m_type == TYPE_INT) return static_cast<double>(m_int);
    return 0.0;
}

OpButtonString OpButtonJson::asString() const {
    if (m_type == TYPE_STRING) return m_string;
    if (m_type == TYPE_INT) return opbuttonToString(m_int);
    if (m_type == TYPE_DOUBLE) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", m_double);
        return OpButtonString(buf);
    }
    if (m_type == TYPE_BOOL) return m_bool ? OpButtonString("true") : OpButtonString("false");
    return OpButtonString();
}

// ── Factory ──

OpButtonJson OpButtonJson::object() {
    OpButtonJson j;
    j.m_type = TYPE_OBJECT;
    return j;
}

OpButtonJson OpButtonJson::array() {
    OpButtonJson j;
    j.m_type = TYPE_ARRAY;
    return j;
}

OpButtonJson OpButtonJson::null_value() {
    return OpButtonJson();
}

// ── Serialization ──

OpButtonString OpButtonJson::escapeString(const OpButtonString& s) {
    OpButtonString out;
    out.reserve(s.size() + 2);
    out += '"';
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    out += '"';
    return out;
}

void OpButtonJson::writeJson(OpButtonString& out) const {
    switch (m_type) {
        case TYPE_NULL:
            out += "null";
            break;
        case TYPE_BOOL:
            out += m_bool ? "true" : "false";
            break;
        case TYPE_INT:
            out += opbuttonToString(m_int);
            break;
        case TYPE_DOUBLE: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%g", m_double);
            out += buf;
            break;
        }
        case TYPE_STRING:
            out += escapeString(m_string);
            break;
        case TYPE_ARRAY: {
            out += '[';
            for (size_t i = 0; i < m_array.size(); ++i) {
                if (i > 0) out += ',';
                m_array[i].writeJson(out);
            }
            out += ']';
            break;
        }
        case TYPE_OBJECT: {
            out += '{';
            bool first = true;
            for (const auto& kv : m_object) {
                if (!first) out += ',';
                first = false;
                out += escapeString(kv.first);
                out += ':';
                kv.second.writeJson(out);
            }
            out += '}';
            break;
        }
    }
}

OpButtonString OpButtonJson::toString() const {
    OpButtonString out;
    writeJson(out);
    return out;
}

// ── Parser ──

OpButtonJson OpButtonJson::parse(const OpButtonString& str) {
    Parser p;
    p.data = str.c_str();
    p.pos = 0;
    p.len = str.size();
    p.skipWhitespace();
    if (p.pos >= p.len) return OpButtonJson();
    return p.parseValue();
}

void OpButtonJson::Parser::skipWhitespace() {
    while (pos < len && (data[pos] == ' ' || data[pos] == '\t' || data[pos] == '\n' || data[pos] == '\r')) {
        ++pos;
    }
}

char OpButtonJson::Parser::peek() {
    return pos < len ? data[pos] : '\0';
}

char OpButtonJson::Parser::advance() {
    return pos < len ? data[pos++] : '\0';
}

OpButtonJson OpButtonJson::Parser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == '{') return parseObject();
    if (c == '[') return parseArray();
    if (c == '"') return OpButtonJson(parseStringValue());
    if (c == '-' || (c >= '0' && c <= '9')) return parseNumber();
    return parseLiteral();
}

OpButtonJson OpButtonJson::Parser::parseObject() {
    advance(); // '{'
    OpButtonJson obj = OpButtonJson::object();
    skipWhitespace();
    if (peek() == '}') { advance(); return obj; }

    while (true) {
        skipWhitespace();
        if (peek() != '"') break;
        OpButtonString key = parseStringValue();
        skipWhitespace();
        if (peek() == ':') advance();
        OpButtonJson value = parseValue();
        obj.put(key, value);
        skipWhitespace();
        if (peek() == ',') { advance(); continue; }
        break;
    }

    skipWhitespace();
    if (peek() == '}') advance();
    return obj;
}

OpButtonJson OpButtonJson::Parser::parseArray() {
    advance(); // '['
    OpButtonJson arr = OpButtonJson::array();
    skipWhitespace();
    if (peek() == ']') { advance(); return arr; }

    while (true) {
        arr.add(parseValue());
        skipWhitespace();
        if (peek() == ',') { advance(); continue; }
        break;
    }

    skipWhitespace();
    if (peek() == ']') advance();
    return arr;
}

OpButtonString OpButtonJson::Parser::parseStringValue() {
    advance(); // '"'
    OpButtonString result;
    while (pos < len) {
        char c = advance();
        if (c == '"') break;
        if (c == '\\') {
            char next = advance();
            switch (next) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    char hex[5] = {};
                    for (int i = 0; i < 4 && pos < len; ++i) {
                        hex[i] = advance();
                    }
                    unsigned int codepoint = static_cast<unsigned int>(strtoul(hex, nullptr, 16));
                    if (codepoint < 0x80) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        result += static_cast<char>(0xC0 | (codepoint >> 6));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        result += static_cast<char>(0xE0 | (codepoint >> 12));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    break;
                }
                default: result += next; break;
            }
        } else {
            result += c;
        }
    }
    return result;
}

OpButtonJson OpButtonJson::Parser::parseNumber() {
    size_t start = pos;
    bool isDouble = false;

    if (peek() == '-') advance();
    while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();

    if (pos < len && data[pos] == '.') {
        isDouble = true;
        advance();
        while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();
    }

    if (pos < len && (data[pos] == 'e' || data[pos] == 'E')) {
        isDouble = true;
        advance();
        if (pos < len && (data[pos] == '+' || data[pos] == '-')) advance();
        while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();
    }

    OpButtonString numStr(data + start, pos - start);
    if (isDouble) {
        return OpButtonJson(strtod(numStr.c_str(), nullptr));
    } else {
        int64_t val = strtoll(numStr.c_str(), nullptr, 10);
        return OpButtonJson(val);
    }
}

OpButtonJson OpButtonJson::Parser::parseLiteral() {
    if (pos + 4 <= len && strncmp(data + pos, "true", 4) == 0) {
        pos += 4;
        return OpButtonJson(true);
    }
    if (pos + 5 <= len && strncmp(data + pos, "false", 5) == 0) {
        pos += 5;
        return OpButtonJson(false);
    }
    if (pos + 4 <= len && strncmp(data + pos, "null", 4) == 0) {
        pos += 4;
        return OpButtonJson();
    }
    advance();
    return OpButtonJson();
}

} // namespace OpButton
