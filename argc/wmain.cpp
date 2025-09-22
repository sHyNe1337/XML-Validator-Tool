#include "pch.h"
#include "vali.h"
#include <io.h>
#include <fcntl.h>

int wmain(int argc, wchar_t* argv[]) {

    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    try {
        XMLValidator validator;

        if (!validator.ParseCommandLine(argc, argv)) {
            validator.PrintUsage();
            return 1;
        }

        if (!validator.Initialize()) {
            wcerr << L"Initialisierung fehlgeschlagen." << endl;
            return 1;
        }

        bool success = true;

        if (validator.GetParams().validate) {
            if (!validator.ValidateXML()) {
                success = false;
            }
        }

        if (validator.GetParams().transform) {
            if (!validator.TransformXML()) {
                success = false;
            }
        }

        if (success) {
            wcout << L"Programmausführung erfolgreich abgeschlossen." << endl;
            return 0;
        }
        else {
            wcout << L"Programmausführung mit Fehlern beendet." << endl;
            return 1;
        }

    }
    catch (const exception& e) {
        wcerr << L"Unbehandelte Ausnahme: " << XMLValidatorUtils::StringToWString(e.what()) << endl;
        return 1;
    }
    catch (...) {
        wcerr << L"Unbekannte Ausnahme aufgetreten." << endl;
        return 1;
    }
}