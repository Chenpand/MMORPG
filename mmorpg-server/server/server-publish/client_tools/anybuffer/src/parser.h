#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>

#include "field.h"

// ÅäÖÃÎÄ¼þºó×º
#define CONFIG_FILE_POSTFIX     ".abs"

class Parser
{
public:
    Parser();
    ~Parser();

    bool ParseFromFile(const char* filename);
    bool Parse(const char *_source);
    
    const std::string GetFileName() const { return fileNameWithoutPostfix_; }
    const std::vector<Struct>& GetStructs() const { return structs_; }
    const std::vector<Enum>& GetEnums() const { return enums_; }
    const Namespace&  GetNamespace() const { return namespace_; }
    const std::vector<std::string>& GetIncludeFiles() const { return include_files_; }
    FileDetail GetFileDetail() const {
        FileDetail detail;
        detail.name = fileNameWithoutPostfix_;
        detail.allStruct = structs_;
        detail.allEnum = enums_;

        return detail;
    }

protected:
    void Next();
    bool IsNext(int t);
    void Expect(int t);
    std::string TokenToStringId(int t);
    Struct* FindStructByName(std::string name);
    const EnumVal* FindEnumValByName(std::string name);

    // Parse Function
    void ParseEnum();
    int64_t ParseHexNum(int nibbles);
    void ParseDecl();
    void ParseNamespace();
    void ParseField(Field& field);
    void ParseType(Field& field);
    void ParseMetaData(std::vector<MetaData>& metaData);
    std::string ParseSingleValue();
    void ParseFieldAttr(Field& field,const std::string& key,const std::string& value);
    void ParseStructAttr(Struct& struct_, const std::string& key, const std::string& value);
    int64_t ParseIntegerFromString();
	std::vector<std::string> FetchComments();

    // Check if string "str" is digit
    bool IsStringAllDigit(const char* str);

private:
    const char *source_, *cursor_;
    int line_;  // the current line being parsed
    int token_;

    std::string                 filePath_;
    std::string                 fileNameWithoutPostfix_;

    std::vector<std::string>    include_files_;
    std::vector<std::string>    doc_comment_;
    std::string                 attribute_;
    Namespace                   namespace_;
    std::vector<Struct>         structs_;
    std::vector<FileDetail>     includedFileDetail_;
    std::vector<Enum>           enums_;
};

#endif