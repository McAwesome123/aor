// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "game.h"
#include "gamewindow.h"
#include "encyclopedia.h"
#include "studyactivity.h"
#include "infomessage.h"

Game::Game()
	: m_game_id(Generators::game_id()), m_tribe_name(Generators::tribe_name()) {}

Characters &Game::characters() {
	return m_explorers;
}

Inventory &Game::inventory() {
	return m_inventory;
}

ForeignTribes &Game::tribes() {
	return m_tribes;
}

GameId Game::game_id() {
	return m_game_id;
}

QString &Game::tribe_name() {
	return m_tribe_name;
}

ItemHistory &Game::history() {
	return m_history;
}

Offer &Game::trade_offer() {
	return m_trade_offer;
}

RemoteOffer &Game::accepted_offer() {
	return m_accepted_offer;
}

bool &Game::accepting_trade() {
	return m_accepting_trade;
}

GameId &Game::trade_partner() {
	return m_trade_partner;
}

AorUInt &Game::threat() {
	return m_threat;
}

RunningActivities &Game::running_activities() {
	return m_running_activities;
}

WorldMap &Game::map() {
	return m_map;
}

ConsumableWaste &Game::forageable_waste() {
	return m_consumable_waste;
}

MineableWaste &Game::mineable_waste() {
	return m_mineable_waste;
}

WasteActionCounts &Game::waste_action_counts() {
	return m_waste_action_counts;
}

SignatureRequirements &Game::signature_requirements() {
	return m_signature_requirements;
}

StudiedItems &Game::studied_items() {
	return m_studied_items;
}

AorInt &Game::lore() {
	return m_lore;
}

Settings &Game::settings() {
	return m_settings;
}

LocationId &Game::current_location_id() {
	return m_current_location_id;
}

LocationDefinition Game::current_location() {
	return LocationDefinition::get_def(m_current_location_id);
}

LocationId &Game::next_location_id() {
	return m_next_location_id;
}

bool &Game::fast_actions() {
	return m_fast_actions;
}

bool &Game::no_exhaustion() {
	return m_no_exhaustion;
}

std::optional<Character *> Game::add_character(const QString &name, const std::multiset<Color> &heritage) {
	AorUInt max = 0;
	while (m_explorers[max].id() != NOBODY && max < MAX_EXPLORERS) {
		max++;
	}

	if (max == MAX_EXPLORERS) {
		return std::optional<Character *>();
	}

	m_explorers[max] = Character(Generators::char_id(), name, heritage);
	return std::optional<Character *>(&m_explorers[max]);
}

bool Game::add_item(const Item &item) {
	if (m_inventory.add_item(item)) {
		for (AorUInt i = 1; i != 1ull << 63; i <<= 1) {
			if (item.def()->type & i) {
				check_tutorial((ItemDomain) i);
			}
		}
		m_history.insert(item.code);
		gw()->encyclopedia()->refresh();

		return true;
	} else {
		return false;
	}
}

TimedActivity &Game::register_activity(TimedActivity &activity) {
	m_running_activities.push_back(activity);
	return m_running_activities.back();
}

void Game::check_hatch() {
	for (Item &item : inventory().items()) {
		if (item.code == Item::code_of("fennahian_egg")) {
			if (m_threat - item.instance_properties[InstanceEggFoundThreatstamp] > THREAT_TO_HATCH) {
				Heritage heritage;

				if (item.instance_properties[InstanceEggFoundFlavor]) {
					heritage.insert((Color) item.instance_properties[InstanceEggFoundFlavor]);
				} else {
					heritage = character(item.instance_properties[InstanceEggParent1]).heritage();
					for (Color c : character(item.instance_properties[InstanceEggParent2]).heritage()) {
						heritage.insert(c);
					}
				}

				auto babey = add_character(Generators::yokin_name(), heritage);
				if (babey) {
					gw()->notify(Discovery, "A new Fennahian was born!");
					call_hooks(HookJustHatched, [&](Character &) -> HookPayload { return { *babey, &item }; });
				}
				inventory().remove_item(item.id);

				gw()->refresh_ui();
			}
		}
	}
}

void Game::check_tutorial(ItemDomain domain) {
	QDrag::cancel();

	switch (domain) {
		case Consumable: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::consumable).show_message();
			break;
		} case Material: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::material).show_message();
			break;
		}
		case SmithingTool: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::smithing_tool).show_message();
			break;
		}
		case ForagingTool: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::foraging_tool).show_message();
			break;
		}
		case MiningTool: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::mining_tool).show_message();
			break;
		}
		case Artifact: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::artifact).show_message();
			break;
		}
		case Signature: {
			InfoMessage::get_message(InfoMessage::InfoMessageList::signature_item).show_message();
			break;
		}
		default: {
			break;
		}
	}
}

AorInt Game::trade_level() {
	return std::accumulate(m_trade_offer.begin(), m_trade_offer.end(), 0, [this](AorInt a, ItemId id) {
		return a + m_inventory.get_item(id).def()->properties[ItemLevel];
	});
}

AorInt Game::foreign_trade_level(GameId tribe_id) {
	auto &offer = m_tribes.at(tribe_id).offer;
	return std::accumulate(offer.begin(), offer.end(), 0, [](AorInt a, const Item &item) {
		return a + item.def()->properties[ItemLevel];
	});
}

ItemProperties Game::total_smithing_resources(CharacterId character_id) {
	ItemProperties resources;

	for (ItemId id : character(character_id).external_items()[Material]) {
		Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
			resources.map[resource_prop] += inventory().get_item(id).def()->properties[resource_prop];
		});
	}

	return resources;
}

ItemProperties Game::total_resources() {
	ItemProperties resources;

	for (const Item &item : m_inventory.items()) {
		Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
			resources.map[resource_prop] += item.def()->properties[resource_prop];
		});
	}

	return resources;
}

ItemDomain Game::intent_of(ItemId item_id) {
	if (item_id == EMPTY_ID) {
		return None;
	}

	AorUInt intent = None;

	if (std::find(m_trade_offer.begin(), m_trade_offer.end(), item_id) != m_trade_offer.end()) {
		intent |= Offering;
	}

	if (std::any_of(m_studied_items.begin(), m_studied_items.end(), [=](auto &pair) {
		return std::find(pair.second.begin(), pair.second.end(), item_id) != pair.second.end();
	})) {
		intent |= Study;
	}

	for (Character &character : m_explorers) {
		auto &materials = character.external_items()[Material];
		if (std::find(materials.begin(), materials.end(), item_id) != materials.end()) {
			intent |= Material;
		}

		auto &artifacts = character.external_items()[Artifact];
		if (std::find(artifacts.begin(), artifacts.end(), item_id) != artifacts.end()) {
			intent |= Artifact;
		}

		auto &tools = character.tools();
		if (tools[SmithingTool] == item_id || tools[ForagingTool] == item_id || tools[MiningTool] == item_id) {
			intent |= Tool;
		}

		for (ActivityId activity_id : character.activities()) {
			const auto &activity_items = gw()->game()->activity(activity_id).owned_item_ids;
			if (std::find(activity_items.begin(), activity_items.end(), item_id) != activity_items.end()) {
				intent |= character.activity().explorer_subtype();
			}
		}
	}

	return static_cast<ItemDomain>(intent);
}

std::vector<TimedActivity> Game::activities_of_type(ItemDomain type) {
	std::vector<TimedActivity> activities;

	auto &acts = m_running_activities;
	std::copy_if(acts.begin(), acts.end(), std::back_inserter(activities), [=](TimedActivity &act) {
		return act.type == type;
	});

	return activities;
}

Game::TravelCheckResult Game::can_travel(LocationId id) {
	if (id == m_current_location_id || m_next_location_id != NOWHERE) {
		return TravelCheckResult::AlreadyHere;
	}

	if (std::any_of(m_explorers.begin(), m_explorers.end(), [=](Character &c) {
		return c.activity().active;
	})) {
		return TravelCheckResult::ConcurrentAction;
	}

	// Temporarily set so that hooks can use it.
	m_next_location_id = id;

	if (!m_map.path_exists_between(m_current_location_id, id)) {
		m_next_location_id = NOWHERE;
		return TravelCheckResult::NoPath;
	}

	bool can_travel = true;
	call_hooks(HookDecideCanTravel, [&](Character &c) -> HookPayload { return { &c, &can_travel }; });

	m_next_location_id = NOWHERE;

	return can_travel ? TravelCheckResult::Ok : TravelCheckResult::InsufficientResources;
}

void Game::start_travel(LocationId id) {
	m_next_location_id = id;

	for (Character &c : m_explorers) {
		c.queue_activity(Travelling, {});
	}
}

AorInt Game::forageables_left(LocationId id) {
	return LocationDefinition::get_def(id).forageables - m_consumable_waste[id];
}

AorInt Game::forageables_left() {
	return forageables_left(m_current_location_id);
}

AorInt Game::mineables_left(LocationId id) {
	return LocationDefinition::get_def(id).mineables - m_mineable_waste[id];
}

AorInt Game::mineables_left() {
	return mineables_left(m_current_location_id);
}

AorInt Game::total_queued_character_activities(ItemDomain domain) {
	return std::accumulate(m_explorers.begin(), m_explorers.end(), 0, [=](AorInt acc, Character &c) {
		return acc + std::count_if(c.activities().begin(), c.activities().end(), [=](ActivityId id) {
			TimedActivity &t = gw()->game()->activity(id);
			return (t.type & domain) && !t.finished;
		});
	});
}

AorUInt Game::signatures_left(LocationId id) {
	AorUInt left = 0;
	LocationDefinition location = LocationDefinition::get_def(id);

	for (AorUInt i = 0; i < 9; i++) {
		ItemProperty signature_property = (ItemProperty) (LocationSignatureItem1 + i);
		if (location.properties[signature_property] == 0) {
			continue;
		}

		if (m_waste_action_counts[id] < m_signature_requirements[id][i]) {
			left++;
		}
	}

	return left;
}

Item Game::next_signature(LocationId id) {
	LocationDefinition location = LocationDefinition::get_def(id);

	for (AorUInt i = 0; i < 9; i++) {
		if (m_waste_action_counts[id] == m_signature_requirements[id][i]) {
			return Item(location.properties[(ItemProperty) (LocationSignatureItem1 + i)]);
		}
	}

	return Item();
}

Character &Game::character(CharacterId id) {
	if (id == NOBODY) {
		return Character::mock_character();
	}

	auto result = std::find_if(m_explorers.begin(), m_explorers.end(), [=](Character &c) {
		return c.id() == id;
	});

	if (result == m_explorers.end()) {
		bugcheck(CharacterByIdLookupMiss, id);
	}

	return *result;
}

TimedActivity &Game::activity(ActivityId id) {
	if (id == NO_ACTION) {
		return TimedActivity::empty_activity;
	}

	auto activity = std::find_if(m_running_activities.begin(), m_running_activities.end(), [=](TimedActivity &a) {
		return a.id == id;
	});

	if (activity != m_running_activities.end()) {
		return *activity;
	}

	bugcheck(ActivityByIdLookupMiss, id);
	return TimedActivity::empty_activity;
}

void Game::call_hooks(HookType type, const std::function<HookPayload(Character &)> &payload_provider, AorUInt int_domain) {
	for (Character &c : m_explorers) {
		if (c.id() == NOBODY || c.dead()) {
			continue;
		}

		// Don't call global effects per-character; we'll do that later.
		AorUInt char_domain = int_domain & ~Weather;
		char_domain &= ~Resident;
		c.call_hooks(type, payload_provider(c), char_domain);
	}

	call_global_hooks(type, payload_provider(m_explorers[0]), int_domain);
}

void Game::call_global_hooks(HookType type, const HookPayload &payload, AorUInt int_domain) {
	if (int_domain & Weather) {
		for (AorUInt i = 0; i < WEATHER_EFFECTS; i++) {
			ItemCode weather = gw()->game()->current_location().properties[(ItemProperty) (WeatherEffect1 + i)];
			Item::def_of(weather)->properties.call_hooks(type, payload, Item());
		}
	}

	if (int_domain & Resident) {
		for (const Item &item : gw()->game()->inventory().items()) {
			item.call_hooks(type, payload, InventoryProperty);
		}
	}
}

void Game::serialize(QIODevice *dev) const {
	Serialize::serialize(dev, m_explorers);
	Serialize::serialize(dev, m_inventory);
	Serialize::serialize(dev, m_trade_offer);
	Serialize::serialize(dev, m_accepting_trade);
	Serialize::serialize(dev, m_accepted_offer);
	Serialize::serialize(dev, m_trade_partner);
	Serialize::serialize(dev, m_game_id);
	Serialize::serialize(dev, m_tribe_name);
	Serialize::serialize(dev, m_history);
	Serialize::serialize(dev, m_threat);
	Serialize::serialize(dev, m_map);
	Serialize::serialize(dev, m_current_location_id);
	Serialize::serialize(dev, m_next_location_id);
	Serialize::serialize(dev, m_consumable_waste);
	Serialize::serialize(dev, m_mineable_waste);
	Serialize::serialize(dev, m_waste_action_counts);
	Serialize::serialize(dev, m_signature_requirements);
	Serialize::serialize(dev, m_running_activities);
	Serialize::serialize(dev, m_studied_items);
	Serialize::serialize(dev, m_lore);
	Serialize::serialize(dev, m_settings);
	InfoMessage::serialize_messages(dev);
}

void Game::deserialize(QIODevice *dev) {
	Serialize::deserialize(dev, m_explorers);
	Serialize::deserialize(dev, m_inventory);
	Serialize::deserialize(dev, m_trade_offer);
	Serialize::deserialize(dev, m_accepting_trade);
	Serialize::deserialize(dev, m_accepted_offer);
	Serialize::deserialize(dev, m_trade_partner);
	Serialize::deserialize(dev, m_game_id);
	Serialize::deserialize(dev, m_tribe_name);
	Serialize::deserialize(dev, m_history);
	Serialize::deserialize(dev, m_threat);
	Serialize::deserialize(dev, m_map);
	Serialize::deserialize(dev, m_current_location_id);
	Serialize::deserialize(dev, m_next_location_id);
	Serialize::deserialize(dev, m_consumable_waste);
	Serialize::deserialize(dev, m_mineable_waste);
	Serialize::deserialize(dev, m_waste_action_counts);
	Serialize::deserialize(dev, m_signature_requirements);
	Serialize::deserialize(dev, m_running_activities);
	Serialize::deserialize(dev, m_studied_items);
	Serialize::deserialize(dev, m_lore);
	Serialize::deserialize(dev, m_settings);
	InfoMessage::deserialize_messages(dev);

	m_tribes[NO_TRIBE];
}

Game *Game::new_game() {
	Game *g = new Game;

	g->add_character(Generators::yokin_name(), { Generators::color(), Generators::color() });
	g->add_character(Generators::yokin_name(), { Generators::color() });

	for (const LocationDefinition &location : LOCATION_DEFINITIONS) {
		for (AorUInt i = 0; i < 9; i++) {
			AorUInt requirement = (location.properties[(ItemProperty) (LocationSignatureItem1 + i)] == 0)
				? -1
				: Generators::uint() % location.mineables + location.forageables;
			g->signature_requirements()[location.id][i] = requirement;
		}
	}

	g->m_tribes[NO_TRIBE];

	return g;
}
