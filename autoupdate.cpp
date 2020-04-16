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

#include "autoupdate.h"
#include "application.h"
#include "garbagecollector.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>

AutoUpdate *AutoUpdate::instance()
{
    static AutoUpdate *theInstance = new AutoUpdate(Application::instance());
    return theInstance;
}

AutoUpdate::AutoUpdate(QObject *parent)
    : QObject(parent)
{
    m_updateTimer.start(1000, this);
}

AutoUpdate::~AutoUpdate()
{

}

void AutoUpdate::setUrl(const QUrl &val)
{
    if(m_url == val)
        return;

    m_url = val;
    emit urlChanged();
}

QUrl AutoUpdate::updateDownloadUrl() const
{
    return QUrl(m_updateInfo.value("link").toString());
}

void AutoUpdate::setUpdateInfo(const QJsonObject &val)
{
    if(m_updateInfo == val)
        return;

    m_updateInfo = val;
    emit updateInfoChanged();
}

void AutoUpdate::checkForUpdates()
{
    static QNetworkAccessManager nam;
    connect(&nam, &QNetworkAccessManager::sslErrors, [](QNetworkReply *reply, const QList<QSslError> &errors) {
        reply->ignoreSslErrors(errors);
    });

    if(m_url.isEmpty() || !m_url.isValid())
        return;

    QNetworkRequest request(m_url);
    QNetworkReply *reply = nam.get(request);
    if(reply == nullptr)
        return;

    connect(reply, &QNetworkReply::finished, [reply,this]() {
        GarbageCollector::instance()->add(reply);

        qDebug() << reply->errorString();

        const QByteArray bytes = reply->readAll();
        if(bytes.isEmpty()) {
            this->checkForUpdatesAfterSometime();
            return;
        }

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(bytes);
        if(jsonDoc.isNull() || jsonDoc.isEmpty()) {
            this->checkForUpdatesAfterSometime();
            return;
        }

        const QJsonObject json = jsonDoc.object();
        this->lookForUpdates(json);
    });
}

void AutoUpdate::checkForUpdatesAfterSometime()
{
    // Check for updates after 1 hour
    m_updateTimer.start(60*60*1000, this);
}

void AutoUpdate::lookForUpdates(const QJsonObject &json)
{
    /**
      {
        "macos": {
            "version": "0.2.7",
            "versionString": "0.2.7-beta",
            "releaseDate": "....",
            "changeLog": "....",
            "link": "...."
        },
        "windows": {

        },
        "linux": {

        }
      }
      */

    QJsonObject info;
    switch(Application::instance()->platform())
    {
    case Application::MacOS:
        info = json.value("macos").toObject();
        break;
    case Application::LinuxDesktop:
        info = json.value("linux").toObject();
        break;
    case Application::WindowsDesktop:
        info = json.value("windows").toObject();
        break;
    }

    if(info.isEmpty())
    {
        this->checkForUpdatesAfterSometime();
        return;
    }

    const QVersionNumber updateVersion = QVersionNumber::fromString(info.value("version").toString());
    if(updateVersion.isNull())
    {
        this->checkForUpdatesAfterSometime();
        return;
    }

    if(updateVersion <= Application::instance()->versionNumber())
    {
        this->checkForUpdatesAfterSometime();
        return;
    }

    this->setUpdateInfo(info);
    // Dont check for updates until this update is used up.
}

void AutoUpdate::timerEvent(QTimerEvent *event)
{
    if(m_updateTimer.timerId() == event->timerId())
    {
        this->checkForUpdates();
        m_updateTimer.stop();
        return;
    }

    QObject::timerEvent(event);
}

