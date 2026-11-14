#pragma once

#include "OpButton/OpButtonTypes.h"
#include <cstdint>

namespace OpButton {

class OpButtonJson {
public:
    enum Type {
        TYPE_NULL,
        TYPE_BOOL,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_ARRAY,
        TYPE_OBJECT
    };

    OpButtonJson() : m_type(TYPE_NULL), m_bool(false), m_int(0), m_double(0.0) {}
    OpButtonJson(bool v) : m_type(TYPE_BOOL), m_bool(v), m_int(0), m_double(0.0) {}
    OpButtonJson(int v) : m_type(TYPE_INT), m_bool(false), m_int(v), m_double(0.0) {}
    OpButtonJson(int64_t v) : m_type(TYPE_INT), m_bool(false), m_int(v), m_double(0.0) {}
    OpButtonJson(double v) : m_type(TYPE_DOUBLE), m_bool(false), m_int(0), m_double(v) {}
    OpButtonJson(const char* v) : m_type(TYPE_STRING), m_bool(false), m_int(0), m_double(0.0), m_string(v ? v : "") {}
    OpButtonJson(const OpButtonString& v) : m_type(TYPE_STRING), m_bool(false), m_int(0), m_double(0.0), m_string(v) {}

    Type type() const { return m_type; }
    bool isNull() const { return m_type == TYPE_NULL; }

    // Object operations
    OpButtonJson& put(const OpButtonString& key, const OpButtonJson& value);
    bool has(const OpButtonString& key) const;
    OpButtonJson get(const OpButtonString& key) const;
    OpButtonString optString(const OpButtonString& key, const OpButtonString& def = OpButtonString()) const;
    int optInt(const OpButtonString& key, int def = 0) const;
    bool optBool(const OpButtonString& key, bool def = false) const;
    double optDouble(const OpButtonString& key, double def = 0.0) const;
    OpButtonJson optObject(const OpButtonString& key) const;
    OpButtonJson optArray(const OpButtonString& key) const;
    int objectSize() const;
    const OpButtonMap<OpButtonString, OpButtonJson>& objectEntries() const { return m_object; }

    // Array operations
    OpButtonJson& add(const OpButtonJson& value);
    int length() const;
    OpButtonJson at(int index) const;

    // Value access
    bool asBool() const;
    int asInt() const;
    int64_t asInt64() const;
    double asDouble() const;
    OpButtonString asString() const;

    // Serialization
    OpButtonString toString() const;

    // Parsing
    static OpButtonJson parse(const OpButtonString& str);

    // Factory
    static OpButtonJson object();
    static OpButtonJson array();
    static OpButtonJson null_value();

private:
    Type m_type;
    bool m_bool;
    int64_t m_int;
    double m_double;
    OpButtonString m_string;
    OpButtonVector<OpButtonJson> m_array;
    OpButtonMap<OpButtonString, OpButtonJson> m_object;

    void writeJson(OpButtonString& out) const;
    static OpButtonString escapeString(const OpButtonString& s);

    struct Parser {
        const char* data;
        size_t pos;
        size_t len;

        OpButtonJson parseValue();
        OpButtonJson parseObject();
        OpButtonJson parseArray();
        OpButtonString parseStringValue();
        OpButtonJson parseNumber();
        OpButtonJson parseLiteral();
        void skipWhitespace();
        char peek();
        char advance();
    };
};

} // namespace OpButton
