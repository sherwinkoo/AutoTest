#pragma once

#define def_AT_ERROR_TYPE_CONTROLLER	0x00000000
#define def_AT_ERROR_TYPE_STARTER		0x01000000
#define def_AT_ERROR_TYPE_MONITOR		0x02000000
#define def_AT_ERROR_TYPE_MASK			0xFF000000

#define def_AT_CONTROLLER_Success		0x00000000


#define def_AT_STARTER_Success			0x00000000			// Starter运行成功
#define def_AT_STARTER_ScriptNotExist	0x00000001			// 脚本不存在
#define def_AT_STARTER_ScriptRunError	0x00000002			// 脚本运行失败
#define def_AT_STARTER_ScriptNotSupport	0x00000002			// 脚本类型不支持


#define def_AT_MONITOR_Success			0x00000000			// 成功检测到target
#define def_AT_MONITOR_Timeout			0x00000001			// 等待target超时
#define def_AT_MONITOR_PathNotExist		0x00000002			// 无法打开日志文件
#define def_AT_MONITOR_Failed			0x00000002			// 

#define MakeErrorCode(Type, Code)		(def_AT_ERROR_TYPE_##Type | Code)

#define MakeErrorStarter(Code)			MakeErrorCode(STARTER, Code)
#define MakeErrorController(Code)		MakeErrorCode(CONTROLLER, Code)
#define MakeErrorMonitor(Code)			MakeErrorCode(MONITOR, Code)

#define AtSuccess(Code)					(!(Code & (~def_AT_ERROR_TYPE_MASK)))

std::string AtGetErrorString(unsigned int nErrCode);
