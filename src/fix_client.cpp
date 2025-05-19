#include <fix8/f8includes.hpp>
#include <fix8/session.hpp>
#include <fix8/sessionwrapper.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"
#include <iostream>
#include <memory>

namespace F44 = FIX8::FIX44;

class MyClientSession : public FIX8::Session {
    F44::FIX44_Router router;
public:
    MyClientSession(const FIX8::F8MetaCntx& ctx, const FIX8::SessionID& sid, FIX8::Persister *persist=nullptr,
        FIX8::Logger *logger=nullptr, FIX8::Logger *plogger=nullptr) 
        : FIX8::Session(ctx, sid, persist, logger, plogger) {}
    
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg) override {
        std::cout << "Received message: " << msg->get_msgtype() << std::endl;
        return msg->process(router);
    }

    void state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after) override {
        std::cout << "State change: " << get_session_state_string(before) << " => " << get_session_state_string(after) << std::endl;
    }

    bool handle_logon(const unsigned seqnum, const FIX8::Message *msg) override {
        std::cout << "Logon received" << std::endl;
        return FIX8::Session::handle_logon(seqnum, msg);
    }
};

int main(int argc, char *argv[]) {
    // Expect an XML config file as an argument:
    // e.g. ./fix_client server.xml
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    try {
        std::cout << "Creating client session..." << std::endl;
        // Create client session using the wrapper
        auto client = std::make_unique<FIX8::ClientSession<MyClientSession>>(
            F44::ctx(), argv[1], "FIX44");

        // Enable debug output
        client->session_ptr()->control() |= FIX8::Session::print;

        std::cout << "Starting session..." << std::endl;
        // Start the session
        client->start(true);

        std::cout << "Connected to server. Press Enter to exit..." << std::endl;
        std::cin.get();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 