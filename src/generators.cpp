#include "generators.h"

QRandomGenerator *Generators::rng() {
    static QRandomGenerator *rng = new QRandomGenerator(QTime::currentTime().msec());
    return rng;
}

QString Generators::yokin_name(int length) {
    const static std::vector<Cluster> consonant_clusters = {
        {"N", 1.0},
        {"M", 1.4},
        {"G", 1.0},
        {"K", 1.0},
        {"L", 1.0},
        {"LL", 1.0},
        {"LH", 1.0},
        {"V", 1.0},
        {"R", 1.0},
        {"T", 1.0},
        {"D", 1.0},
        {"SH", 1.0},
        {"ZS", 1.0},
        {"Z", 1.0},
        {"S", 1.0},
        {"J", 0.5},
        {"DZ", 0.8},
        {"XX", 0.2},
    };

    const static std::vector<Cluster> vowel_clusters = {
        {"A", 0.8},
        {"E", 0.8},
        {"I", 0.3},
        {"AE", 1.5},
        {"EA", 1.0},
        {"AI", 1.0},
        {"Y", 0.5},
        {"O", 0.2},
        {"II", 0.2},
    };

    bool consonant = rng()->generate() % 2 == 0;
    QString name;

    while (name.length() < length) {
        name += consonant
            ? sample_with_weights(consonant_clusters)
            : sample_with_weights(vowel_clusters);
        consonant = !consonant;
    }

    return name.left(length);
}

QString Generators::tribe_name() {
    const static std::vector<Cluster> LOCATION_ADJECTIVES = {
        {"Clear", 1.0},
        {"Warm", 1.0},
        {"Sandy", 1.0},
        {"Dark", 1.0},
        {"Murk", 1.0},
        {"Ashen", 1.0},
        {"Painted", 1.0},
        {"Striped", 1.0},
        {"Dry", 1.0},
        {"Wet", 1.0},
        {"Frost", 1.0},
        {"Crystal", 1.0},
        {"Rock", 1.0},
        {"Lucent", 1.0},
        {"Shining", 1.0},
        {"High", 1.0},
        {"Lush", 1.0},
        {"Vivid", 1.0},
        {"Under", 1.0},
    };

    const static std::vector<Cluster> LOCATION_NOUNS = {
        {"stone", 1.0},
        {"garden", 1.0},
        {"spines", 1.0},
        {"spires", 1.0},
        {"tail", 1.0},
        {"foot", 1.0},
        {"head", 1.0},
        {"mesa", 1.0},
        {"woods", 1.0},
        {"plains", 1.0},
        {"fields", 1.0},
        {"cliffs", 1.0},
        {"thorns", 1.0},
        {"canyons", 1.0},
        {"chasms", 1.0},
        {"hills", 1.0},
        {"crater", 1.0},
        {"caves", 1.0},
        {"caverns", 1.0},
        {"lake", 1.0}
    };

    return sample_with_weights(LOCATION_ADJECTIVES) + sample_with_weights(LOCATION_NOUNS);
}

std::vector<Item> Generators::base_items(const std::vector<Item> &inputs, const Item &tool, ItemDomain action) {
    std::vector<Item> outputs;
    const ItemProperties &tool_properties = tool.def()->properties;

    switch (action) {
        case Smithing: {
            if (inputs.size() != 3) {
                qFatal("Tried to smith without exactly 3 inputs (%zu)", inputs.size());
            }

            std::multiset<ItemCode> input_codes {
                inputs[0].code,
                inputs[1].code,
                inputs[2].code,
            };
            std::multiset<ItemCode> combo_codes {
                tool_properties[ToolComboIngredient1],
                tool_properties[ToolComboIngredient2],
                tool_properties[ToolComboIngredient3],
            };

            if (input_codes == combo_codes) {
                outputs.emplace_back(tool_properties[ToolComboResult]);
            } else {
                std::vector<std::pair<ItemCode, double>> clusters;
                for (const Item &item : inputs) {
                    clusters.emplace_back(item.def()->properties[MaterialForges], 1.0);
                }
                outputs.emplace_back(Generators::sample_with_weights(clusters));
            }

            break;
        }
        case Foraging:
        case Mining: {
            if (tool.id == EMPTY_ID) {
                if (action == Foraging) {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("globfruit"), 1 },
                            { Item("byteberry"), 1 }
                        })
                    };
                } else {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("obsilicon"), 1 },
                            { Item("oolite"), 1 }
                        })
                    };
                }
            }

            std::vector<std::pair<Item, double>> possible_discoveries;
            for (int i = (int) ToolCanDiscover1; i <= (int) ToolCanDiscover9; i++) {
                if (tool_properties[(ItemProperty) i] != 0) {
                    possible_discoveries.emplace_back(
                        tool_properties[(ItemProperty) i],
                        tool_properties[(ItemProperty) (i + 9)]
                    );
                }
            }

            outputs.emplace_back(Generators::sample_with_weights(possible_discoveries));

            break;
        }
        case Trading: {
            // Trading generates no base items; must be handled elsewhere
            return {};
        }
        default: {
            qWarning("Tried to generate items with unknown action domain (%d)", action);
        }
    }

    return outputs;
}

ItemId Generators::item_id() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    return ((milliseconds & 0xffffffff) + ((std::uint64_t) Generators::rng()->generate() << 32)) & 0x7fffffffffffffff;
}

Color Generators::color() {
    return sample_with_weights<Color>({
        { Banana, 1 },
        { Orange, 1 },
        { Cherry, 1, },
        { Blueberry, 1 },
        { BlueRaspberry, 1 },
        { Plum, 1 },
        { Cucumber, 1 },
        { Lime, 1 },
    });
}

GameId Generators::game_id() {
    return item_id();
}
