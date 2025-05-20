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