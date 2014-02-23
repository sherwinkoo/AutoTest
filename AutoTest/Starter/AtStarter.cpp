#include "../stdafx.h"
#include "../Controller/AtController.h"
#include "AtStarter.h"

unsigned int AtStarter::start(AtScriptCmd* pCmd)
{
	unsigned int nErrCode		= 0;

	if (pCmd->type == def_AT_SCRIPT_TYPE_bat)
	{
		nErrCode = system(pCmd->ScriptPath.c_str());
		if (nErrCode)
		{
			return MakeErrorStarter(def_AT_STARTER_ScriptRunError);
		}
	}
	else
	{
		return	MakeErrorStarter(def_AT_STARTER_ScriptNotSupport);
	}

	return	MakeErrorStarter(def_AT_STARTER_Success);
}

