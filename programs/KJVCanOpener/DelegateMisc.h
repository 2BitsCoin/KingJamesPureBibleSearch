#ifndef DELEGATEMISC_H
#define DELEGATEMISC_H

#include "windows.h"
#include <QWidget>
#include <QStyleOptionViewItemV3>
#include <QStyleOptionViewItemV4>
#include <QStyle>
#include <QApplication>
#include <QWindowsVistaStyle>
#include <QPalette>

namespace StyledItemDelegateMisc
{
	inline const QWidget *widget(const QStyleOptionViewItem &option)
	{
		if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
			return v3->widget;
		return 0;
	}

	inline QStyle* style(const QStyleOptionViewItem &option)
	{
		const QWidget *widget = StyledItemDelegateMisc::widget(option);
		return widget ? widget->style() : QApplication::style();
	}

	inline void fixupViewItemPalette(QPalette *palette) {
//		if(qobject_cast<QWindowsVistaStyle *>(QApplication::style())) {

			bool useXP = false;
			HMODULE themeLib = GetModuleHandleW(L"uxtheme.dll");
			if (themeLib) {
				typedef bool (WINAPI *PtrIsAppThemed)();
				typedef bool (WINAPI *PtrIsThemeActive)();
				static PtrIsAppThemed pIsAppThemed = 0;
				static PtrIsThemeActive pIsThemeActive = 0;
				pIsAppThemed = (PtrIsAppThemed)GetProcAddress(themeLib,"IsAppThemed");
				pIsThemeActive = (PtrIsThemeActive)GetProcAddress(themeLib,"IsThemeActive");
				useXP = pIsThemeActive() && pIsAppThemed();
			}

			if(useXP) {
				// QWindowsVistaStyle::drawControl(CE_ItemViewItem,...)
				// paints itemview selection highlights in a pale blue gradient (not in QPalette::Highlight),
				// which means painting text in QPalette::HighlightedText doesn't have enough constrast.
				// To compensate, it speficially changes around the palette before drawing contents, as below

				palette->setColor(QPalette::All, QPalette::HighlightedText, palette->color(QPalette::Active, QPalette::Text));
				// Note that setting a saturated color here results in ugly XOR colors in the focus rect
				palette->setColor(QPalette::All, QPalette::Highlight, palette->base().color().darker(108));
			}
//		}
	}

}


#endif // DELEGATEMISC_H
