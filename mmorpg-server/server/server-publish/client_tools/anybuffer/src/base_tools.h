#ifndef __BASE_TOOLS_H__
#define __BASE_TOOLS_H__

#include <string>
#include <vector>

#include "utility.h"

// The order of these matters for Is*() functions below.
// Additionally, Parser::ParseType assumes bool..string is a contiguous range
// of type tokens.
#define FLATBUFFERS_GEN_TYPES_SCALAR(TD) \
  TD(NONE,   "",       uint8_t,  byte,   byte,    byte,   uint8) \
  TD(UTYPE,  "",       uint8_t,  byte,   byte,    byte,   uint8) /* begin scalar/int */ \
  TD(BOOL,   "bool",   uint8_t,  boolean,byte,    bool,   bool) \
  TD(CHAR,   "byte",   int8_t,   byte,   int8,    sbyte,  int8) \
  TD(UCHAR,  "ubyte",  uint8_t,  byte,   byte,    byte,   uint8) \
  TD(SHORT,  "short",  int16_t,  short,  int16,   short,  int16) \
  TD(USHORT, "ushort", uint16_t, short,  uint16,  ushort, uint16) \
  TD(INT,    "int",    int32_t,  int,    int32,   int,    int32) \
  TD(UINT,   "uint",   uint32_t, int,    uint32,  uint,   uint32) \
  TD(LONG,   "long",   int64_t,  long,   int64,   long,   int64) \
  TD(ULONG,  "ulong",  uint64_t, long,   uint64,  ulong,  uint64) /* end int */ \
  TD(FLOAT,  "float",  float,    float,  float32, float,  float32) /* begin float */ \
  TD(DOUBLE, "double", double,   double, float64, double, float64) /* end float/scalar */
#define FLATBUFFERS_GEN_TYPES_POINTER(TD) \
  TD(STRING, "string", Offset<void>, int, int, StringOffset, int) \
  TD(VECTOR, "",       Offset<void>, int, int, VectorOffset, int) \
  TD(STRUCT, "",       Offset<void>, int, int, int, int) \
  TD(UNION,  "",       Offset<void>, int, int, int, int)

// The fields are:
// - enum
// - FlatBuffers schema type.
// - C++ type.
// - Java type.
// - Go type.
// - C# / .Net type.
// - Python type.

// using these macros, we can now write code dealing with types just once, e.g.

/*
switch (type) {
#define FLATBUFFERS_TD(ENUM, IDLTYPE, CTYPE, JTYPE, GTYPE, NTYPE, PTYPE) \
case BASE_TYPE_ ## ENUM: \
// do something specific to CTYPE here
FLATBUFFERS_GEN_TYPES(FLATBUFFERS_TD)
#undef FLATBUFFERS_TD
}
*/

#define FLATBUFFERS_GEN_TYPES(TD) \
        FLATBUFFERS_GEN_TYPES_SCALAR(TD) \
        FLATBUFFERS_GEN_TYPES_POINTER(TD)

// Declare tokens we'll use. Single character tokens are represented by their
// ascii character code (e.g. '{'), others above 256.
#define FLATBUFFERS_GEN_TOKENS(TD) \
  TD(Eof, 256, "end of file") \
  TD(StringConstant, 257, "string constant") \
  TD(IntegerConstant, 258, "integer constant") \
  TD(FloatConstant, 259, "float constant") \
  TD(Identifier, 260, "identifier") \
  TD(Table, 261, "table") \
  TD(Struct, 262, "struct") \
  TD(Enum, 263, "enum") \
  TD(Union, 264, "union") \
  TD(NameSpace, 265, "namespace") \
  TD(RootType, 266, "root_type") \
  TD(FileIdentifier, 267, "file_identifier") \
  TD(FileExtension, 268, "file_extension") \
  TD(Include, 269, "include") \
  TD(Attribute, 270, "attribute")
#ifdef __GNUC__
__extension__  // Stop GCC complaining about trailing comma with -Wpendantic.
#endif
enum {
#define FLATBUFFERS_TOKEN(NAME, VALUE, STRING) kToken ## NAME = VALUE,
    FLATBUFFERS_GEN_TOKENS(FLATBUFFERS_TOKEN)
#undef FLATBUFFERS_TOKEN
#define FLATBUFFERS_TD(ENUM, IDLTYPE, CTYPE, JTYPE, GTYPE, NTYPE, PTYPE) \
      kToken ## ENUM,
    FLATBUFFERS_GEN_TYPES(FLATBUFFERS_TD)
#undef FLATBUFFERS_TD
};

static std::string TokenToString(int t) {
    static const char *tokens[] = {
#define FLATBUFFERS_TOKEN(NAME, VALUE, STRING) STRING,
        FLATBUFFERS_GEN_TOKENS(FLATBUFFERS_TOKEN)
#undef FLATBUFFERS_TOKEN
#define FLATBUFFERS_TD(ENUM, IDLTYPE, CTYPE, JTYPE, GTYPE, NTYPE, PTYPE) \
      IDLTYPE,
        FLATBUFFERS_GEN_TYPES(FLATBUFFERS_TD)
#undef FLATBUFFERS_TD
    };
    if (t < 256) {  // A single ascii char token.
        std::string s;
        s.append(1, static_cast<char>(t));
        return s;
    }
    else {       // Other tokens.
        return tokens[t - 256];
    }
}

#endif

