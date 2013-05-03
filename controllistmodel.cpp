#include "controllistmodel.h"

ControlListItem::ControlListItem(const QString &name, QString img, QObject *parent) : QObject(parent),
    m_name(name), m_img(img)
{

}

ControlListItem::ControlListItem(const QString &name, QString img, QString command, QObject *parent) :
    QObject(parent), m_name(name), m_img(img), m_command(command)
{

}

QString ControlListItem::name() const
{
    return m_name;
}

QString ControlListItem::image() const
{
    return m_img;
}

QString ControlListItem::command() const
{
    return m_command;
}

/*
 *
 *
 *
 */


ControlListModel::ControlListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void ControlListModel::addControlItem(ControlListItem *item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list << item;
    endInsertRows();
}

int ControlListModel::rowCount(const QModelIndex &parent) const
{
    return m_list.count();
}

QVariant ControlListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
      return QVariant();

    const ControlListItem *item = m_list[index.row()];
    if (role == NameRole)
        return item->name();
    else if (role == ImageRole)
        return item->image();
    else if (role == CommandRole)
        return item->command();
    return QVariant();

}

void ControlListModel::appendRow(ControlListItem* item)
{
    appendRows(QList<ControlListItem*>() << item);
}

void ControlListModel::appendRows(const QList<ControlListItem*> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(ControlListItem *item, items) {
      connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
      m_list.append(item);
    }
    endInsertRows();
}

void ControlListModel::insertRow(int row, ControlListItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.insert(row, item);
    endInsertRows();
}

void ControlListModel::handleItemChange()
{
  ControlListItem* item = static_cast<ControlListItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

bool ControlListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool ControlListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) > m_list.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
      delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

ControlListItem* ControlListModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    ControlListItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

ControlListItem* ControlListModel::find(const QString &name) const
{
    foreach(ControlListItem* item, m_list) {
      if(item->name() == name) return item;
    }
    return 0;
}
ControlListItem* ControlListModel::getRow(int row)
{
    ControlListItem *item=m_list.at(row);
    return item;
}

QModelIndex ControlListModel::indexFromItem( const ControlListItem* item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
      if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}
void ControlListModel::clear()
{
    qDeleteAll(m_list);
    m_list.clear();
}

QHash<int, QByteArray> ControlListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ImageRole] = "image";
    roles[CommandRole] = "command";
    return roles;
}
