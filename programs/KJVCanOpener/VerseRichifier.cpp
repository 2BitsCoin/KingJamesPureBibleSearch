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

#include "VerseRichifier.h"
#include "dbstruct.h"
#include "ParseSymbols.h"

#include "PersistentSettings.h"
#include "PhraseEdit.h"
#include "Highlighter.h"

#define OUTPUT_HEBREW_PS119 1
#define PSALMS_BOOK_NUM 19

// ============================================================================
// ============================================================================

static QString psalm119HebrewPrefix(const CRelIndex &ndx, bool bAddAnchors)
{
	if ((ndx.book() != PSALMS_BOOK_NUM) || (ndx.chapter() != 119) || (((ndx.verse()-1)%8) != 0)) return QString();

	QString strHebrewPrefix;

	// Add special Start tag so KJVBrowser can know to ignore the special Hebrew text insertion during highlighting:
	if (bAddAnchors) strHebrewPrefix += QString("<a id=\"\"A%1\"\"> </a>").arg(ndx.asAnchor());

#if (OUTPUT_HEBREW_PS119)
	switch ((ndx.verse()-1)/8) {
		case 0:
			// ALEPH
			strHebrewPrefix += QChar(0x005D0);
			break;
		case 1:
			// BETH
			strHebrewPrefix += QChar(0x005D1);
			break;
		case 2:
			// GIMEL
			strHebrewPrefix += QChar(0x005D2);
			break;
		case 3:
			// DALETH
			strHebrewPrefix += QChar(0x005D3);
			break;
		case 4:
			// HE
			strHebrewPrefix += QChar(0x005D4);
			break;
		case 5:
			// VAU
			strHebrewPrefix += QChar(0x005D5);
			break;
		case 6:
			// ZAIN
			strHebrewPrefix += QChar(0x005D6);
			break;
		case 7:
			// CHETH
			strHebrewPrefix += QChar(0x005D7);
			break;
		case 8:
			// TETH
			strHebrewPrefix += QChar(0x005D8);
			break;
		case 9:
			// JOD
			strHebrewPrefix += QChar(0x005D9);
			break;
		case 10:
			// CAPH
			strHebrewPrefix += QChar(0x005DB);		// Using nonfinal-CAPH
			break;
		case 11:
			// LAMED
			strHebrewPrefix += QChar(0x005DC);
			break;
		case 12:
			// MEM
			strHebrewPrefix += QChar(0x005DE);		// Using nonfinal-Mem
			break;
		case 13:
			// NUN
			strHebrewPrefix += QChar(0x005E0);		// Using nonfinal-Nun
			break;
		case 14:
			// SAMECH
			strHebrewPrefix += QChar(0x005E1);
			break;
		case 15:
			// AIN
			strHebrewPrefix += QChar(0x005E2);
			break;
		case 16:
			// PE
			strHebrewPrefix += QChar(0x005E4);		// Using nonfinal-Pe
			break;
		case 17:
			// TZADDI
			strHebrewPrefix += QChar(0x005E6);		// Using nonfinal-Tzaddi
			break;
		case 18:
			// KOPH
			strHebrewPrefix += QChar(0x005E7);
			break;
		case 19:
			// RESH
			strHebrewPrefix += QChar(0x005E8);
			break;
		case 20:
			// SCHIN
			strHebrewPrefix += QChar(0x005E9);
			break;
		case 21:
			// TAU
			strHebrewPrefix += QChar(0x005EA);
			break;
	}
	strHebrewPrefix += " ";
#endif

	switch ((ndx.verse()-1)/8) {
		case 0:
			strHebrewPrefix += "(ALEPH).";
			break;
		case 1:
			strHebrewPrefix += "(BETH).";
			break;
		case 2:
			strHebrewPrefix += "(GIMEL).";
			break;
		case 3:
			strHebrewPrefix += "(DALETH).";
			break;
		case 4:
			strHebrewPrefix += "(HE).";
			break;
		case 5:
			strHebrewPrefix += "(VAU).";
			break;
		case 6:
			strHebrewPrefix += "(ZAIN).";
			break;
		case 7:
			strHebrewPrefix += "(CHETH).";
			break;
		case 8:
			strHebrewPrefix += "(TETH).";
			break;
		case 9:
			strHebrewPrefix += "(JOD).";
			break;
		case 10:
			strHebrewPrefix += "(CAPH).";
			break;
		case 11:
			strHebrewPrefix += "(LAMED).";
			break;
		case 12:
			strHebrewPrefix += "(MEM).";
			break;
		case 13:
			strHebrewPrefix += "(NUN).";
			break;
		case 14:
			strHebrewPrefix += "(SAMECH).";
			break;
		case 15:
			strHebrewPrefix += "(AIN).";
			break;
		case 16:
			strHebrewPrefix += "(PE).";
			break;
		case 17:
			strHebrewPrefix += "(TZADDI).";
			break;
		case 18:
			strHebrewPrefix += "(KOPH).";
			break;
		case 19:
			strHebrewPrefix += "(RESH).";
			break;
		case 20:
			strHebrewPrefix += "(SCHIN).";
			break;
		case 21:
			strHebrewPrefix += "(TAU).";
			break;
	}

	if (bAddAnchors) {
		// Add special End tag so KJVBrowser can know to ignore the special Hebrew text insertion during highlighting:
		strHebrewPrefix += QString("<a id=\"\"B%1\"\"> </a>").arg(ndx.asAnchor());
	} else {
		// Otherwise, just add a space:
		strHebrewPrefix += " ";
	}

	return strHebrewPrefix;
}

// ============================================================================
// ============================================================================

CVerseTextRichifier::CVerseTextRichifier(const QChar &chrMatchChar, const QString &strXlateText, const CVerseTextRichifier *pRichNext)
	:	m_pRichNext(pRichNext),
		m_chrMatchChar(chrMatchChar),
		m_pVerse(NULL),
		m_strXlateText(strXlateText),
		m_bAddAnchors(false)
{

}

CVerseTextRichifier::CVerseTextRichifier(const QChar &chrMatchChar, const CVerseEntry *pVerse, const CVerseTextRichifier *pRichNext, bool bAddAnchors)
	:	m_pRichNext(pRichNext),
		m_chrMatchChar(chrMatchChar),
		m_pVerse(pVerse),
		m_bAddAnchors(bAddAnchors)
{
	assert(pVerse != NULL);
}

CVerseTextRichifier::~CVerseTextRichifier()
{

}

void CVerseTextRichifier::parse(CRichifierBaton &parseBaton, const QString &strNodeIn) const
{
	if (m_chrMatchChar.isNull()) {
		parseBaton.m_strVerseText.append(strNodeIn);
		return;
	}

	QStringList lstSplit;

	if (m_pVerse != NULL) {
		lstSplit.reserve(m_pVerse->m_nNumWrd + 1);
		lstSplit = parseBaton.m_strTemplate.split(m_chrMatchChar);
		assert(static_cast<unsigned int>(lstSplit.size()) == (m_pVerse->m_nNumWrd + 1));
		assert(strNodeIn.isNull());
	} else {
		lstSplit = strNodeIn.split(m_chrMatchChar);
	}
	assert(lstSplit.size() != 0);

	for (int i=0; i<lstSplit.size(); ++i) {
		if (m_pVerse != NULL) {
			parseBaton.m_bOutput = (static_cast<unsigned int>(i) >= parseBaton.m_nStartWord);
			if ((parseBaton.m_pWordCount != NULL) && ((*parseBaton.m_pWordCount) == 0)) parseBaton.m_bOutput = false;
		}
		if (i > 0) {
			if (m_pVerse != NULL) {
#ifdef OSIS_PARSER_BUILD
				QString strWord = m_pVerse->m_lstRichWords.at(i-1);
#else
				QString strWord = parseBaton.m_pBibleDatabase->wordAtIndex(m_pVerse->m_nWrdAccum + i);
#endif
				parseBaton.m_ndxCurrent.setWord(i);
				if (parseBaton.m_bOutput) {
					if (m_bAddAnchors) parseBaton.m_strVerseText.append(QString("<a id=\"%1\">").arg(parseBaton.m_ndxCurrent.asAnchor()));
				}
				if (!parseBaton.m_strDivineNameFirstLetterParseText.isEmpty()) {
					if (parseBaton.m_bOutput) {
						parseBaton.m_strVerseText.append(strWord.left(1)
														+ parseBaton.m_strDivineNameFirstLetterParseText
														+ strWord.mid(1));
					}
					parseBaton.m_strDivineNameFirstLetterParseText.clear();
				} else {
					if (parseBaton.m_bOutput) parseBaton.m_strVerseText.append(strWord);
				}
				if (parseBaton.m_bOutput) {
					if (m_bAddAnchors) parseBaton.m_strVerseText.append("</a>");
				}
				if ((parseBaton.m_bOutput) && (parseBaton.m_pWordCount != NULL) && ((*parseBaton.m_pWordCount) > 0)) --(*parseBaton.m_pWordCount);
			} else {
				if (m_chrMatchChar == QChar('D')) {
					parseBaton.m_strDivineNameFirstLetterParseText = m_strXlateText;
				} else if (m_chrMatchChar == QChar('R')) {
					assert(parseBaton.m_pHighlighter != NULL);
					// Note: for searchResult, we always have to check the intersection and handle
					//		enter/exit of m_bInSearchResult since we are called to parse twice -- once
					//		for the begin tags and once for the end tags.  Otherwise we don't know when
					//		to start/stop and which to output:
					CRelIndex ndxWord = parseBaton.m_ndxCurrent;
					ndxWord.setWord(ndxWord.word()+1);
					if ((parseBaton.m_bOutput) &&
						(!parseBaton.m_bInSearchResult) &&
						(parseBaton.m_pHighlighter->intersects(parseBaton.m_pBibleDatabase, TPhraseTag(ndxWord)))) {
						parseBaton.m_strVerseText.append(m_strXlateText);
						parseBaton.m_bInSearchResult = true;
					}
				} else if (m_chrMatchChar == QChar('r')) {
					assert(parseBaton.m_pHighlighter != NULL);
					CRelIndex ndxWord = parseBaton.m_ndxCurrent;
					ndxWord.setWord(ndxWord.word()+1);
					if ((parseBaton.m_bOutput) &&
						(parseBaton.m_bInSearchResult) &&
						((!parseBaton.m_pHighlighter->isContinuous()) ||
							(!parseBaton.m_pHighlighter->intersects(parseBaton.m_pBibleDatabase, TPhraseTag(ndxWord))))) {
						parseBaton.m_strVerseText.append(m_strXlateText);
						parseBaton.m_bInSearchResult = false;
					}
				} else if (m_chrMatchChar == QChar('N')) {
					CRelIndex ndxWord = parseBaton.m_ndxCurrent;
					ndxWord.setWord(ndxWord.word()+1);
					if (ndxWord.word() > 1) parseBaton.m_strVerseText.append(' ');
					parseBaton.m_strVerseText.append(m_strXlateText);		// Opening '('
					const CFootnoteEntry *pFootnote = parseBaton.m_pBibleDatabase->footnoteEntry(ndxWord);
					assert(pFootnote != NULL);
					parseBaton.m_strVerseText.append(pFootnote->text());
				} else if (m_chrMatchChar == QChar('n')) {
					parseBaton.m_strVerseText.append(m_strXlateText);		// Closing ')'
					parseBaton.m_strVerseText.append(' ');			// Add separator.  Note that we will trim baton whitespace at the end anyway
				} else {
					if (parseBaton.m_bOutput) parseBaton.m_strVerseText.append(m_strXlateText);
				}
			}
		}
		if (m_pRichNext) {
			m_pRichNext->parse(parseBaton, lstSplit.at(i));
		} else {
			if (parseBaton.m_bOutput) parseBaton.m_strVerseText.append(lstSplit.at(i));
		}
	}
}

QString CVerseTextRichifier::parse(const CRelIndex &ndxRelative, const CBibleDatabase *pBibleDatabase, const CVerseEntry *pVerse,
										const CVerseTextRichifierTags &tags, bool bAddAnchors, int *pWordCount, const CBasicHighlighter *pHighlighter)
{
	assert(pBibleDatabase != NULL);
	assert(pVerse != NULL);

	CRelIndex ndxRelVerse(ndxRelative.book(), ndxRelative.chapter(), ndxRelative.verse(), 0);	// Relative as verse only

	// Note: While it would be most optimum to reverse this and
	//		do the verse last so we don't have to call the entire
	//		tree for every word, we can't reverse it because doing
	//		so then creates sub-lists of 'w' tags and then we
	//		no longer know where we are in the list:
	CVerseTextRichifier rich_r('r', tags.searchResultsEnd());
	CVerseTextRichifier rich_R('R', tags.searchResultsBegin(), &rich_r);
	CVerseTextRichifier rich_n('n', tags.inlineNoteEnd(), &rich_R);
	CVerseTextRichifier rich_N('N', tags.inlineNoteBegin(), &rich_n);
	CVerseTextRichifier rich_d('d', tags.divineNameEnd(), &rich_N);
	CVerseTextRichifier rich_D('D', tags.divineNameBegin(), &rich_d);				// D/d must be last for font start/stop to work correctly with special first-letter text mode
	CVerseTextRichifier rich_t('t', tags.transChangeAddedEnd(), &rich_D);
	CVerseTextRichifier rich_T('T', tags.transChangeAddedBegin(), &rich_t);
	CVerseTextRichifier rich_j('j', tags.wordsOfJesusEnd(), &rich_T);
	CVerseTextRichifier rich_J('J', tags.wordsOfJesusBegin(), &rich_j);
	CVerseTextRichifier rich_M('M', (tags.addRichPs119HebrewPrefix() ? psalm119HebrewPrefix(ndxRelVerse, bAddAnchors) : ""), &rich_J);
	CVerseTextRichifier richVerseText('w', pVerse, &rich_M, bAddAnchors);

	QString strTemplate = pVerse->m_strTemplate;
	if ((pHighlighter != NULL) &&
		(pHighlighter->enabled())) {
		strTemplate.replace(QChar('w'), "Rwr");
	}

	CRichifierBaton baton(pBibleDatabase, ndxRelative, strTemplate, pWordCount, pHighlighter);
	if (((pVerse->m_nPilcrow == CVerseEntry::PTE_MARKER) || (pVerse->m_nPilcrow == CVerseEntry::PTE_MARKER_ADDED)) &&
		(ndxRelative.word() <= 1) &&
		(tags.showPilcrowMarkers())) {
		baton.m_strVerseText.append(g_chrPilcrow);
		baton.m_strVerseText.append(QChar(' '));
	}
	richVerseText.parse(baton);

	return baton.m_strVerseText.trimmed();
}

// ============================================================================
// ============================================================================

void CVerseTextRichifierTags::setFromPersistentSettings(const CPersistentSettings &aPersistentSettings, bool bCopyOptions)
{
	setWordsOfJesusTagsByColor(aPersistentSettings.colorWordsOfJesus());
	setSearchResultsTagsByColor(aPersistentSettings.colorSearchResults());

	if (bCopyOptions) {
		switch (aPersistentSettings.transChangeAddWordMode()) {
			case CPhraseNavigator::TCAWME_NO_MARKING:
				setTransChangeAddedTags(QString(), QString());
				break;
			case CPhraseNavigator::TCAWME_ITALICS:
				setTransChangeAddedTags(QString("<i>"), QString("</i>"));
				break;
			case CPhraseNavigator::TCAWME_BRACKETS:
				setTransChangeAddedTags(QString("["), QString("]"));
				break;
			default:
				assert(false);
				break;
		}
	}

	setShowPilcrowMarkers(bCopyOptions ? aPersistentSettings.copyPilcrowMarkers() : aPersistentSettings.showPilcrowMarkers());
}

// ============================================================================
// ============================================================================

