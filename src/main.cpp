// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "main.h"
#include "generators.h"
#include "die.h"
#include "infomessage.h"

int main(int argc, char **argv) {
#ifdef Q_NO_DEBUG
	std::set_terminate([]() {
		bugcheck(UncaughtUnknownException);
	});
#endif

	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/assets/img/items/welchian_rune.png"));

	app.setStyleSheet(
		"*[slot=\"true\"] { border: 1px solid #bbb; border-radius: 3px; background-color: white; }"
		"*[tooltip=\"true\"] { border-radius: 3px; }"
	);

#ifdef Q_NO_DEBUG
	try {
#endif
		LKGameWindow::instantiate_singleton();
		InfoMessage::initialize_messages();

		if (!gw()->save_file_exists()) {
			new_game_prompt();
		} else {
			gw()->load();
		}

		gw()->show();
		gw()->refresh_ui();

		InfoMessage::get_message(InfoMessage::InfoMessageList::welcome).show_message();

		CheatConsole console(gw());
		if (argc == 2 && strcmp(argv[1], "cheat") == 0) {
			console.show();
		}

		return app.exec();
#ifdef Q_NO_DEBUG
	}
	catch (std::out_of_range &e) {
		bugcheck(OutOfRangeException, e.what());
	} catch (IO::RetryException &e) {
		bugcheck(DeserializationFailure);
	}
#endif
}

void new_game_prompt() {

	if (InfoMessage::get_message(InfoMessage::InfoMessageList::new_game).show_message(false) != QMessageBox::Yes) {
		exit(1);
	}
}

