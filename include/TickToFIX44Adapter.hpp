#pragma once
#include "Tick.hpp"
#include <fix8/f8includes.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"

namespace F44 = FIX8::FIX44;

class TickToFIX44Adapter {
public:
    static F44::MarketDataSnapshotFullRefresh tickToMDSnapshot(const Tick& tick);
}; 