#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include <string>
#include <sstream>
#include "parser.h"


class CodeGenerator {
public:
    CodeGenerator() {}
    virtual ~CodeGenerator() {}
    
    virtual std::string GenCode(const Parser* parser, std::string root = "", std::string name = "") {
        std::stringstream ss;

        if (!parser) {
            return ss.str();
        }

        ss << GenCodeBegin();
        ss << GenIncludeFile(parser->GetFileName(), parser->GetIncludeFiles());
        ss << GenNameSpaceBegin(parser->GetNamespace());

        for (auto myEnum : parser->GetEnums()) {
            ss << GenEnum(myEnum);
            ss << std::endl;
        }

        for (auto myStruct : parser->GetStructs()) {
            ss << GenStruct(myStruct);
            ss << std::endl;
        }

        ss << GenNameSpaceEnd(parser->GetNamespace());
        ss << GenCodeEnd();
        
        return ss.str();
    }

    virtual std::string GetFilePostfix() = 0;

protected:
    virtual std::string GenCodeBegin() = 0;
    virtual std::string GenIncludeFile(const std::string& filename, const std::vector<std::string>& includeFiles) = 0;
    virtual std::string GenCodeEnd() = 0;
    virtual std::string GenEnum(const Enum& myEnum) = 0;
    virtual std::string GenStruct(const Struct& myStruct) = 0;
    virtual std::string GenNameSpaceBegin(const Namespace& myNamespace) = 0;
    virtual std::string GenNameSpaceEnd(const Namespace& myNamespace) = 0;
	virtual std::string GenGetLenFunction(const Struct& myStruct) = 0;
	virtual std::string GenComment(const std::string comment) = 0;

protected:
    void IncPreTable() { preTable += "\t"; }
    void DecPreTable() { preTable.erase(preTable.length() - 1); }
    const std::string& GetPreTable() const { return preTable; }

private:
    std::string             preTable;
};

#endif
