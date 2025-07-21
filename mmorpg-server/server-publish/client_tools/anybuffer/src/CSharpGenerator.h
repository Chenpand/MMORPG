#ifndef __CSHARP_GENERATOR_H__
#define __CSHARP_GENERATOR_H__

#include "CodeGenerator.h"

struct StreamFunc 
{
    std::string encode;
    std::string decode;
	std::string length;
};

class CSharpGenerator : public CodeGenerator {
public:
    CSharpGenerator();
    ~CSharpGenerator();

    virtual std::string GetFilePostfix() { return ".cs"; }

protected:
    virtual std::string GenCodeBegin();
    virtual std::string GenIncludeFile(const std::string& fileName, const std::vector<std::string>& includeFiles);
    virtual std::string GenCodeEnd();

    virtual std::string GenEnum(const Enum& myEnum);
    virtual std::string GenStruct(const Struct& myStruct);
    virtual std::string GenNameSpaceBegin(const Namespace& myNamespace);
    virtual std::string GenNameSpaceEnd(const Namespace& myNamespace);

    virtual std::string GenMsgIDFunction(const Struct& myStruct);
    virtual std::string GenSequenceFunction(const Struct& myStruct);

    virtual std::string GenEncodeFunction(std::string bufferType, const Struct& myStruct);
    virtual std::string EncodeField(const Field& field);
    virtual std::string EncodeArray(const FieldType& type, const std::string& name, int fixedLen);

    virtual std::string GenDecodeFunction(std::string bufferType, const Struct& myStruct);
    virtual std::string DecodeField(const Field& field);
    virtual std::string DecodeArray(const FieldType& type, const std::string& name, int fixedLen);

	virtual std::string GenGetLenFunction(const Struct& myStruct);
	virtual std::string GetFieldLen(const Field& field);

	virtual std::string GenComment(const std::string comment);

private:
    StreamFunc              streamFunces[TE_BASE_NUM];
    
};

#endif
