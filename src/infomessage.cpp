#include "infomessage.h"
#include <qmessagebox.h>

void InfoMessage::initialize_messages()
{
	info_messages.clear();

	info_messages.push_back(InfoMessage(
		"New Game",

		"You don't appear to have an existing game.",

		"Will you step foot into a new world?<br>"
		"<br>"
		"<i>(If you do have an existing game, make sure the save.rho file is in the same directory as this executable.)</i>",

		false, "aor", QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::Question
	));

	info_messages.push_back(InfoMessage(
		"Welcome",

		"<b>The sun rises on a new adventure!</b>",

		"The passage here was not easy.<br>"
		"We've been sent alone, and gaze into the sprawling horizon of Rhodon with only the clothes on our backs...<br>"
		"<br>"
		"Still! We have much work to do, so we musn't get distracted.<br>"
		"<br>"
		"...Maybe I should start by <b>foraging for something to eat?</b>",

		true, "aor", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoIcon
	));

	info_messages.push_back(InfoMessage(
		"Injury",

		"<b>I just suffered an injury...</b><br>",

		"<b>Injuries</b> inflict our explorers with negative effects.<br>"
		"They are triggered by certain events, such as running out of energy or spirit,<br>"
		"and can happen randomly under certain conditions.<br>"
		"They heal over time, and can be healed faster by eating <b>consumables</b>.<br>"
		"<br>"
		"Injuries should not be left to fester; once an explorer fills all her injury slots, she will <b>die</b>."
	));

	info_messages.push_back(InfoMessage(
		"Winner",

		"<b>Congrats!</b><br>",

		"Your are winner!<br>"
	));

	info_messages.push_back(InfoMessage(
		"Consumable",

		"<b>I just found a consumable!</b><br>",

		"<b>Consumables</b> are used to restore our explorers' energy and health.</b><br>"
		"When eaten, consumables will usually refill some amount of <b>energy.</b> They will also <b>reduce the timer of all active injuries by 1 action.</b><br>"
		"Many will also have a bonus effect on top of this.<br>"
		"Drag a consumable to an explorer's <b>portrait</b> to have her eat it.</b><br>"
	));

	info_messages.push_back(InfoMessage(
		"Material",

		"<b>I just found a material!</b><br>",

		"<b>Materials</b> are used to craft new items.</b><br>"
		"All materials have some amount of value in one of the <b>five resource types</b>.<br>"
		"Materials can be dragged into an explorer's <b>smithing slots</b> to use them for crafting - the total value of all materials in her smithing slots determines what item she will make.<br>"
		"Materials can also be dragged to an explorer's <b>portrait</b> to have her <b>defile</b> it, which will destroy the item to restore <b>25 spirit per item level</b>.<br>"
		"<br>"
		"Why don't I check the <b>encyclopedia</b> and see if I can smith <b>a new tool</b>?"
	));

	info_messages.push_back(InfoMessage(
		"Smithing Tool",

		"<b>I just made a smithing tool!</b><br>",

		"When equipped, <b>smithing tools</b> increase the amount of resources I can put into <b>smithing slots</b>.<br>"
		"The maximum amount of resources a smithing tool can support is referred to as its <b>power</b>.<br>"
		"Regardless of tool, I can always support at least <b>10 of each resource type</b>.<br>"
	));

	info_messages.push_back(InfoMessage(
		"Foraging Tool",

		"<b>I just made a foraging tool!</b><br>",

		"When equipped, <b>foraging tools</b> allow me to find new consumables when I forage.<br>"
		"Each foraging tool has a unique pool of items it can discover; some potentially rarer than others.<br>"
		"While I hold a foraging tool, I also have a chance to find <b>loose eggs</b> that can <b>hatch into new explorers!</b><br>"
	));

	info_messages.push_back(InfoMessage(
		"Mining Tool",

		"<b>I just made a mining tool!</b><br>",

		"When equipped, <b>mining tools</b> allow me to find new materials when I mine.<br>"
		"Each mining tool has a unique pool of items it can discover; some potentially rarer than others.<br>"
	));

	info_messages.push_back(InfoMessage(
		"Artifact",

		"<b>I just found an artifact!</b><br>",

		"Artifacts provide <b>constant bonuses</b> when equipped.<br>"
		"Each explorer can hold up to 3."
	));

	info_messages.push_back(InfoMessage(
		"Signature Item",

		"<b>I just found a signature item!</b><br>",

		"Signature items are <b>limited, location-specific</b> items. Each location usually has only one, and they are typically <b>exceedingly unique and powerful.</b><br>"
		"They are found randomly after <b>either</b> foraging or mining, regardless of what they are - food, material, artifact, etc.<br>"
		"The items are not tied to the location once found - I can bring them elsewhere or even trade them away.<br>"
		"The only restriction on their use is that <b>signature artifacts may not be unequipped</b> once slotted on an explorer.<br>"
	));

	info_messages.push_back(InfoMessage(
		"Save Damaged",

		"The save file is damaged or of an incompatible version (1.x.x).",

		"",

		false, "Aegis of Rhodon", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::Critical
	));

	info_messages.push_back(InfoMessage(
		"Save Incompatible",

		"The save file is of an incompatible version.",

		"",

		false, "Aegis of Rhodon", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::Critical
	));

	info_messages.push_back(InfoMessage(
		"Quit",

		"Are you sure you want to quit?",

		"Your game will be saved.",

		false, "aor", QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::NoButton, QMessageBox::Question
	));

	info_messages.push_back(InfoMessage(
		"New Expedition",

		"Are you sure you want to start a new expedition?",

		"<b>Your current progress will be lost.</b>",

		false, "Aegis of Rhodon", QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton, QMessageBox::Question
	));
}

int InfoMessage::show_message(bool ignore_if_seen)
{
	if (ignore_if_seen && m_has_been_seen)
	{
		return 0;
	}
	m_has_been_seen = true;

	QMessageBox message;
	message.setText(m_text);
	message.setInformativeText(m_informative_text);
	message.setStandardButtons(m_buttons);
	message.setDefaultButton(m_default_button);
	message.setIcon(m_icon);
	return message.exec();
}

void InfoMessage::serialize_messages(QIODevice *dev)
{
	std::vector<bool> has_been_seen = std::vector<bool>();

	for (InfoMessage &msg : info_messages)
	{
		has_been_seen.push_back(msg.m_has_been_seen);
	}

	Serialize::serialize(dev, has_been_seen);
}

void InfoMessage::deserialize_messages(QIODevice *dev)
{
	std::vector<bool> has_been_seen = std::vector<bool>();

	Serialize::deserialize(dev, has_been_seen);

	for (InfoMessage &msg : info_messages)
	{
		if (has_been_seen.empty())
			break;
		msg.m_has_been_seen = has_been_seen.front();
		has_been_seen.erase(has_been_seen.begin());
	}
}

InfoMessage::InfoMessage(QString name, QString text, QString informative_text, bool can_show_again, QString title,
	QMessageBox::StandardButtons buttons, QMessageBox::StandardButton default_button, QMessageBox::Icon icon)
	: m_name(name), m_text(text), m_informative_text(informative_text), m_can_show_again(can_show_again),
	m_title(title), m_buttons(buttons), m_default_button(default_button), m_icon(icon), m_has_been_seen(false)
{ }
