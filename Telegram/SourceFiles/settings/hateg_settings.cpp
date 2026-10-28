/*
This file is part of HateG, based on Telegram Desktop.
*/
#include "settings/hateg_settings.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtGui/QImage>

#include <algorithm>
#include <array>

namespace HateG {
namespace {

constexpr auto kDemoGiftPrice = 100;

const auto &NameColors() {
	static const auto result = std::array{
		QColor(u"#7767D8"_q),
		QColor(u"#D05C9B"_q),
		QColor(u"#2B9A8C"_q),
		QColor(u"#D4773A"_q),
		QColor(u"#4E86D8"_q),
	};
	return result;
}

const auto &NameColorNames() {
	static const auto result = std::array{
		u"Фиолетовый"_q,
		u"Розовый"_q,
		u"Бирюзовый"_q,
		u"Оранжевый"_q,
		u"Синий"_q,
	};
	return result;
}

QSettings Storage() {
	return QSettings(u"HateG"_q, u"HateG Desktop"_q);
}

} // namespace

LocalSettings &LocalSettings::Instance() {
	static auto result = LocalSettings();
	return result;
}

LocalSettings::LocalSettings() {
	auto storage = Storage();
	_bannerPath = storage.value(u"visuals/banner"_q).toString();
	_nameColorIndex = std::clamp(
		storage.value(u"visuals/nameColor"_q, 0).toInt(),
		0,
		int(NameColors().size()) - 1);
	_gifts = std::max(storage.value(u"wallet/gifts"_q, 0).toInt(), 0);
	_balance = std::max(storage.value(u"wallet/balance"_q, 0).toInt(), 0);
}

QString LocalSettings::bannerPath() const {
	return _bannerPath.current();
}

rpl::producer<QString> LocalSettings::bannerPathValue() const {
	return _bannerPath.value();
}

void LocalSettings::setBannerPath(QString path) {
	const auto image = QImage(path);
	if (!image.isNull()) {
		const auto directory = QStandardPaths::writableLocation(
			QStandardPaths::AppDataLocation) + u"/hateg"_q;
		QDir().mkpath(directory);
		const auto stored = directory + u"/profile-banner.jpg"_q;
		if (image.save(stored, "JPG", 90)) {
			path = stored;
		}
	}
	_bannerPath = std::move(path);
	save(u"visuals/banner"_q, _bannerPath.current());
}

int LocalSettings::nameColorIndex() const {
	return _nameColorIndex.current();
}

QColor LocalSettings::nameColor() const {
	return NameColors()[nameColorIndex()];
}

QString LocalSettings::nameColorName() const {
	return NameColorNames()[nameColorIndex()];
}

rpl::producer<int> LocalSettings::nameColorIndexValue() const {
	return _nameColorIndex.value();
}

void LocalSettings::selectNextNameColor() {
	_nameColorIndex = (nameColorIndex() + 1) % int(NameColors().size());
	save(u"visuals/nameColor"_q, nameColorIndex());
}

int LocalSettings::gifts() const {
	return _gifts.current();
}

rpl::producer<int> LocalSettings::giftsValue() const {
	return _gifts.value();
}

int LocalSettings::balance() const {
	return _balance.current();
}

rpl::producer<int> LocalSettings::balanceValue() const {
	return _balance.value();
}

bool LocalSettings::buyDemoGift() {
	if (balance() < kDemoGiftPrice) {
		return false;
	}
	_balance = balance() - kDemoGiftPrice;
	_gifts = gifts() + 1;
	save(u"wallet/balance"_q, balance());
	save(u"wallet/gifts"_q, gifts());
	return true;
}

bool LocalSettings::claimDemoBalance() {
	if (balance() || gifts()) {
		return false;
	}
	_balance = 500;
	save(u"wallet/balance"_q, balance());
	return true;
}

void LocalSettings::save(const QString &key, const QVariant &value) {
	auto storage = Storage();
	storage.setValue(key, value);
	storage.sync();
}

} // namespace HateG
