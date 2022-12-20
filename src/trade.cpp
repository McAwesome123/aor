#include "trade.h"
#include "gamewindow.h"

DoughbyteConnection::DoughbyteConnection()
{
    m_socket.connectToHost("doughbyte.com", 10241);

    QObject::connect(&m_socket, &QTcpSocket::connected, [=]() {
        IO::write_long(&m_socket, gw()->game().game_id());
        IO::write_string(&m_socket, gw()->game().tribe_name());
        availability_changed(true);
        want_game_state();
        qDebug("trade server connected");
        m_is_connected = true;
    });

    QObject::connect(&m_socket, &QTcpSocket::disconnected, [=]() {
        qDebug("disconnected from trade server, oh no!!");
        m_is_connected = false;
    });

    QObject::connect(&m_socket, &QTcpSocket::readyRead, [=]() {
        try {
            while (true) {
                m_socket.startTransaction();
                char message_code = IO::read_byte(&m_socket);
                switch (message_code) {
                    case MT_OFFERCHANGED:             { update_offers(); break; }
                    case MT_AGREEMENTCHANGED:         { update_agreements(); break; }
                    case MT_TRIBEAVAILABILITYCHANGED: { update_availability(); break; }
                    case MT_WANTGAMESTATE:            { send_info(); break; }
                    case MT_MYINFO:                   { update_all(); break; }
                    case MT_EXECUTETRADE:             { execute_trade(); break; }
                    default: {
                        qWarning("server sent an unknown request (%x) (%c)", message_code, message_code);
                    }
                }
                m_socket.commitTransaction();
            }
        } catch (IO::RetryException) { }
    });
}

bool DoughbyteConnection::is_connected() {
    return m_is_connected;
}

void DoughbyteConnection::offer_changed(const Item &item, int n) {
    IO::write_byte(&m_socket, MT_OFFERCHANGED);

    IO::write_short(&m_socket, item.code);
    IO::write_byte(&m_socket, item.uses_left);
    IO::write_short(&m_socket, n);
}

void DoughbyteConnection::agreement_changed(GameId partner_id, bool accepting) {
    IO::write_byte(&m_socket, MT_AGREEMENTCHANGED);

    IO::write_long(&m_socket, partner_id);
    IO::write_bool(&m_socket, accepting);
}

void DoughbyteConnection::availability_changed(bool available) {
    IO::write_byte(&m_socket, MT_TRIBEAVAILABILITYCHANGED);

    IO::write_bool(&m_socket, available);
}

void DoughbyteConnection::send_info() {
    GameId to = IO::read_long(&m_socket);

    Inventory &inventory = gw()->game().inventory();
    send_info(
        to,
        {
            inventory.get_item(gw()->game().trade_offer()[0]),
            inventory.get_item(gw()->game().trade_offer()[1]),
            inventory.get_item(gw()->game().trade_offer()[2]),
            inventory.get_item(gw()->game().trade_offer()[3]),
            inventory.get_item(gw()->game().trade_offer()[4]),
        },
        gw()->game().accepting_trade()
    );
}

void DoughbyteConnection::send_info(GameId to, const std::array<Item, TRADE_SLOTS> &items, bool accepting) {
    IO::write_byte(&m_socket, MT_MYINFO);

    IO::write_long(&m_socket, to);
    IO::write_string(&m_socket, gw()->game().tribe_name());
    for (int i = 0; i < TRADE_SLOTS; i++) {
        IO::write_short(&m_socket, items[i].code);
        IO::write_byte(&m_socket, items[i].uses_left);
    }
    IO::write_bool(&m_socket, accepting);
}

void DoughbyteConnection::want_game_state() {
    IO::write_byte(&m_socket, MT_WANTGAMESTATE);
}

void DoughbyteConnection::execute_trade() {
    gw()->selected_char().queue_activity(Trading, {});

    gw()->game().trade_partner() = gw()->selected_tribe_id();
    gw()->game().accepted_offer() = gw()->game().tribes().at(gw()->selected_tribe_id()).offer;

    gw()->refresh_ui();
}

void DoughbyteConnection::update_offers() {
    quint64 tribe_id = IO::read_long(&m_socket);
    quint16 item_code = IO::read_short(&m_socket);
    char item_uses = IO::read_byte(&m_socket);
    quint16 index = IO::read_short(&m_socket);

    update_offers(tribe_id, item_code, item_uses, index);
}

void DoughbyteConnection::update_offers(GameId tribe_id, ItemCode code, char uses, int n) {
    Item item(code);
    item.uses_left = uses;
    gw()->game().tribes()[tribe_id].offer[n] = item;

    // If we witnessed the remote offer change, stop accepting
    if (tribe_id == gw()->window().trade_partner_combobox->currentData().toULongLong()) {
        gw()->game().accepting_trade() = false;
        agreement_changed(tribe_id, false);
        gw()->refresh_ui_buttons();
    }

    gw()->refresh_slots();
    gw()->refresh_trade_ui();
}

void DoughbyteConnection::update_agreements() {
    quint64 tribe_id = IO::read_long(&m_socket);
    bool accepting = IO::read_bool(&m_socket);

    update_agreements(tribe_id, accepting);
}

void DoughbyteConnection::update_agreements(GameId tribe_id, bool agrees) {
    gw()->game().tribes()[tribe_id].remote_accepted = agrees;
    gw()->refresh_trade_ui();
}

void DoughbyteConnection::update_availability() {
    quint64 tribe_id = IO::read_long(&m_socket);
    QString tribe_name = IO::read_string(&m_socket);
    bool available = IO::read_bool(&m_socket);

    update_availability(tribe_id, tribe_name, available);
}

void DoughbyteConnection::update_availability(GameId tribe_id, const QString &tribe_name, bool available) {
    QComboBox *partner_box = gw()->window().trade_partner_combobox;
    int index = partner_box->findData(QVariant::fromValue(tribe_id));

    if (available && index == -1) {
        partner_box->insertItem(0, tribe_name, QVariant::fromValue(tribe_id));
    } else if (!available && index != -1) {
        partner_box->removeItem(index);
    }

    gw()->refresh_ui_buttons();
}

void DoughbyteConnection::update_all() {
    quint64 tribe_id = IO::read_long(&m_socket);
    QString tribe_name = IO::read_string(&m_socket);
    quint16 item_code_1 = IO::read_short(&m_socket);
    char item_uses_1 = IO::read_byte(&m_socket);
    quint16 item_code_2 = IO::read_short(&m_socket);
    char item_uses_2 = IO::read_byte(&m_socket);
    quint16 item_code_3 = IO::read_short(&m_socket);
    char item_uses_3 = IO::read_byte(&m_socket);
    quint16 item_code_4 = IO::read_short(&m_socket);
    char item_uses_4 = IO::read_byte(&m_socket);
    quint16 item_code_5 = IO::read_short(&m_socket);
    char item_uses_5 = IO::read_byte(&m_socket);
    bool accepting = IO::read_bool(&m_socket);

    update_offers(tribe_id, item_code_1, item_uses_1, 0);
    update_offers(tribe_id, item_code_2, item_uses_2, 1);
    update_offers(tribe_id, item_code_3, item_uses_3, 2);
    update_offers(tribe_id, item_code_4, item_uses_4, 3);
    update_offers(tribe_id, item_code_5, item_uses_5, 4);
    update_agreements(tribe_id, accepting);
    update_availability(tribe_id, tribe_name, true);
}

void DoughbyteConnection::notify_trade(GameId tribe_id) {
    IO::write_byte(&m_socket, MT_EXECUTETRADE);
    IO::write_long(&m_socket, tribe_id);
}

