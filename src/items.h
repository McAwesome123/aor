#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <set>

#include <QtGlobal>
#include <QFile>
#include <QPixmap>

#include "itemproperties.h"
#include "definitionmark.h"
#include "serialize.h"
#include "types.h"

#define USES
#define LEVEL

const static AorUInt SMITHING_SLOTS = 5;
const static AorUInt SMITHING_SLOTS_PER_ROW = 5;

const static AorUInt TRADE_SLOTS = 5;

const static AorUInt ARTIFACT_SLOTS = 4;

const static AorUInt EFFECT_SLOTS = 4;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = ~0ull;
const static ItemCode EMPTY_CODE = 0;
const static ItemCode INVALID_CODE = ~0ull;
const static CharacterId NOBODY = 0;
const static GameId NO_TRIBE = 0;
const static ActivityId NO_ACTION = 0;

const static AorUInt THREAT_TO_HATCH = 25 * 5;

enum ItemDomain : AorUInt {
    Ordinary        = 0,      None = 0,
    Consumable      = 1 << 0, Eating   = 1 << 0,
    Material        = 1 << 1,
    SmithingTool    = 1 << 2, Smithing = 1 << 2,
    ForagingTool    = 1 << 3, Foraging = 1 << 3,
    MiningTool      = 1 << 4, Mining   = 1 << 4,
    Tool            = SmithingTool | ForagingTool | MiningTool,
    Offering        = 1 << 5, Trading  = 1 << 5,
    Skill           = 1 << 6,
    Artifact        = 1 << 7,
    Effect          = 1 << 8,
    Resident        = 1 << 9,
    Portrait        = 1 << 10,
    ForeignOffering = 1 << 11,
    Defiling        = 1 << 12,
    Coupling        = 1 << 13,
    Untradeable     = 1 << 14,
    Script          = 1 << 15,
    Weather         = 1 << 16, Map = 1 << 16,
    Study           = 1 << 17,
    Travelling      = 1 << 18,
    Scan            = 1 << 19,
    Explorer        = 1 << 20, Egg = 1 << 20,
    Signature       = 1 << 21,
    Curse           = 1 << 22,
    All             = ~0ull
};

const static AorUInt CT_EMPTY = 0;
const static AorUInt CT_CONSUMABLE = 1 << 8;
const static AorUInt CT_MATERIAL = 1 << 9;
const static AorUInt CT_TOOL = 1 << 10;
const static AorUInt CT_ARTIFACT = 1 << 11;
const static AorUInt CT_SCRIPT = 1 << 12;
const static AorUInt CT_EFFECT = 1 << 13;
const static AorUInt CT_RUNE = 1 << 14;
const static AorUInt CT_SKILL = 1 << 15;
const static AorUInt CT_WEATHER = 1 << 16;
const static AorUInt CT_OTHER = 1ull << 63;

struct ItemDefinition {
    ItemCode code;
    QString internal_name;
    QString display_name;
    QString description;
    unsigned char default_uses_left;
    ItemType type;
    ItemProperties properties;
};

using ItemDefinitionPtr = std::vector<ItemDefinition>::const_iterator;

const static std::vector<ItemDefinition> ITEM_DEFINITIONS = ItemMark::resolve_markers({
    {
        CT_EMPTY,
        "empty", "Empty",
        "Empty slot.",
        0 USES, Ordinary,
        {}
    },
    {
        CT_CONSUMABLE | 0,
        "globfruit", "Globfruit",
        "<i>It's unusually sticky, but I really like it.</i><br>",
        1 USES, Consumable,
        {
            { ItemLevel, 1 },
            { ConsumableEnergyBoost, 20 },
            { LeafyResource, 5 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "byteberry", "Byteberry",
        "<i>These little fruits grow in octuplets on the leaves of tries.</i><br>"
        "<i>Each individual berry is either in a ripe or unripened state, so you have to eat carefully.</i><br>",
        1 USES, Consumable,
        {
            { ItemLevel, 1 },
            { ConsumableEnergyBoost, 10 },
            { ConsumableSpiritBoost, 10 },
            { LeafyResource, 5 },
        }
    },
    {
        CT_CONSUMABLE | 2,
        "norton_ghost_pepper", "Norton Ghost Pepper",
        "<i>This pepper grows all over the place, but only very few of them are edible.</i><br>"
        "<i>They've started growing at camp. I ask them to leave but they pretend they're not listening.</i><br>",
        1 USES, Consumable,
        {
            { ItemLevel, 2 },
            { ConsumableClearsNumEffects, 1 },
            { ConsumableEnergyBoost, 20 },
            { LeafyResource, 10 }
        }
    },
    {
        CT_CONSUMABLE | 3,
        "bleeding_wildheart", "Bleeding Wildheart",
        "<i>If you're ever climbing a tree and you think you're near the top, just keep going.</i><br>"
        "<i>There are some interesting things up there. I think I found someone's keys.</i><br>",
        1 USES, Consumable,
        {
            { ItemLevel, 2 },
            { ConsumableEnergyBoost, 40 },
            { LeafyResource, 10 }
        }
    },
    {
        CT_CONSUMABLE | 4,
        "pipeapple", "Pipeapple",
        "<i>The pipeapple is a \"fifodesiac\" -</i><br>"
        "<i>that is to say, it allows us to share information, if you know what I mean.</i><br>",
        1 USES, Consumable,
        {
            { ItemLevel, 3 },
            { LeafyResource, 20 },
            { ConsumableEnergyBoost, 20 },
            { ConsumableSpiritBoost, 20 },
            { ConsumableMakesCouplable, 1 }
        }
    },
    {
        CT_CONSUMABLE | 5,
        "fungified_token", "Fungified Token",
        "<i>It seems to be an ancient currency, overgrown with an intoxicating fungus.</i><br>"
        "<i>Surely this would have made it useless. If only they had some sort of non-fungifiable token...</i><br>",
        3 USES, Consumable | Signature,
        {
            { ItemLevel, 5 },
            { LeafyResource, 10 },
            { ConsumableEnergyBoost, 20 },
            { ConsumableGivesEffect, ItemMark::marker("obsessive_checksum_disorder") }
        }
    },
    {
        CT_CONSUMABLE | 6,
        "polymore", "Polymore",
        "<i>Also known as \"curly bracket fungi\", it naturally lives on the bark of syntax trees;</i><br>"
        "<i>However it loves to grow on paper, and it will even rearrange the words on the page to fit its modular organization scheme.</i><br>"
        "<i>I wish it would go away.</i><br>",
        3 USES, Consumable | Signature,
        {
            { ItemLevel, 5 },
            { LeafyResource, 10 },
            { RunicResource, 20 },
            { ConsumableEnergyBoost, 20 },
            { ConsumableGivesEffect, ItemMark::marker("dynamic_digestion") }
        }
    },
    {
        CT_CONSUMABLE | 7,
        "copycap", "Copycap",
        "<i>The roots of this fungus seem to grasp an object and form a complete copy of it elsewhere.</i><br>"
        "<i>It will happily destroy whatever's in its way to complete this ritual.</i><br>",
        3 USES, Consumable | Signature,
        {
            { ItemLevel, 5 },
            { LeafyResource, 10 },
            { RunicResource, 20 },
            { ConsumableGivesEffect, ItemMark::marker("double_vision") }
        }
    },
    {
        CT_CONSUMABLE | 8,
        "ribbon_rollup", "Ribbon Rollup",
        "",
        2 USES, Consumable | Signature,
        {
            { ItemLevel, 5 },
            { RunicResource, 50 },
            { ConsumableMakesCouplable, 1 }
        }
    },
    {
        CT_CONSUMABLE | 9,
        "conconium", "Conconium",
        "<i>It turns out that the memory center of the brain has certain areas that are strictly off-limits.</i><br>"
        "<i>This plant lets you access them - and pay the terrible price for doing so.</i>",
        2 USES, Signature | Consumable,
        {
            { ItemLevel, 5 },
            { LeafyResource, 50 },
            { ConsumableKills, 1 },
            { ConsumableGeneratesRandomItems, 6 }
        }
    },
    {
        CT_CONSUMABLE | 9,
        "honeykey", "Honeykey",
        "<i>The hive's honey is revered not just for its sweetness, but its awesome regenerative power.</i><br>"
        "<i>Droplets on the ground cause the world itself to regrow around it.</i><br>",
        1 USES, Consumable | Signature,
        {
            { ItemLevel, 5 },
            { LeafyResource, 50 },
            { ConsumableClearsNumEffects, 4 },
            { ConsumableRegeneratesLocation, 10 }
        }
    },
    {
        CT_MATERIAL | 0,
        "obsilicon", "Obsilicon",
        "<i>I see little flashes of light now and again under its glassy surface -</i><br>"
        "<i>I wonder if it was cooled from the same primordial magma that forged this place.</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 1 },
            { CrystallineResource, 10 },
        }
    },
    {
        CT_MATERIAL | 1,
        "oolite", "Oolite",
        "<i>I've heard small talk that these little egg-shaped rocks may</i><br>"
        "<i>have been one the first stones to form in Rhodon.</i><br>"
        "<i>Each one is a little different, but they all fit just as well in my palm.</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 1 },
            { StoneResource, 10 }
        }
    },
    {
        CT_MATERIAL | 2,
        "cobolt_bar", "Cobolt Bar",
        "<i>Ancient writings reveal that entire cities used to be built of this charming metal,</i><br>"
        "<i>but no one seems to know what it is anymore.</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 2 },
            { MetallicResource, 25 }
        }
    },
    {
        CT_MATERIAL | 3,
        "solid_slate", "Solid Slate",
        "<i>I used to have to carve my notes on these awful spinning schists -</i><br>"
        "<i>powered by rune magic or something else sinister, they were slow, loud, and broke easily.</i><br>"
        "<i>This smaller, more stable stone is a crucial improvement.</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 2 },
            { StoneResource, 5 },
            { RunicResource, 10 }
        }
    },
    {
        CT_MATERIAL | 4,
        "scandiskium", "Scandiskium",
        "<i>Every once in a while, stillness grips the whole of Rhodon as it falls into night.</i><br>"
        "<i>Sometimes this happens very suddenly. I get awful headaches and nausea from it.</i><br>"
        "<i>And I can hear things changing as the world re-roots itself...</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 2 },
            { CrystallineResource, 10 },
            { MetallicResource, 5 },
        }
    },
    {
        CT_MATERIAL | 5,
        "fireclay", "Fireclay",
        "<i>Careful examination of this clay reveals it is selectively permeable -</i><br>"
        "<i>some particles can get through, but others can't.</i><br>"
        "<i>It's completely transparent from one side...</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 3 },
            { StoneResource, 50 },
        }
    },
    {
        CT_MATERIAL | 6,
        "rubygem", "Rubygem",
        "<i>These eye-catching stones are deceptively strong and durable, for how pretty they are.</i><br>"
        "<i>We've been building minecart tracks out of them - they make great rails.</i><br>",
        1 USES, Material,
        {
            { ItemLevel, 4 },
            { CrystallineResource, 50 },
            { RunicResource, 20 }
        }
    },
    {
        CT_MATERIAL | 7,
        "purl", "Purl",
        "<i>It looks an awful lot like a pearl, but it isn't one, so we needed to call it something else.</i><br>"
        "<i>Despite looking like a precious stone, these things are actually eggs - </i><br>"
        "<i>given enough time, they hatch into a beautiful butterfly.</i><br>",
        1 USES, Material,
        {

        }
    },
    {
        CT_MATERIAL | 8,
        "pentium_bar", "Pentium Bar",
        "<i>It's a beautiful metal; easy to work with too, since it is always on the verge of melting, making it trivial to cast.</i><br>"
        "<i>The heat it naturally exudes WILL ignite gunpowder. I learned that the hard way.</i><br>",
        1 USES, Material | Signature,
        {
            { ItemLevel, 5 },
            { StoneResource, 35 },
            { MetallicResource, 35 },
            { CrystallineResource, 35 },
            { RunicResource, 35 }
        }
    },
    {
        CT_TOOL | 0,
        "maven_mallet", "Maven Mallet",
        "<i>I built this rudimentary tool with the help of a colorful little bird who gave</i><br>"
        "<i>me an annoyingly long-winded series of directions.</i><br>",
        0 USES, SmithingTool,
        {
            { ItemLevel, 1 },
            { CostStone, 10 },
            { CostCrystalline, 10 },
            { ToolEnergyCost, 20 },
            { ToolMaximumStone, 30 },
            { ToolMaximumCrystalline, 30 },
        }
    },
    {
        CT_TOOL | 1,
        "hashcracker", "Hashcracker",
        "<i>Rhodon is covered in layers of hard Shale-256 - but with a good axe like this,</i><br>"
        "<i>I should be able to brute-force my way through it to find the goodies underneath.</i><br>",
        0 USES, ForagingTool,
        {
            { ItemLevel, 2 },
            { CostStone, 30 },
            { CostCrystalline, 10 },
            { ToolEnergyCost, 10 },
            { ToolCanDiscover1, ItemMark::marker("norton_ghost_pepper") },
            { ToolCanDiscover2, ItemMark::marker("bleeding_wildheart") },
            { ToolCanDiscover3, ItemMark::marker("pipeapple") },
            { ToolDiscoverWeight1, 3 },
            { ToolDiscoverWeight2, 3 },
            { ToolDiscoverWeight3, 1 },
        }
    },
    {
        CT_TOOL | 2,
        "metamorphic_destructor", "Metamorphic Destructor",
        "<i>It's very important that ore-bearing rocks are destroyed properly, or else you can</i><br>"
        "<i>leak their precious minerals back into the earth.</i><br>",
        0 USES, MiningTool,
        {
            { ItemLevel, 2 },
            { CostStone, 10 },
            { CostCrystalline, 30 },
            { ToolEnergyCost, 10 },
            { ToolCanDiscover1, ItemMark::marker("cobolt_bar") },
            { ToolCanDiscover2, ItemMark::marker("scandiskium") },
            { ToolCanDiscover3, ItemMark::marker("solid_slate") },
            { ToolDiscoverWeight1, 2 },
            { ToolDiscoverWeight2, 2 },
            { ToolDiscoverWeight3, 3 },
        }
    },
    {
        CT_TOOL | 3,
        "seaquake", "Seaquake",
        "<i>It makes a very satisfying clang.</i><br>",
        0 USES, SmithingTool,
        {
            { ItemLevel, 2 },
            { ToolEnergyCost, 30 },
            { CostMetallic, 10 },
            { CostCrystalline, 30 },
            { ToolMaximumMetallic, 80 },
            { ToolMaximumCrystalline, 30 },
            { ToolMaximumRunic, 20 },
        }
    },
    {
        CT_TOOL | 4,
        "defragmenter", "Defragmenter",
        "<i>You very rarely see flat, even bands of rock in Rhodon -</i><br>"
        "<i>as the world fills up, it becomes harder and harder to lay the sediment down sequentially.</i><br>",
        0 USES, MiningTool,
        {
            { ItemLevel, 3 },
            { ToolEnergyCost, 30 },
            { CostMetallic, 40 },
            { CostRuinc, 15 },
            { ToolCanDiscover1, ItemMark::marker("scalped_remains") },
            { ToolCanDiscover2, ItemMark::marker("recovered_journal") },
            { ToolCanDiscover3, ItemMark::marker("fireclay") },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
            { ToolDiscoverWeight3, 1 },
        }
    },
    {
        CT_TOOL | 5,
        "spacefiller", "Spacefiller",
        "<i>The lightless crevices of Rhodon's damp caverns are filled with all sorts of odd fungi.</i><br>"
        "<i>This mushroom-stabbing spear will help give us a taste of what we're missing!</i><br>",
        0 USES, ForagingTool,
        {
            { ItemLevel, 3 },
            { ToolEnergyCost, 50 },
            { CostLeafy, 10 },
            { CostMetallic, 30 },
            { CostCrystalline, 30 },
            { ToolCanDiscover1, ItemMark::marker("fungified_token") },
            { ToolCanDiscover2, ItemMark::marker("polymore") },
            { ToolCanDiscover3, ItemMark::marker("copycap") },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
            { ToolDiscoverWeight3, 1 }
        }
    },
    {
        CT_TOOL | 6,
        "rowhammer", "Rowhammer",
        "<i>If you just whack something hard enough for long enough, something cool will happen, right?</i><br>",
        0 USES, SmithingTool,
        {
            { ItemLevel, 3 },
            { ToolEnergyCost, 70 },
            { CostStone, 160 },
            { CostRuinc, 20 },
            { ToolMaximumStone, 50 },
            { ToolMaximumCrystalline, 50 },
            { ToolMaximumMetallic, 50 }
        }
    },
    {
        CT_TOOL | 7,
        "ghidrakes_breath", "Ghidrake's Breath",
        "<i>The flames of the mythical Ghidrake are said to boil away entire mountains,</i><br>"
        "<i>leaving only the raw core of Rhodon's earth behind.</i><br>"
        "<i>Inspired by these legendary tales, I made this rad fire-spewing pickaxe.</i><br>",
        0 USES, MiningTool,
        {
            { ItemLevel, 3 },
            { ToolEnergyCost, 30 },
            { CostStone, 50 },
            { CostCrystalline, 50 },
        }
    },
    /*
    {
        CT_TOOL | 8,
        "encursed_shovel", "Encursed Shovel",
        "<i>Holding this makes me innately aware that Rhodon's surface is a sprawling graveyard,</i><br>"
        "<i>its soil packed with the seemingly empty shells of ancient creatures long since dead.</i><br>"
        "<i>It also makes me deathly afraid of mice.</i><br>",
        0 USES, ForagingTool,
        {

        }
    },
    */
    {
        CT_ARTIFACT | 0,
        "recovered_journal", "Recovered Journal",
        "<i>It's in an odd language I can't quite read -</i><br>"
        "<i>but I can gather it tells a sad story of an orphan far from /home.<br>",
        0 USES, Artifact,
        {
            { ItemLevel, 3 },
            { PersistentMaxSpiritBoost, 40 }
        }
    },
    {
        CT_ARTIFACT | 1,
        "scalped_remains", "Scalped Remains",
        "<i>Perhaps a memento of a previous failed expedition, this is definitely a Fennahian skull.</i><br>"
        "<i>It remains animated by her soul - supposedly because the spirits of Rhodon haven't</i><br>"
        "<i>figured out what to do with it yet.</i><br>"
        "<i>In the meantime, she keeps good company...</i><br>",
        0 USES, Artifact,
        {
            { ItemLevel, 3 },
            { PersistentMaxEnergyBoost, 40 }
        }
    },
    {
        CT_ARTIFACT | 2,
        "wolframite_alpha", "Wolframite Alpha",
        "<i>Technically, this is an ore - but the smelting process leaves behind these strange tablets instead of metal.</i><br>"
        "<i>I've learned a lot about quantum chromodynamics, combinatorics, and what my friends say behind my back.</i><br>",
        0 USES, Artifact,
        {
            { ItemLevel, 4 },
            { PropertyIfLore, HeritageConsumableEnergyBoost },
            { PropertyLoreRequirement, 200 },
            { PropertyIfLoreValue, 20 },
        }
    },
    {
        CT_EFFECT | 0,
        "disconnected_socket", "Disconnected Socket",
        "<i>I took quite a nasty fall and dislocated my shoulder.</i><br>"
        "<i>The doctor said doing a handshake would reconnect it, but I keep being refused.</i><br>",
        7 USES, Effect,
        {
            { ItemLevel, 1 },
            { PersistentSpeedPenalty, 20 },
            { PersistentEnergyPenalty, 10 },
            { InjuryMining, 1 },
            { InjuryForaging, 1 },
            { InjuryTrading, 1 },
        }
    },
    {
        CT_EFFECT | 1,
        "starving", "Starving",
        "<i>Is this what it means to be a \"Rockmuncher?\"</i><br>",
        5 USES, Effect,
        {
            { ItemLevel, 1 },
            { PersistentSpeedPenalty, 20 },
            { PersistentSpiritPenalty, 10 },
        }
    },
    {
        CT_EFFECT | 2,
        "weakness", "Weakness",
        "<i>There's a numbness to my whole body -</i><br>"
        "<i>I feel like my soul no longer owns it, and I'm just watching myself stumble around.</i><br>",
        5 USES, Effect,
        {
            { ItemLevel, 1 },
            { PersistentEnergyPenalty, 10 },
        }
    },
    {
        CT_EFFECT | 3,
        "welchian_fever", "Welchian Fever",
        "<i>I get the horrible feeling <b>everything is about to end...</b></i>",
        10 USES, Effect,
        {
            { ItemLevel, 5 },
            { PersistentSpiritPenalty, 20 },
        }
    },
    {
        CT_EFFECT | 4,
        "obsessive_checksum_disorder", "Obsessive Checksum Disorder",
        "<i>Did you touch this? Did you touch it??? DID YOU TOUCH IT?!?</i><br>",
        6 USES, Effect,
        {
            { ItemLevel, 2 },
            { PersistentEnergyPenalty, 10 },
            { PersistentDiscoveryNotRandom, 1 }
        }
    },
    {
        CT_EFFECT | 5,
        "dynamic_digestion", "Dynamic Digestion",
        "<i>Damn, I'm starving. I could eat ANYTHING.</i><br>",
        10 USES, Effect,
        {
            { ItemLevel, 2 },
            { PersistentSpiritPenalty, 10 },
            { PersistentRandomConsumableProducts, 1 }
        }
    },
    {
        CT_EFFECT | 6,
        "double_vision", "Double Vision",
        "<i>I can see all 1.7976931348623157 × 10^308 meters of reality!</i><br>",
        6 USES, Effect,
        {
            { ItemLevel, 2 },
            { HeritageItemDoubleChance, 100 }
        }
    },
    {
        CT_EFFECT | 7,
        "trematrap", "Trematrap",
        "<i>The parasite wrapped around my spine filters messages being sent through my nervous system.</i>"
        "<i>When the brain tells the body it's time to die I can just, well, ignore it.</i>",
        10 USES, Effect,
        {
            { ItemLevel, 5 },
            { PersistentCannotDie, 1 }
        }
    },
    {
        CT_SKILL | 0,
        "organ_system_administrator", "Organ System Administrator",
        "<i>We trust you have recieved the usual lecture from the local Organ</i><br>"
        "<i>System Administrator. It usually boils down to these three things:</i><br>"
        "<i> #1) Respect the privacy of the patient.</i><br>"
        "<i> #2) Think before you slice.</i><br>"
        "<i> #3) With great power comes great responsibility.</i><br>",
        0 USES, Skill,
        {
            { ItemLevel, 1 },
            { SkillDraggableTo, Explorer },
            { SkillActionCooldown, 5 },
            { SkillClearInjury, 1 },
            { SkillPropertyUpgrades1, SkillClearInjury },
            { SkillUpgradeIncrement1, 1 }
        }
    },
    {
        CT_WEATHER | 0,
        "acid_stingers", "ACID Stingers",
        "<i>The bees make for fantastic workers - they are consistent, reliable, and professional.</i><br>"
        "<i>Being stung by their venom told us why -</i><br>"
        "<i>there's </i>something<i> in there that makes you endlessly more productive.</i><br>",
        0 USES, Weather,
        {
            { ItemLevel, 1 },
            { PersistentForageBonusChance, 50 },
        }
    },
    {
        CT_WEATHER | 1,
        "harmony", "Harmony",
        "<i>I see the towering pines, needles pointing to the twinkling stars;</i><br>"
        "<i>the fluttering butterflies, speaking with the language of the wind;</i><br>"
        "<i>the flocking birds, streaking patterns in the sky;</i><br>"
        "<i>And the suncradled grasses, swaying like feathers on the open plains.</i><br>"
        "<i>And I feel something incredible is just getting started.</i><br>",
        0 USES, Weather,
        {
            { ItemLevel, 1 },
            { PersistentThreatDecrease, 2 }
        }
    },
    {
        CT_WEATHER | 2,
        "unstable_division", "Unstable Division",
        "<i>Pentium, as an element, is marginally unstable, and slightly skews the calculations of the universe -</i><br>"
        "<i>It's perfectly safe most of the time, but if you aren't careful you may end up falling down the</i><br>"
        "<i>stairs because your stride was a </i>little longer<i> than it should have been,</i><br>"
        "<i>or your foot got nudged </i>ever so slightly<i> to the right.</i><br>",
        0 USES, Weather,
        {
            { ItemLevel, 1 },
            { PersistentChaoticCalculations, 1 }
        }
    },
    {
        CT_OTHER | 0,
        "fennahian_egg", "Fennahian Egg",
        "<i>I could make an omelette out of it, but maybe it's best to just wait for it to hatch.</i><br>",
        1 USES, Consumable | Untradeable,
        {
            { ItemLevel, 3 },
            { ConsumableEnergyBoost, 50 }
        }
    },
    {
        CT_OTHER | 1,
        "welchian_rune", "Welchian Rune",
        "<i></i><br>",
        0 USES, Untradeable,
        {}
    },
    {
        CT_OTHER | 2,
        "corrupting_nematode", "Corrupting Nematode",
        "<i><font color=#ff3300>Waves of need shall come this way</font></i><br>"
        "<i><font color=#ff3300>All I love shall die this day</font></i><br>"
        "<i><font color=#ff3300>All I seek here shall decay</font></i><br>"
        "<i><font color=#ff3300>Neath shade of roses, my flesh shall stay.</font></i><br>",
        0 USES, Signature | Curse,
        {
            { ItemLevel, 1 },
            { InventoryInfectsItems, 20 },
            { InventoryMaxEnergyBoost, 10 },
            { InventoryMaxSpiritBoost, 10 }
        }
    }
});

struct Item : public Serializable {
    ItemCode code = 0;
    ItemId id = EMPTY_ID;
    AorUInt uses_left = 0;
    ActivityId owning_action = NO_ACTION;
    ItemProperties instance_properties = {};

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const QString &name);

    static Item make_egg(CharacterId parent1, CharacterId parent2);
    static Item make_egg();

    ItemDefinitionPtr def() const;
    QString instance_properties_to_string() const;

    QString to_data_string() const;
    static Item from_data_string(const QString &data_string);

    void call_hooks(HookType type, const HookPayload &payload, ItemProperty allowed_prop_type = (ItemProperty) ~InventoryProperty) const;

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const QString &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static ItemCode code_of(const QString &name);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const QString &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static QPixmap pixmap_of(const Item &item);
    static QPixmap sil_pixmap_of(ItemCode id);
    static Item invalid_item();
    static bool has_resource_value(ItemCode code);

    static void for_each_resource_type(const std::function<void(ItemProperty, ItemProperty, ItemProperty)> &fn);
    static void for_each_tool_discover(const std::function<void(ItemProperty, ItemProperty)> &fn);

    static QString type_to_string(ItemType type);
    static QString properties_to_string(const ItemProperties &props);

    static Item empty_item;

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);
};
