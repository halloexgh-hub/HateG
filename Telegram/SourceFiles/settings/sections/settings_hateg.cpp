/*
This file is part of HateG, based on Telegram Desktop.
*/
#include "settings/sections/settings_hateg.h"

#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/toast/toast.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Settings {
namespace {

using namespace Builder;

class HateG final : public Section<HateG> {
public:
	HateG(
		QWidget *parent,
		not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();
};

HateG::HateG(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> HateG::title() {
	return rpl::single(u"HateG"_q);
}

void HateG::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	const auto buildMethod = [](
			not_null<Ui::VerticalLayout*> container,
			not_null<Window::SessionController*> controller,
			Fn<void(Type)> showOther,
			rpl::producer<>) {
		auto builder = SectionBuilder(WidgetContext{
			.container = container,
			.controller = controller,
			.showOther = std::move(showOther),
			.isPaused = Window::PausedIn(
				controller,
				Window::GifPauseReason::Layer),
		});
		const auto comingSoon = [=](QString feature) {
			return [=] {
				controller->showToast({
					.text = { feature + u": скоро в HateG"_q },
				});
			};
		};

		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Оформление профиля"_q));
		builder.addButton({
			.id = u"hateg/banner"_q,
			.title = rpl::single(u"Баннер профиля"_q),
			.icon = { &st::menuIconPhoto },
			.label = rpl::single(u"Скоро"_q),
			.onClick = comingSoon(u"Баннер профиля"_q),
			.keywords = { u"banner"_q, u"profile"_q },
		});
		builder.addButton({
			.id = u"hateg/name-color"_q,
			.title = rpl::single(u"Цвет ника"_q),
			.icon = { &st::menuIconPalette },
			.label = rpl::single(u"Фиолетовый"_q),
			.onClick = comingSoon(u"Цвет ника"_q),
			.keywords = { u"color"_q, u"name"_q },
		});

		builder.addDivider();
		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Подарки HateG"_q));
		builder.addButton({
			.id = u"hateg/gifts"_q,
			.title = rpl::single(u"Моя витрина подарков"_q),
			.icon = { &st::menuIconGiftPremium },
			.label = rpl::single(u"0"_q),
			.onClick = comingSoon(u"Витрина подарков"_q),
			.keywords = { u"gifts"_q, u"showcase"_q },
		});
		builder.addButton({
			.id = u"hateg/gift-store"_q,
			.title = rpl::single(u"Магазин косметики"_q),
			.icon = { &st::menuIconShop },
			.label = rpl::single(u"Скоро"_q),
			.onClick = comingSoon(u"Магазин косметики"_q),
			.keywords = { u"store"_q, u"gifts"_q },
		});

		builder.addDivider();
		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Косметический кошелёк"_q));
		builder.addButton({
			.id = u"hateg/wallet"_q,
			.title = rpl::single(u"Баланс HateG"_q),
			.icon = { &st::menuIconTon },
			.label = rpl::single(u"0 HG"_q),
			.onClick = comingSoon(u"Косметический кошелёк"_q),
			.keywords = { u"wallet"_q, u"balance"_q },
		});
		builder.addDividerText(rpl::single(
			u"HG — только внутренняя косметика HateG, не деньги и не Telegram Stars."_q));

		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Подключение"_q));
		builder.addButton({
			.id = u"hateg/connection"_q,
			.title = rpl::single(u"HateG Connection Rescue"_q),
			.icon = { &st::menuIconNetwork },
			.label = rpl::single(u"В разработке"_q),
			.onClick = comingSoon(u"Connection Rescue"_q),
			.keywords = { u"proxy"_q, u"connection"_q, u"network"_q },
		});
		builder.addSkip();
	};

	build(content, buildMethod);
	Ui::ResizeFitChild(this, content);
}

} // namespace

Type HateGId() {
	return HateG::Id();
}

} // namespace Settings
