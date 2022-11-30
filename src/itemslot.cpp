#include "itemslot.h"
#include "items.h"
#include "qnamespace.h"

ItemSlot::ItemSlot(LKGameWindow *game)
    : QFrame(game->window().inventory_group), y(INVALID_COORD), x(INVALID_COORD), m_game_window(game)
{
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(56, 56));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setMouseTracking(true);
    setAcceptDrops(true);

    QLayout *layout {new QHBoxLayout(this)};
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_item_layout = layout;

    QLabel *label {new QLabel(this)};
    label->setMinimumSize(QSize(48, 48));
    label->setMaximumSize(QSize(48, 48));
    label->setMouseTracking(true);
    m_item_label = label;

    layout->addWidget(label);

    m_opacity_effect.setOpacity(1.0);
    m_item_label->setGraphicsEffect(&m_opacity_effect);
}

ItemSlot::ItemSlot(LKGameWindow *game, int y, int x)
    : ItemSlot(game)
{
    setObjectName(make_internal_name("inventory_slot", y, x));
    m_item_layout->setObjectName(make_internal_name("inventory_layout", y, x));
    m_item_label->setObjectName(make_internal_name("inventory_label", y, x));

    game->register_slot_name(objectName());

    this->y = y;
    this->x = x;
}

Item ItemSlot::get_item() {
    return m_game_window->game().inventory().get_item(y, x);
}

void ItemSlot::set_item(const Item &item) {
    m_game_window->game().inventory().put_item(item, y, x);
}

ItemDomain ItemSlot::type() {
    return Ordinary;
}

void ItemSlot::refresh_pixmap() {
    Item item = get_item();

    m_item_label->setPixmap(Item::pixmap_of(item));

    if (item.intent != None && type() == Ordinary) {
        m_opacity_effect.setOpacity(0.5);
    } else {
        m_opacity_effect.setOpacity(1.0);
    }
}

std::vector<ItemSlot *> ItemSlot::get_slots_of_same_type() {
    std::vector<ItemSlot *> item_slots;

    for (const QString &name : m_game_window->item_slot_names()) {
        ItemSlot *slot = findChild<ItemSlot *>(name);
        if (slot->type() == type()) {
            item_slots.push_back(slot);
        }
    }

    return item_slots;
}

void ItemSlot::drop_external_item() {
    Item item_to_drop = get_item();
    if (item_to_drop.id == EMPTY_ID) {
        return;
    }

    ItemId external_item_id = get_item().id;
    m_game_window->game().inventory().get_item_ref(external_item_id).intent = None;
    set_item(Item());

    refresh_pixmap();
    m_game_window->refresh_ui();
}

void ItemSlot::insert_inventory_slots(LKGameWindow &window) {
    for (unsigned x = 0; x < INVENTORY_COLS; x++) {
        for (unsigned y = 0; y < INVENTORY_ROWS; y++) {
            insert_inventory_slot(window, y, x);
        }
    }
}

void ItemSlot::insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x) {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout*>(window.window().inventory_slots->layout());
    inventory_grid->addWidget(new ItemSlot(&window, y, x), y, x);
}

QString ItemSlot::make_internal_name(const QString &base, int y, int x) {
    return QString("%1;%2:%3").arg(base).arg(y).arg(x);
}

void ItemSlot::enterEvent(QEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    QEnterEvent *enter_event = (QEnterEvent *) event;
    m_game_window->tooltip().move(enter_event->globalPos());
    m_game_window->tooltip().set_text(m_game_window->game().tooltip_text_for(item));
    m_game_window->tooltip().widget.item_image->setPixmap(Item::pixmap_of(item));
    m_game_window->tooltip().show();
}

void ItemSlot::mouseMoveEvent(QMouseEvent *event) {
    if (get_item().id != EMPTY_ID) {
        m_game_window->tooltip().move(event->globalPos());
    }
}

void ItemSlot::leaveEvent(QEvent *) {
    m_game_window->tooltip().hide();
}

void ItemSlot::mousePressEvent(QMouseEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    bool is_inventory_slot = type() == Ordinary;
    bool item_being_used = get_item().intent != None;

    if (event->button() == Qt::RightButton && !is_inventory_slot && !m_game_window->selected_char().activity_ongoing()) {
        drop_external_item();
        return;
    }

    if (event->button() == Qt::LeftButton
        && (!item_being_used || (!is_inventory_slot && !m_game_window->selected_char().activity_ongoing()))
    ) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;

        data->setText(objectName());
        drag->setMimeData(data);
        drag->setPixmap(Item::pixmap_of(item));

        drag->exec();
    }
}

void ItemSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    if (!data->hasFormat("text/plain")) {
        return;
    }

    event->acceptProposedAction();
}

void ItemSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = m_game_window->findChild<ItemSlot *>(source_slot_name);

    if (source_slot->type() == Ordinary) {
        // Dragging between inventory slots swaps the items in each slot.
        Item source_item = source_slot->get_item();
        Item dest_item = get_item();

        m_game_window->game().inventory().put_item(source_item, y, x);
        m_game_window->game().inventory().put_item(dest_item, source_slot->y, source_slot->x);

        source_slot->refresh_pixmap();
    } else {
        // Dragging from an external slot to an inventory slot clears the external
        // slot and returns the intent of the original item to NoIntent.
        source_slot->drop_external_item();
    }

    m_game_window->refresh_ui();
}