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

#ifndef LOCATIONSCREENPLAYREPORT_H
#define LOCATIONSCREENPLAYREPORT_H

#include "abstractscreenplaysubsetreport.h"

class LocationScreenplayReport : public AbstractScreenplaySubsetReport
{
    Q_OBJECT
    Q_CLASSINFO("Title", "Location Screenplay")
    Q_CLASSINFO("Description", "Generates screenplay with only those scenes at one or more locations.")

public:
    Q_INVOKABLE LocationScreenplayReport(QObject *parent=nullptr);
    ~LocationScreenplayReport();

    Q_CLASSINFO("locations_FieldGroup", "Locations")
    Q_CLASSINFO("locations_FieldLabel", "Locations")
    Q_CLASSINFO("locations_FieldEditor", "MultipleLocationSelector")
    Q_PROPERTY(QStringList locations READ locations WRITE setLocations NOTIFY locationsChanged)
    void setLocations(const QStringList &val);
    QStringList locations() const { return m_locations; }
    Q_SIGNAL void locationsChanged();

protected:
    // AbstractScreenplaySubsetReport interface
    bool includeScreenplayElement(const ScreenplayElement *) const;
    QString screenplaySubtitle() const;
    void configureScreenplayTextDocument(ScreenplayTextDocument &stDoc);

    // AbstractScreenplayTextDocumentInjectionInterface interface
    void inject(QTextCursor &, InjectLocation);

private:
    int m_summaryLocation = -1;
    QStringList m_locations;
    mutable QMap< QString, QList<const ScreenplayElement *> > m_locationSceneNumberList;
};

#endif // LOCATIONSCREENPLAYREPORT_H
