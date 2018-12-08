// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MACNOTIFICATIONHANDLER_H
#define MACNOTIFICATIONHANDLER_H

#include <QObject>

class MacNotificationHandler : public QObject
{
    Q_OBJECT

public:
    void showNotification(const QString &title, const QString &text);
    void sendAppleScript(const QString &script);
    bool hasUserNotificationCenterSupport(void);
    static MacNotificationHandler *instance();
};

#endif // MACNOTIFICATIONHANDLER_H
