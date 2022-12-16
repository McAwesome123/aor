#include "actions.h"
#include "externalslot.h"
#include "gamewindow.h"

CharacterActivity::CharacterActivity(CharacterId id, ItemDomain action, qint64 ms_total, qint64 ms_left)
    : m_action(action),
      m_ms_left(ms_total),
      m_ms_total(ms_left),
      m_char_id(id)
{
    if (m_action != None) {
        m_timer_id = gw()->startTimer(ACTIVITY_TICK_RATE_MS);
    }
}

ItemDomain &CharacterActivity::action() {
    return m_action;
}

qint64 &CharacterActivity::ms_left() {
    return m_ms_left;
}

qint64 &CharacterActivity::ms_total() {
    return m_ms_total;
}

int CharacterActivity::timer_id() {
    return m_timer_id;
}

double CharacterActivity::percent_complete() {
    if (m_action == None) {
        return 0.0;
    }

    return ((double) (m_ms_total - m_ms_left) / m_ms_total);
}

bool CharacterActivity::ongoing() {
    return m_action != None && m_ms_left > 0;
}

void CharacterActivity::progress(qint64 ms) {
    m_ms_left -= ms;

    if (m_ms_left <= 0) {
        complete();
    }
}

void CharacterActivity::complete() {
    gw()->killTimer(m_timer_id);

    exhaust_character();
    give_bonuses();
    give_injuries();
    std::vector<Item> items = products();
    exhaust_reagents();
    give(items);

    m_action = None;
    m_ms_total = 0;

    gw()->refresh_ui();
    gw()->game().actions_done()++;

    gw()->game().check_hatch();
}

std::vector<Item> CharacterActivity::products() {
    Character &character = gw()->game().characters().at(m_char_id);
    switch (m_action) {
        case Smithing: {
            ItemCode smithing_result = character.smithing_result();

            if (smithing_result != 0) {
                Item result = Item(smithing_result);

                int heritage_use_boost = character.heritage_properties()[HeritageSmithProductUsageBoost];
                result.uses_left += heritage_use_boost;

                return { result };
            } else {
                return {};
            }
        }
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(character.tool_id(m_action));
            const ItemProperties &tool_props = tool.def()->properties;

            if (tool.id == EMPTY_ID) {
                if (m_action == Foraging) {
                    return { Item(Generators::sample_with_weights<ItemCode>({ { CT_CONSUMABLE | 0, 1 }, { CT_CONSUMABLE | 1, 1 } })) };
                } else if (m_action == Mining) {
                    return { Item(Generators::sample_with_weights<ItemCode>({ { CT_MATERIAL | 0, 1 }, { CT_MATERIAL | 1, 1 } })) };
                }
            }

            std::vector<std::pair<ItemCode, double>> weighted_discoverables;
            Item::for_each_tool_discover([&](ItemProperty product_prop, ItemProperty weight_prop) {
                if (product_prop != 0) {
                    weighted_discoverables.push_back({ tool_props[product_prop], tool_props[weight_prop] });
                }
            });

            return { Item(Generators::sample_with_weights<ItemCode>(weighted_discoverables)) };
        }
        case Eating:
        case Defiling: {
            return {};
        }
        case Coupling: {
            auto &characters = gw()->game().characters();

            // Coupling actions always end in pairs.
            // To avoid making two eggs, the first one that finishes their
            // coupling action resets their partner's partner member.
            // (That happens later in this very function.)
            // Here, we check to see if our partner has already made an egg.
            auto partner = std::find_if(begin(characters), end(characters), [&](Character &other) {
                return other.partner() == character.id();
            });

            if (partner == end(characters)) {
                return {};
            }

            std::vector<Item> egg = { Item::make_egg(m_char_id, partner->id()) };

            partner->partner() = NOBODY;
            character.partner() = NOBODY;

            return egg;
        }
        default: {
            qFatal("Tried to get products for unknown domain (%d)", m_action);
            return {};
        }
    }
}

void CharacterActivity::exhaust_reagents() {
    Character &character = gw()->game().characters().at(m_char_id);

    if (m_action == Smithing) {
        for (ItemId id : character.external_items().at(Material)) {
            exhaust_item(id);
        }
    } else if (m_action == Trading) {
        for (ItemId id : character.external_items().at(Offering)) {
            gw()->game().inventory().remove_item(id);
        }
    } else if (m_action == Eating || m_action == Defiling) {
        for (const Item &item : gw()->game().inventory().items_of_intent(m_char_id, m_action)) {
            exhaust_item(item.id);
        }
    }

    exhaust_item(character.tool_id(m_action));
}

void CharacterActivity::exhaust_character() {
    Character &character = gw()->game().characters().at(m_char_id);

    Item tool = gw()->game().inventory().get_item(character.tool_id(m_action));

    if (m_action != Eating && m_action != Defiling) {
        character.add_energy(-tool.def()->properties[ToolEnergyCost]);
        character.add_morale(-character.base_morale_cost());
    }

    for (Item &effect : character.effects()) {
        if (effect.id == EMPTY_ID) {
            continue;
        }

        effect.uses_left -= 1;
        if (effect.uses_left == 0) {
            effect = Item();
        }
    }
}

void CharacterActivity::exhaust_item(ItemId id) {
    Character &character = gw()->game().characters().at(m_char_id);

    if (id != EMPTY_ID) {
        Item &item = gw()->game().inventory().get_item_ref(id);
        if (item.uses_left > 0) {
            item.uses_left -= 1;
            if (item.uses_left == 0) {
                if (item.code & CT_TOOL) {
                    gw()->notify(Warning, QString("%1's %2 broke!")
                        .arg(gw()->game().characters().at(m_char_id).name())
                        .arg(item.def()->display_name)
                    );

                    character.tools().at(m_action) = EMPTY_ID;
                }

                gw()->game().inventory().remove_item(id);
            } else {
                if (!(item.intent & Tool)) {
                    item.intent = None;
                }
            }
        }

        if (m_action == Smithing) {
            for (ItemId &mid : character.external_items().at(Material)) {
                if (id == mid) {
                    mid = EMPTY_ID;
                }
            }
        } else if (m_action == Trading) {
            for (ItemId &oid : character.external_items().at(Offering)) {
                if (id == oid) {
                    oid = EMPTY_ID;
                }
            }
        }
    }
}

void CharacterActivity::give(const std::vector<Item> &items) {
    for (const Item &item : items) {
        if (!gw()->game().add_item(item)) {
            gw()->notify(Warning, QString("%1 discovered a(n) %2, but the inventory was too full to accept it!")
                .arg(gw()->game().characters().at(m_char_id).name())
                .arg(item.def()->display_name)
            );
        } else {
            gw()->notify(Discovery, QString("%1 discovered a(n) %2!")
                .arg(gw()->game().characters().at(m_char_id).name())
                .arg(item.def()->display_name)
            );
        }
    }
}

void CharacterActivity::give_bonuses() {
    Character &character = gw()->game().characters().at(m_char_id);
    if (m_action == Eating) {
        for (const Item &item : gw()->game().inventory().items_of_intent(m_char_id, Eating)) {
            const ItemProperties &props = item.def()->properties;

            character.add_energy(props[ConsumableEnergyBoost]);
            character.add_morale(props[ConsumableMoraleBoost]);

            int heritage_energy_boost = character.heritage_properties()[HeritageConsumableEnergyBoost];
            character.add_energy(heritage_energy_boost);

            for (int i = 0; i < props[ConsumableClearsNumEffects]; i++) {
                character.clear_last_effect();
            }

            if (props[ConsumableMakesCouplable]) {
                character.can_couple() = true;
            }
        }
    } else if (m_action == Defiling) {
        for (const Item &item : gw()->game().inventory().items_of_intent(m_char_id, Defiling)) {
            character.add_morale(item.def()->item_level * 25);
        }
    }
}

void CharacterActivity::give_injuries() {
    Character &character = gw()->game().characters().at(m_char_id);

    int injury_chance = 5 + (gw()->game().inventory().get_item(character.tool_id(m_action)).def()->item_level * 6);
    int injury_dampen = character.heritage_properties()[HeritageInjuryResilience];
    injury_chance -= injury_dampen;

    if (!Generators::percent_chance(100)) {
        return;
    }

    std::vector<std::pair<ItemCode, double>> possible_weighted_injuries;
    for (const ItemDefinition &def : ITEM_DEFINITIONS) {
        if (!(def.type & Effect)) {
            continue;
        }

        switch (m_action) {
            case Smithing: { if (!def.properties[InjurySmithing]) { continue; } break; }
            case Foraging: { if (!def.properties[InjuryForaging]) { continue; } break; }
            case Mining: { if (!def.properties[InjuryMining]) { continue; } break; }
            case Eating: { if (!def.properties[InjuryEating]) { continue; } break; }
            case Defiling: { if (!def.properties[InjuryDefiling]) { continue; } break; }
            case Trading: { if (!def.properties[InjuryTrading]) { continue; } break; }
            case Coupling: { continue; } // COCK INJURED
            default: { qFatal("Tried to give injuries for unknown action domain (%d)", m_action); }
        }

        possible_weighted_injuries.push_back({ def.code, 1 });
    }

    if (possible_weighted_injuries.empty()) {
        return;
    }

    character.push_effect(Item(Generators::sample_with_weights(possible_weighted_injuries)));
    gw()->notify(Warning, QString("%1 suffered an injury...").arg(character.name()));
}
