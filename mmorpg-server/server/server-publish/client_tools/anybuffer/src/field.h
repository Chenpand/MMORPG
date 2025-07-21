#ifndef __FIELD_H__
#define __FIELD_H__

#include <string>
#include <vector>

enum TypeEnum {
    TE_UINT8,
    TE_UINT16,
    TE_INT16,
    TE_UINT32,
    TE_INT32,
    TE_UINT64,
    TE_INT64,
    TE_STRING,

    TE_BASE_NUM,

    TE_ENUM,
    TE_STRUCT,
    TE_UNKNOWN,
};

struct FieldType {
    void SetStructType(std::string name, const std::string& filename) {
        type = TE_STRUCT;
        identifInFile = name;
        identifInCSharp = name;
        identifyInLua = name;

        definedFileName = filename;
    }

    void SetEnumType(std::string name, const std::string& filename) {
        type = TE_ENUM;
        identifInFile = name;
        identifInCSharp = name;
        identifyInLua = name;

        definedFileName = filename;
    }

    TypeEnum					type;
    std::string					identifInFile;
    std::string					identifInCSharp;
    std::string					identifyInLua;

    std::string					definedFileName;      // 定义该结构的文件,去掉文件后缀
};

// 数组中元素顺序必须跟枚举定义顺序一样
static FieldType s_BaseFieldTypes[] = {
    { TE_UINT8,     "byte",     "byte",     "type_uint8"    },
    { TE_UINT16,    "ushort",   "UInt16",   "type_uint16"   },
    { TE_INT16,     "short",    "Int16",    "type_int16"    },
    { TE_UINT32,    "uint",     "UInt32",   "type_uint32"   },
    { TE_INT32,     "int",      "Int32",    "type_int32"    },
    { TE_UINT64,    "ulong",    "UInt64",   "type_uint64"   },
    { TE_INT64,     "long",     "Int64",    "type_int64"    },
    { TE_STRING,    "string",   "string",   "type_string"   },
};

struct MetaData {
    std::string key;
    std::string value;
};

struct Field {
    Field()
    {
        isArray = false;
        arrayFixedLen = 0;
    }

    FieldType					type;
    std::string					name;
    bool						isArray;
    int							arrayFixedLen;          // 数组固定长度
	std::vector<std::string>	comments;
};

struct Struct {
    Struct(std::string _name, const std::string filename) {
        msgId = -1;
        name = _name;
        type.SetStructType(name, filename);
    }

    int							msgId;

    FieldType					type;
    std::string					name;
    std::vector<Field>			fields;
	std::vector<std::string>	comments;
};

struct Namespace {
    std::vector<std::string>    components;
};

struct EnumVal {
    std::string                 name;
    uint32_t                    value;
	std::vector<std::string>	comments;
};

struct Enum {
    FieldType                   type;
    std::string                 name;
    std::vector<EnumVal>        values;
    std::vector<std::string>	comments;
};

struct FileDetail {
    std::string                 name;
    std::vector<Struct>         allStruct;
    std::vector<Enum>           allEnum;
};

#endif