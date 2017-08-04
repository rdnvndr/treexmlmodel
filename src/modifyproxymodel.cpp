#include "modifyproxymodel.h"
#include "treexmlhashmodel.h"
#include <QFont>

namespace RTPTechGroup {
namespace XmlModel {

struct PrivateModelIndex
{
    qint32 r, c;
    void *p;
    const QAbstractItemModel *m;
};

ModifyProxyModel::ModifyProxyModel(QObject *parent)
    :QAbstractProxyModel (parent)
{
    m_hiddenRow = true;
    m_editable  = false;
}

bool ModifyProxyModel::submitAll()
{
    // Удаление строк
    QHashIterator<QPersistentModelIndex,
            QList<QPersistentModelIndex> > iterRemovedParentIndex(m_removedRow);
    while (iterRemovedParentIndex.hasNext())
    {
        iterRemovedParentIndex.next();
        QListIterator<QPersistentModelIndex> iterRemovedIndex(
                    iterRemovedParentIndex.value());
        while (iterRemovedIndex.hasNext()){
            QPersistentModelIndex index = iterRemovedIndex.next();
            if (!m_hiddenRow) {
                beginRemoveRows(mapFromSource(index.parent()),
                                index.row(),index.row());
                sourceModel()->removeRow(index.row(),index.parent());
                endRemoveRows();
            } else
                sourceModel()->removeRow(index.row(),index.parent());
        }
    }
    m_removedRow.clear();

    // Вставка строк
    QHashIterator<QPersistentModelIndex,
            QList<QPersistentModelIndex *> > iterParentList(m_insertedRow);
    while (iterParentList.hasNext())
    {
        iterParentList.next();
        QPersistentModelIndex parent = iterParentList.key();
        if (isInsertRow(parent))
            continue;

        insertSourceRows(parent);
    }

    // Модификация строк
    QHashIterator<QPersistentModelIndex,
            QHash<qint32, QVariant> > iterIndex(m_updatedRow);
    while (iterIndex.hasNext())
    {
        iterIndex.next();
        QModelIndex index = iterIndex.key();
        QHashIterator<qint32, QVariant> iterRole(iterIndex.value());
        while (iterRole.hasNext()) {
            iterRole.next();
            QVariant value = iterRole.value();
            qint32 role = iterRole.key();
            sourceModel()->setData(mapToSource(index),value,role);
        }
        m_updatedRow[index].clear();
    }
    m_updatedRow.clear();

    return true;
}

void ModifyProxyModel::revertAll()
{
    // Отмена вставки строк
    QHashIterator<QPersistentModelIndex,
            QList<QPersistentModelIndex *> > iterParentList(m_insertedRow);
    while (iterParentList.hasNext())
    {
        iterParentList.next();
        QPersistentModelIndex parent = iterParentList.key();
        if (parent.isValid()) {
            qint32 count = m_insertedRow[parent].count();
            removeRows(rowCount(parent)-count,count,parent);
        }
        m_insertedRow[parent].clear();
        m_insertedRow.remove(parent);
    }

    // Отмена модификации строк
    QHashIterator<QPersistentModelIndex,
            QHash<qint32, QVariant> > iterIndex(m_updatedRow);
    while (iterIndex.hasNext())
    {
        iterIndex.next();
        QModelIndex index = iterIndex.key();
        m_updatedRow[index].clear();
        emit dataChanged(index, index);
    }
    m_updatedRow.clear();

    // Отмена удаления строк
    if (!m_removedRow.isEmpty()) {
        m_removedRow.clear();
        emit layoutChanged();
    }
}

void ModifyProxyModel::sourceDataChanged(const QModelIndex &left,
                                         const QModelIndex &right)
{
    for(qint32 column = left.column(); column <= right.column(); ++column)
        for(qint32 row = left.row(); row <= right.row(); ++row) {
            void *p = left.sibling(row, column).internalPointer();
            QPersistentModelIndex removeIndex(createIndex(row,column,p));
            if (m_updatedRow.contains(removeIndex)) {
                m_updatedRow.remove(removeIndex);
            }
        }

    emit dataChanged(mapFromSource(left), mapFromSource(right));
}

void ModifyProxyModel::sourceRowsRemoved(const QModelIndex &parent,
                                         qint32 start, qint32 end)
{
    if (parent.isValid())
        for (qint32 i = start; i < end+1; ++i)
            if (m_srcRemovedRow.contains(parent))
                if (m_srcRemovedRow[parent].contains(i)) {
                    beginRemoveRows(mapFromSource(parent),i,i);
                    endRemoveRows();
                    m_srcRemovedRow[parent].remove(i);
                    if (m_srcRemovedRow[parent].isEmpty())
                        m_srcRemovedRow.remove(parent);
                }
}

void ModifyProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent,
                                                  qint32 start, qint32 end)
{
    if (parent.isValid())
        for (qint32 i = start; i < end+1; ++i) {
            QPersistentModelIndex index = sourceModel()->index(i,0,parent);
            for (qint32 j = 0; j < sourceModel()->columnCount(parent);++j) {
                QPersistentModelIndex removeIndex(mapFromSource(index.sibling(i,j)));
                if (m_updatedRow.contains(removeIndex)) {
                    m_updatedRow.remove(removeIndex);
                }
            }
            if (m_removedRow.contains(QPersistentModelIndex(index.parent())))
                if (m_removedRow[index.parent()].contains(index)) {
                    m_removedRow[index.parent()].removeOne(index);
                    continue;
                }
            m_srcRemovedRow[parent].insert(i);
        }
}

void ModifyProxyModel::sourceRowsInserted(const QModelIndex &parent, qint32 start, qint32 end)
{
    beginInsertRows(mapFromSource(parent), start, end);
    endInsertRows();
}

bool ModifyProxyModel::insertSourceRows(const QPersistentModelIndex &parent,
                                        const QPersistentModelIndex &sourceParent)
{
    qint32 count = m_insertedRow[parent].count();
    QPersistentModelIndex srcParent = (sourceParent.isValid())?
                sourceParent : QPersistentModelIndex(mapToSource(parent));

    TreeXmlHashModel *xmlModel = qobject_cast<TreeXmlHashModel *>(sourceModel());
    for (qint32 i = 0; i < count; ++i) {
        bool isInserted = false;
        qint32 row = sourceModel()->rowCount(srcParent);

        QPersistentModelIndex indexProxy(
                    index((sourceParent.isValid())? row: row+i,
                          0,
                          parent));

        qint32 lastRow = row;
        // Обработка TreeXMLModel
        if (xmlModel) {
            if (m_updatedRow.contains(indexProxy))
                if (m_updatedRow[indexProxy].contains(TreeXmlModel::TagRole))
                {

                    QModelIndex lastIndexRow =
                            xmlModel->insertLastRows(lastRow,1,srcParent,
                                                     m_updatedRow[indexProxy][TreeXmlModel::TagRole].toString());
                    isInserted = lastIndexRow.isValid();
                    lastRow = lastIndexRow.row();
                }
        } else
            isInserted = sourceModel()->insertRow(lastRow, srcParent);

        QPersistentModelIndex indexSource;
        if (isInserted) {
            indexSource = QPersistentModelIndex(sourceModel()->index(
                                                    lastRow, 0, srcParent));
            // Модификация потомков
            if (m_insertedRow.contains(indexProxy))
                insertSourceRows(indexProxy,indexSource);
        }

        // Запись значений
        for (qint32 column = 0; column < columnCount(parent); ++column) {
            QModelIndex index = indexProxy.sibling(indexProxy.row(),column);
            if (m_updatedRow.contains(index)) {
                if (isInserted) {
                    QHashIterator<qint32, QVariant> iterRole(m_updatedRow[index]);
                    while (iterRole.hasNext()) {
                        iterRole.next();
                        QVariant value = iterRole.value();
                        qint32 role = iterRole.key();
                        sourceModel()->setData(
                                    indexSource.sibling(indexSource.row(),column),
                                    value,role);
                    }
                }
                m_updatedRow[index].clear();
                m_updatedRow.remove(QPersistentModelIndex(index));
            }
        }

    }
    removeRows(rowCount(parent)-count,count,parent);

    return true;
}

qint32 ModifyProxyModel::columnCount(const QModelIndex &parent) const
{
    if(!sourceModel())
        return 0;

    // Количество столбцов для вставленных строк
    if (isInsertRow(parent))
        return columnCount();

    return sourceModel()->columnCount(mapToSource(parent));
}

qint32 ModifyProxyModel::rowCount(const QModelIndex &parent) const
{
    if(!sourceModel())
        return 0;

    // Количество вставленных строк
    qint32 insertRowCount = 0;
    QPersistentModelIndex rowIndex(parent.sibling(parent.row(),0));
    if (m_insertedRow.contains(rowIndex))
        insertRowCount = m_insertedRow[rowIndex].count();
    if (isInsertRow(parent))
        return  insertRowCount;

    // Количество удаленных строк
    qint32 removeRowCount = 0;
    if (m_removedRow.contains(QPersistentModelIndex(mapToSource(parent))) && m_hiddenRow)
        removeRowCount =
                m_removedRow[QPersistentModelIndex(mapToSource(parent))].count();

    qint32 parentRowCount = sourceModel()->rowCount(mapToSource(parent));
    return parentRowCount + insertRowCount - removeRowCount;
}

QModelIndex ModifyProxyModel::index(qint32 row, qint32 column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    // Получение вставленной строки
    QPersistentModelIndex rowIndex(parent.sibling(parent.row(),0));
    if (m_insertedRow.contains(rowIndex)) {
        qint32 count = m_insertedRow[rowIndex].count();
        if (row >= rowCount(parent)-count) {
            return createIndex(row,column,
                               m_insertedRow[rowIndex].at(
                                   row - rowCount(parent) + count)
                               );
        }
    }

    // Скрытие удаленных строк
    qint32 removeRowCount = 0;
    if (m_removedRow.contains(mapToSource(parent)) && m_hiddenRow) {
        foreach (const QPersistentModelIndex &removedIndex,
                 m_removedRow[QPersistentModelIndex(mapToSource(parent))])
        {
            if (mapFromSource(removedIndex).row() <= row)
                ++removeRowCount;

        }
    }
    QPersistentModelIndex removeIndex(sourceModel()->index(
                                          row+removeRowCount, column, mapToSource(parent)));

    return mapFromSource(removeIndex);
}

QModelIndex ModifyProxyModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    // Получение вставленной строки
    QPersistentModelIndex *rowIndex =
            static_cast<QPersistentModelIndex *>(index.internalPointer());
    if (rowIndex)
        if (m_insertedRow.contains(*rowIndex))
            return *rowIndex;

    if (!mapToSource(index).isValid())
        return QModelIndex();
    return mapFromSource(mapToSource(index).parent());
}

QModelIndex ModifyProxyModel::mapFromSource(const QModelIndex &index) const
{
    if(index.isValid()) {
        qint32 removeRowCount = 0;
        if (m_removedRow.contains(QPersistentModelIndex(index.parent())) && m_hiddenRow) {
            foreach (const QPersistentModelIndex &removedIndex,
                     m_removedRow[QPersistentModelIndex(index.parent())])
            {
                if (index.row() > removedIndex.row())
                    ++removeRowCount;

            }
        }

        QModelIndex sourceIndex = createIndex(index.row() - removeRowCount,
                                              index.column(),
                                              index.internalPointer());
        if (sourceIndex.isValid())
            return sourceIndex;
    }

    return QModelIndex();
}

QModelIndex ModifyProxyModel::mapToSource(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QModelIndex sourceIndex;
    PrivateModelIndex *hack = reinterpret_cast<PrivateModelIndex*>(&sourceIndex);

    hack->r = index.row();
    hack->c = index.column();
    hack->p = index.internalPointer();
    hack->m = sourceModel();

    if (!sourceIndex.isValid())
        return QModelIndex();

    if (m_removedRow.contains(QPersistentModelIndex(sourceIndex.parent())) && m_hiddenRow)
        foreach (const QPersistentModelIndex &removedIndex,
                 m_removedRow[QPersistentModelIndex(sourceIndex.parent())])
        {
            if (mapFromSource(removedIndex).row() <= index.row())
                ++hack->r;

        }

    if (!sourceIndex.isValid())
        return QModelIndex();

    return sourceIndex;
}

QVariant ModifyProxyModel::data(const QModelIndex &proxyIndex, qint32 role) const
{
    QPersistentModelIndex dataIndex((role == TreeXmlModel::TagRole)
                                    ? proxyIndex.sibling(proxyIndex.row(),0)
                                      :proxyIndex);

    if (role == Qt::DecorationRole) {
         TreeXmlModel *xmlModel = qobject_cast<TreeXmlModel *>(sourceModel());
         if (xmlModel) {
             QIcon icon = xmlModel->tagIcon(proxyIndex.data(TreeXmlModel::TagRole).toString());
             if (!icon.isNull() && proxyIndex.column()==0) return icon;
         }
    }

    // Получение измененных данных
    if (m_updatedRow.contains(dataIndex))  {
        qint32 updateRole = (role==Qt::DisplayRole) ? Qt::EditRole : role;
        QVariant linkData = (m_updatedRow[dataIndex].contains(updateRole)) ?
                    m_updatedRow[dataIndex][updateRole] : QVariant();
        if (linkData.isValid()) {
            if (role == Qt::DisplayRole) {
                TreeXmlHashModel *hashModel = qobject_cast<TreeXmlHashModel *>(sourceModel());
                if (hashModel && proxyIndex.isValid()) {
                    QString tag  = proxyIndex.data(TreeXmlModel::TagRole).toString();
                    QString attr = hashModel->displayedAttr(tag, proxyIndex.column());
                    QModelIndex linkIndex  = hashModel->indexLink(tag, attr, linkData);
                    return (linkIndex.isValid()) ? linkIndex.data() : linkData;
                }
            }
            return linkData;
        }
    }

    // Пустое значение для вставленных строк
    if (isInsertRow(proxyIndex))
        return QVariant();

    // Устанавливает зачеркивание удаленной строки
    if (role == Qt::FontRole && !m_hiddenRow) {
        QPersistentModelIndex removeIndex = mapToSource(proxyIndex);
        if (m_removedRow.contains(removeIndex.parent()))
            if (m_removedRow[removeIndex.parent()].contains(removeIndex)) {
                QFont font = sourceModel()->data(
                            mapToSource(proxyIndex),role).value<QFont>();
                font.setStrikeOut(true);
                return font;
            }
    }


    return sourceModel()->data(mapToSource(proxyIndex),role);
}

bool ModifyProxyModel::setData(const QModelIndex &index, const QVariant &value, qint32 role)
{
    QPersistentModelIndex dataIndex((role == TreeXmlModel::TagRole)
                                    ? index.sibling(index.row(),0)
                                      :index);
    qint32 updateRole = (role==Qt::DisplayRole) ? Qt::EditRole : role;
    m_updatedRow[dataIndex][updateRole] = value;
    emit dataChanged(index,index);

    return true;
}

bool ModifyProxyModel::hasChildren(const QModelIndex &parent) const
{
    return rowCount(parent);
}

void ModifyProxyModel::setSourceModel(QAbstractItemModel *srcModel)
{
    QAbstractProxyModel::setSourceModel(srcModel);

    connect(srcModel, &QAbstractItemModel::rowsRemoved,
            this, &ModifyProxyModel::sourceRowsRemoved);
    connect(srcModel, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &ModifyProxyModel::sourceRowsAboutToBeRemoved);
    connect(srcModel, &QAbstractItemModel::rowsInserted,
            this, &ModifyProxyModel::sourceRowsInserted);
    connect(srcModel, &QAbstractItemModel::dataChanged,
            this, &ModifyProxyModel::sourceDataChanged);
}

Qt::ItemFlags ModifyProxyModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)

    if (m_editable)
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ModifyProxyModel::setEditable(bool flag)
{
    m_editable = flag;
}

bool ModifyProxyModel::isEditable()
{
    return m_editable;
}

QVariant ModifyProxyModel::headerData(qint32 section, Qt::Orientation orientation, qint32 role) const
{    
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (m_header[section].isNull())
            return sourceModel()->headerData(section,orientation,role);
        else
            return  m_header[section];
    }
    return  QVariant();

}

bool ModifyProxyModel::setHeaderData(qint32 section, Qt::Orientation orientation,
                                     const QVariant &value, qint32 role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    m_header[section] = value.toString();
    emit headerDataChanged(orientation, section, section);

    return true;
}

bool ModifyProxyModel::removeRows(qint32 row, qint32 count, const QModelIndex &parent)
{

    qint32 beginRowInsert = row + count;

    // Удаление вставленных строк
    QPersistentModelIndex rowIndex(parent.sibling(parent.row(),0));
    if (m_insertedRow.contains(rowIndex)) {

        qint32 countRowInCache =  m_insertedRow[rowIndex].count();
        qint32 beginRowInCache =  row - rowCount(parent) + countRowInCache;

        qint32 removeRowCountInCache =
                (beginRowInCache >= 0) ? count : count + beginRowInCache;

        if (beginRowInCache < 0)
            beginRowInCache = 0;

        if (removeRowCountInCache > 0) {

            beginRowInsert = beginRowInCache + rowCount(parent)
                    - countRowInCache;
            beginRemoveRows(parent, beginRowInsert,
                            beginRowInsert+removeRowCountInCache-1);

            for (qint32 i = beginRowInsert; i < beginRowInsert + removeRowCountInCache; ++i)
                for (qint32 j = 0; j < sourceModel()->columnCount(parent);++j) {
                    QPersistentModelIndex removeIndex(parent.child(i,j));
                    if (m_updatedRow.contains(removeIndex)) {
                        m_updatedRow.remove(removeIndex);
                    }
                }

            for (qint32 i = beginRowInCache + removeRowCountInCache-1;
                 i >= beginRowInCache; --i)
            {
                delete m_insertedRow[rowIndex].at(i);
                m_insertedRow[rowIndex].removeAt(i);

            }
            if (m_insertedRow[rowIndex].isEmpty())
                m_insertedRow.remove(rowIndex);

            endRemoveRows();
        }
    }

    // Удаление строк в исходной модели
    for (qint32 i = row; i< beginRowInsert; ++i) {
        QModelIndex insertIndex = mapToSource(index(i,0, parent));
        if (m_hiddenRow) {
            beginRemoveRows(parent, i, i);
            m_removedRow[insertIndex.parent()].append(QPersistentModelIndex(insertIndex));
            endRemoveRows();
        } else
            m_removedRow[insertIndex.parent()].append(QPersistentModelIndex(insertIndex));
    }
    emit layoutChanged();

    return true;
}

bool ModifyProxyModel::insertRows(qint32 row, qint32 count, const QModelIndex &parent)
{
    return insertLastRows(row, count, parent).isValid();
}

QModelIndex ModifyProxyModel::insertLastRows(qint32 row, qint32 count, const QModelIndex &parent)
{
    Q_UNUSED(row)
    QPersistentModelIndex rowIndex(parent.sibling(parent.row(),0));

    qint32 position = rowCount(rowIndex);

    beginInsertRows(parent,position,position+count-1);
    for (qint32 i = position; i < position+count; ++i) {
        QPersistentModelIndex *index = new QPersistentModelIndex(rowIndex);
        m_insertedRow[rowIndex].append(index);
    }
    endInsertRows();

    if (parent.isValid())
        return parent.child(position+count-1,0);
    else
        return index(position+count-1,0,parent);
}

bool ModifyProxyModel::isInsertRow(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    QPersistentModelIndex *rowIndex =
            static_cast<QPersistentModelIndex *>(
                index.sibling(index.row(),0).internalPointer());
    if (rowIndex) {
        if (m_insertedRow.contains(*rowIndex))
            return  true;
    }
    return false;
}

void ModifyProxyModel::setHiddenRow(bool enabled)
{
    m_hiddenRow = enabled;
}


bool ModifyProxyModel::isHiddenRow()
{
    return m_hiddenRow;
}

QModelIndex ModifyProxyModel::buddy(const QModelIndex &index) const
{
    return QAbstractItemModel::buddy(index);
}

bool ModifyProxyModel::canFetchMore(const QModelIndex &parent) const
{
    return QAbstractItemModel::canFetchMore(parent);
}

}}
