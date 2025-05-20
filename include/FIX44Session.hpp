#pragma once
#include <fix8/f8includes.hpp>
#include <fix8/session.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"

namespace F44 = FIX8::FIX44;

class FIX44Session : public FIX8::Session {
protected:
    F44::FIX44_Router router;

public:
    FIX44Session(const FIX8::F8MetaCntx& ctx, const FIX8::sender_comp_id& sci, FIX8::Persister *persist=nullptr,
        FIX8::Logger *logger=nullptr, FIX8::Logger *plogger=nullptr) 
        : FIX8::Session(ctx, sci, persist, logger, plogger) {}
    
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg) override {
        return msg->process(router);
    }
}; 