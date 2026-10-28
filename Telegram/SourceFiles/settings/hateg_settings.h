/*
This file is part of HateG, based on Telegram Desktop.
*/
#pragma once

#include "base/qt/qt_common_adapters.h"
#include "rpl/variable.h"

#include <QtCore/QVariant>
#include <QtGui/QColor>

namespace HateG {

class LocalSettings final {
public:
	[[nodiscard]] static LocalSettings &Instance();

	[[nodiscard]] QString bannerPath() const;
	[[nodiscard]] rpl::producer<QString> bannerPathValue() const;
	void setBannerPath(QString path);

	[[nodiscard]] int nameColorIndex() const;
	[[nodiscard]] QColor nameColor() const;
	[[nodiscard]] QString nameColorName() const;
	[[nodiscard]] rpl::producer<int> nameColorIndexValue() const;
	void selectNextNameColor();

	[[nodiscard]] int gifts() const;
	[[nodiscard]] rpl::producer<int> giftsValue() const;
	void addFreeGift();

private:
	LocalSettings();
	void save(const QString &key, const QVariant &value);

	rpl::variable<QString> _bannerPath;
	rpl::variable<int> _nameColorIndex;
	rpl::variable<int> _gifts;
};

} // namespace HateG
