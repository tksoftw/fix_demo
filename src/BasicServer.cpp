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
        server = std::make_unique<FIX8::ServerSession<FIX44Session>>(FIX8::FIX44::ctx(), config_file, "FIX44");
        std::cout << "Server session created, waiting for connections..." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create server session: " << e.what() << std::endl;
        throw;
    }
}

BasicServer::~BasicServer() {
    stop();
}

bool BasicServer::start(bool wait_for_logon) {
    if (!server) {
        std::cerr << "Server not initialized" << std::endl;
        return false;
    }

    try {
        std::cout << "Waiting for client connections..." << std::endl;
        // Keep polling until a connection initiates
        while (!server->poll()) {
            std::cout << "Waiting for client connection..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Client connection detected, creating session..." << std::endl;
        // Create session instance for the client
        session = std::unique_ptr<FIX8::SessionInstanceBase>(server->create_server_instance());
        
        // Enable debug output
        session->session_ptr()->control() |= FIX8::Session::print;
        
        // Start the session
        const FIX8::ProcessModel pm(server->get_process_model(server->_ses));
        std::cout << "Starting session in " << (pm == FIX8::pm_pipeline ? "pipeline" : "threaded") << " mode" << std::endl;
        session->start(pm == FIX8::pm_pipeline, 0, 0);
        
        std::cout << "Session started successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to start session: " << e.what() << std::endl;
        return false;
    }
}

void BasicServer::stop() {
    if (session) {
        try {
            std::cout << "Stopping session..." << std::endl;
            session->stop();
            session.reset();
        }
        catch (const std::exception& e) {
            std::cerr << "Error stopping session: " << e.what() << std::endl;
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