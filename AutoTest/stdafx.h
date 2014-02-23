// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <direct.h>
#include <time.h>

// TODO: 在此处引用程序需要的其他头文件
#include <Windows.h>
#include <vector>
#include <string>
#include "TinyXml/tinyxml.h"
#include "AtErrorCode.h"

#define ERROR_CODE(lv, cv)		(lv * 10 + cv)

