#include "DBNReader.hpp"
#include <databento/record.hpp>

bool DBNReader::next(Tick& t) {
    const databento::Record* record = reader.NextRecord();
    if (!record) return false;
    
    if (record->Holds<databento::TradeMsg>()) {
        const auto& msg = record->Get<databento::TradeMsg>();
        t.timestamp = msg.ts_recv.time_since_epoch().count();
        t.price = msg.price;
        t.size = msg.size;
        return true;
    }
    return false;
}
