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
	_visualGiftId = storage.value(u"visuals/giftId"_q, 0).toULongLong();
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

uint64 LocalSettings::visualGiftId() const {
	return _visualGiftId.current();
}

rpl::producer<uint64> LocalSettings::visualGiftIdValue() const {
	return _visualGiftId.value();
}

void LocalSettings::setVisualGiftId(uint64 id) {
	_visualGiftId = id;
	save(u"visuals/giftId"_q, QVariant::fromValue<qulonglong>(id));
}

void LocalSettings::save(const QString &key, const QVariant &value) {
	auto storage = Storage();
	storage.setValue(key, value);
	storage.sync();
}

} // namespace HateG
