// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "tradeslot.h"

TradeSlot::TradeSlot(size_t n)
    : ExternalSlot(n) {}

void TradeSlot::install() {
    gw()->window().trade_slot_layout->addWidget(this);
}

bool TradeSlot::will_accept_drop(const SlotMessage &message) {
    if (!ItemSlot::will_accept_drop(message)) {
        return false;
    }

    return (gw()->connection().is_connected())
        && (gw()->game()->trade_partner() == NOBODY)
        && !(get_item(message).def()->type & Untradeable);
}

void TradeSlot::accept_message(const SlotMessage &message) {
    ExternalSlot::accept_message(message);

    switch (message.type) {
        case SlotSetItem: {
            gw()->connection().offer_changed(my_item(), (int)m_n);
            break;
        } case SlotForgetItem: {
            gw()->connection().offer_changed(Item(), (int)m_n);
            break;
        } default: {}
    }
}

ItemId &TradeSlot::my_item_id() {
    return gw()->game()->trade_offer()[m_n];
}
