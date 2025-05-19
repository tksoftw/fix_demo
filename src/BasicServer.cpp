#include "BasicServer.hpp"
#include <fix8/f8includes.hpp>
#include <fix8/message.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

BasicServer::BasicServer(const char* config_file) {
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

BasicServer::~BasicServer() {
    stop();
}

bool BasicServer::start(bool wait_for_logon) {
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

void BasicServer::stop() {
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

void BasicServer::send_message(FIX8::Message& msg)
{
    // guard against missing session
    if (!session || !session->session_ptr() || session->session_ptr()->is_shutdown()) {
        std::cerr << "Cannot send message - session not available or shutdown\n";
        return;
    }

    try {
        // Send the message
        session->session_ptr()->send(msg);
    }
    catch (std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << "\n";
    }
}