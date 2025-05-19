#include "Replayer.hpp"
#include <fix8/f8includes.hpp>
#include <fix8/message.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

Replayer::Replayer(const char* config_file) {
    try {
        // Create server session that will handle connections
        server = make_unique<ServerSession<ConcreteSession>>(FIX8::FIX44::ctx(), config_file, "FIX44");
        cout << "Server session created, waiting for connections..." << endl;
    }
    catch (const exception& e) {
        cerr << "Failed to create server session: " << e.what() << endl;
        throw;
    }
}

Replayer::~Replayer() {
    stop();
}

bool Replayer::start(bool wait_for_logon) {
    if (!server) {
        cerr << "Server not initialized" << endl;
        return false;
    }

    try {
        cout << "Waiting for client connections..." << endl;
        // Keep polling until a connection initiates
        while (!server->poll()) {
            cout << "Waiting for client connection..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }

        cout << "Client connection detected, creating session..." << endl;
        // Create session instance for the client
        session = unique_ptr<SessionInstanceBase>(server->create_server_instance());
        
        // Enable debug output
        session->session_ptr()->control() |= Session::print;
        
        // Start the session
        const ProcessModel pm(server->get_process_model(server->_ses));
        cout << "Starting session in " << (pm == pm_pipeline ? "pipeline" : "threaded") << " mode" << endl;
        session->start(pm == pm_pipeline, 0, 0);
        
        cout << "Session started successfully" << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "Failed to start session: " << e.what() << endl;
        return false;
    }
}

void Replayer::stop() {
    if (session) {
        try {
            cout << "Stopping session..." << endl;
            session->stop();
            session.reset();
        }
        catch (const exception& e) {
            cerr << "Error stopping session: " << e.what() << endl;
        }
    }
}

void Replayer::send_tick(const Tick& t)
{
    // guard against missing session
    if (!session || !session->session_ptr() || session->session_ptr()->is_shutdown()) {
        std::cerr << "Cannot send tick – session not available or shutdown\n";
        return;
    }

    try {
        // Compute price in FIX format
        double px = static_cast<double>(t.price) / 1e9;
        
        /*
        Build the message.
        Mostly Hardcoding REQUIRED values...
        */ 
        FIX44::MarketDataSnapshotFullRefresh md; // Top-level message with header & instrument fields
        md += new FIX44::MDReqID("SIM");
        md += new FIX44::Symbol("AAPL");
        md += new FIX44::SecurityType("CS");
        
        // Declare to send exactly 1 MDEntry group
        md += new FIX44::NoMDEntries(1);

        // Fetch the exact GroupBase Fix8 created for tag 268
        auto* grps = md.find_group
          <FIX44::MarketDataSnapshotFullRefresh::NoMDEntries>();

        // Create the subgroup instance (one MDEntry)
        auto* entry = grps->create_group(false);

        // Populate the entry – Tag 269 (MDEntryType) MUST come first
        *entry += new FIX44::MDEntryType(FIX44::MDEntryType_TRADE);
        *entry += new FIX44::MDEntryPx(px);
        *entry += new FIX44::MDEntrySize(t.size);

        // Add UTC date/time
        time_t now = t.timestamp / 1000000000;
        tm* utc = gmtime(&now);
        *entry += new FIX44::MDEntryDate(*utc);
        *entry += new FIX44::MDEntryTime(*utc);

        // Other FIX fields
        *entry += new FIX44::MDMkt("SIM");
        *entry += new FIX44::TradingSessionID("SIM");

        // Attach the entry to the container for tag 268
        grps->add(entry);

        // Send
        std::cout << "Sending market data: price=" << px
                  << ", size=" << t.size << "\n";
        session->session_ptr()->send(md);
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending tick: " << e.what() << "\n";
    }
}

