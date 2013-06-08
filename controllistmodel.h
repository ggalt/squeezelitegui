#ifndef CONTROLLISTMODEL_H
#define CONTROLLISTMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QDebug>
#include "squeezedefines.h"

class ControlListItem : public QObject
{
    Q_OBJECT
public:
  ControlListItem(const QString &name, QString img, QObject *parent = 0);
  ControlListItem(const QString &name, QString img, QString command, QObject *parent = 0);
  ~ControlListItem() {}

  QString name() const;
  QString image() const;
  QString command() const;

signals:
  void dataChanged();

private:
  QString m_name;
  QString m_img;
  QString m_command;
};


class ControlListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ControlItemRoles {
        NameRole = Qt::UserRole+1,
        ImageRole,
        CommandRole
    };

    ControlListModel(QObject *parent = 0);

    void addControlItem(ControlListItem *item);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void appendRow(ControlListItem* item);
    void appendRows(const QList<ControlListItem*> &items);
    void insertRow(int row, ControlListItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    ControlListItem* takeRow(int row);
    ControlListItem* find(const QString &name) const;
    ControlListItem* getRow(int row);
    QModelIndex indexFromItem( const ControlListItem* item) const;
    void clear();

signals:
    
public slots:
    void handleItemChange();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<ControlListItem *> m_list;
};

#endif // CONTROLLISTMODEL_H
