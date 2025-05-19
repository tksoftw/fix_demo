#include "BasicServer.hpp"
#include "DBNReader.hpp"
#include "TickToFIX44Adapter.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <config_file> <dbn_file>" << endl;
        return 1;
    }

    const char* config_file = argv[1];
    const string dbn_file = argv[2];

    try {
        // Initialize components
        BasicServer server(config_file);
        DBNReader reader(dbn_file);

        // Start the FIX server
        if (!server.start(true)) {
            cerr << "Failed to start FIX server" << endl;
            return 1;
        }

        cout << "Server started, processing ticks..." << endl;

        // Process ticks
        Tick tick;
        while (reader.next(tick)) {
            // convert to message
            auto msg = TickToFIX44Adapter::tickToMDSnapshot(tick);
            server.send_message(msg);
        }

        cout << "Finished processing ticks" << endl;
        server.stop();
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
