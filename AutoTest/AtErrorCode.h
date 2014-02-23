#pragma once

#define def_AT_ERROR_TYPE_CONTROLLER	0x00000000
#define def_AT_ERROR_TYPE_STARTER		0x01000000
#define def_AT_ERROR_TYPE_MONITOR		0x02000000
#define def_AT_ERROR_TYPE_MASK			0xFF000000

#define def_AT_CONTROLLER_Success		0x00000000


#define def_AT_STARTER_Success			0x00000000			// Starter���гɹ�
#define def_AT_STARTER_ScriptNotExist	0x00000001			// �ű�������
#define def_AT_STARTER_ScriptRunError	0x00000002			// �ű�����ʧ��
#define def_AT_STARTER_ScriptNotSupport	0x00000002			// �ű����Ͳ�֧��


#define def_AT_MONITOR_Success			0x00000000			// �ɹ���⵽target
#define def_AT_MONITOR_Timeout			0x00000001			// �ȴ�target��ʱ
#define def_AT_MONITOR_PathNotExist		0x00000002			// �޷�����־�ļ�
#define def_AT_MONITOR_Failed			0x00000002			// 

#define MakeErrorCode(Type, Code)		(def_AT_ERROR_TYPE_##Type | Code)

#define MakeErrorStarter(Code)			MakeErrorCode(STARTER, Code)
#define MakeErrorController(Code)		MakeErrorCode(CONTROLLER, Code)
#define MakeErrorMonitor(Code)			MakeErrorCode(MONITOR, Code)

#define AtSuccess(Code)					(!(Code & (~def_AT_ERROR_TYPE_MASK)))

std::string AtGetErrorString(unsigned int nErrCode);
