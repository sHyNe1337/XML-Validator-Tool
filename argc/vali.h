#pragma once
#include "pch.h"


struct CommandLineParams {
    wstring xmlPath;        // :P :Path
    wstring xmlName;        // :N :Name  
    bool validate = false;  // :V :Validierung
    bool transform = false; // :T :Transformieren

    wstring GetFullXMLPath() const {
        if (!xmlPath.empty() && !xmlName.empty()) {
            filesystem::path fullPath = filesystem::path(xmlPath) / xmlName;
            return fullPath.wstring();
        }
        return xmlName;
    }

    wstring GetLogFileName() const {
        if (!xmlName.empty()) {
            filesystem::path xmlFile(xmlName);
            xmlFile.replace_extension(LOG_EXTENSION);
            return xmlFile.filename().wstring();
        }
        return L"validation.log";
    }
};


class XMLValidator {
private:
    CommandLineParams m_params;
    wofstream m_logFile;


    bool InitializeCOM();
    void CleanupCOM();
    bool FindFirstXMLInCurrentDirectory();
    wstring FindXSDFile(const wstring& xmlFilePath);
    void LogMessage(const wstring& message, bool consoleOutput = true);
    void LogError(const wstring& error, bool consoleOutput = true);
    wstring GetErrorDetails(CComPtr<IXMLDOMParseError> parseError);

public:
    XMLValidator();
    ~XMLValidator();


    bool ParseCommandLine(int argc, wchar_t* argv[]);
    bool Initialize();
    bool ValidateXML();
    bool TransformXML();
    void PrintUsage();


    const CommandLineParams& GetParams() const { return m_params; }
};


namespace XMLValidatorUtils {
    wstring StringToWString(const string& str);
    string WStringToString(const wstring& wstr);
    bool FileExists(const wstring& filePath);
    vector<wstring> FindXMLFiles(const wstring& directory);
}