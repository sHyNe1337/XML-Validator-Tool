#include "pch.h"
#include "vali.h"

XMLValidator::XMLValidator() {

}

XMLValidator::~XMLValidator() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    CleanupCOM();
}

bool XMLValidator::ParseCommandLine(int argc, wchar_t* argv[]) {

    for (int i = 1; i < argc; i++) {
        wstring arg = argv[i];
        transform(arg.begin(), arg.end(), arg.begin(), ::towupper);

        if (arg == L":P" || arg == L":PATH") {
            if (i + 1 < argc) {
                m_params.xmlPath = argv[++i];
            }
        }
        else if (arg == L":N" || arg == L":NAME") {
            if (i + 1 < argc) {
                m_params.xmlName = argv[++i];
            }
        }
        else if (arg == L":V" || arg == L":VALIDIERUNG") {
            m_params.validate = true;
        }
        else if (arg == L":T" || arg == L":TRANSFORMIEREN") {
            m_params.transform = true;
        }
    }


    if (argc == 1) {

        if (!FindFirstXMLInCurrentDirectory()) {
            LogError(L"Keine XML-Datei im aktuellen Verzeichnis gefunden.");
            return false;
        }
        m_params.validate = true;
        m_params.transform = true;
    }
    else if (!m_params.xmlPath.empty() && !m_params.xmlName.empty()) {

        if (!m_params.validate && !m_params.transform) {
            m_params.validate = true;
            m_params.transform = true;
        }
    }
    else if (!m_params.xmlPath.empty() && !m_params.xmlName.empty() && m_params.transform) {

        m_params.validate = true;
    }

    return true;
}

bool XMLValidator::Initialize() {
    if (!InitializeCOM()) {
        LogError(L"Fehler beim Initialisieren von COM.");
        return false;
    }


    wstring xmlFilePath = m_params.GetFullXMLPath();
    if (!XMLValidatorUtils::FileExists(xmlFilePath)) {
        LogError(L"XML-Datei nicht gefunden: " + xmlFilePath);
        return false;
    }


    wstring logFileName = m_params.GetLogFileName();
    m_logFile.open(logFileName, ios::out | ios::app);
    if (!m_logFile.is_open()) {
        wcout << L"Warnung: Logdatei konnte nicht geöffnet werden: " << logFileName << endl;
    }

    LogMessage(L"=== XML-Validierung gestartet ===");
    LogMessage(L"XML-Datei: " + xmlFilePath);

    return true;
}

bool XMLValidator::ValidateXML() {
    if (!m_params.validate) {
        LogMessage(L"Validierung übersprungen (nicht angefordert).");
        return true;
    }

    LogMessage(L"Starte XML-Validierung...");

    try {
 
        CComPtr<IXMLDOMDocument2> xmlDoc;
        HRESULT hr = xmlDoc.CoCreateInstance(__uuidof(DOMDocument60));
        if (FAILED(hr)) {
            LogError(L"Fehler beim Erstellen des XML-Dokuments.");
            return false;
        }


        xmlDoc->put_async(VARIANT_FALSE);
        xmlDoc->put_validateOnParse(VARIANT_TRUE);
        xmlDoc->put_resolveExternals(VARIANT_TRUE);


        wstring xmlFilePath = m_params.GetFullXMLPath();
        _variant_t xmlFile(xmlFilePath.c_str());
        VARIANT_BOOL success = VARIANT_FALSE;

        hr = xmlDoc->load(xmlFile, &success);
        if (FAILED(hr) || success == VARIANT_FALSE) {
            CComPtr<IXMLDOMParseError> parseError;
            xmlDoc->get_parseError(&parseError);
            wstring errorDetails = GetErrorDetails(parseError);
            LogError(L"XML-Parsing-Fehler: " + errorDetails);
            return false;
        }

        wstring xsdFilePath = FindXSDFile(xmlFilePath);
        if (!xsdFilePath.empty()) {
            LogMessage(L"XSD-Schema gefunden: " + xsdFilePath);


            CComPtr<IXMLDOMSchemaCollection2> schemaCollection;
            hr = schemaCollection.CoCreateInstance(__uuidof(XMLSchemaCache60));
            if (SUCCEEDED(hr)) {
                _variant_t xsdFile(xsdFilePath.c_str());
                hr = schemaCollection->add(CComBSTR(L""), xsdFile);
                if (SUCCEEDED(hr)) {
    
                    CComVariant varSchema(schemaCollection);
                    hr = xmlDoc->putref_schemas(varSchema);

                    hr = xmlDoc->load(xmlFile, &success);
                    if (FAILED(hr) || success == VARIANT_FALSE) {
                        CComPtr<IXMLDOMParseError> parseError;
                        xmlDoc->get_parseError(&parseError);
                        wstring errorDetails = GetErrorDetails(parseError);
                        LogError(L"XML-Validierung fehlgeschlagen: " + errorDetails);
                        return false;
                    }
                }
                else {
                    LogError(L"Schema konnte nicht geladen werden.");
                    return false;
                }
            }
            else {
                LogError(L"Fehler beim Erstellen der Schema-Sammlung.");
                return false;
            }
        }
        else {
            LogMessage(L"Keine XSD-Datei gefunden - nur Well-formedness geprüft.");
        }

        LogMessage(L"Validierung erfolgreich");
        return true;

    }
    catch (const _com_error& e) {
        wstring error = L"COM-Fehler bei der Validierung: ";
        error += e.ErrorMessage();
        LogError(error);
        return false;
    }
    catch (...) {
        LogError(L"Unbekannter Fehler bei der Validierung.");
        return false;
    }
}

bool XMLValidator::TransformXML() {
    if (!m_params.transform) {
        return true;
    }

    LogMessage(L"Transformation in process");

    return true;
}

void XMLValidator::PrintUsage() {
    wcout << L"Usage: XMLValidator [options]" << endl;
    wcout << L"Options:" << endl;
    wcout << L"  :P :Path <path>           - Pfad zum XML-Verzeichnis" << endl;
    wcout << L"  :N :Name <filename>       - Name der XML-Datei" << endl;
    wcout << L"  :V :Validierung          - XML validieren" << endl;
    wcout << L"  :T :Transformieren       - XML transformieren" << endl;
    wcout << endl;
    wcout << L"Parameterregeln:" << endl;
    wcout << L"  Nur :P und :N -> :V und :T werden automatisch gesetzt" << endl;
    wcout << L"  :P, :N und :T -> :V wird automatisch gesetzt" << endl;
    wcout << L"  Keine Parameter -> Erste XML im aktuellen Ordner, :V und :T gesetzt" << endl;
}

bool XMLValidator::InitializeCOM() {
    HRESULT hr = CoInitialize(nullptr);
    return SUCCEEDED(hr);
}

void XMLValidator::CleanupCOM() {
    CoUninitialize();
}

bool XMLValidator::FindFirstXMLInCurrentDirectory() {
    try {
        auto xmlFiles = XMLValidatorUtils::FindXMLFiles(L".");
        if (!xmlFiles.empty()) {
            m_params.xmlName = xmlFiles[0];
            m_params.xmlPath = L".";
            return true;
        }
    }
    catch (...) {
    }
    return false;
}

wstring XMLValidator::FindXSDFile(const wstring& xmlFilePath) {
    filesystem::path xmlPath(xmlFilePath);
    filesystem::path xsdPath = xmlPath;
    xsdPath.replace_extension(XSD_EXTENSION);

    if (XMLValidatorUtils::FileExists(xsdPath.wstring())) {
        return xsdPath.wstring();
    }

    return L"";
}

void XMLValidator::LogMessage(const wstring& message, bool consoleOutput) {
    if (consoleOutput) {
        wcout << message << endl;
    }

    if (m_logFile.is_open()) {
        m_logFile << message << endl;
        m_logFile.flush();
    }
}

void XMLValidator::LogError(const wstring& error, bool consoleOutput) {
    wstring errorMsg = L"FEHLER: " + error;
    LogMessage(errorMsg, consoleOutput);
}

wstring XMLValidator::GetErrorDetails(CComPtr<IXMLDOMParseError> parseError) {
    if (!parseError) {
        return L"Unbekannter Parsing-Fehler";
    }

    BSTR reason = nullptr;
    long line = 0, pos = 0, errorCode = 0;

    parseError->get_reason(&reason);
    parseError->get_line(&line);
    parseError->get_linepos(&pos);
    parseError->get_errorCode(&errorCode);

    wstringstream ss;
    ss << L"Fehlercode: " << errorCode;
    if (reason) {
        ss << L", Grund: " << reason;
        SysFreeString(reason);
    }
    ss << L", Zeile: " << line << L", Position: " << pos;

    return ss.str();
}

namespace XMLValidatorUtils {
    wstring StringToWString(const string& str) {
        if (str.empty()) return wstring();
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        wstring result(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
        return result;
    }

    string WStringToString(const wstring& wstr) {
        if (wstr.empty()) return string();
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        string result(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
        return result;
    }

    bool FileExists(const wstring& filePath) {
        return filesystem::exists(filePath);
    }

    vector<wstring> FindXMLFiles(const wstring& directory) {
        vector<wstring> xmlFiles;
        try {
            for (const auto& entry : filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    wstring filename = entry.path().filename().wstring();
                    if (filename.size() >= 4) {
                        wstring ext = filename.substr(filename.size() - 4);
                        transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
                        if (ext == XML_EXTENSION) {
                            xmlFiles.push_back(filename);
                        }
                    }
                }
            }
        }
        catch (...) {
        }
        return xmlFiles;
    }
}