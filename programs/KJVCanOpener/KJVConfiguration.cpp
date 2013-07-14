/****************************************************************************
**
** Copyright (C) 2013 Donna Whisnant, a.k.a. Dewtronics.
** Contact: http://www.dewtronics.com/
**
** This file is part of the KJVCanOpener Application as originally written
** and developed for Bethel Church, Festus, MO.
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU General Public License
** version 3.0 as published by the Free Software Foundation and appearing
** in the file gpl-3.0.txt included in the packaging of this file. Please
** review the following information to ensure the GNU General Public License
** version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and
** Dewtronics.
**
****************************************************************************/

#include "KJVConfiguration.h"
#include "ui_KJVTextFormatConfig.h"

#include "ScriptureEdit.h"
#include "KJVSearchResult.h"
#include "KJVSearchCriteria.h"
#include "PersistentSettings.h"
#include "UserNotesDatabase.h"
#include "Highlighter.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QGridLayout>
#include <QSplitter>
#include <QSizePolicy>
#include <QFontDatabase>
#include <QListWidget>
#include <QListWidgetItem>
#include <QwwColorButton>
#include <QCheckBox>
#include <QLineEdit>

// ============================================================================

CHighlighterColorButtonSignalReflector::CHighlighterColorButtonSignalReflector(CKJVTextFormatConfig *pConfigurator, const QString &strUserDefinedHighlighterName)
	:	QObject(NULL),
		m_strUserDefinedHighlighterName(strUserDefinedHighlighterName)
{
	connect(this, SIGNAL(colorPicked(const QString &, const QColor &)), pConfigurator, SLOT(en_HighlighterColorPicked(const QString &, const QColor &)));
	connect(this, SIGNAL(colorClicked(const QString &)), pConfigurator, SLOT(en_HighlighterColorClicked(const QString &)));
	connect(this, SIGNAL(enableChanged(const QString &, bool)), pConfigurator, SLOT(en_HighlighterEnableChanged(const QString &, bool)));
}

CHighlighterColorButtonSignalReflector::~CHighlighterColorButtonSignalReflector()
{

}

void CHighlighterColorButtonSignalReflector::en_colorPicked(const QColor &color)
{
	emit colorPicked(m_strUserDefinedHighlighterName, color);
}

void CHighlighterColorButtonSignalReflector::en_clicked()
{
	emit colorClicked(m_strUserDefinedHighlighterName);
}

void CHighlighterColorButtonSignalReflector::en_enableClicked(bool bEnabled)
{
	emit enableChanged(m_strUserDefinedHighlighterName, bEnabled);
}

// ============================================================================

class CHighlighterColorButton : public CHighlighterColorButtonSignalReflector, public QListWidgetItem
{
public:
	CHighlighterColorButton(CKJVTextFormatConfig *pConfigurator, QListWidget *pList, const QString &strUserDefinedHighlighterName);
	~CHighlighterColorButton();

protected slots:
	virtual void en_setTextBrightness(bool bInvert, int nBrightness);
	virtual void en_adjustDialogElementBrightnessChanged(bool bAdjust);

private:
	void setBrightness(bool bAdjust, bool bInvert, int nBrightness);

private:
	QWidget *m_pWidget;
	QHBoxLayout *m_pHorzLayout;
	QwwColorButton *m_pColorButton;
	QCheckBox *m_pEnableCheckbox;
};

CHighlighterColorButton::CHighlighterColorButton(CKJVTextFormatConfig *pConfigurator, QListWidget *pList, const QString &strUserDefinedHighlighterName)
	:	CHighlighterColorButtonSignalReflector(pConfigurator, strUserDefinedHighlighterName),
		QListWidgetItem(pList, 0),
		m_pWidget(NULL),
		m_pHorzLayout(NULL),
		m_pColorButton(NULL),
		m_pEnableCheckbox(NULL)
{
	assert(pList != NULL);
	assert(g_pUserNotesDatabase != NULL);

	m_pWidget = new QWidget(pList);
	m_pWidget->setObjectName(QString("widget_%1").arg(strUserDefinedHighlighterName));

	m_pHorzLayout = new QHBoxLayout(m_pWidget);
	m_pHorzLayout->setObjectName(QString("hboxLayout_%1").arg(strUserDefinedHighlighterName));
	m_pHorzLayout->setMargin(0);
	m_pHorzLayout->setContentsMargins(0, 0, 0, 0);

	m_pColorButton = new QwwColorButton(m_pWidget);
	m_pColorButton->setObjectName(QString("buttonHighlighterColor_%1").arg(strUserDefinedHighlighterName));
	m_pColorButton->setShowName(false);			// Must do this before setting our real text
	m_pColorButton->setText(strUserDefinedHighlighterName);
	m_pColorButton->setCurrentColor(g_pUserNotesDatabase->highlighterColor(strUserDefinedHighlighterName));
	m_pColorButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_pColorButton->updateGeometry();
	m_pColorButton->setMinimumWidth(m_pColorButton->sizeHint().width());
	m_pHorzLayout->addWidget(m_pColorButton);

	m_pEnableCheckbox = new QCheckBox(m_pWidget);
	m_pEnableCheckbox->setObjectName(QString("checkbox_%1").arg(strUserDefinedHighlighterName));
	m_pEnableCheckbox->setCheckable(true);
	m_pEnableCheckbox->setChecked(g_pUserNotesDatabase->highlighterEnabled(strUserDefinedHighlighterName));
	m_pEnableCheckbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_pEnableCheckbox->setText(tr("Enable"));
	m_pEnableCheckbox->setToolTip(tr("Enable/Disable this highlighter"));
	m_pEnableCheckbox->updateGeometry();
	m_pHorzLayout->addWidget(m_pEnableCheckbox);

	setBrightness(CPersistentSettings::instance()->adjustDialogElementBrightness(), CPersistentSettings::instance()->invertTextBrightness(), CPersistentSettings::instance()->textBrightness());
	connect(CPersistentSettings::instance(), SIGNAL(changedTextBrightness(bool, int)), this, SLOT(en_setTextBrightness(bool, int)));
	connect(CPersistentSettings::instance(), SIGNAL(adjustDialogElementBrightnessChanged(bool)), this, SLOT(en_adjustDialogElementBrightnessChanged(bool)));

	m_pHorzLayout->addStretch(0);

	m_pWidget->setLayout(m_pHorzLayout);
	m_pWidget->updateGeometry();
	setSizeHint(m_pWidget->sizeHint());
	pList->setItemWidget(this, m_pWidget);

	connect(m_pColorButton, SIGNAL(colorPicked(const QColor &)), this, SLOT(en_colorPicked(const QColor &)));
	connect(m_pColorButton, SIGNAL(clicked()), this, SLOT(en_clicked()));
	connect(m_pEnableCheckbox, SIGNAL(clicked(bool)), this, SLOT(en_enableClicked(bool)));
}

CHighlighterColorButton::~CHighlighterColorButton()
{

}

void CHighlighterColorButton::en_setTextBrightness(bool bInvert, int nBrightness)
{
	setBrightness(CPersistentSettings::instance()->adjustDialogElementBrightness(), bInvert, nBrightness);
}

void CHighlighterColorButton::en_adjustDialogElementBrightnessChanged(bool bAdjust)
{
	setBrightness(bAdjust, CPersistentSettings::instance()->invertTextBrightness(), CPersistentSettings::instance()->textBrightness());
}

void CHighlighterColorButton::setBrightness(bool bAdjust, bool bInvert, int nBrightness)
{
	QColor clrBackground = CPersistentSettings::textBackgroundColor(bInvert, nBrightness);
	QColor clrForeground = CPersistentSettings::textForegroundColor(bInvert, nBrightness);

	if (bAdjust) {
		if (!bInvert) {
			m_pEnableCheckbox->setStyleSheet(QString("QCheckBox { background-color:%1; color:%2; }\n"
													)
													.arg(clrBackground.name())
													.arg(clrForeground.name()));

		} else {
			m_pEnableCheckbox->setStyleSheet(QString("QCheckBox { background-color:%1; color:%2; }\n"
													 "QCheckBox::indicator {\n"
													 "    color: %2;\n"
													 "    background-color: %1;\n"
													 "    border: 1px solid %2;\n"
													 "    width: 9px;\n"
													 "    height: 9px;\n"
													 "}\n"
													 "QCheckBox::indicator:checked {\n"
													 "    image:url(:/res/checkbox.png);\n"
													 "}\n"
													)
													.arg(clrBackground.name())
													.arg(clrForeground.name()));
		}
	} else {
		m_pEnableCheckbox->setStyleSheet(QString());
	}
}

// ============================================================================

CKJVTextFormatConfig::CKJVTextFormatConfig(CBibleDatabasePtr pBibleDatabase, QWidget *parent) :
	QWidget(parent),
	//	m_pBibleDatabase(pBibleDatabase),
	m_previewSearchPhrase(pBibleDatabase),
	m_pSearchResultsTreeView(NULL),
	m_pScriptureBrowser(NULL),
	m_bIsDirty(false),
	ui(new Ui::CKJVTextFormatConfig)
{
	assert(pBibleDatabase != NULL);
	assert(g_pUserNotesDatabase != NULL);

	ui->setupUi(this);

	// --------------------------------------------------------------

	//	Swapout the treeViewSearchResultsPreview from the layout with
	//		one that we can set the database on:

	int ndx = ui->splitter->indexOf(ui->treeViewSearchResultsPreview);
	assert(ndx != -1);
	if (ndx == -1) return;

	m_pSearchResultsTreeView = new CSearchResultsTreeView(pBibleDatabase, this);
	m_pSearchResultsTreeView->setObjectName(QString::fromUtf8("treeViewSearchResultsPreview"));
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(5);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(m_pSearchResultsTreeView->sizePolicy().hasHeightForWidth());
	m_pSearchResultsTreeView->setSizePolicy(sizePolicy1);
	m_pSearchResultsTreeView->setContextMenuPolicy(Qt::NoContextMenu);
	m_pSearchResultsTreeView->setToolTip(tr("Search Results Preview"));

	delete ui->treeViewSearchResultsPreview;
	ui->treeViewSearchResultsPreview = NULL;
	ui->splitter->insertWidget(ndx, m_pSearchResultsTreeView);

	// --------------------------------------------------------------

	//	Swapout the textScriptureBrowserPreview from the layout with
	//		one that we can set the database on:

	ndx = ui->splitter->indexOf(ui->textScriptureBrowserPreview);
	assert(ndx != -1);
	if (ndx == -1) return;

	m_pScriptureBrowser = new CScriptureBrowser(pBibleDatabase, this);
	m_pScriptureBrowser->setObjectName(QString::fromUtf8("textScriptureBrowserPreview"));
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy2.setHorizontalStretch(10);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(m_pScriptureBrowser->sizePolicy().hasHeightForWidth());
	m_pScriptureBrowser->setSizePolicy(sizePolicy2);
	m_pScriptureBrowser->setMouseTracking(true);
	m_pScriptureBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_pScriptureBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pScriptureBrowser->setTabChangesFocus(true);
	m_pScriptureBrowser->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
	m_pScriptureBrowser->setOpenLinks(false);
	m_pScriptureBrowser->setContextMenuPolicy(Qt::NoContextMenu);
	m_pScriptureBrowser->setToolTip(tr("Scripture Browser Preview"));			// Note:  Also disables the "Press Ctrl-D" tooltip, since that mode isn't enable in the configurator

	delete ui->textScriptureBrowserPreview;
	ui->textScriptureBrowserPreview = NULL;
	ui->splitter->insertWidget(ndx, m_pScriptureBrowser);

	// --------------------------------------------------------------

	delete ui->buttonWordsOfJesusColor;
	delete ui->buttonSearchResultsColor;
	delete ui->buttonCursorFollowColor;

	ui->buttonWordsOfJesusColor = new QwwColorButton(this);
	ui->buttonWordsOfJesusColor->setObjectName(QString::fromUtf8("buttonWordsOfJesusColor"));
	toQwwColorButton(ui->buttonWordsOfJesusColor)->setShowName(false);			// Must do this before setting our real text
	ui->buttonWordsOfJesusColor->setText(tr("Words of Jesus"));
	ui->buttonWordsOfJesusColor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	ui->vertLayoutColorOptions->addWidget(ui->buttonWordsOfJesusColor);

	ui->buttonSearchResultsColor = new QwwColorButton(this);
	ui->buttonSearchResultsColor->setObjectName(QString::fromUtf8("buttonSearchResultsColor"));
	toQwwColorButton(ui->buttonSearchResultsColor)->setShowName(false);			// Must do this before setting our real text
	ui->buttonSearchResultsColor->setText(tr("Search Results"));
	ui->buttonSearchResultsColor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	ui->vertLayoutColorOptions->addWidget(ui->buttonSearchResultsColor);

	ui->buttonCursorFollowColor = new QwwColorButton(this);
	ui->buttonCursorFollowColor->setObjectName(QString::fromUtf8("buttonCursorFollowColor"));
	toQwwColorButton(ui->buttonCursorFollowColor)->setShowName(false);			// Must do this before setting our real text
	ui->buttonCursorFollowColor->setText(tr("Cursor Tracker"));
	ui->buttonCursorFollowColor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	ui->vertLayoutColorOptions->addWidget(ui->buttonCursorFollowColor);

	toQwwColorButton(ui->buttonWordsOfJesusColor)->setCurrentColor(CPersistentSettings::instance()->colorWordsOfJesus());
	toQwwColorButton(ui->buttonSearchResultsColor)->setCurrentColor(CPersistentSettings::instance()->colorSearchResults());
	toQwwColorButton(ui->buttonCursorFollowColor)->setCurrentColor(CPersistentSettings::instance()->colorCursorFollow());

	connect(toQwwColorButton(ui->buttonWordsOfJesusColor), SIGNAL(colorPicked(const QColor &)), this, SLOT(en_WordsOfJesusColorPicked(const QColor &)));
	connect(toQwwColorButton(ui->buttonSearchResultsColor), SIGNAL(colorPicked(const QColor &)), this, SLOT(en_SearchResultsColorPicked(const QColor &)));
	connect(toQwwColorButton(ui->buttonCursorFollowColor), SIGNAL(colorPicked(const QColor &)), this, SLOT(en_CursorTrackerColorPicked(const QColor &)));

	ui->listWidgetHighlighterColors->setSelectionMode(QAbstractItemView::NoSelection);

	const TUserDefinedColorMap &mapHighlighters = g_pUserNotesDatabase->highlighterDefinitionsMap();
	for (TUserDefinedColorMap::const_iterator itrHighlighters = mapHighlighters.constBegin(); itrHighlighters != mapHighlighters.constEnd(); ++itrHighlighters) {
		new CHighlighterColorButton(this, ui->listWidgetHighlighterColors, itrHighlighters.key());
		ui->comboBoxHighlighters->addItem(itrHighlighters.key());
	}
	recalcColorListWidth();

	ui->comboBoxHighlighters->clearEditText();
	ui->comboBoxHighlighters->lineEdit()->setMaxLength(MAX_HIGHLIGHTER_NAME_SIZE);
	ui->toolButtonAddHighlighter->setEnabled(false);
	ui->toolButtonRemoveHighlighter->setEnabled(false);

	connect(ui->comboBoxHighlighters, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(en_comboBoxHighlightersTextChanged(const QString &)));
	connect(ui->comboBoxHighlighters, SIGNAL(editTextChanged(const QString &)), this, SLOT(en_comboBoxHighlightersTextChanged(const QString &)));
	connect(ui->comboBoxHighlighters, SIGNAL(enterPressed()), ui->toolButtonAddHighlighter, SLOT(click()));

	connect(ui->toolButtonAddHighlighter, SIGNAL(clicked()), this, SLOT(en_addHighlighterClicked()));
	connect(ui->toolButtonRemoveHighlighter, SIGNAL(clicked()), this, SLOT(en_removeHighlighterClicked()));

	connect(ui->listWidgetHighlighterColors, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(en_currentColorListViewItemChanged(QListWidgetItem*, QListWidgetItem*)));

	// --------------------------------------------------------------

	// Reinsert them in the correct TabOrder:
	QWidget::setTabOrder(ui->toolButtonRemoveHighlighter, ui->buttonWordsOfJesusColor);
	QWidget::setTabOrder(ui->buttonWordsOfJesusColor, ui->buttonSearchResultsColor);
	QWidget::setTabOrder(ui->buttonSearchResultsColor, ui->buttonCursorFollowColor);
	QWidget::setTabOrder(ui->buttonCursorFollowColor, m_pSearchResultsTreeView);
	QWidget::setTabOrder(m_pSearchResultsTreeView, m_pScriptureBrowser);

	// --------------------------------------------------------------

	m_fntScriptureBrowser = CPersistentSettings::instance()->fontScriptureBrowser();
	m_fntSearchResults = CPersistentSettings::instance()->fontSearchResults();

	ui->fontComboBoxScriptureBrowser->setCurrentFont(m_fntScriptureBrowser);
	ui->fontComboBoxSearchResults->setCurrentFont(m_fntSearchResults);

	QList<int> lstStandardFontSizes = QFontDatabase::standardSizes();
	int nFontMin = -1;
	int nFontMax = -1;
	for (int ndx=0; ndx<lstStandardFontSizes.size(); ++ndx) {
		if ((nFontMin == -1) || (lstStandardFontSizes.at(ndx) < nFontMin)) nFontMin = lstStandardFontSizes.at(ndx);
		if ((nFontMax == -1) || (lstStandardFontSizes.at(ndx) > nFontMax)) nFontMax = lstStandardFontSizes.at(ndx);
	}
	ui->dblSpinBoxScriptureBrowserFontSize->setRange(nFontMin, nFontMax);
	ui->dblSpinBoxScriptureBrowserFontSize->setValue(m_fntScriptureBrowser.pointSizeF());
	ui->dblSpinBoxSearchResultsFontSize->setRange(nFontMin, nFontMax);
	ui->dblSpinBoxSearchResultsFontSize->setValue(m_fntSearchResults.pointSizeF());

	connect(ui->fontComboBoxScriptureBrowser, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(en_ScriptureBrowserFontChanged(const QFont &)));
	connect(ui->fontComboBoxSearchResults, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(en_SearchResultsFontChanged(const QFont &)));
	connect(ui->dblSpinBoxScriptureBrowserFontSize, SIGNAL(valueChanged(double)), this, SLOT(en_ScriptureBrowserFontSizeChanged(double)));
	connect(ui->dblSpinBoxSearchResultsFontSize, SIGNAL(valueChanged(double)), this, SLOT(en_SearchResultsFontSizeChanged(double)));

	// --------------------------------------------------------------

	m_bInvertTextBrightness = CPersistentSettings::instance()->invertTextBrightness();
	m_nTextBrightness = CPersistentSettings::instance()->textBrightness();
	m_bAdjustDialogElementBrightness = CPersistentSettings::instance()->adjustDialogElementBrightness();

	ui->checkBoxInvertTextBrightness->setChecked(m_bInvertTextBrightness);
	ui->horzSliderTextBrigtness->setValue(m_nTextBrightness);
	ui->checkBoxAdjustDialogElementBrightness->setChecked(m_bAdjustDialogElementBrightness);

	connect(ui->checkBoxInvertTextBrightness, SIGNAL(clicked(bool)), this, SLOT(en_InvertTextBrightnessChanged(bool)));
	connect(ui->horzSliderTextBrigtness, SIGNAL(valueChanged(int)), this, SLOT(en_TextBrightnessChanged(int)));
	connect(ui->checkBoxAdjustDialogElementBrightness, SIGNAL(clicked(bool)), this, SLOT(en_AdjustDialogElementBrightness(bool)));

	// --------------------------------------------------------------

	navigateToDemoText();
	setPreview();
}

CKJVTextFormatConfig::~CKJVTextFormatConfig()
{
	delete ui;
}

void CKJVTextFormatConfig::saveSettings()
{
	CPersistentSettings::instance()->setFontScriptureBrowser(m_fntScriptureBrowser);
	CPersistentSettings::instance()->setFontSearchResults(m_fntSearchResults);
	CPersistentSettings::instance()->setInvertTextBrightness(m_bInvertTextBrightness);
	CPersistentSettings::instance()->setTextBrightness(m_nTextBrightness);
	CPersistentSettings::instance()->setAdjustDialogElementBrightness(m_bAdjustDialogElementBrightness);
	m_bIsDirty = false;
}

void CKJVTextFormatConfig::en_ScriptureBrowserFontChanged(const QFont &font)
{
	m_fntScriptureBrowser.setFamily(font.family());
	m_pScriptureBrowser->setFont(m_fntScriptureBrowser);
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_SearchResultsFontChanged(const QFont &font)
{
	m_fntSearchResults.setFamily(font.family());
	m_pSearchResultsTreeView->setFontSearchResults(m_fntSearchResults);
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_ScriptureBrowserFontSizeChanged(double nFontSize)
{
	m_fntScriptureBrowser.setPointSizeF(nFontSize);
	m_pScriptureBrowser->setFont(m_fntScriptureBrowser);
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_SearchResultsFontSizeChanged(double nFontSize)
{
	m_fntSearchResults.setPointSizeF(nFontSize);
	m_pSearchResultsTreeView->setFontSearchResults(m_fntSearchResults);
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_InvertTextBrightnessChanged(bool bInvert)
{
	m_bInvertTextBrightness = bInvert;
	setPreview();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_TextBrightnessChanged(int nBrightness)
{
	m_nTextBrightness = nBrightness;
	setPreview();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_AdjustDialogElementBrightness(bool bAdjust)
{
	m_bAdjustDialogElementBrightness = bAdjust;
	setPreview();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_WordsOfJesusColorPicked(const QColor &color)
{
	CPersistentSettings::instance()->setColorWordsOfJesus(color);
	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_SearchResultsColorPicked(const QColor &color)
{
	CPersistentSettings::instance()->setColorSearchResults(color);
	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_CursorTrackerColorPicked(const QColor &color)
{
	CPersistentSettings::instance()->setColorCursorFollow(color);
//	setPreview();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_HighlighterColorPicked(const QString &strUserDefinedHighlighterName, const QColor &color)
{
	assert(g_pUserNotesDatabase != NULL);
	assert(g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName));
	g_pUserNotesDatabase->setHighlighterColor(strUserDefinedHighlighterName, color);
	recalcColorListWidth();			// If color was previously invalid and is now valid, we'll have a preview to paint and so the width can change
	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
	en_HighlighterColorClicked(strUserDefinedHighlighterName);
}

void CKJVTextFormatConfig::en_HighlighterColorClicked(const QString &strUserDefinedHighlighterName)
{
	ui->comboBoxHighlighters->setEditText(strUserDefinedHighlighterName);
}

void CKJVTextFormatConfig::en_HighlighterEnableChanged(const QString &strUserDefinedHighlighterName, bool bEnabled)
{
	assert(g_pUserNotesDatabase != NULL);
	assert(g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName));
	g_pUserNotesDatabase->setHighlighterEnabled(strUserDefinedHighlighterName, bEnabled);
	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
	en_HighlighterColorClicked(strUserDefinedHighlighterName);
}

void CKJVTextFormatConfig::en_comboBoxHighlightersTextChanged(const QString &strUserDefinedHighlighterName)
{
	assert(g_pUserNotesDatabase != NULL);
	ui->toolButtonAddHighlighter->setEnabled(!strUserDefinedHighlighterName.trimmed().isEmpty() && !g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName.trimmed()) &&
													(strUserDefinedHighlighterName.size() <= MAX_HIGHLIGHTER_NAME_SIZE));
	ui->toolButtonRemoveHighlighter->setEnabled(!strUserDefinedHighlighterName.trimmed().isEmpty() && g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName.trimmed()));
}

void CKJVTextFormatConfig::en_addHighlighterClicked()
{
	assert(g_pUserNotesDatabase != NULL);
	QString strUserDefinedHighlighterName = ui->comboBoxHighlighters->currentText().trimmed();
	assert(!strUserDefinedHighlighterName.isEmpty() && !g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName));
	if ((strUserDefinedHighlighterName.isEmpty()) || (g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName))) return;
	if (strUserDefinedHighlighterName.size() > MAX_HIGHLIGHTER_NAME_SIZE) return;

	g_pUserNotesDatabase->setHighlighterColor(strUserDefinedHighlighterName, QColor());
	new CHighlighterColorButton(this, ui->listWidgetHighlighterColors, strUserDefinedHighlighterName);
	ui->comboBoxHighlighters->addItem(strUserDefinedHighlighterName);
	// Note: ComboBox text might change above, so use currentText() here, not strUserDefinedHighlighterName:
	en_comboBoxHighlightersTextChanged(ui->comboBoxHighlighters->currentText());		// Update add/remove controls

	recalcColorListWidth();

	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_removeHighlighterClicked()
{
	assert(g_pUserNotesDatabase != NULL);
	QString strUserDefinedHighlighterName = ui->comboBoxHighlighters->currentText().trimmed();
	assert(!strUserDefinedHighlighterName.isEmpty() && g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName));
	if ((strUserDefinedHighlighterName.isEmpty()) || (!g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName))) return;

	g_pUserNotesDatabase->removeHighlighter(strUserDefinedHighlighterName);
	assert(!g_pUserNotesDatabase->existsHighlighter(strUserDefinedHighlighterName));
	int nComboIndex = ui->comboBoxHighlighters->findText(strUserDefinedHighlighterName);
	assert(nComboIndex != -1);
	if (nComboIndex != -1) {
		ui->comboBoxHighlighters->removeItem(nComboIndex);
	}
	// Note: ComboBox text might change above, so use currentText() here, not strUserDefinedHighlighterName:
	en_comboBoxHighlightersTextChanged(ui->comboBoxHighlighters->currentText());		// Update add/remove controls

	int nListWidgetIndex = -1;
	for (int ndx = 0; ndx < ui->listWidgetHighlighterColors->count(); ++ndx) {
		if (static_cast<CHighlighterColorButton *>(ui->listWidgetHighlighterColors->item(ndx))->highlighterName().compare(strUserDefinedHighlighterName) == 0) {
			nListWidgetIndex = ndx;
			break;
		}
	}
	assert(nListWidgetIndex != -1);
	if (nListWidgetIndex != -1) {
		QListWidgetItem *pItem = ui->listWidgetHighlighterColors->takeItem(nListWidgetIndex);
		delete pItem;
	}

	recalcColorListWidth();

	navigateToDemoText();
	m_bIsDirty = true;
	emit dataChanged();
}

void CKJVTextFormatConfig::en_currentColorListViewItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious)
{
	Q_UNUSED(pPrevious);
	if (pCurrent != NULL) {
		CHighlighterColorButton *pButtonItem = static_cast<CHighlighterColorButton *>(pCurrent);
		en_HighlighterColorClicked(pButtonItem->highlighterName());
	}
}

void CKJVTextFormatConfig::recalcColorListWidth()
{
	ui->listWidgetHighlighterColors->setMinimumWidth(0);
	ui->listWidgetHighlighterColors->updateGeometry();
	int nWidth = 0;
	for (int ndx = 0; ndx < ui->listWidgetHighlighterColors->count(); ++ndx) {
		QWidget *pButton = ui->listWidgetHighlighterColors->itemWidget(ui->listWidgetHighlighterColors->item(ndx));
		pButton->updateGeometry();
		pButton->setMinimumWidth(pButton->sizeHint().width());
		ui->listWidgetHighlighterColors->item(ndx)->setSizeHint(pButton->sizeHint());
		nWidth = qMax(nWidth, pButton->sizeHint().width());
	}
	ui->listWidgetHighlighterColors->setMinimumWidth(nWidth + ui->listWidgetHighlighterColors->verticalScrollBar()->sizeHint().width() + ui->listWidgetHighlighterColors->spacing()*2 );
	updateGeometry();
	adjustSize();
}

void CKJVTextFormatConfig::navigateToDemoText()
{
	CRelIndex ndxPreview(41, 9, 1, 1);						// Goto Mark 9:1 for Preview (as it has some red-letter text)
	CRelIndex ndxPreview2(41, 9, 3, 1);						// Goto Mark 9:3 for additional Search Results highlight so we can get all combinations of highlighters...
	m_pScriptureBrowser->navigator().setDocumentToChapter(ndxPreview);
	m_pScriptureBrowser->navigator().selectWords(TPhraseTag(ndxPreview));
	m_pScriptureBrowser->navigator().doHighlighting(CSearchResultHighlighter(TPhraseTag(ndxPreview, 5)));
	m_pScriptureBrowser->navigator().doHighlighting(CSearchResultHighlighter(TPhraseTag(ndxPreview2, 32)));

	uint32_t nNormalizedIndex = m_pSearchResultsTreeView->vlmodel()->bibleDatabase()->NormalizeIndex(ndxPreview) + 10;

	for (int i = 0; i < 3; ++i) {
		// Originally I had a const_iterator over the g_pUserNotesDatabase->highlighterDefinitionsMap, but something
		//		in the process of doHighlighting kept invalidating our iterator and causing an infinite loop.
		//		Solution was to iterate over the buttons in our QListWidget of Highlighter Set buttons.. <sigh>
		for (int ndx = 0; ndx < ui->listWidgetHighlighterColors->count(); ++ndx) {
			CHighlighterColorButton *pColorButton = static_cast<CHighlighterColorButton *>(ui->listWidgetHighlighterColors->item(ndx));
			assert(pColorButton != NULL);
			if (pColorButton == NULL) continue;
			if (!g_pUserNotesDatabase->highlighterDefinition(pColorButton->highlighterName()).isValid()) continue;
			if (!g_pUserNotesDatabase->highlighterEnabled(pColorButton->highlighterName())) continue;
			m_pScriptureBrowser->navigator().doHighlighting(CUserDefinedHighlighter(pColorButton->highlighterName(),
															TPhraseTag(CRelIndex(m_pSearchResultsTreeView->vlmodel()->bibleDatabase()->DenormalizeIndex(nNormalizedIndex)), 5)));
			nNormalizedIndex += 7;
		}
	}

	m_previewSearchPhrase.ParsePhrase("trumpet");
	m_previewSearchPhrase.FindWords(m_previewSearchPhrase.GetCursorWordPos());
	TParsedPhrasesList lstPhrases;
	lstPhrases.append(&m_previewSearchPhrase);
	CSearchCriteria aSearchCriteria;
	m_pSearchResultsTreeView->setParsedPhrases(aSearchCriteria, lstPhrases);
	m_pSearchResultsTreeView->setDisplayMode(CVerseListModel::VDME_RICHTEXT);
	m_pSearchResultsTreeView->setTreeMode(CVerseListModel::VTME_TREE_CHAPTERS);
	m_pSearchResultsTreeView->expandAll();
}

void CKJVTextFormatConfig::setPreview()
{
	m_pSearchResultsTreeView->setTextBrightness(m_bInvertTextBrightness, m_nTextBrightness);

	m_pScriptureBrowser->setTextBrightness(m_bInvertTextBrightness, m_nTextBrightness);
}

// ============================================================================

CKJVConfiguration::CKJVConfiguration(CBibleDatabasePtr pBibleDatabase, QWidget *parent)
	:	QwwConfigWidget(parent)
{
	assert(pBibleDatabase != NULL);

	m_pTextFormatConfig = new CKJVTextFormatConfig(pBibleDatabase, this);

	addGroup(m_pTextFormatConfig, QIcon(":res/Font_Graphics_Color_Icon_128.png"), "Text Color and Fonts");
	setCurrentGroup(m_pTextFormatConfig);

	connect(m_pTextFormatConfig, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CKJVConfiguration::~CKJVConfiguration()
{

}

void CKJVConfiguration::saveSettings()
{
	m_pTextFormatConfig->saveSettings();
}

// ============================================================================

CKJVConfigurationDialog::CKJVConfigurationDialog(CBibleDatabasePtr pBibleDatabase, QWidget *parent) :
	QDialog(parent),
	m_pConfiguration(NULL),
	m_pButtonBox(NULL)
{
	assert(pBibleDatabase != NULL);
	assert(g_pUserNotesDatabase != NULL);

	// --------------------------------------------------------------

	// Make a working copy of our settings:
	CPersistentSettings::instance()->togglePersistentSettingData(true);
	g_pUserNotesDatabase->toggleUserNotesDatabaseData(true);

	// --------------------------------------------------------------

	QVBoxLayout *pLayout = new QVBoxLayout(this);
	pLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	m_pConfiguration = new CKJVConfiguration(pBibleDatabase, this);
	m_pConfiguration->setObjectName(QString::fromUtf8("configurationWidget"));
	pLayout->addWidget(m_pConfiguration);

	m_pButtonBox = new QDialogButtonBox(this);
	m_pButtonBox->setObjectName(QString::fromUtf8("buttonBox"));
	m_pButtonBox->setOrientation(Qt::Horizontal);
	m_pButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
	m_pButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
	pLayout->addWidget(m_pButtonBox);

	m_pConfiguration->setMinimumWidth(m_pConfiguration->sizeHint().width());
	updateGeometry();

	connect(m_pButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_pButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(m_pButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

	connect(m_pConfiguration, SIGNAL(dataChanged()), this, SLOT(en_dataChanged()));
}

CKJVConfigurationDialog::~CKJVConfigurationDialog()
{

}

void CKJVConfigurationDialog::en_dataChanged()
{
	updateGeometry();
	m_pButtonBox->button(QDialogButtonBox::Apply)->setEnabled(m_pConfiguration->isDirty());
}

void CKJVConfigurationDialog::accept()
{
	m_pConfiguration->saveSettings();
	QDialog::accept();
	// Note: Leave the settings permanent, by not copying
	//		them back in the persistent settings object
}

void CKJVConfigurationDialog::reject()
{
	assert(g_pUserNotesDatabase != NULL);

	// Restore original settings by switching back to the original
	//		settings without copying:
	CPersistentSettings::instance()->togglePersistentSettingData(false);
	g_pUserNotesDatabase->toggleUserNotesDatabaseData(false);
	QDialog::reject();
}

void CKJVConfigurationDialog::apply()
{
	assert(g_pUserNotesDatabase != NULL);

	// Make sure our persistent settings have been updated, and we'll
	//		copy the settings over to the original, making them permanent
	//		as the user is "applying" them:
	m_pConfiguration->saveSettings();
	CPersistentSettings::instance()->togglePersistentSettingData(true);
	g_pUserNotesDatabase->toggleUserNotesDatabaseData(true);
	en_dataChanged();
}

// ============================================================================
