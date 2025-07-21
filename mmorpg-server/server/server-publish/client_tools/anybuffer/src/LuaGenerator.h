#ifndef __LUA_GENERATOR_H__
#define __LUA_GENERATOR_H__

#include "CodeGenerator.h"

class LuaGenerator : public CodeGenerator {
public:
    LuaGenerator();
    ~LuaGenerator();

    virtual std::string GetFilePostfix() { return ".lua"; }

protected:
    virtual std::string GenCodeBegin();
    virtual std::string GenIncludeFile(const std::string& filename, const std::vector<std::string>& includeFiles);
    virtual std::string GenCodeEnd();

    virtual std::string GenEnum(const Enum& myEnum);
    virtual std::string GenStruct(const Struct& myStruct);
    virtual std::string GenNameSpaceBegin(const Namespace& myNamespace);
    virtual std::string GenNameSpaceEnd(const Namespace& myNamespace);

	virtual std::string GenComment(const std::string comment);

	virtual std::string GenGetLenFunction(const Struct& myStruct) { return ""; }

private:
    std::string GenField(const Field& field, bool structDefinedInOtherFile);
    bool IsStructDefinedInOtherFile(const Struct& myStruct, const Field& field);

	std::string GenExportSymbols() const;

	void AddSymbol(std::string name);

private:
    const std::string DescriptorModuleName = "descriptor";

	// 需要导出的符号
	std::vector<std::string>			m_ExportSymbols;
};

#endif