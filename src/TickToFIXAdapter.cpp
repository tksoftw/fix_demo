#include "TickToFIX44Adapter.hpp"
#include <fix8/f8includes.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"

namespace F44 = FIX8::FIX44;

F44::MarketDataSnapshotFullRefresh TickToFIX44Adapter::tickToMDSnapshot(const Tick& tick) {
    // Compute price in FIX format
    double px = static_cast<double>(tick.price) / 1e9;
    
    /*
    Build the message.
    Mostly Hardcoding REQUIRED values...
    */ 
    F44::MarketDataSnapshotFullRefresh md; // Top-level message with header & instrument fields
    md += new F44::MDReqID("SIM");
    md += new F44::Symbol("SPY");
    md += new F44::SecurityType("CS");
    
    // Declare to send exactly 1 MDEntry group
    md += new F44::NoMDEntries(1);

    // Fetch the exact GroupBase Fix8 created for tag 268
    auto* grps = md.find_group<F44::MarketDataSnapshotFullRefresh::NoMDEntries>();

    // Create the subgroup instance (one MDEntry)
    auto* entry = grps->create_group(false);

    // Populate the entry – Tag 269 (MDEntryType) MUST come first
    *entry += new F44::MDEntryType(F44::MDEntryType_TRADE);
    *entry += new F44::MDEntryPx(px);
    *entry += new F44::MDEntrySize(tick.size);

    // Add UTC date/time
    time_t now = tick.timestamp / 1000000000;
    tm* utc = gmtime(&now);
    *entry += new F44::MDEntryDate(*utc);
    *entry += new F44::MDEntryTime(*utc);

    // Other FIX fields
    *entry += new F44::MDMkt("SIM");
    *entry += new F44::TradingSessionID("SIM");

    // Attach the entry to the container for tag 268
    grps->add(entry);

    return md;
} 