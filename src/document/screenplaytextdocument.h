/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SCREENPLAYTEXTDOCUMENT_H
#define SCREENPLAYTEXTDOCUMENT_H

#include <QTextDocument>
#include <QQmlParserStatus>
#include <QPagedPaintDevice>
#include <QAbstractTextDocumentLayout>

#include "scene.h"
#include "formatting.h"
#include "screenplay.h"
#include "qobjectproperty.h"

class ScreenplayTextDocument;
class AbstractScreenplayTextDocumentInjectionInterface
{
public:
    enum InjectLocation
    {
        AfterTitlePage,
        AfterSceneHeading,
        AfterLastScene,
        BeforeSceneElement,
        AfterSceneElement
    };
    virtual void inject(QTextCursor &, InjectLocation) { }
    virtual bool filterSceneElement() const { return false; }

    const ScreenplayElement *screenplayElement() const { return m_screenplayElement; }
    const SceneElement *sceneElement() const { return m_sceneElement; }

private:
    friend class ScreenplayTextDocument;
    void setScreenplayElement(const ScreenplayElement *element) {
        m_screenplayElement = element;
    }
    void setSceneElement(const SceneElement *element) {
        m_sceneElement = element;
    }

private:
    const ScreenplayElement *m_screenplayElement = nullptr;
    const SceneElement *m_sceneElement = nullptr;
};

#define AbstractScreenplayTextDocumentInjectionInterface_iid "io.scrite.AbstractScreenplayTextDocumentInjectionInterface"
Q_DECLARE_INTERFACE(AbstractScreenplayTextDocumentInjectionInterface, AbstractScreenplayTextDocumentInjectionInterface_iid)

class QQmlEngine;
class ScreenplayTextDocumentUpdate;
class ScreenplayTextDocument : public QObject,
                               public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    ScreenplayTextDocument(QObject *parent=nullptr);
    ScreenplayTextDocument(QTextDocument *document, QObject *parent=nullptr);
    ~ScreenplayTextDocument();

    Q_PROPERTY(QTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged RESET resetTextDocument)
    void setTextDocument(QTextDocument* val);
    QTextDocument* textDocument() const { return m_textDocument; }
    Q_SIGNAL void textDocumentChanged();

    Q_PROPERTY(Screenplay* screenplay READ screenplay WRITE setScreenplay NOTIFY screenplayChanged RESET resetScreenplay)
    void setScreenplay(Screenplay* val);
    Screenplay* screenplay() const { return m_screenplay; }
    Q_SIGNAL void screenplayChanged();

    Q_PROPERTY(ScreenplayFormat* formatting READ formatting WRITE setFormatting NOTIFY formattingChanged RESET resetFormatting)
    void setFormatting(ScreenplayFormat* val);
    ScreenplayFormat* formatting() const { return m_formatting; }
    Q_SIGNAL void formattingChanged();

    Q_PROPERTY(bool titlePage READ hasTitlePage WRITE setTitlePage NOTIFY titlePageChanged)
    void setTitlePage(bool val);
    bool hasTitlePage() const { return m_titlePage; }
    Q_SIGNAL void titlePageChanged();

    Q_PROPERTY(bool sceneNumbers READ hasSceneNumbers WRITE setSceneNumbers NOTIFY sceneNumbersChanged)
    void setSceneNumbers(bool val);
    bool hasSceneNumbers() const { return m_sceneNumbers; }
    Q_SIGNAL void sceneNumbersChanged();

    Q_PROPERTY(bool sceneIcons READ hasSceneIcons WRITE setSceneIcons NOTIFY sceneIconsChanged)
    void setSceneIcons(bool val);
    bool hasSceneIcons() const { return m_sceneIcons; }
    Q_SIGNAL void sceneIconsChanged();

    Q_PROPERTY(bool syncEnabled READ isSyncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
    void setSyncEnabled(bool val);
    bool isSyncEnabled() const { return m_syncEnabled; }
    Q_SIGNAL void syncEnabledChanged();

    Q_PROPERTY(bool listSceneCharacters READ isListSceneCharacters WRITE setListSceneCharacters NOTIFY listSceneCharactersChanged)
    void setListSceneCharacters(bool val);
    bool isListSceneCharacters() const { return m_listSceneCharacters; }
    Q_SIGNAL void listSceneCharactersChanged();

    Q_PROPERTY(QStringList highlightDialoguesOf READ highlightDialoguesOf WRITE setHighlightDialoguesOf NOTIFY highlightDialoguesOfChanged)
    void setHighlightDialoguesOf(QStringList val);
    QStringList highlightDialoguesOf() const { return m_highlightDialoguesOf; }
    Q_SIGNAL void highlightDialoguesOfChanged();

    Q_PROPERTY(bool includeSceneSynopsis READ isIncludeSceneSynopsis WRITE setIncludeSceneSynopsis NOTIFY includeSceneSynopsisChanged)
    void setIncludeSceneSynopsis(bool val);
    bool isIncludeSceneSynopsis() const { return m_includeSceneSynopsis; }
    Q_SIGNAL void includeSceneSynopsisChanged();

    enum Purpose { ForDisplay, ForPrinting };
    Q_ENUM(Purpose)
    Q_PROPERTY(Purpose purpose READ purpose WRITE setPurpose NOTIFY purposeChanged)
    void setPurpose(Purpose val);
    Purpose purpose() const { return m_purpose; }
    Q_SIGNAL void purposeChanged();

    Q_PROPERTY(bool printEachSceneOnANewPage READ isPrintEachSceneOnANewPage WRITE setPrintEachSceneOnANewPage NOTIFY printEachSceneOnANewPageChanged)
    void setPrintEachSceneOnANewPage(bool val);
    bool isPrintEachSceneOnANewPage() const { return m_printEachSceneOnANewPage; }
    Q_SIGNAL void printEachSceneOnANewPageChanged();

    Q_PROPERTY(bool updating READ isUpdating NOTIFY updatingChanged)
    bool isUpdating() const { return m_updating; }
    Q_SIGNAL void updatingChanged();

    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    int pageCount() const { return m_pageCount; }
    Q_SIGNAL void pageCountChanged();

    Q_PROPERTY(int currentPage READ currentPage NOTIFY currentPageChanged)
    int currentPage() const { return m_currentPage; }
    Q_SIGNAL void currentPageChanged();

    Q_INVOKABLE void print(QObject *printerObject);

    QList< QPair<int,int> > pageBreaksFor(ScreenplayElement *element) const;

    QList< QPair<int,int> > pageBoundaries() const { return m_pageBoundaries; }
    Q_SIGNAL void pageBoundariesChanged();

    Q_INVOKABLE qreal lengthInPixels(ScreenplayElement *element) const;
    Q_INVOKABLE qreal lengthInPages(ScreenplayElement *element) const;

    Q_PROPERTY(QObject* injection READ injection WRITE setInjection NOTIFY injectionChanged RESET resetInjection)
    void setInjection(QObject* val);
    QObject* injection() const { return m_injection; }
    Q_SIGNAL void injectionChanged();

    void syncNow();

signals:
    void updateScheduled();
    void updateStarted();
    void updateFinished();

protected:
    // QQmlParserStatus implementation
    void classBegin();
    void componentComplete();

    // QObject interface
    void timerEvent(QTimerEvent *event);

private:
    void init();
    void setUpdating(bool val);
    void setPageCount(int val);
    void setCurrentPage(int val);
    void resetFormatting();
    void resetTextDocument();

    void loadScreenplay();
    void includeMoreAndContdMarkers();
    void loadScreenplayLater();
    void resetScreenplay();

    void connectToScreenplaySignals();
    void connectToScreenplayFormatSignals();

    void disconnectFromScreenplaySignals();
    void disconnectFromScreenplayFormatSignals();

    void connectToSceneSignals(Scene *scene);
    void disconnectFromSceneSignals(Scene *scene);

    // When screenplay is being cleared
    void onScreenplayAboutToReset();
    void onScreenplayReset();

    // Hook to signals that convey change in sequencing of scenes
    void onSceneMoved(ScreenplayElement *ptr, int from, int to);
    void onSceneRemoved(ScreenplayElement *ptr, int index);
    void onSceneInserted(ScreenplayElement *element, int index);

    // Hook to signals that convey changes to a specific scene content
    void onSceneReset();
    void onSceneRefreshed();
    void onSceneAboutToReset();
    void onSceneHeadingChanged();
    void onSceneElementChanged(SceneElement *element, Scene::SceneElementChangeType type);
    void onSceneAboutToResetModel();
    void onSceneResetModel();

    // Hook to signals that convey change in formatting
    void onElementFormatChanged();
    void onDefaultFontChanged();
    void onFormatScreenChanged();
    void onFormatFontPointSizeDeltaChanged();

    // Hook to signals to know current element and cursor position,
    // so that we can report current page number.
    void onActiveSceneChanged();
    void onActiveSceneDestroyed(Scene *ptr);
    void onActiveSceneCursorPositionChanged();

    // Other methods
    void evaluateCurrentPage();
    void evaluatePageBoundaries();
    void evaluatePageBoundariesLater();
    void formatAllBlocks();
    void loadScreenplayElement(const ScreenplayElement *element, QTextCursor &cursor);
    void formatBlock(const QTextBlock &block, const QString &text=QString());

    void removeTextFrame(const ScreenplayElement *element);
    void registerTextFrame(const ScreenplayElement *element, QTextFrame *frame);
    QTextFrame *findTextFrame(const ScreenplayElement *element) const;
    void onTextFrameDestroyed(QObject *object);
    void clearTextFrames();

    void addToSceneResetList(Scene *scene);
    void processSceneResetList();

    void resetInjection();

private:
    int m_pageCount = 0;
    bool m_updating = false;
    bool m_titlePage = false;
    int m_currentPage = 0;
    bool m_sceneIcons = true;
    Purpose m_purpose = ForDisplay;
    bool m_syncEnabled = true;
    bool m_sceneNumbers = true;
    Scene *m_activeScene = nullptr;
    bool m_componentComplete = true;
    bool m_listSceneCharacters = false;
    bool m_includeSceneSynopsis = false;
    bool m_screenplayIsBeingReset = false;
    QList<Scene*> m_sceneResetList;
    ExecLaterTimer m_sceneResetTimer;
    bool m_printEachSceneOnANewPage = false;
    ExecLaterTimer m_loadScreenplayTimer;
    QStringList m_highlightDialoguesOf;
    ExecLaterTimer m_pageBoundaryEvalTimer;
    QTextFrameFormat m_sceneFrameFormat;
    QObjectProperty<QObject> m_injection;
    bool m_connectedToScreenplaySignals = false;
    bool m_connectedToFormattingSignals = false;
    QPagedPaintDevice::PageSize m_paperSize = QPagedPaintDevice::Letter;
    QList< QPair<int,int> > m_pageBoundaries;
    QObjectProperty<Screenplay> m_screenplay;
    friend class ScreenplayTextDocumentUpdate;
    QObjectProperty<QTextDocument> m_textDocument;
    QObjectProperty<ScreenplayFormat> m_formatting;
    ModificationTracker m_screenplayModificationTracker;
    ModificationTracker m_formattingModificationTracker;
    QMap<QObject *, const ScreenplayElement*> m_frameElementMap;
    QMap<const ScreenplayElement*, QTextFrame*> m_elementFrameMap;
};

class ScreenplayElementPageBreaks : public QObject
{
    Q_OBJECT

public:
    ScreenplayElementPageBreaks(QObject *parent=nullptr);
    ~ScreenplayElementPageBreaks();

    Q_PROPERTY(ScreenplayTextDocument* screenplayDocument READ screenplayDocument WRITE setScreenplayDocument NOTIFY screenplayDocumentChanged RESET resetScreenplayDocument)
    void setScreenplayDocument(ScreenplayTextDocument* val);
    ScreenplayTextDocument* screenplayDocument() const { return m_screenplayDocument; }
    Q_SIGNAL void screenplayDocumentChanged();

    Q_PROPERTY(ScreenplayElement* screenplayElement READ screenplayElement WRITE setScreenplayElement NOTIFY screenplayElementChanged RESET resetScreenplayElement)
    void setScreenplayElement(ScreenplayElement* val);
    ScreenplayElement* screenplayElement() const { return m_screenplayElement; }
    Q_SIGNAL void screenplayElementChanged();

    Q_PROPERTY(QVariantList pageBreaks READ pageBreaks NOTIFY pageBreaksChanged)
    QVariantList pageBreaks() const { return m_pageBreaks; }
    Q_SIGNAL void pageBreaksChanged();

private:
    void resetScreenplayDocument();
    void resetScreenplayElement();
    void updatePageBreaks();
    void setPageBreaks(const QVariantList &val);

private:
    QVariantList m_pageBreaks;
    QObjectProperty<ScreenplayElement> m_screenplayElement;
    QObjectProperty<ScreenplayTextDocument> m_screenplayDocument;
};

class ScreenplayTitlePageObjectInterface : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    ScreenplayTitlePageObjectInterface(QObject *parent=nullptr);
    ~ScreenplayTitlePageObjectInterface();

    enum { Kind=QTextFormat::UserObject+2 };
    enum Property
    {
        ScreenplayProperty = QTextFormat::UserProperty+10,
    };

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
};

class ScreenplayTextObjectInterface : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    ScreenplayTextObjectInterface(QObject *parent=nullptr);
    ~ScreenplayTextObjectInterface();

    enum { Kind=QTextFormat::UserObject+1 };
    enum Type { SceneNumberType, MoreMarkerType, ContdMarkerType, SceneIconType };
    enum Property
    {
        TypeProperty = QTextFormat::UserProperty+1,
        DataProperty
    };

    // QTextObjectInterface interface
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

private:
    void drawSceneNumber(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawMoreMarker(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawSceneIcon(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawText(QPainter *painter, const QRectF &rect, const QString &text);
};

#endif // SCREENPLAYTEXTDOCUMENT_H
