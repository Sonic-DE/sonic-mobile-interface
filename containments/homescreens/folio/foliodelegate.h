// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QEnableSharedFromThis>
#include <QObject>
#include <QSharedPointer>

#include "folioapplication.h"
#include "folioapplicationfolder.h"
#include "foliowidget.h"
#include "homescreen.h"

class HomeScreen;
class FolioApplication;
class FolioApplicationFolder;
class FolioWidget;

class FolioDelegate : public QObject, public QEnableSharedFromThis<FolioDelegate>
{
    Q_OBJECT
    Q_PROPERTY(FolioDelegate::Type type READ type CONSTANT)
    Q_PROPERTY(FolioApplication *application READ applicationRaw CONSTANT)
    Q_PROPERTY(FolioApplicationFolder *folder READ folderRaw CONSTANT)
    Q_PROPERTY(FolioWidget *widget READ widgetRaw CONSTANT)

public:
    typedef QSharedPointer<FolioDelegate> Ptr;

    enum Type {
        None,
        Application,
        Folder,
        Widget,
    };
    Q_ENUM(Type)

    FolioDelegate(HomeScreen *parent = nullptr);
    FolioDelegate(QSharedPointer<FolioApplication> application, HomeScreen *parent);
    FolioDelegate(QSharedPointer<FolioApplicationFolder> folder, HomeScreen *parent);
    FolioDelegate(QSharedPointer<FolioWidget> widget, HomeScreen *parent);

    static QSharedPointer<FolioDelegate> fromJson(QJsonObject &obj, HomeScreen *parent);

    virtual QJsonObject toJson() const;

    FolioDelegate::Type type() const;

    QSharedPointer<FolioApplication> application();
    FolioApplication *applicationRaw();

    QSharedPointer<FolioApplicationFolder> folder();
    FolioApplicationFolder *folderRaw();

    QSharedPointer<FolioWidget> widget();
    FolioWidget *widgetRaw();

protected:
    FolioDelegate::Type m_type;
    QSharedPointer<FolioApplication> m_application{nullptr};
    QSharedPointer<FolioApplicationFolder> m_folder{nullptr};
    QSharedPointer<FolioWidget> m_widget{nullptr};
};

class FolioPageDelegate : public FolioDelegate
{
    Q_OBJECT
    Q_PROPERTY(int row READ row NOTIFY rowChanged)
    Q_PROPERTY(int column READ column NOTIFY columnChanged)
    QML_UNCREATABLE("")

public:
    typedef QSharedPointer<FolioPageDelegate> Ptr;

    FolioPageDelegate(int row = 0, int column = 0, HomeScreen *parent = nullptr);
    FolioPageDelegate(int row, int column, QSharedPointer<FolioApplication> application, HomeScreen *parent);
    FolioPageDelegate(int row, int column, QSharedPointer<FolioApplicationFolder> folder, HomeScreen *parent);
    FolioPageDelegate(int row, int column, QSharedPointer<FolioWidget> widget, HomeScreen *parent);
    FolioPageDelegate(int row, int column, QSharedPointer<FolioDelegate> delegate, HomeScreen *parent);

    static QSharedPointer<FolioPageDelegate> fromJson(QJsonObject &obj, HomeScreen *parent);
    static int getTranslatedTopLeftRow(HomeScreen *homeScreen, int realRow, int realColumn, QSharedPointer<FolioDelegate> fd);
    static int getTranslatedTopLeftColumn(HomeScreen *homeScreen, int realRow, int realColumn, QSharedPointer<FolioDelegate> fd);
    static int getTranslatedRow(HomeScreen *homeScreen, int realRow, int realColumn);
    static int getTranslatedColumn(HomeScreen *homeScreen, int realRow, int realColumn);

    QJsonObject toJson() const override;

    int row();
    void setRow(int row);

    int column();
    void setColumn(int column);

    QSharedPointer<FolioPageDelegate> sharedPageDelegate();

Q_SIGNALS:
    void rowChanged();
    void columnChanged();

private:
    void setRowOnly(int row);
    void setColumnOnly(int column);
    void init();

    HomeScreen *m_homeScreen{nullptr};

    int m_realRow;
    int m_realColumn;
    int m_row;
    int m_column;
};
