#include "LuaGenerator.h"
#include "utility.h"

#include <sstream>

LuaGenerator::LuaGenerator() {

}

LuaGenerator::~LuaGenerator() {

}


std::string LuaGenerator::GenCodeBegin() {
    return "";
}

std::string LuaGenerator::GenIncludeFile(const std::string& filename, const std::vector<std::string>& includeFiles) {
    std::stringstream ss;

    ss << GetPreTable() << "local " << DescriptorModuleName << " = require \"descriptor\"" << std::endl;

    for (auto file : includeFiles) {
        ss << GetPreTable() << "local " << file <<  " = require \"" << file << "\"" << std::endl;
    }

    ss << std::endl;

//     ss << GetPreTable() << "module \"" << filename << "\"" << std::endl;
//     ss << std::endl;

    return ss.str();
}

std::string LuaGenerator::GenCodeEnd() {
    return GenExportSymbols();
}

std::string LuaGenerator::GenEnum(const Enum& myEnum) {
    /*
    msgid = c.def_enum("msgid",
      {
        c.def_enum_value("login",1001),
        c.def_enum_value("begin",1002),
        c.def_enum_value("end",1003)
      }
    )
    */

	AddSymbol(myEnum.name);

    std::stringstream ss;

	for (auto& comment : myEnum.comments) {
		ss << GetPreTable() << GenComment(comment) << std::endl;
	}

    ss << GetPreTable() << myEnum.name << " = " << DescriptorModuleName << ".def_enum(\"" << myEnum.name << "\"," << std::endl;
    IncPreTable();
    ss << GetPreTable() << "{" << std::endl;
    IncPreTable();

    for (auto enumVal : myEnum.values) {
		for (auto& comment : enumVal.comments) {
			ss << GetPreTable() << GenComment(comment) << std::endl;
		}
        ss << GetPreTable() << DescriptorModuleName << ".def_enum_value(\"" << enumVal.name << "\", " << enumVal.value << ")," << std::endl;
    }

    DecPreTable();
    ss << GetPreTable() << "}" << std::endl;
    DecPreTable();
    ss << GetPreTable() << ")" << std::endl;

    return ss.str();
}

std::string LuaGenerator::GenStruct(const Struct& myStruct) {
    /*
    playinfo_message = c.def_message("playerinfo",20000,        // 如果是消息
                        c.def_struct("playerinfo",              // 如果是结构体
        {
          c.def_scalar_field("name",c.type_string,""),
          c.def_scalar_field("id",c.type_int32,0),
          c.def_scalar_field("description",c.type_string,""),
        } 	
    )
    */

	AddSymbol(myStruct.name);

    std::stringstream ss;

	for (auto& comment : myStruct.comments) {
		ss << GetPreTable() << GenComment(comment) << std::endl;
	}

    if (myStruct.msgId >= 0) {
        ss << GetPreTable() << myStruct.name << " = " << DescriptorModuleName << ".def_message" << "(\"" << myStruct.name << "\", " << myStruct.msgId << ", " << std::endl;
    } else {
        ss << GetPreTable() << myStruct.name << " = " << DescriptorModuleName << ".def_struct" << "(\"" << myStruct.name << "\", " << std::endl;
    }
    
    IncPreTable();
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
    for (auto field : myStruct.fields) {
		for (auto& comment : field.comments) {
			ss << GetPreTable() << GenComment(comment) << std::endl;
		}
        ss << GenField(field, IsStructDefinedInOtherFile(myStruct, field));
    }
    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;
    DecPreTable();

    ss << GetPreTable() << ")" << std::endl;

    return ss.str();
}

std::string LuaGenerator::GenNameSpaceBegin(const Namespace& myNamespace) {
    return "";
}

std::string LuaGenerator::GenNameSpaceEnd(const Namespace& myNamespace) {
    return "";
}

std::string LuaGenerator::GenField(const Field& field, bool structDefinedInOtherFile) {
    /*
    c.def_scalar_field("name",c.type_string,""),
    c.def_scalar_field("id",c.type_int32,0),
    c.def_scalar_field("description",c.type_string,""),
    */

    std::stringstream ss;

    std::string defFunc = DescriptorModuleName + ".";
    std::string defaultValue = "0";
    std::string fieldType = "";

    if (field.type.type == TE_STRUCT) {
        if (structDefinedInOtherFile) {
            fieldType = field.type.definedFileName + "." + field.type.identifyInLua;
        } else {
            fieldType = field.type.identifyInLua;
        }

        if (field.isArray) {
            defFunc += "def_message_vector_field";
        } else {
            defFunc += "def_message_field";
        }
    } else {
        if (field.isArray) {
            defFunc += "def_scalar_vector_field";
        }
        else {
            defFunc += "def_scalar_field";
        }

        fieldType = DescriptorModuleName + "." + field.type.identifyInLua;

        if (field.type.type == TE_STRING) {
            defaultValue = "\"\"";
        }
    }
    
    ss << GetPreTable() << defFunc << "(\"" << field.name << "\", " << fieldType;
    if (field.type.type != TE_STRUCT) {
        ss << ", " << defaultValue;
    }

    ss << ")," << std::endl;

    return ss.str();
}

bool LuaGenerator::IsStructDefinedInOtherFile(const Struct& myStruct, const Field& field) {
    if (field.type.type != TE_STRUCT) {
        return false;
    }

    if (myStruct.type.definedFileName == field.type.definedFileName) {
        return false;
    }

    return true;
}

std::string LuaGenerator::GenComment(const std::string comment)
{
	std::stringstream ss;
	ss << "-- " << comment;
	return ss.str();
}

std::string LuaGenerator::GenExportSymbols() const
{
	std::stringstream ss;
	ss << "return {" << std::endl;

	for (auto& symbol : m_ExportSymbols)
	{
		ss << "\t" << symbol << " = " << symbol << ", " << std::endl;
	}

	ss << "}";
	return ss.str();
}

void LuaGenerator::AddSymbol(std::string name)
{
	m_ExportSymbols.push_back(name);
}