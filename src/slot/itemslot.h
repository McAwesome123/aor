#pragma once

#include "slot.h"

class ItemSlot : public Slot {
public:
    ItemSlot() = default;

    virtual QPixmap pixmap() override;
    virtual bool is_draggable() override;
    virtual PayloadVariant user_drop_data() override;

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

protected:
    virtual Item my_item();
    virtual Item get_item(const SlotMessage &message);
    Inventory &inventory();
};
