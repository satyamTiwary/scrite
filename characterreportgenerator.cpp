/****************************************************************************
**
** Copyright (C) Prashanth Udupa, Bengaluru
** Email: prashanth.udupa@gmail.com
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "characterreportgenerator.h"

#include <QTextTable>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextBlockFormat>

CharacterReportGenerator::CharacterReportGenerator(QObject *parent)
    : AbstractReportGenerator(parent),
      m_includeDialogues(true),
      m_includeSceneHeadings(true)
{

}

CharacterReportGenerator::~CharacterReportGenerator()
{

}

void CharacterReportGenerator::setCharacterNames(const QStringList &val)
{
    if(m_characterNames == val)
        return;

    m_characterNames = val;
    emit characterNamesChanged();
}

void CharacterReportGenerator::setIncludeSceneHeadings(bool val)
{
    if(m_includeSceneHeadings == val)
        return;

    m_includeSceneHeadings = val;
    emit includeSceneHeadingsChanged();
}

void CharacterReportGenerator::setIncludeDialogues(bool val)
{
    if(m_includeDialogues == val)
        return;

    m_includeDialogues = val;
    emit includeDialoguesChanged();
}

bool CharacterReportGenerator::doGenerate(QPdfWriter *pdfWriter)
{
    if(m_characterNames.isEmpty())
    {
        this->error()->setErrorMessage("No character was selected for report generation.");
        return false;
    }

    Screenplay *screenplay = this->document()->screenplay();

    QTextDocument document;
    QTextCursor cursor(&document);

    const QFont defaultFont = this->document()->formatting()->defaultFont();

    QTextBlockFormat defaultBlockFormat;

    QTextCharFormat defaultCharFormat;
    defaultCharFormat.setFontFamily(defaultFont.family());
    defaultCharFormat.setFontPointSize(11);

    this->progress()->setProgressStepFromCount(screenplay->elementCount()+2);

    // Report Title
    {
        QTextBlockFormat blockFormat = defaultBlockFormat;
        blockFormat.setAlignment(Qt::AlignHCenter);
        cursor.setBlockFormat(blockFormat);

        QTextCharFormat charFormat = defaultCharFormat;
        charFormat.setFontPointSize(18);
        charFormat.setFontCapitalization(QFont::AllUppercase);
        charFormat.setFontWeight(QFont::Bold);
        charFormat.setFontUnderline(true);
        cursor.setCharFormat(charFormat);

        QString title = screenplay->title();
        if(title.isEmpty())
            title = "Untitled Screenplay";
        cursor.insertText(title);

        blockFormat.setBottomMargin(20);

        cursor.insertBlock(blockFormat, charFormat);
        if(m_characterNames.size() == 1)
            cursor.insertText("Character Report For \"" + m_characterNames.first() + "\"");
        else
        {
            cursor.insertText("Characters Report For ");
            for(int i=0; i<m_characterNames.length(); i++)
            {
                if(i)
                    cursor.insertText(", ");
                cursor.insertText("\"" + m_characterNames.at(i) + "\"");
            }
        }

        blockFormat = defaultBlockFormat;
        blockFormat.setAlignment(Qt::AlignHCenter);
        blockFormat.setBottomMargin(20);
        charFormat = defaultCharFormat;
        cursor.insertBlock(blockFormat, charFormat);
        cursor.insertHtml("This report was generated using <strong>scrite</strong><br/>(<a href=\"https://scrite.teriflix.com\">https://scrite.teriflix.com</a>)");
    }
    this->progress()->tick();

    const int reportSummaryPosition = cursor.position();
    QMap<QString,int> dialogCount;

    // Report Detail
    {
        QTextBlockFormat blockFormat = defaultBlockFormat;
        blockFormat.setAlignment(Qt::AlignLeft);
        blockFormat.setTopMargin(20);

        QTextCharFormat charFormat = defaultCharFormat;
        charFormat.setFontPointSize(15);
        charFormat.setFontCapitalization(QFont::AllUppercase);
        charFormat.setFontWeight(QFont::Bold);
        charFormat.setFontItalic(true);

        cursor.insertBlock(blockFormat, charFormat);
        cursor.insertText("DETAIL:");

        const int nrScenes = screenplay->elementCount();
        for(int i=0; i<nrScenes; i++)
        {
            QTextTable *dialogueTable = nullptr;
            bool sceneInfoWritten = false;
            Scene *scene = screenplay->elementAt(i)->scene();
            const int nrElements = scene->elementCount();
            for(int j=0; j<nrElements; j++)
            {
                SceneElement *element = scene->elementAt(j);
                if(element->type() == SceneElement::Character)
                {
                    QString characterName = element->text();
                    characterName = characterName.section('(', 0, 0).trimmed();
                    if(m_characterNames.contains(characterName))
                    {
                        while(1)
                        {
                            ++j;
                            element = scene->elementAt(j);
                            if(element == nullptr || element->type() == SceneElement::Dialogue)
                                break;
                            if(element->type() != SceneElement::Parenthetical)
                                continue;
                        }

                        if(element == nullptr || element->type() != SceneElement::Dialogue)
                            continue;

                        if(sceneInfoWritten == false && m_includeSceneHeadings)
                        {
                            // Write Scene Information First
                            QTextBlockFormat blockFormat = defaultBlockFormat;
                            blockFormat.setLeftMargin(10);
                            if(!dialogCount.isEmpty())
                                blockFormat.setTopMargin(20);

                            QTextCharFormat charFormat = defaultCharFormat;
                            charFormat.setFontPointSize(12);
                            charFormat.setFontCapitalization(QFont::AllUppercase);
                            charFormat.setFontWeight(QFont::Bold);
                            charFormat.setFontItalic(false);

                            cursor.insertBlock(blockFormat, charFormat);
                            cursor.insertText("Scene [" + QString::number(i+1) + "]: " + scene->heading()->toString());
                            sceneInfoWritten = true;
                        }

                        if(m_includeDialogues)
                        {
                            // Write dialogue information next
                            if(dialogueTable == nullptr)
                            {
                                QTextTableFormat tableFormat;
                                tableFormat.setLeftMargin(20);
                                tableFormat.setCellSpacing(0);
                                tableFormat.setCellPadding(5);
                                tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);

                                dialogueTable = cursor.insertTable(1, 2, tableFormat);
                            }
                            else
                                dialogueTable->appendRows(1);

                            QTextBlockFormat blockFormat = defaultBlockFormat;

                            QTextCharFormat charFormat = defaultCharFormat;
                            charFormat.setFontCapitalization(QFont::AllUppercase);
                            charFormat.setFontWeight(QFont::Bold);

                            cursor = dialogueTable->cellAt(dialogueTable->rows()-1,0).firstCursorPosition();
                            cursor.setCharFormat(charFormat);
                            cursor.setBlockFormat(blockFormat);
                            cursor.insertText(characterName);

                            cursor = dialogueTable->cellAt(dialogueTable->rows()-1,1).firstCursorPosition();
                            blockFormat.setAlignment(Qt::AlignJustify);
                            charFormat = defaultCharFormat;
                            cursor.setCharFormat(charFormat);
                            cursor.setBlockFormat(blockFormat);
                            cursor.insertText(element->text());
                        }

                        dialogCount[characterName] = dialogCount.value(characterName,0)+1;
                    }
                }
            }

            cursor.movePosition(QTextCursor::End);
            this->progress()->tick();
        }
    }

    // Report Summary
    {
        cursor.setPosition(reportSummaryPosition);

        QTextBlockFormat blockFormat = defaultBlockFormat;
        blockFormat.setAlignment(Qt::AlignLeft);
        blockFormat.setTopMargin(20);

        QTextCharFormat charFormat = defaultCharFormat;
        charFormat.setFontPointSize(15);
        charFormat.setFontCapitalization(QFont::AllUppercase);
        charFormat.setFontWeight(QFont::Bold);
        charFormat.setFontItalic(true);

        cursor.insertBlock(blockFormat, charFormat);
        cursor.insertText("SUMMARY:");

        blockFormat = defaultBlockFormat;
        blockFormat.setLeftMargin(10);

        QMap<QString,int>::const_iterator it = dialogCount.constBegin();
        QMap<QString,int>::const_iterator end = dialogCount.constEnd();
        while(it != end)
        {
            charFormat = defaultCharFormat;
            charFormat.setFontWeight(QFont::Bold);
            charFormat.setFontCapitalization(QFont::AllUppercase);

            cursor.insertBlock(blockFormat, charFormat);
            cursor.insertText(it.key());

            charFormat = defaultCharFormat;
            cursor.setCharFormat(charFormat);
            cursor.insertText(" speaks " + QString::number(it.value()) + " times ");

            ++it;
        }
    }
    this->progress()->tick();

    document.print(pdfWriter);

    return true;
}