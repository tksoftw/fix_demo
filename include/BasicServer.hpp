#pragma once
#include "Tick.hpp"
#include <fix8/f8includes.hpp>
#include <fix8/f8types.hpp>
#include <fix8/session.hpp>
#include <fix8/sessionwrapper.hpp>
#include <chrono>
#include <memory>
#include <iostream>
#include "FIX44Session.hpp"

namespace F44 = FIX8::FIX44;

class ConcreteSession : public FIX8::Session {
    F44::FIX44_Router router;
public:
    ConcreteSession(const FIX8::F8MetaCntx& ctx, const FIX8::sender_comp_id& sci, FIX8::Persister *persist=nullptr,
        FIX8::Logger *logger=nullptr, FIX8::Logger *plogger=nullptr) 
        : FIX8::Session(ctx, sci, persist, logger, plogger) {}
    
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg) override {
        return msg->process(router);
    }
};

class BasicServer {
    std::unique_ptr<FIX8::ServerSession<FIX44Session>> server;
    std::unique_ptr<FIX8::SessionInstanceBase> session;
public:
    explicit BasicServer(const char* config_file);
    ~BasicServer();

    bool start(bool wait_for_logon = true);
    void stop();
    void send_message(FIX8::Message& msg);
};