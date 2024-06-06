#pragma once

#include "serialize.h"
#include <qmessagebox.h>
#include <qiodevice.h>

class InfoMessage : public Serializable
{
public:
	const enum class InfoMessageList : size_t
	{
		new_game,
		welcome,
		injury,
		winner,
		consumable,
		material,
		smithing_tool,
		foraging_tool,
		mining_tool,
		artifact,
		signature_item,
		save_damaged,
		save_incompatible,
		quit,
		new_expedition,
	};

	static void initialize_messages();
	static InfoMessage& get_message(InfoMessageList message) { return info_messages.at(static_cast<size_t>(message)); }
	static std::vector<InfoMessage>& get_messages() { return info_messages; }
	int show_message(bool ignore_if_seen = true);
	inline bool has_been_seen() { return m_has_been_seen; }
	inline bool can_show_again() { return m_can_show_again; }
	inline QString get_name() { return m_name; }

	static void serialize_messages(QIODevice *dev);
	static void deserialize_messages(QIODevice *dev);

private:
	InfoMessage(QString name, QString text, QString informative_text, bool can_show_again = true, QString title = "aor", QMessageBox::StandardButtons buttons = QMessageBox::StandardButton::Ok,
		QMessageBox::StandardButton default_button = QMessageBox::StandardButton::NoButton, QMessageBox::Icon icon = QMessageBox::Icon::Information);

	const QString m_name;
	const QString m_text;
	const QString m_informative_text;
	const bool m_can_show_again;
	const QString m_title;
	const QMessageBox::StandardButtons m_buttons;
	const QMessageBox::StandardButton m_default_button;
	const QMessageBox::Icon m_icon;

	bool m_has_been_seen;

	static inline std::vector<InfoMessage> info_messages = std::vector<InfoMessage>();
};
