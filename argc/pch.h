#pragma once


#include <windows.h>

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <map>
#include <sstream>


#include <winrt/base.h>


#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <msxml6.h>
#include <comutil.h>


using namespace std;
using namespace winrt;


constexpr auto XML_EXTENSION = L".xml";
constexpr auto XSD_EXTENSION = L".xsd";
constexpr auto LOG_EXTENSION = L".log";

