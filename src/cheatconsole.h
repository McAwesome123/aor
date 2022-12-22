#pragma once

#include <algorithm>

#include <QWidget>
#include <QString>

class CheatConsole;

#include "gamewindow.h"
#include "itemslot.h"
#include "../ui_cheat.h"

#define QS_TO_INT(i, s) \
int i; \
{ \
    bool ok; \
    i = s.toInt(&ok); \
    if (!ok) { \
        qWarning("Expected a number for argument (%s)", s.toStdString().c_str()); \
        return; \
    } \
}

#define OOB_CHECK(y, x) \
if (Inventory::are_yx_coords_oob(y, x)) { \
    qWarning("Yx coords were oob (y: %d, x: %d)", y, x); \
    return; \
}

using CheatAction = std::function<void(LKGameWindow *game, const QStringList &args)>;

struct CheatCommand {
    QString name;
    QString help;
    int nargs;
    CheatAction action;
};

const static std::vector<CheatCommand> COMMANDS = {
    {
        "help",
        "Display the help text for a command",
        1,
        [](LKGameWindow *, const QStringList &args) {
            QString command_name = args[0];
            auto match_name = [&command_name](const CheatCommand command) -> bool {
                return command.name == command_name;
            };

            auto command = std::find_if(begin(COMMANDS), end(COMMANDS), match_name);
            if (command == end(COMMANDS)) {
                qWarning("Failed to find command for printing help (%s)", command_name.toStdString().c_str());
                return;
            }

            qDebug("help: %s", command->help.toStdString().c_str());
        }
    },
    {
        "print",
        "Print detailed information about the item at yx $0, $1 in the inventory",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(y, args[0])
            QS_TO_INT(x, args[1])

            OOB_CHECK(y, x);

            Item item = game->game().inventory().get_item(y, x);

            qDebug("print: item yx (%d, %d): code (%d), id (%llx), name (%s), intent (%d)",
                y, x,
                item.code,
                item.id,
                Item::def_of(item.code)->internal_name.toStdString().c_str(),
                item.intent
            );
        }
    },
    {
        "make",
        "Make a new item with name $0 at yx $1, $2; if $1 and $2 are '.', then make it in the first available slot",
        3,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[1] == "." && args[2] == ".") {
                game->game().add_item(Item(args[0]));
            }

            QS_TO_INT(y, args[1]);
            QS_TO_INT(x, args[2]);

            OOB_CHECK(y, x);

            game->game().inventory().put_item(Item(args[0]), y, x);
        }
    },
    {
        "save",
        "Save the current state of the game to disk",
        0,
        [](LKGameWindow *game, const QStringList &) {
            game->save();
        }
    },
    {
        "load",
        "Load a saved character state to disk",
        0,
        [](LKGameWindow *game, const QStringList &) {
            game->load();
        }
    },
    {
        "name",
        "Change the current character's name to $0; if $0 is '?', show the current name instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            QString new_name = args[0];
            if (new_name == "?") {
                qDebug("My name is (%s)", game->selected_char().name().toStdString().c_str());
                return;
            }

            game->selected_char().name() = new_name;
        }
    },
    {
        "energy",
        "Change the current character's energy to $0; if $0 is '?', show the current energy instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current energy is (%d)", game->selected_char().energy());
                return;
            }

            QS_TO_INT(energy, args[0]);

            if (energy < 0 || energy > 100) {
                qWarning("New energy value is oob (%d)", energy);
                return;
            }

            game->selected_char().energy() = energy;
        }
    },
    {
        "spirit",
        "Change the current character's spirit to $0; if $0 is '?', show the current spirit instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current spirit is (%d)", game->selected_char().spirit());
                return;
            }

            QS_TO_INT(spirit, args[0]);

            if (spirit < 0 || spirit > 100) {
                qWarning("New spirit value is oob (%d)", spirit);
                return;
            }

            game->selected_char().spirit() = spirit;
        }
    },
    {
        "timel",
        "Change the amount of ms left in the current activity to $0; if $0 is '?', show the amount of ms left instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Amount of ms left in activity is (%lld)", game->selected_char().activity().ms_left());
                return;
            }

            QS_TO_INT(ms, args[0]);

            game->selected_char().activity().ms_left() = ms;
        }
    },
    {
        "effect",
        "Set the effect at n = $0 to a new item with code $1",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(n, args[0]);
            QS_TO_INT(code, args[1]);

            game->selected_char().effects()[n] = Item(code);
        }
    },
    {
        "couple",
        "Add an egg from explorers with names $0 and $1 to your inventory that hatches after 1 action",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            auto &characters = game->game().characters();

            CharacterId parent_id1 = std::find_if(begin(characters), end(characters), [&](Character &a) {
                return a.name() == args[0];
            })->id();

            CharacterId parent_id2 = std::find_if(begin(characters), end(characters), [&](Character &a) {
                return a.name() == args[1];
            })->id();

            Item egg = Item::make_egg(parent_id1, parent_id2);
            egg.instance_properties.map[InstanceEggFoundActionstamp] = 0;
            game->game().add_item(egg);
        }
    },
    {
        "egg",
        "Put a random found egg into the inventory",
        0,
        [](LKGameWindow *game, const QStringList &) {
            game->game().add_item(Item::make_egg());
        }
    },
    {
        "fast",
        "If $0 is 0, turn off accelerated actions; otherwise turn it on",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(fast, args[0]);

            game->game().fast_actions() = fast;
        }
    },
    {
        "actions",
        "Change the game's action counter to $0; if $0 is '?', show the current actions instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Action counter at (%lld)", game->game().actions_done());
                return;
            }

            QS_TO_INT(actions, args[0]);

            game->game().actions_done() = actions;
        }
    }
};

class CheatConsole : public QWidget {
    Q_OBJECT;

public:
    CheatConsole(LKGameWindow *window);

    void execute(const QString &input);

    LKGameWindow *game;
    Ui::CheatWindow console;

public slots:
    void consume_input();
};
