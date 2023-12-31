// 该头文件主要用于引入与COM组件密切相关的头文件
// 可放入预编译头
#pragma once
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shcore.h>
#include <unknwn.h>
#include <comdef.h>
#include <wincodec.h>
#include <uianimation.h>
#include <oleacc.h>
#include <taskschd.h>
#include <comutil.h>
#include <pathcch.h>
#include <oleidl.h>
#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "oleacc.lib")
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "pathcch.lib")