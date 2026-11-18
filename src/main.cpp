#include "../include/telemetry_fsm.h"
#include "../include/exceptions.h"
#include <iostream>

// (Non serve includere fake_receiver.h qui nel main)

int main(int argc, char* argv[])
{
    TelemetryFSM telemetry;

    try
    {
        if (argc > 1)
        {
            telemetry.start(argv[1]);
        }
        else
        {
            telemetry.start();
        }
    }
    catch (const OpenCanError& e)
    {
        std::cerr << "[ERRORE FATALE] " << e.what() << "\n";
        return 1;
    }
    catch (const LogFileError& e)
    {
        std::cerr << "[ERRORE I/O] " << e.what() << "\n";
        return 2;
    }
    catch (const CsvFileError& e)
    {
        std::cerr << "[ERRORE I/O] " << e.what() << "\n";
        return 3;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERRORE FATALE] Eccezione generica: " << e.what() << "\n";
        return 4;
    }
    catch (...)
    {
        std::cerr << "[ERRORE FATALE] Eccezione sconosciuta.\n";
        return 5;
    }

    return 0;
}
