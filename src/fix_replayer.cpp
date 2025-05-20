#include "BasicServer.hpp"
#include "DBNReader.hpp"
#include "TickToFIX44Adapter.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file> <dbn_file>" << std::endl;
        return 1;
    }

    const char* config_file = argv[1];
    const std::string dbn_file = argv[2];

    try {
        // Initialize components
        BasicServer server(config_file);
        DBNReader reader(dbn_file);

        // Start the FIX server
        if (!server.start(true)) {
            std::cerr << "Failed to start FIX server" << std::endl;
            return 1;
        }

        std::cout << "Server started, processing ticks..." << std::endl;

        // Process ticks
        Tick tick;
        while (reader.next(tick)) {
            // convert to message
            auto msg = TickToFIX44Adapter::tickToMDSnapshot(tick);
            server.send_message(msg);
        }

        std::cout << "Finished processing ticks" << std::endl;
        server.stop();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
