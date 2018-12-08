// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QObject>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QSystemTrayIcon;
#ifdef USE_DBUS
class QDBusInterface;
#endif
QT_END_NAMESPACE

class Notificator: public QObject
{
    Q_OBJECT

public:
    // Ownership of trayIcon is not transferred to this object.
    Notificator(const QString &programName=QString(), QSystemTrayIcon *trayIcon=0, QWidget *parent=0);
    ~Notificator();

    enum Class
    {
        Information, Warning, Critical
    };

public slots:
    void notify(Class cls, const QString &title, const QString &text, const QIcon &icon = QIcon(), int millisTimeout = 10000);

private:
    QWidget *parent;

    enum Mode {
        None, Freedesktop, QSystemTray, Growl12, Growl13, UserNotificationCenter
    };

    QString programName;
    Mode mode;
    QSystemTrayIcon *trayIcon;

#ifdef USE_DBUS
    QDBusInterface *interface;

    void notifyDBus(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#endif

    void notifySystray(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);

#ifdef Q_OS_MAC
    void notifyGrowl(Class cls, const QString &title, const QString &text, const QIcon &icon);
    void notifyMacUserNotificationCenter(Class cls, const QString &title, const QString &text, const QIcon &icon);
#endif
};

#endif // NOTIFICATOR_H
