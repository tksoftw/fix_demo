#pragma once
#include <fix8/f8includes.hpp>
#include <fix8/session.hpp>
#include <fix8/sessionwrapper.hpp>
#include <memory>
#include <string>
#include "FIX44Session.hpp"

class Replayer {
    std::unique_ptr<FIX8::ServerSession<FIX44Session>> server;
    std::unique_ptr<FIX8::SessionInstanceBase> session;
public:
    explicit Replayer(const char* config_file);
    ~Replayer();

    bool start();
    void stop();
    void replay(const std::string& filename);
};