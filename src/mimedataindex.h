#ifndef MIMEDATAINDEX_H
#define MIMEDATAINDEX_H

#include "treexmlmodelglobal.h"
#include <QMimeData>
#include <QPersistentModelIndex>
#include <QModelIndex>
#include <QList>
#include <QStringList>

namespace RTPTechGroup {
namespace XmlModel {

typedef QList<QPersistentModelIndex> PersistentIndexes;

//! MimeDataXml контейнер для записи информации с mime типом
/*! MimeDataXml расширенный контейнер QMimeData. В нем можно
 *  передавать экземпляры классов типа QModelIndex
 */
class XMLMODELLIB MimeDataIndex : public QMimeData
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit MimeDataIndex();

    //! Записывает объект
    virtual void setIndexes(PersistentIndexes indexes);

    //! Возвращает объект
    PersistentIndexes indexes() const;

    //! Возвращает true если объект внутри существует
    virtual bool hasIndexes() const;

    //! Возвращает список mime типов записанной информации
    QStringList formats() const;

private:
    //! Предназначено для хранения индекса
    PersistentIndexes m_indexes;
    
};

}}

#endif
