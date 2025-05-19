#include <fix8/f8includes.hpp>
#include <fix8/session.hpp>
#include <fix8/sessionwrapper.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"
#include <iostream>
#include <memory>

using namespace std;
using namespace FIX8;

class MyClientSession : public Session {
    FIX44::FIX44_Router router;
public:
    MyClientSession(const F8MetaCntx& ctx, const SessionID& sid, Persister *persist=nullptr,
        Logger *logger=nullptr, Logger *plogger=nullptr) 
        : Session(ctx, sid, persist, logger, plogger) {}
    
    bool handle_application(const unsigned seqnum, const Message *&msg) override {
        cout << "Received message: " << msg->get_msgtype() << endl;
        return msg->process(router);
    }

    void state_change(const States::SessionStates before, const States::SessionStates after) override {
        cout << "State change: " << get_session_state_string(before) << " => " << get_session_state_string(after) << endl;
    }

    bool handle_logon(const unsigned seqnum, const Message *msg) override {
        cout << "Logon received" << endl;
        return Session::handle_logon(seqnum, msg);
    }
};

int main(int argc, char *argv[]) {
    // Expect an XML config file as an argument:
    // e.g. ./fix_client server.xml
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <config_file>" << endl;
        return 1;
    }

    try {
        cout << "Creating client session..." << endl;
        // Create client session using the wrapper
        auto client = make_unique<ClientSession<MyClientSession>>(
            FIX44::ctx(), argv[1], "FIX44");

        // Enable debug output
        client->session_ptr()->control() |= Session::print;

        cout << "Starting session..." << endl;
        // Start the session
        client->start(true);

        cout << "Connected to server. Press Enter to exit..." << endl;
        cin.get();

        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
} 