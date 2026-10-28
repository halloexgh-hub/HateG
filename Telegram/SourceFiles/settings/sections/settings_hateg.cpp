/*
This file is part of HateG, based on Telegram Desktop.
*/
#include "settings/sections/settings_hateg.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "settings/hateg_settings.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/toast/toast.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QFileInfo>
#include <QtGui/QImageReader>

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
		auto &settings = ::HateG::LocalSettings::Instance();

		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Оформление профиля"_q));
		builder.addButton({
			.id = u"hateg/banner"_q,
			.title = rpl::single(u"Баннер профиля"_q),
			.icon = { &st::menuIconPhoto },
			.label = settings.bannerPathValue() | rpl::map([](QString path) {
				return path.isEmpty()
					? u"Не выбран"_q
					: QFileInfo(path).fileName();
			}),
			.onClick = [=] {
				FileDialog::GetOpenPath(
					Core::App().getFileDialogParent(),
					u"Выберите баннер HateG"_q,
					u"Изображения (*.png *.jpg *.jpeg *.webp)"_q,
					crl::guard(controller, [=](
							const FileDialog::OpenResult &result) {
						if (result.paths.isEmpty()) {
							return;
						}
						const auto path = result.paths.front();
						if (!QImageReader(path).canRead()) {
							controller->showToast(Ui::Toast::Config{
								.text = { u"HateG не смог прочитать изображение."_q },
							});
							return;
						}
						::HateG::LocalSettings::Instance().setBannerPath(path);
						controller->showToast(Ui::Toast::Config{
							.text = { u"Баннер HateG сохранён."_q },
						});
					}));
			},
			.keywords = { u"banner"_q, u"profile"_q },
		});
		builder.addButton({
			.id = u"hateg/name-color"_q,
			.title = rpl::single(u"Цвет ника"_q),
			.icon = { &st::menuIconPalette },
			.label = settings.nameColorIndexValue() | rpl::map([](int) {
				return ::HateG::LocalSettings::Instance().nameColorName();
			}),
			.onClick = [=] {
				::HateG::LocalSettings::Instance().selectNextNameColor();
			},
			.keywords = { u"color"_q, u"name"_q },
		});
		builder.addButton({
			.id = u"hateg/banner-reset"_q,
			.title = rpl::single(u"Убрать баннер"_q),
			.icon = { &st::menuIconDelete },
			.onClick = [=] {
				::HateG::LocalSettings::Instance().setBannerPath(QString());
				controller->showToast(Ui::Toast::Config{
					.text = { u"Баннер HateG убран."_q },
				});
			},
			.keywords = { u"banner"_q, u"reset"_q },
		});

		builder.addDivider();
		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Подарки HateG"_q));
		builder.addButton({
			.id = u"hateg/gifts"_q,
			.title = rpl::single(u"Моя витрина подарков"_q),
			.icon = { &st::menuIconGiftPremium },
			.label = settings.giftsValue() | rpl::map([](int count) {
				return QString::number(count);
			}),
			.onClick = [=] {
				const auto count = ::HateG::LocalSettings::Instance().gifts();
				controller->showToast(Ui::Toast::Config{
					.text = { count
						? u"Косметических подарков: %1"_q.arg(count)
						: u"Витрина пока пустая."_q },
				});
			},
			.keywords = { u"gifts"_q, u"showcase"_q },
		});
		builder.addButton({
			.id = u"hateg/gift-store"_q,
			.title = rpl::single(u"Магазин косметики"_q),
			.icon = { &st::menuIconShop },
			.label = rpl::single(u"Подарок — 100 HG"_q),
			.onClick = [=] {
				const auto bought
					= ::HateG::LocalSettings::Instance().buyDemoGift();
				controller->showToast(Ui::Toast::Config{
					.text = { bought
						? u"Косметический подарок добавлен в витрину."_q
						: u"Недостаточно HG. Откройте кошелёк."_q },
				});
			},
			.keywords = { u"store"_q, u"gifts"_q },
		});
		builder.addDivider();
		builder.addSkip();
		builder.addSubsectionTitle(rpl::single(u"Косметический кошелёк"_q));
		builder.addButton({
			.id = u"hateg/wallet"_q,
			.title = rpl::single(u"Баланс HateG"_q),
			.icon = { &st::menuIconTon },
			.label = settings.balanceValue() | rpl::map([](int balance) {
				return QString::number(balance) + u" HG"_q;
			}),
			.onClick = [=] {
				const auto claimed
					= ::HateG::LocalSettings::Instance().claimDemoBalance();
				controller->showToast(Ui::Toast::Config{
					.text = { claimed
						? u"Начислено 500 тестовых HG."_q
						: u"HG — бесплатная косметическая валюта HateG."_q },
				});
			},
			.keywords = { u"wallet"_q, u"balance"_q },
		});
		builder.addDividerText(rpl::single(
			u"HG — только внутренняя косметика HateG, не деньги и не Telegram Stars."_q));

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
