#pragma once
#include "Tick.hpp"
#include <fix8/f8includes.hpp>
#include <fix8/f8types.hpp>
#include <fix8/session.hpp>
#include <fix8/sessionwrapper.hpp>
#include <chrono>
#include <memory>
#include <iostream>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"

using namespace std;
using namespace FIX8;

class ConcreteSession : public Session {
    FIX44::FIX44_Router router;
public:
    ConcreteSession(const F8MetaCntx& ctx, const sender_comp_id& sci, Persister *persist=nullptr,
        Logger *logger=nullptr, Logger *plogger=nullptr) 
        : Session(ctx, sci, persist, logger, plogger) {}
    
    bool handle_application(const unsigned seqnum, const Message *&msg) override {
        return msg->process(router);
    }
};

class Replayer {
    unique_ptr<ServerSession<ConcreteSession>> server;
    unique_ptr<SessionInstanceBase> session;
public:
    explicit Replayer(const char* config_file);
    ~Replayer();

    bool start(bool wait_for_logon = true);
    void stop();
    void send_tick(const Tick& t);

    static constexpr std::chrono::milliseconds throttle{5};
};
