#include "parser.h"
#include "base_tools.h"

FieldType GetFieldTypeByName(std::string name)
{
    for (auto field : s_BaseFieldTypes)
    {
        if (name == field.identifInFile)
        {
            return field;
        }
    }

    static FieldType s_Unknown;
    s_Unknown.type = TE_UNKNOWN;

    return s_Unknown;
}

Parser::Parser() {


}

Parser::~Parser() {

}

bool Parser::ParseFromFile(const char* filename) {
    std::string data;
    if (!LoadFile(filename, false, &data)) {
        printf("read file %s failed.\n" , filename);
        return false;
    }

    filePath_ = StripFileName(filename);
    fileNameWithoutPostfix_ = GetFileNameWithoutPostfix(StripPath(filename));
    return Parse(data.c_str());
}

bool Parser::Parse(const char *source) {
    source_ = cursor_ = source;
    line_ = 1;

    try {
        Next();
        // ToDo...
        // Includes must come before type declarations:
        
        // Now parse all other kinds of declarations:
        while (token_ != kTokenEof) {
            if (token_ == '{') {
                // ToDo...

            }
            else if (token_ == kTokenEnum) {
                ParseEnum();
                continue;
            }
            else if (token_ == kTokenUnion) {
                //ParseEnum(true);
            }
            if (token_ == kTokenFileIdentifier) {
                Next();
                // ToDo...

                Expect(kTokenStringConstant);
                Expect(';');
            }
            else if (token_ == kTokenFileExtension) {
                Next();
                // ToDo...

                Expect(kTokenStringConstant);
                Expect(';');
            }
            else if (token_ == kTokenInclude) {
                Next();

                std::string includeFile = attribute_;
                include_files_.push_back(includeFile);

                std::string fullName = filePath_ + PathSeparatorSet + includeFile + CONFIG_FILE_POSTFIX;

                Parser parser;
                if (!parser.ParseFromFile(fullName.c_str())) {
                    Error("Parse " + fullName + "failed.");
                }

                includedFileDetail_.push_back(parser.GetFileDetail());

                Expect(kTokenIdentifier);
                Expect(';');
            }
            else if (token_ == kTokenAttribute) {
                Next();
                auto name = attribute_;
                Expect(kTokenStringConstant);
                Expect(';');
            }
            else if (token_ == kTokenNameSpace) {
                ParseNamespace();
            }
            else {
                ParseDecl();
            }
        }
        
    }
    catch (const std::string &msg) {
        printf("%s\n",msg.c_str());
        return false;
    }

    return true;
}

void Parser::Next() {
    bool seen_newline = false;
    attribute_.clear();
    for (;;) {
        char c = *cursor_++;
        token_ = c;
        switch (c) {
        case '\0': cursor_--; token_ = kTokenEof; return;
        case ' ': case '\r': case '\t': break;
        case '\n': line_++; seen_newline = true; break;
        case '{': case '}': case '(': case ')': case '[': case ']': return;
        case ',': case ':': case ';': case '=': return;
        case '.':
            if (!isdigit(*cursor_)) return;
            Error("floating point constant can\'t start with \".\"");
            break;
        case '\"':
        case '\'':
            while (*cursor_ != c) {
                if (*cursor_ < ' ' && *cursor_ >= 0)
                    Error("illegal character in string constant");
                if (*cursor_ == '\\') {
                    cursor_++;
                    switch (*cursor_) {
                    case 'n':  attribute_ += '\n'; cursor_++; break;
                    case 't':  attribute_ += '\t'; cursor_++; break;
                    case 'r':  attribute_ += '\r'; cursor_++; break;
                    case 'b':  attribute_ += '\b'; cursor_++; break;
                    case 'f':  attribute_ += '\f'; cursor_++; break;
                    case '\"': attribute_ += '\"'; cursor_++; break;
                    case '\'': attribute_ += '\''; cursor_++; break;
                    case '\\': attribute_ += '\\'; cursor_++; break;
                    case '/':  attribute_ += '/';  cursor_++; break;
                    case 'x': {  // Not in the JSON standard
                        cursor_++;
                        attribute_ += static_cast<char>(ParseHexNum(2));
                        break;
                    }
                    case 'u': {
                        cursor_++;
                        ToUTF8(static_cast<int>(ParseHexNum(4)), &attribute_);
                        break;
                    }
                    default: Error("unknown escape code in string constant"); break;
                    }
                }
                else { // printable chars + UTF-8 bytes
                    attribute_ += *cursor_++;
                }
            }
            cursor_++;
            token_ = kTokenStringConstant;
            return;
        case '/':
            if (*cursor_ == '/') {
                const char *start = ++cursor_;
                while (*cursor_ && *cursor_ != '\n' && *cursor_ != '\r') cursor_++;
                doc_comment_.push_back(std::string(start, cursor_));
                break;
            }
            else if (*cursor_ == '*') {
                cursor_++;
                // TODO: make nested.
                while (*cursor_ != '*' || cursor_[1] != '/') {
                    if (!*cursor_) Error("end of file in comment");
                    cursor_++;
                }
                cursor_ += 2;
                break;
            }
            // fall thru
        default:
            if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
                // Collect all chars of an identifier:
                const char *start = cursor_ - 1;
                while (isalnum(static_cast<unsigned char>(*cursor_)) ||
                    *cursor_ == '_')
                    cursor_++;
                attribute_.append(start, cursor_);
                // First, see if it is a type keyword from the table of types:
                #define FLATBUFFERS_TD(ENUM, IDLTYPE, CTYPE, JTYPE, GTYPE, NTYPE, \
                    PTYPE) \
                    if (attribute_ == IDLTYPE) { \
                        token_ = kToken ## ENUM; \
                        return; \
                    }
                    FLATBUFFERS_GEN_TYPES(FLATBUFFERS_TD)
                #undef FLATBUFFERS_TD

                // If it's a boolean constant keyword, turn those into integers,
                // which simplifies our logic downstream.
                if (attribute_ == "true" || attribute_ == "false") {
                    attribute_ = NumToString(attribute_ == "true");
                    token_ = kTokenIntegerConstant;
                    return;
                }
                // Check for declaration keywords:
                if (attribute_ == "table") { token_ = kTokenTable;     return; }
                if (attribute_ == "struct") { token_ = kTokenStruct;    return; }
                if (attribute_ == "enum") { token_ = kTokenEnum;      return; }
                if (attribute_ == "union") { token_ = kTokenUnion;     return; }
                if (attribute_ == "namespace") { token_ = kTokenNameSpace; return; }
                if (attribute_ == "root_type") { token_ = kTokenRootType;  return; }
                if (attribute_ == "include") { token_ = kTokenInclude;   return; }
                if (attribute_ == "attribute") { token_ = kTokenAttribute; return; }
                if (attribute_ == "file_identifier") {
                    token_ = kTokenFileIdentifier;
                    return;
                }
                if (attribute_ == "file_extension") {
                    token_ = kTokenFileExtension;
                    return;
                }
                // If not, it is a user-defined identifier:
                token_ = kTokenIdentifier;
                return;
            }
            else if (isdigit(static_cast<unsigned char>(c)) || c == '-') {
                const char *start = cursor_ - 1;
                if (c == '0' && (*cursor_ == 'x' || *cursor_ == 'X')) {
                    cursor_++;
                    while (isxdigit(static_cast<unsigned char>(*cursor_))) cursor_++;
                    attribute_.append(start + 2, cursor_);
                    attribute_ = NumToString(StringToUInt(attribute_.c_str(), 16));
                    token_ = kTokenIntegerConstant;
                    return;
                }
                while (isdigit(static_cast<unsigned char>(*cursor_))) cursor_++;
                if (*cursor_ == '.' || *cursor_ == 'e' || *cursor_ == 'E') {
                    if (*cursor_ == '.') {
                        cursor_++;
                        while (isdigit(static_cast<unsigned char>(*cursor_))) cursor_++;
                    }
                    // See if this float has a scientific notation suffix. Both JSON
                    // and C++ (through strtod() we use) have the same format:
                    if (*cursor_ == 'e' || *cursor_ == 'E') {
                        cursor_++;
                        if (*cursor_ == '+' || *cursor_ == '-') cursor_++;
                        while (isdigit(static_cast<unsigned char>(*cursor_))) cursor_++;
                    }
                    token_ = kTokenFloatConstant;
                }
                else {
                    token_ = kTokenIntegerConstant;
                }
                attribute_.append(start, cursor_);
                return;
            }
            std::string ch;
            ch = c;
            if (c < ' ' || c > '~') ch = "code: " + NumToString(c);
            Error("illegal character: " + ch);
            break;
        }
    }
}

bool Parser::IsNext(int t) {
    bool isnext = t == token_;
    if (isnext) Next();
    return isnext;
}

// Expect a given token to be next, consume it, or error if not present.
void Parser::Expect(int t) {
    if (t != token_) {
        char info[1024];
        
        sprintf(info, "%s.abs(%d): 语法错误: \"%s\", 可能是 \"%s\"", fileNameWithoutPostfix_.c_str(), line_, TokenToStringId(token_).c_str()
                                                                    , TokenToString(t).c_str());
        Error(info);
    }
    Next();
}

std::string Parser::TokenToStringId(int t) {
    return TokenToString(t) + (t == kTokenIdentifier ? ": " + attribute_ : "");
}

Struct* Parser::FindStructByName(std::string name) {
    for (auto i = 0; i < structs_.size(); i++) {
        if (structs_[i].name == name) {
            return &(structs_[i]);
        }
    }

    for (auto i = 0; i < includedFileDetail_.size(); i++) {
        for (auto j = 0; j < includedFileDetail_[i].allStruct.size(); j++) {
            Struct& tmpStruct = includedFileDetail_[i].allStruct[j];
            if (tmpStruct.name == name) {
                return &tmpStruct;
            }
        }
    }

    return NULL;
}

const EnumVal* Parser::FindEnumValByName(std::string name)
{
    for (const auto& enum_ : enums_) {
        for (const auto& enumVal : enum_.values) {
            if (enumVal.name == name) {
                return &enumVal;
            }
        }
    }

    for (const auto& includeFile : includedFileDetail_) {
        for (const auto& enum_ : includeFile.allEnum) {
            for (const auto& enumVal : enum_.values) {
                if (enumVal.name == name) {
                    return &enumVal;
                }
            }
        }
    }

    return NULL;
}

// Parses exactly nibbles worth of hex digits into a number, or error.
int64_t Parser::ParseHexNum(int nibbles) {
    for (int i = 0; i < nibbles; i++)
        if (!isxdigit(cursor_[i]))
            Error("escape code must be followed by " + NumToString(nibbles) +
                " hex digits");
    std::string target(cursor_, cursor_ + nibbles);
    auto val = StringToUInt(target.c_str(), 16);
    cursor_ += nibbles;
    return val;
}

void Parser::ParseEnum() {
    Next();
    std::string enumName = attribute_;
    Expect(kTokenIdentifier);
    Enum enumDef;
	enumDef.comments = FetchComments();
    enumDef.type.SetEnumType(enumName, fileNameWithoutPostfix_);
    enumDef.name = enumName;

    uint32_t curValue = 0;
    std::vector<MetaData> metaData;
    ParseMetaData(metaData);
    Expect('{');

    do {
        EnumVal enumVal;
        enumVal.name = attribute_;
		enumVal.comments = FetchComments();

        Expect(kTokenIdentifier);
        
        if (IsNext('=')) {
            enumVal.value = atoi(attribute_.c_str());
            Expect(kTokenIntegerConstant);
            curValue = enumVal.value + 1;
        } else {
            enumVal.value = curValue++;
        }

        Expect(';');
        enumDef.values.push_back(enumVal);
    } while (!IsNext('}'));

    enums_.push_back(enumDef);
    return;
}

void Parser::ParseDecl() {
    std::vector<std::string> dc = doc_comment_;
    bool fixed = IsNext(kTokenStruct);
    if (!fixed) Expect(kTokenTable);
    std::string name = attribute_;

    Struct tmpStruct(name,fileNameWithoutPostfix_);
	tmpStruct.comments = FetchComments();

    Expect(kTokenIdentifier);

    std::vector<MetaData> metaData;
    ParseMetaData(metaData);
    for (auto meta : metaData)
    {
        ParseStructAttr(tmpStruct, meta.key, meta.value);
    }

    Expect('{');
    while (token_ != '}') {
        Field field;
		field.comments = FetchComments();
        ParseField(field);
        tmpStruct.fields.push_back(field);
    }
    Expect('}');

    structs_.push_back(tmpStruct);
}

void Parser::ParseNamespace() {
    Next();
    if (token_ != ';') {
        for (;;) {
            namespace_.components.push_back(attribute_);
            Expect(kTokenIdentifier);
            if (!IsNext('.')) break;
        }
    }
    Expect(';');
}

void Parser::ParseField(Field& field) {
    field.name = attribute_;
    std::vector<std::string> dc = doc_comment_;
    Expect(kTokenIdentifier);
    Expect(':');

    ParseType(field);

    std::vector<MetaData> metaData;
    ParseMetaData(metaData);
    for (auto meta : metaData)
    {
        ParseFieldAttr(field, meta.key, meta.value);
    }

    Expect(';');
    //Next();
}

void Parser::ParseType(Field& field) {
    if (token_ >= kTokenBOOL && token_ <= kTokenSTRING) {
        std::string name = attribute_;
        field.type = GetFieldTypeByName(name);

        Next();
    }
    else {
        if (token_ == kTokenIdentifier) {
            // 嵌套类型

            std::string name = attribute_;

            Struct* targetStruct = FindStructByName(name);
            if (!targetStruct) {
                Error("used undefined type:" + name);
            }

            field.type = targetStruct->type;
            Expect(kTokenIdentifier);
        }
        else if (token_ == '[') {
            Next();
            field.isArray = true;
            ParseType(field);
            Expect(']');
        }
        else {
            Error("illegal type syntax");
        }
    }
}

void Parser::ParseMetaData(std::vector<MetaData>& metaData) {
    if (IsNext('(')) {
        for (;;) {
            MetaData data;
            data.key = attribute_;
            Expect(kTokenIdentifier);

            if (IsNext(':')) {
                data.value =  ParseSingleValue();
            }

            metaData.push_back(data);

            if (IsNext(')')) break;
            Expect(',');
        }
    }
}

std::string Parser::ParseSingleValue() {
    std::string value = attribute_;
    Next();
    return value;
}

void Parser::ParseFieldAttr(Field& field, const std::string& key, const std::string& value) {
    if (key == "len") {
        field.arrayFixedLen = (int)StringToInt(value.c_str());
    }
    else {
        Error("Error Field Attr " + key + ":" + value);
    }
}

void Parser::ParseStructAttr(Struct& struct_, const std::string& key, const std::string& value)
{
    if (key == "msgid") {
        if (IsStringAllDigit(value.c_str())) {
            struct_.msgId = (int)StringToInt(value.c_str());
        } else {
            auto enumVal = FindEnumValByName(value);
            if (!enumVal) {
                Error("Undefined msgid: " +  value);
            } else {
                struct_.msgId = enumVal->value;
            }
        }
    }
    else {
        Error("Error Struct Attr " + key + ":" + value);
    }
}

int64_t Parser::ParseIntegerFromString() {
    int64_t result = 0;
    // Parse one or more enum identifiers, separated by spaces.
    const char *next = attribute_.c_str();
    do {
        const char *divider = strchr(next, ' ');
        std::string word;
        if (divider) {
            word = std::string(next, divider);
            next = divider + strspn(divider, " ");
        }
        else {
            word = next;
            next += word.length();
        }
    } while (*next);
    return result;
}

std::vector<std::string> Parser::FetchComments()
{
	std::vector<std::string> comments = doc_comment_;
	doc_comment_.clear();
	return comments;
}

bool Parser::IsStringAllDigit(const char* str) {
    if (!str) {
        return false;
    }

    while (*str) {
        if (!isdigit(*str++)) {
            return false;
        }
    }

    return true;
}
