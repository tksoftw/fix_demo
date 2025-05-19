#pragma once
#include <databento/dbn_file_store.hpp>
#include "Tick.hpp"
#include <fstream>

class DBNReader {
    databento::DbnFileStore reader;
public:
    explicit DBNReader(const std::string& path)
        : reader(path) {}

    bool next(Tick& t);
};
