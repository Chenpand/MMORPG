#include "CSharpGenerator.h"
#include "utility.h"

#include <sstream>

CSharpGenerator::CSharpGenerator() 
{
    streamFunces[TE_UINT8]  = { "BaseDLL.encode_int8",      "BaseDLL.decode_int8"       , "1"};
    streamFunces[TE_UINT16] = { "BaseDLL.encode_uint16",    "BaseDLL.decode_uint16"     , "2"};
    streamFunces[TE_INT16]  = { "BaseDLL.encode_int16",     "BaseDLL.decode_int16"      , "2"};
    streamFunces[TE_UINT32] = { "BaseDLL.encode_uint32",    "BaseDLL.decode_uint32"     , "4"};
    streamFunces[TE_INT32]  = { "BaseDLL.encode_int32",     "BaseDLL.decode_int32"      , "4"};
    streamFunces[TE_UINT64] = { "BaseDLL.encode_uint64",    "BaseDLL.decode_uint64"     , "8"};
    streamFunces[TE_INT64]  = { "BaseDLL.encode_int64",     "BaseDLL.decode_int64"      , "8"};
    streamFunces[TE_STRING] = { "BaseDLL.encode_string",    "BaseDLL.decode_string"     , ""};
}

CSharpGenerator::~CSharpGenerator() {

}

std::string CSharpGenerator::GenCodeBegin() {
    std::stringstream ss;

    ss << "using System;" << std::endl;
    ss << "using System.Text;" << std::endl;
    ss << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenIncludeFile(const std::string& fileName, const std::vector<std::string>& includeFiles) {
    std::stringstream ss;

    return ss.str();
}

std::string CSharpGenerator::GenCodeEnd() {
    return "";
}

std::string CSharpGenerator::GenEnum(const Enum& myEnum) {
    std::stringstream ss;

	for (auto& comment : myEnum.comments) {
		ss << GenComment(comment) << std::endl;
	}
    ss << GetPreTable() << "public enum " << myEnum.name << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
   
    for (auto enumVal : myEnum.values) {
		for (auto& comment : enumVal.comments) {
			ss << GenComment(comment) << std::endl;
		}
        ss << GetPreTable() << enumVal.name << " = " << enumVal.value << "," << std::endl;
    }

    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenStruct(const Struct& myStruct) {
    std::stringstream ss;
	
	for (auto& comment : myStruct.comments) {
		ss << GenComment(comment) << std::endl;
	}

    if (myStruct.msgId >= 0)
    {
        ss << GetPreTable() << "[Protocol]" << std::endl;
    }

    ss << GetPreTable() << "public class " << myStruct.name << " : Protocol.IProtocolStream";
    if (myStruct.msgId >= 0) {
        ss << ", Protocol.IGetMsgID";
    }
    ss << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();

    if (myStruct.msgId >= 0) {
        ss << GetPreTable() << "public const UInt32 MsgID = " << myStruct.msgId << ";" << std::endl;
        ss << GetPreTable() << "public UInt32 Sequence;" << std::endl;
    }

    for (auto field : myStruct.fields) {
		for (auto& comment : field.comments) {
			ss << GenComment(comment) << std::endl;
		}

        if (!field.isArray) {
			if (field.type.type == TE_STRUCT) {
				ss << GetPreTable() << "public " << field.type.identifInCSharp << " " << field.name << " = new "
					<< field.type.identifInCSharp << "();" << std::endl;
			}
			else
			{
				ss << GetPreTable() << "public " << field.type.identifInCSharp << " " << field.name << ";" << std::endl;
			}
        } else {
            ss << GetPreTable() << "public " << field.type.identifInCSharp << "[] " << field.name << " = new "
                << field.type.identifInCSharp << "[" << field.arrayFixedLen << "];" << std::endl;
        }
    }
    

    ss << std::endl;

    ss << GetPreTable() << "#region METHOD" << std::endl;
    IncPreTable();

    ss << GenMsgIDFunction(myStruct);
    ss << std::endl;
    ss << GenSequenceFunction(myStruct);
    ss << std::endl;
    ss << GenEncodeFunction("byte[]", myStruct);
    ss << std::endl;
    ss << GenDecodeFunction("byte[]", myStruct);
	ss << std::endl;
	ss << GenEncodeFunction("MapViewStream", myStruct);
	ss << std::endl;
	ss << GenDecodeFunction("MapViewStream", myStruct);
	ss << std::endl;
	ss << GenGetLenFunction(myStruct);

    DecPreTable();
    ss << GetPreTable() << "#endregion" << std::endl;

    ss << std::endl;
    DecPreTable();
    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenNameSpaceBegin(const Namespace& myNamespace) {
    std::stringstream ss;

    if (myNamespace.components.empty()) {
        return ss.str();
    }

    ss << GetPreTable() << "namespace ";
    for (auto i = 0; i < myNamespace.components.size(); i++) {
        ss << myNamespace.components[i];
        if (i != myNamespace.components.size() - 1) {
            ss << ".";
        }
        else {
            ss << std::endl;
        }
    }
    
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
    return ss.str();
}

std::string CSharpGenerator::GenNameSpaceEnd(const Namespace& myNamespace) {
    std::stringstream ss;
    
    if (myNamespace.components.empty()) {
        return ss.str();
    }

    DecPreTable();
    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenMsgIDFunction(const Struct& myStruct) {
    std::stringstream ss;

    if (myStruct.msgId < 0)
    {
        return ss.str();
    }

    ss << GetPreTable() << "public UInt32 GetMsgID()" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();

        ss << GetPreTable() << "return MsgID;" << std::endl;
    
    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenSequenceFunction(const Struct& myStruct)
{
    std::stringstream ss;

    if (myStruct.msgId < 0)
    {
        return ss.str();
    }

    ss << GetPreTable() << "public UInt32 GetSequence()" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();

    ss << GetPreTable() << "return Sequence;" << std::endl;

    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    ss << std::endl;

    ss << GetPreTable() << "public void SetSequence(UInt32 sequence)" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();

    ss << GetPreTable() << "Sequence = sequence;" << std::endl;

    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenEncodeFunction(std::string bufferType, const Struct& myStruct) {
    std::stringstream ss;
    ss << GetPreTable() << "public void encode(" << bufferType << " buffer, ref int pos_)" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
    for (auto field : myStruct.fields) {
        ss  << EncodeField(field);
    }
    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;
    
    return ss.str();
}

std::string CSharpGenerator::EncodeField(const Field& field) {
    std::stringstream ss;

    if (field.type.type == TE_STRING) {
        // byte[] bytes = StringHelper.StringToUTF8Bytes(myString);
        // BaseDll.encode_string(buffer, ref pos_, bytes, (UInt16)(buffer.Length - pos_));
		if (!field.isArray) {
			std::string stringByteName = field.name + "Bytes";
			ss << GetPreTable() << "byte[] " << stringByteName << " = StringHelper.StringToUTF8Bytes(" << field.name << ");" << std::endl;
			ss << GetPreTable() << streamFunces[TE_STRING].encode << "(buffer, ref pos_, " << stringByteName << ", (UInt16)(buffer.Length - pos_));" << std::endl;
		}
		else {
			ss << EncodeArray(field.type, field.name, field.arrayFixedLen);
		}
    } else if (field.type.type < TE_BASE_NUM) {

        if (!field.isArray) {
            ss << GetPreTable() << streamFunces[field.type.type].encode << "(buffer, ref pos_, " << field.name << ");" << std::endl;
        } else {
            ss << EncodeArray(field.type, field.name, field.arrayFixedLen);
        }
    } else {
        if (!field.isArray) {
            ss << GetPreTable() << field.name << ".encode(buffer, ref pos_);" << std::endl;
        } else {
            ss << EncodeArray(field.type, field.name, field.arrayFixedLen);
        }
        
    }

    return ss.str();
}

std::string CSharpGenerator::EncodeArray(const FieldType& type, const std::string& name, int arrayFixedLen)
{
    std::stringstream ss;

    std::string arrayLenStr = name + ".Length";

    if (arrayFixedLen == 0) {
        ss << GetPreTable() << streamFunces[TE_UINT16].encode << "(buffer, ref pos_, (UInt16)" << arrayLenStr << ");" << std::endl;
    }

    ss << GetPreTable() << "for(int i = 0; i < " << arrayLenStr << "; i++)" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
    if (type.type < TE_STRING) {
        ss << GetPreTable() << streamFunces[type.type].encode << "(buffer, ref pos_, " << name << "[i]);" << std::endl;
	} else if (type.type == TE_STRING) {
		std::string stringByteName = name + "Bytes";
		ss << GetPreTable() << "byte[] " << stringByteName << " = StringHelper.StringToUTF8Bytes(" << name << "[i]);" << std::endl;
		ss << GetPreTable() << streamFunces[TE_STRING].encode << "(buffer, ref pos_, " << stringByteName << ", (UInt16)(buffer.Length - pos_));" << std::endl;
	} else if (type.type == TE_STRUCT) {
        ss << GetPreTable() << name << "[i].encode(buffer, ref pos_);" << std::endl;
    } else {
        printf("unknown type:%s for field:%s\n", type.identifInCSharp.c_str(), name.c_str());
        exit(0);
    }

    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenDecodeFunction(std::string bufferType, const Struct& myStruct) {
    std::stringstream ss;
    ss << GetPreTable() << "public void decode(" << bufferType << " buffer, ref int pos_)" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();
    for (auto field : myStruct.fields) {
        ss << DecodeField(field);
    }
    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::DecodeField(const Field& field) {
    std::stringstream ss;

    if (field.type.type == TE_STRING) {
		if (!field.isArray) {
			std::string stringLen = field.name + "Len";
			std::string stringBytes = field.name + "Bytes";
			ss << GetPreTable() << "UInt16 " << stringLen << " = 0;" << std::endl;
			ss << GetPreTable() << streamFunces[TE_UINT16].decode << "(buffer, ref pos_, ref " << stringLen << ");" << std::endl;
			ss << GetPreTable() << "byte[] " << stringBytes << " = new byte[" << stringLen << "];" << std::endl;

			ss << GetPreTable() << "for(int i = 0; i < " << stringLen << "; i++)" << std::endl;
			ss << GetPreTable() << "{" << std::endl;

			IncPreTable();
			ss << GetPreTable() << streamFunces[TE_UINT8].decode << "(buffer, ref pos_, ref " << stringBytes << "[i]);" << std::endl;
			DecPreTable();

			ss << GetPreTable() << "}" << std::endl;

			ss << GetPreTable() << field.name << " = StringHelper.BytesToString(" << stringBytes << ");" << std::endl;
		} else {
			ss << DecodeArray(field.type, field.name, field.arrayFixedLen);
		}
    }
    else if (field.type.type < TE_BASE_NUM) {
        if (!field.isArray) {
            ss << GetPreTable() << streamFunces[field.type.type].decode << "(buffer, ref pos_, ref " << field.name << ");" << std::endl;
        }
        else {
            ss << DecodeArray(field.type, field.name, field.arrayFixedLen);
        }
    }
    else {
        if (field.isArray)
        {
            ss << DecodeArray(field.type, field.name, field.arrayFixedLen);
        }
        else
        {
            ss << GetPreTable() << field.name << ".decode(buffer, ref pos_);" << std::endl;
        }
        
    }

    return ss.str();
}

std::string CSharpGenerator::DecodeArray(const FieldType& type, const std::string& name, int arrayFixedLen)
{
    std::stringstream ss;

    std::string cntFieldName = arrayFixedLen == 0 ? (name + "Cnt") : NumToString(arrayFixedLen);

    if (arrayFixedLen == 0) {
        ss << GetPreTable() << "UInt16 " << cntFieldName << " = 0;" << std::endl;
        ss << GetPreTable() << streamFunces[TE_UINT16].decode << "(buffer, ref pos_, ref " << cntFieldName << ");" << std::endl;
        ss << GetPreTable() << name << " = new " << type.identifInCSharp << "[" << cntFieldName << "];" << std::endl;
    }

    ss << GetPreTable() << "for(int i = 0; i < " << name << ".Length; i++)" << std::endl;
    ss << GetPreTable() << "{" << std::endl;

    IncPreTable();

    if (type.type == TE_STRUCT)
    {

        ss << GetPreTable() << name << "[i] = new " << type.identifInCSharp << "();" << std::endl;
        ss << GetPreTable() << name << "[i].decode(buffer, ref pos_);" << std::endl;
    }
	else if (type.type == TE_STRING)
	{
		std::string stringLen = name + "Len";
		std::string stringBytes = name + "Bytes";
		ss << GetPreTable() << "UInt16 " << stringLen << " = 0;" << std::endl;
		ss << GetPreTable() << streamFunces[TE_UINT16].decode << "(buffer, ref pos_, ref " << stringLen << ");" << std::endl;
		ss << GetPreTable() << "byte[] " << stringBytes << " = new byte[" << stringLen << "];" << std::endl;

		ss << GetPreTable() << "for(int j = 0; j < " << stringLen << "; j++)" << std::endl;
		ss << GetPreTable() << "{" << std::endl;

		IncPreTable();
		ss << GetPreTable() << streamFunces[TE_UINT8].decode << "(buffer, ref pos_, ref " << stringBytes << "[j]);" << std::endl;
		DecPreTable();

		ss << GetPreTable() << "}" << std::endl;

		ss << GetPreTable() << name << "[i] = StringHelper.BytesToString(" << stringBytes << ");" << std::endl;
	}
    else
    {
        ss << GetPreTable() << streamFunces[type.type].decode << "(buffer, ref pos_, ref " << name << "[i]);" << std::endl;
    }
    
    DecPreTable();

    ss << GetPreTable() << "}" << std::endl;

    return ss.str();
}

std::string CSharpGenerator::GenGetLenFunction(const Struct& myStruct)
{
	std::stringstream ss;
	ss << GetPreTable() << "public int getLen()" << std::endl;
	ss << GetPreTable() << "{" << std::endl;

	IncPreTable();
	ss << GetPreTable() << "int _len = 0;" << std::endl;
	for (auto field : myStruct.fields) {
		ss << GetPreTable() << "// " << field.name << std::endl;
		ss << GetFieldLen(field);
	}
	ss << GetPreTable() << "return _len;" << std::endl;
	DecPreTable();

	ss << GetPreTable() << "}" << std::endl;

	return ss.str();
}

std::string CSharpGenerator::GetFieldLen(const Field& field)
{
	std::stringstream ss;

	if (field.arrayFixedLen > 0 || field.isArray)
	{
		if (field.type.type >= TE_UINT8 && field.type.type < TE_STRING)
		{
			ss << GetPreTable() << "_len += ";
			if (field.arrayFixedLen == 0)
			{
				ss << "2 + ";
			}
			ss << streamFunces[field.type.type].length << " * " << field.name << ".Length;" << std::endl;
			return ss.str();
		}

		// สýื้
		if (field.arrayFixedLen == 0)
		{
			ss << GetPreTable() << "_len += 2;" << std::endl;
		}

		ss << GetPreTable() << "for(int j = 0; j < " << field.name << ".Length; j++)" << std::endl;
		ss << GetPreTable() << "{" << std::endl;
		IncPreTable();

		Field tmpField = field;
		tmpField.arrayFixedLen = 0;
		tmpField.isArray = 0;
		tmpField.name = field.name + "[j]";
		ss << GetFieldLen(tmpField);

		DecPreTable();
		ss << GetPreTable() << "}" << std::endl;
	}
	else if (field.type.type == TE_STRUCT)
	{
		ss << GetPreTable() << "_len += " << field.name << ".getLen();" << std::endl;
	}
	else if (field.type.type == TE_STRING)
	{
		std::string stringByteName = "_strBytes";
		ss << GetPreTable() << "{" << std::endl;
		IncPreTable();
		ss << GetPreTable() << "byte[] " << stringByteName << " = StringHelper.StringToUTF8Bytes(" << field.name << ");" << std::endl;
		ss << GetPreTable() << "_len += 2 + " << stringByteName << ".Length;" << std::endl;
		DecPreTable();
		ss << GetPreTable() << "}" << std::endl;
	}
	else
	{
		ss << GetPreTable() << "_len += " << streamFunces[field.type.type].length << ";" << std::endl;
	}

	return ss.str();
}

std::string CSharpGenerator::GenComment(const std::string comment)
{
    std::string context = comment;
    if (auto contextPos = comment.find_first_not_of(' ') != std::string::npos)
    {
        context = comment.substr(contextPos);
    }

	std::stringstream ss;
    ss << GetPreTable() << "/// <summary>" << std::endl;
    ss << GetPreTable() << "/// " << comment << std::endl;
    ss << GetPreTable() << "/// </summary>";
    //ss << GetPreTable() << "// " << comment << std::endl;

	return ss.str();
}
