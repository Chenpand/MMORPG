#include "utility.h"
#include "parser.h"
#include "CSharpGenerator.h"
#include "CSharpMockGenerator.h"
#include "LuaGenerator.h"


int main(int argc, char* argv[])
{
    std::vector<CodeGenerator*> generators;
    std::string filename;
    std::string targetName;
    std::string targetPath;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-cs") {
            generators.push_back(new CSharpGenerator());
        }
		else if (arg == "-csmock")
		{
			generators.push_back(new CSharpMockGenerator());
		}
        else if (arg == "-lua") {
            generators.push_back(new LuaGenerator());
        }
        else if (arg == "-t") {
            if (++i >= argc) {
                printf("mising target path following %s.\n", arg.c_str());
                return -1;
            }
            targetPath = argv[i];
        }
        else {
            if (arg[0] != '-') {
                filename = arg;
            }
            else {
                printf("unknown argument %s.\n", arg.c_str());
                printf("anybuffer [-cs] [-lua] [-t target_path] protofile.\n");
                return -1;
            }
        }
    }

    if (filename.empty()) {
        printf("missing input files.\n");
        return -1;
    }

    if (generators.empty()) {
        printf("no target source type[cs|lua].\n");
        return -1;
    }

    targetName = GetFileNameWithoutPostfix(StripPath(filename));

    Parser parser;
    if (!parser.ParseFromFile(filename.c_str()))
    {
        return -1;
    }

    for (auto gen : generators) {
        std::string outFile = targetPath + PathSeparatorSet + targetName + gen->GetFilePostfix();
        std::string code = gen->GenCode(&parser, targetPath, targetName);

		if (!CheckDir(outFile.c_str(), targetPath.c_str()))
		{
			printf("can not make dir %s\n", targetPath.c_str());
		}
		else 
        {
			if (!SaveFile(outFile.c_str(), code.c_str(), false))
				printf("save file %s failed.\n", outFile.c_str());
        }
    }

    printf("generate %s%s success.\n", targetName.c_str(), CONFIG_FILE_POSTFIX);

    return 0;
}

