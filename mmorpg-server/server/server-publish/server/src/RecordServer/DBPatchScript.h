#ifndef _DBPATCH_SCRIPT_H_
#define _DBPATCH_SCRIPT_H_

#include <AvalonScriptFile.h>

class DBPatchScript
	: public Avalon::ScriptFile
{
public:
	DBPatchScript(Avalon::ScriptState* state);
	~DBPatchScript() {};
};

#endif