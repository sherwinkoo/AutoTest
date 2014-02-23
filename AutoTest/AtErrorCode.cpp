#include "stdafx.h"
#include "AtErrorCode.h"

static char*	error_type_str[] ={
	"controller: ",
	"starter: ",
	"Monitor: "
};

static char*	error_str_controller[] = {
	"Success"
};

static char*	error_str_starter[] = {
	"Success",
	"Script not Exist",
	"Script Run Error"
};

static char*	error_str_monitor[] = {
	"Success",
	"Timeout",
	"Failed"
};

std::string AtGetErrorString(unsigned int nErrCode)
{
	unsigned int	nErrType		= (nErrCode & def_AT_ERROR_TYPE_MASK);
	unsigned int	nSubCode		= nErrCode & (~def_AT_ERROR_TYPE_MASK);
	std::string		strErrorInfo	= "[Unknown]";

	if ((nErrType >> 24) < sizeof(error_type_str))
	{
		strErrorInfo = error_type_str[nErrType >> 24];
	}

	if (nErrType == def_AT_ERROR_TYPE_CONTROLLER)
	{
		if (nSubCode >= sizeof(error_str_controller))
		{
			goto RetUnknown;
		}

		return	strErrorInfo + error_str_controller[nSubCode];
	}

	if (nErrType == def_AT_ERROR_TYPE_STARTER)
	{
		if (nSubCode >= sizeof(error_str_starter))
		{
			goto RetUnknown;
		}

		return strErrorInfo + error_str_starter[nSubCode];
	}

	if (nErrType == def_AT_ERROR_TYPE_MONITOR)
	{
		if (nSubCode >= sizeof(error_str_monitor))
		{
			goto RetUnknown;
		}

		return strErrorInfo + error_str_monitor[nSubCode];
	}

RetUnknown:

	return	strErrorInfo + "Error";
}