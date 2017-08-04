#include <QtXml>
#include <QUuid>

#include "mimedataindex.h"
#include "tagxmlitem.h"
#include "treexmlmodel.h"
#include "clogging.h"

namespace RTPTechGroup {
namespace XmlModel {

TreeXmlModel::TreeXmlModel(QDomNode document, QObject *parent)
    : QAbstractItemModel(parent)
{
    bool isNewModel = document.isNull();

    if (isNewModel) {
        QDomDocument doc;
        QDomNode node = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        document.insertBefore(node, doc.firstChild());
        m_rootItem = new TagXmlItem(doc, NULL);
    } else
        m_rootItem = new TagXmlItem(document, NULL);

    m_column = 1;
}

TreeXmlModel::~TreeXmlModel()
{
    delete m_rootItem;
}

qint32 TreeXmlModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return  m_column;
}

void TreeXmlModel::addTagFilter(const QString &tag)
{
    m_filterTags << tag;
}

QStringList TreeXmlModel::tagsFilter()
{
    return m_filterTags;
}

void TreeXmlModel::clearTagsFilter()
{
    m_filterTags.clear();
}

void TreeXmlModel::addAttrTag(const QString &classTag, const QString &attrTag)
{
    m_attrTags[classTag] << attrTag;
}

QStringList TreeXmlModel::attrTags(const QModelIndex &parent) const
{
    return m_attrTags[parent.data(TreeXmlModel::TagRole).toString()];
}

void TreeXmlModel::clearAttrTags(){
    m_attrTags.clear();
}

bool TreeXmlModel::isInherited(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    TagXmlItem *item = toItem(index);
    return item->isInherited();
}

bool TreeXmlModel::moveIndex(const QModelIndex &srcIndex, const QModelIndex &destIndex,
                             qint32 row, bool recursively)
{
    if (!copyIndex(srcIndex, destIndex, row, recursively))
        return false;

    return true;
}

bool TreeXmlModel::copyIndex(const QModelIndex &srcIndex, const QModelIndex &destIndex,
                             qint32 row, bool recursively)
{
    if (isInherited(srcIndex))
        return true;

    QString tag = srcIndex.data(TreeXmlModel::TagRole).toString();

    QModelIndex index = insertLastRows((row >= 0) ? row : 0, 1, destIndex, tag);
    if (!index.isValid())
        return false;

    for (qint32 i = 0; i<this->columnCount(srcIndex); ++i) {
        QVariant value = srcIndex.sibling(srcIndex.row(), i).data(Qt::EditRole);
        if (value.isValid())
            if (!setData(index.sibling(index.row(), i), value)) {
                removeRow(index.row(),destIndex);
                return false;
            }
    }

    if (recursively) {
        qint32 row = 0;
        QModelIndex childIndex = srcIndex.child(row,0);
        while (childIndex.isValid())
        {
            if (!copyIndex(childIndex, index, row, recursively)) {
                removeRow(index.row(),destIndex);
                return false;
            }
            childIndex = srcIndex.child(++row,0);
        }
    }

    return true;
}

bool TreeXmlModel::isAttr(const QModelIndex &parent, QString tag) const
{
    if (attrTags(parent).contains(tag))
        return true;

    return false;
}

bool TreeXmlModel::isAttr(const QModelIndex &index) const
{
    TagXmlItem *item = toItem(index);
    QString nodeName = item->nodeName();
    TagXmlItem *parentItem = item->parent();
    QString tag = (parentItem)? parentItem->nodeName():"";
    if (m_attrTags[tag].contains(nodeName))
        return true;

    return false;
}

bool TreeXmlModel::isInsert(qint32 row, const QModelIndex &index, QString tag) const
{
    TagXmlItem *item = toItem(index);

    if (index.isValid()){
        if (!m_insertTags[item->nodeName()].contains(tag))
            return false;

        if (item->count(m_filterTags,QStringList())<row)
            return false;

        return true;
    } else {
        if (!hasChildren(index)) {
            return !isAttr(index);
        } else {
            qCWarning(lcXmlModel)
                    << tr("У дерева может быть только один корневой узел");
        }
    }
    return false;
}

bool TreeXmlModel::hasChildren(const QModelIndex &parent, const QStringList &tags) const
{
    TagXmlItem *parentItem = toItem(parent);
    return parentItem->hasChildren(tags, attrTags(parent));
}

bool TreeXmlModel::hasChildren(const QModelIndex &parent) const
{
    TagXmlItem *parentItem = toItem(parent);
    return parentItem->hasChildren(m_filterTags, attrTags(parent));
}

void TreeXmlModel::addDisplayedAttr(const QString &tag, const QStringList &value, QIcon icon)
{
    if (value.count()>m_column)
        m_column = value.count();
    m_displayedAttr[tag] = value;

    if (!icon.isNull())
        m_displayedIcon[tag] = icon;
}

QIcon TreeXmlModel::tagIcon(const QString &tag)
{
    return m_displayedIcon[tag];
}

void TreeXmlModel::addInsertTags(const QString &tag, const QStringList &value)
{
    m_insertTags.insert(tag,value);
}

qint32 TreeXmlModel::columnDisplayedAttr(const QString &tag, const QString &attr) const
{
    for (qint32 i = m_displayedAttr[tag].count() - 1; i >= 0; --i){
        if (m_displayedAttr[tag].at(i)==attr)
            return i;
    }
    return -1;
}

QString TreeXmlModel::displayedAttr(const QString &tag, qint32 column) const
{
    if (m_displayedAttr[tag].count()<=column || column < 0)
        return QString("");
    return m_displayedAttr[tag].at(column);
}

void TreeXmlModel::removeDisplayedAttr(const QString &tag)
{
    bool flagcolumn = false;
    if (m_displayedAttr[tag].count() == m_column)
        flagcolumn = true;

    m_displayedAttr.remove(tag);
    m_displayedIcon.remove(tag);

    if (flagcolumn){
        m_column = 1;
        foreach (const QStringList &diplayedAttr, m_displayedAttr.values())
            if (diplayedAttr.count()>m_column)
                m_column = diplayedAttr.count();
    }
}

QVariant TreeXmlModel::data(const QModelIndex &index, qint32 role) const
{
    TagXmlItem *item = toItem(index);
    QString tag = item->nodeName();

    if (role == Qt::DecorationRole)
        if (!m_displayedIcon[tag].isNull()&&index.column()==0)
            return m_displayedIcon[tag];

    if (role == TreeXmlModel::TagRole){
        return tag;
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole){
        if (m_displayedAttr[tag].isEmpty())
            return tag;

        if (index.column()<m_displayedAttr[tag].count()){
            QString attrName = displayedAttr(tag,index.column());
            return item->value(attrName);
        }
    }
    return QVariant();
}

void TreeXmlModel::updateModifyRow(qint32 emptyRowAttr, const QModelIndex &parent, qint32 column)
{
    qint32 i = 0;
    QModelIndex index = parent.child(i,0);
    while (index.isValid())
    {
        if (!isAttr(index)){
            qint32 row = this->rowCount(index)-emptyRowAttr;
            updateModifyRow(emptyRowAttr, index, column);
            QModelIndex updateIndex = index.child(row,0);
            emit dataChanged(updateIndex, updateIndex);
        }
        index = parent.child(++i,0);
    }
}

bool TreeXmlModel::setData(const QModelIndex &index, const QVariant &value,
                           qint32 role)
{
    if (role != Qt::EditRole && role != TreeXmlModel::TagRole) return false;

    TagXmlItem *item = toItem(index);

    if (role == TreeXmlModel::TagRole) {
        item->setNodeName(value.toString());
        return true;
    }
    QString nodeName = item->nodeName();
    if (index.column()>=m_displayedAttr[nodeName].count())
        return false;

    QString attrName = displayedAttr(nodeName,index.column());

    if (attrName.isEmpty())
        return false;

    QVariant dataValue = value.toString();
    if (attrName == QString("parent"))
        dataValue = data(index,Qt::EditRole).toString();

    item->setValue(attrName,dataValue);

    if (isAttr(index)){
        qint32 emptyRowAttr = 0;
        QModelIndex parent = index.parent();
        qint32 i = index.row();
        QModelIndex idx = parent.child(i,0);
        while (idx.isValid())
        {
            if (isAttr(idx))
                ++emptyRowAttr;
            idx = parent.child(++i,0);
        }
        updateModifyRow(emptyRowAttr,parent, index.column());
    }
    emit dataChanged(index,index);
    return true;
}

TagXmlItem *TreeXmlModel::rootItem()
{
    return m_rootItem;
}

Qt::ItemFlags TreeXmlModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsDropEnabled;
}

QVariant TreeXmlModel::headerData(qint32 section, Qt::Orientation orientation,
                                  qint32 role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (m_header[section].isNull())
            return QString("%1").arg(section);
        else
            return  m_header[section];
    }
    return  QVariant();
}

bool TreeXmlModel::setHeaderData(qint32 section, Qt::Orientation orientation,
                                 const QVariant &value, qint32 role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    m_header[section] = value.toString();
    emit headerDataChanged(orientation, section, section);

    return true;
}

QModelIndex TreeXmlModel::index(qint32 row, qint32 column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TagXmlItem *parentItem = toItem(parent);
    TagXmlItem *childItem = parentItem->child(row,m_filterTags,attrTags(parent));

    if (childItem){
        return createIndex(row, column, childItem);
    } else
        return QModelIndex();
}

bool TreeXmlModel::hasIndex(qint32 row, qint32 column, const QModelIndex &parent) const
{
    Q_UNUSED (parent)

    if (row < 0 || column < 0)
        return false;
    return true;
}

QModelIndex TreeXmlModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TagXmlItem *childItem = toItem(child);
    TagXmlItem *parentItem = childItem->parent();

    return fromItem(parentItem);
}

qint32 TreeXmlModel::rowCount(const QModelIndex &parent) const
{
    return this->rowCount(parent, m_filterTags, attrTags(parent));
}

qint32 TreeXmlModel::rowCount(const QModelIndex &parent,
                           const QStringList &tags) const
{
    return this->rowCount(parent, tags, attrTags(parent));
}

qint32 TreeXmlModel::rowCount(const QModelIndex &parent, const QStringList &tags,
                           const QStringList &attrTags) const
{
    TagXmlItem *parentItem = toItem(parent);
    return parentItem->count(tags, attrTags);
}

bool TreeXmlModel::insertRows(qint32 row, qint32 count, const QModelIndex &parent)
{
    QModelIndex index = insertLastRows(row, count, parent);
    if (index.isValid())
        return true;

    return false;
}

QModelIndex TreeXmlModel::insertLastRows(qint32 row, qint32 count, const QModelIndex &parent, QString tag)
{
    TagXmlItem *parentItem = toItem(parent);
    if (!isInsert(row, parent, tag))
        return QModelIndex().child(-1,-1);

    beginInsertRows(parent,row,row+count-1);
    qint32 emptyRowAttr = 0;
    qint32 i = row;
    QModelIndex indexChild = parent.child(i,0);
    while(indexChild.isValid())
    {
        if (isAttr(indexChild))
            ++emptyRowAttr;
        indexChild = parent.child(++i,0);
    }
    updateInsertRows(emptyRowAttr, count, parent, tag);

    bool success = true;
    qint32 revertCount;
    for (qint32 i = 0; i < count; ++i) {
        success = parentItem->insertChild(tag,
                                          row,
                                          m_filterTags,
                                          attrTags(parent));
        if (!success) {
            revertCount = i;
            break;
        }
    }
    endInsertRows();

    if (success) {
        // Добавление корневого узла
        if (parent.isValid())
            return parent.child(row+count-1,0);
        else
            return index(row+count-1,0,parent);
    } else {
        revertInsertRows(row, revertCount, parent, tag);
    }

    return QModelIndex().child(-1,-1);
}

void TreeXmlModel::updateInsertRows(qint32 emptyRowAttr, qint32 count, const QModelIndex &parent, QString tag)
{    
    // Если атрибут то обновляем по дереву наследования
    if (isAttr(parent, tag)) {
        qint32 i = 0;
        QModelIndex index = parent.child(i,0);
        while (index.isValid())
        {
            if (!isAttr(index)
                    && parent.data(TreeXmlModel::TagRole) == index.data(TreeXmlModel::TagRole)){
                qint32 row = this->rowCount(index)-emptyRowAttr;
                beginInsertRows(index,row,row+count-1);
                updateInsertRows(emptyRowAttr,count,index, tag);
                endInsertRows();
            }
            index = parent.child(++i,0);
        }
    }
}

void TreeXmlModel::revertInsertRows(qint32 row, qint32 count, const QModelIndex &parent, QString tag)
{
    // Если атрибут
    if (isAttr(parent, tag)) {
        qint32 i = 0;
        QModelIndex index = parent.child(i,0);
        while (index.isValid())
        {
            if (!isAttr(index)
                    && parent.data(TreeXmlModel::TagRole) == index.data(TreeXmlModel::TagRole))
                revertInsertRows(this->rowCount(index),count,index, tag);
            index = parent.child(++i,0);
        }
    }

    beginRemoveRows(parent,row,row+count-1);
    endRemoveRows();
}

void TreeXmlModel::updateRemoveRows(qint32 emptyRowAttr,qint32 count, const QModelIndex &parent)
{
    // Если атрибут то обновляем по дереву наследования
    qint32 i = 0;
    QModelIndex index = parent.child(i,0);
    while (index.isValid())
    {
        if (!isAttr(index)
                && parent.data(TreeXmlModel::TagRole) == index.data(TreeXmlModel::TagRole)){
            updateRemoveRows(emptyRowAttr,count,index);
            qint32 row = this->rowCount(index)-emptyRowAttr;
            beginRemoveRows(index,row,row+count-1);
            endRemoveRows();
        }
        index = parent.child(++i,0);
    }
}

bool TreeXmlModel::removeRows(qint32 row, qint32 count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;

    TagXmlItem *parentItem = toItem(parent);

    if  (!parentItem->checkRemoveChild(row))
        return false;

    beginRemoveRows(parent,row,row+count-1);

    qint32 emptyRowAttr = 0;
    qint32 i=row;
    QModelIndex index = parent.child(i,0);
    while (index.isValid())
    {
        if (isAttr(index))
            ++emptyRowAttr;
        else if (emptyRowAttr == 0 && i == row+count-1) {
            break;
        }
        index = parent.child(++i,0);
    }
    updateRemoveRows(emptyRowAttr,count,parent);

    for (qint32 i=count-1;i>=0;--i)
        parentItem->removeChild(row+i);

    endRemoveRows();

    return true;
}

bool TreeXmlModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                qint32 row, qint32 column, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;

    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("application/classxmlmodel"))
        return false;
    if (column >= columnCount(parent))
        return false;
    if (isAttr(parent))
        return false;

    const  MimeDataIndex *mimeData
            = qobject_cast<const MimeDataIndex *>(data);
    foreach (const QModelIndex& index, mimeData->indexes())
        if (index.isValid()) {
            if (action == Qt::MoveAction)
                return moveIndex(index, parent, row, true);
            else
                return copyIndex(index, parent, row, true);
        }

    return true;
}

TagXmlItem *TreeXmlModel::toItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TagXmlItem *item = static_cast<TagXmlItem*>(index.internalPointer());
        if (item) return item;
    }
    return m_rootItem;
}

QModelIndex TreeXmlModel::fromItem(TagXmlItem *item) const
{
    if (!item || item == m_rootItem)
        return QModelIndex();
    TagXmlItem *parentItem  = item->parent();
    QString tag = parentItem->nodeName();
    qint32 row = parentItem->childNumber(item,m_filterTags,m_attrTags[tag]);

    return createIndex(row , 0, item);
}

Qt::DropActions TreeXmlModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions TreeXmlModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList TreeXmlModel::mimeTypes() const
{
    QStringList types;
    types << "application/classxmlmodel";
    return types;
}

QMimeData *TreeXmlModel::mimeData(const QModelIndexList &indexes) const
{
    PersistentIndexes persistentIndex;

    foreach (const QModelIndex& index,indexes){
        if (index.isValid())
            if (!isInherited(index))
                persistentIndex.append(QPersistentModelIndex(index));
    }

    MimeDataIndex *mimeData = new MimeDataIndex();
    mimeData->setIndexes(persistentIndex);
    return mimeData;
}

}}
