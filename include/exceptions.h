//
// Created by francesco on 07/11/26.
//

#ifndef PROJECT2_EXCEPTIONS_H
#define PROJECT2_EXCEPTIONS_H

#include <exception>

// Eccezione per errori di formattazione del messaggio CAN
class InvalidRawMessage : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Non valid raw message";
    }
};

// Eccezione per errore apertura stream CAN
class OpenCanError : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Error in file opening";
    }
};

// Eccezione per errori sul file di log della sessione
class LogFileError : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Errore irreversibile: impossibile aprire o scrivere sul file di Log.";
    }
};

// Eccezione per errori sul file CSV delle statistiche
class CsvFileError : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Errore irreversibile: impossibile aprire o scrivere sul file CSV.";
    }
};

#endif //PROJECT2_EXCEPTIONS_H
