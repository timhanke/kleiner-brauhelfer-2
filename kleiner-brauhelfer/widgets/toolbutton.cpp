#include "toolbutton.h"
#include <QGuiApplication>
#include <QColor>
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

static QString filterHighlightStyle()
{
    if (!gSettings)
        return QString();

    const QColor buttonColor = gSettings->paletteChanged.color(QPalette::Button);
    const QColor borderColor = buttonColor.darker(125);
    return QStringLiteral(
        "QToolButton[filterHighlight=\"true\"][filterHighlightActive=\"true\"] {"
        " background-color: %1;"
        " border: 1px solid %2;"
        " border-radius: 3px;"
        "}"
    ).arg(buttonColor.name(QColor::HexArgb), borderColor.name(QColor::HexArgb));
}

ToolButton::ToolButton(QWidget *parent) :
    QToolButton(parent),
    mError(false)
{
    setPopupMode(ToolButtonPopupMode::InstantPopup);
    setStyleSheet(filterHighlightStyle());
    if (gSettings)
        connect(gSettings, &Settings::themeChanged, this, [this](){ setStyleSheet(filterHighlightStyle()); updatePalette(); });
    connect(this, &QAbstractButton::clicked, this, &ToolButton::updatePalette);
}

void ToolButton::addChangeDecorator()
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool ToolButton::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QToolButton::event(event);
}

void ToolButton::updatePalette()
{
    if (property("filterHighlight").toBool())
        setStyleSheet(filterHighlightStyle());

    if (WidgetDecorator::contains(this))
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (property("filterHighlight").toBool() && property("filterHighlightActive").toBool())
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (mError)
    {
        if (palette() != gSettings->paletteError)
            setPalette(gSettings->paletteError);
    }
    else
    {
        if (palette() != QGuiApplication::palette())
            setPalette(QGuiApplication::palette());
    }
}

void ToolButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
