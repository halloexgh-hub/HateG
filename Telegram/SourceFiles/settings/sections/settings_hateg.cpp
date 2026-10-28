/*
This file is part of HateG, based on Telegram Desktop.
*/
#include "settings/sections/settings_hateg.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "boxes/star_gift_box.h"
#include "data/data_user.h"
#include "info/peer_gifts/info_peer_gifts_common.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/hateg_settings.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/toast/toast.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QFileInfo>
#include <QtGui/QImageReader>

namespace Settings {
namespace {

using namespace Builder;

void VisualGiftCatalogBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller) {
	box->setTitle(rpl::single(u"Подарки Telegram — бесплатно в HateG"_q));
	box->setWidth(st::boxWideWidth);
	box->setNoContentMargin(true);
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });

	const auto session = &controller->session();
	const auto peer = not_null<PeerData*>{ session->user().get() };
	auto gifts = Info::PeerGifts::GiftsStars(
		session,
		peer
	) | rpl::map([](std::vector<Info::PeerGifts::GiftTypeStars> list) {
		auto result = Ui::GiftsDescriptor();
		result.list.reserve(list.size());
		for (auto &gift : list) {
			result.list.emplace_back(std::move(gift));
		}
		return result;
	});
	box->verticalLayout()->add(Ui::MakeGiftsList({
		.window = controller,
		.peer = peer,
		.gifts = std::move(gifts),
		.handler = [=](Info::PeerGifts::GiftDescriptor descriptor) {
			const auto gift = std::get_if<Info::PeerGifts::GiftTypeStars>(
				&descriptor);
			if (!gift) {
				return;
			}
			::HateG::LocalSettings::Instance().setVisualGiftId(
				gift->info.id);
			controller->showToast(Ui::Toast::Config{
				.text = { u"Подарок выбран для профиля HateG бесплатно."_q },
			});
			box->closeBox();
		},
	}));
}

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
			.title = rpl::single(u"Каталог подарков Telegram"_q),
			.icon = { &st::menuIconGiftPremium },
			.label = settings.visualGiftIdValue() | rpl::map([](uint64 id) {
				return id ? u"Выбран"_q : u"Открыть"_q;
			}),
			.onClick = [=] {
				controller->show(Box(VisualGiftCatalogBox, controller));
			},
			.keywords = { u"gifts"_q, u"showcase"_q },
		});
		builder.addDividerText(rpl::single(
			u"Это настоящие визуалы Telegram Gifts. Выбор бесплатный и действует только внутри HateG."_q));

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
