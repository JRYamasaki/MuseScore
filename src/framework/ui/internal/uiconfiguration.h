/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MU_UI_UICONFIGURATION_H
#define MU_UI_UICONFIGURATION_H

#include "iuiconfiguration.h"
#include "imainwindow.h"
#include "internal/iplatformtheme.h"
#include "iworkspacesettings.h"
#include "val.h"

#include "modularity/ioc.h"

namespace mu::ui {
class UiConfiguration : public IUiConfiguration
{
    INJECT(ui, IMainWindow, mainWindow)
    INJECT(ui, IPlatformTheme, platformTheme)
    INJECT(ui, framework::IWorkspaceSettings, workspaceSettings)

public:
    void init();
    void deinit();

    ThemeList themes() const override;
    QStringList possibleFontFamilies() const override;
    QStringList possibleAccentColors() const override;

    const ThemeInfo& currentTheme() const override;
    void setCurrentTheme(const ThemeCode& codeKey) override;
    void setCurrentThemeStyleValue(ThemeStyleKey key, const Val& val) override;
    async::Notification currentThemeChanged() const override;

    std::string fontFamily() const override;
    void setFontFamily(const std::string& family) override;
    int fontSize(FontSizeType type) const override;
    void setBodyFontSize(int size) override;
    async::Notification fontChanged() const override;

    std::string iconsFontFamily() const override;
    int iconsFontSize(IconSizeType type) const override;
    async::Notification iconsFontChanged() const override;

    std::string musicalFontFamily() const override;
    int musicalFontSize() const override;
    async::Notification musicalFontChanged() const override;

    float guiScaling() const override;
    float physicalDotsPerInch() const override;

    void setPhysicalDotsPerInch(std::optional<float> dpi) override;

    QByteArray pageState(const std::string& pageName) const override;
    void setPageState(const std::string& pageName, const QByteArray& state) override;

    QByteArray windowGeometry() const override;
    void setWindowGeometry(const QByteArray& geometry) override;
    async::Notification windowGeometryChanged() const override;

    void applyPlatformStyle(QWindow* window) override;

private:
    bool needFollowSystemTheme() const;

    void initThemes();
    void notifyAboutCurrentThemeChanged();
    void updateCurrentTheme();
    void updateThemes();

    ThemeCode currentThemeCodeKey() const;
    ThemeInfo makeStandardTheme(const ThemeCode& codeKey) const;

    ThemeList readThemes() const;
    void writeThemes(const ThemeList& themes);

    Val uiArrangmentValue(const std::string& key) const;
    void setUiArrangmentValue(const std::string& key, const Val& value) const;

    QByteArray stringToByteArray(const std::string& string) const;
    std::string byteArrayToString(const QByteArray& byteArray) const;

    async::Notification m_currentThemeChanged;
    async::Notification m_fontChanged;
    async::Notification m_musicalFontChanged;
    async::Notification m_iconsFontChanged;
    async::Notification m_windowGeometryChanged;

    ThemeList m_themes;
    size_t m_currentThemeIndex = 0;
    std::optional<float> m_customDPI;
};
}

#endif // MU_UI_UICONFIGURATION_H
