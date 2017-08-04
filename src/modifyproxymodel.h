#ifndef MODIFYPROXYMODEL_H
#define MODIFYPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QSet>
#include "treexmlmodelglobal.h"

namespace RTPTechGroup {
namespace XmlModel {

//! Класс прокси модель  для кэшеирования измений в модели источника
/*! Данный класс предназначен для кэшеирования изменений в модели-источнике
    с последующим их примением при вызове функции submitAll(). \n
    Пример использования:
    \code
    ModifyProxyModel *modifyModel = new ModifyProxyModel();
    modifyModel->setSourceModel(model);
    ...
    ... Изменение данных в прокси модели ...
    ...
    modifyModel->submitAll();
    \endcode
*/

class XMLMODELLIB ModifyProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:

    //! Конструктор модели
    explicit ModifyProxyModel(QObject *parent = 0);


    //! Возращает количество столбцов в индексе родителя
    qint32 columnCount (const QModelIndex& parent = QModelIndex()) const;

    //! Возращает количество строк в индексе родителя
    qint32 rowCount(const QModelIndex& parent) const;

    //! Возращает True если имеются потомки
    bool hasChildren(const QModelIndex &parent) const;


    //! Возращает индекс модели для строки и колонки
    QModelIndex index(qint32 row, qint32 column, const QModelIndex& parent = QModelIndex()) const ;

    //! Возращает индекс родителя
    QModelIndex parent (const QModelIndex& index) const ;


    //! Преобразует индекс модели источника в текущую модель
    QModelIndex mapFromSource(const QModelIndex& index) const ;

    //! Преобразует индекс текущей модели в индекс модели источника
    QModelIndex mapToSource(const QModelIndex& index) const ;


    //! Возращает хранимые данные
    QVariant data(const QModelIndex &proxyIndex, qint32 role = Qt::DisplayRole) const;

    //! Устанавливает значение для указанной записи
    bool setData(const QModelIndex &index, const QVariant &value, qint32 role = Qt::EditRole);

    //! Установка модели источника
    void setSourceModel(QAbstractItemModel *srcModel);

    //! Возвращает флаг записи
    Qt::ItemFlags flags(const QModelIndex &index) const;

    //! Устанавливает разрешение на редактирование
    void setEditable(bool flag);

    //! Возращает разрешение на редактирование
    bool isEditable();


    //! Возращает название заголовка
    QVariant headerData(qint32 section, Qt::Orientation orientation, qint32 role) const;

    //! Устанавливает название заголовка
    bool setHeaderData(qint32 section, Qt::Orientation orientation,
                       const QVariant &value, qint32 role = Qt::EditRole);


    //! Удаление строки
    bool removeRows(qint32 row, qint32 count, const QModelIndex &parent = QModelIndex());

    //! Вставка строки
    bool insertRows(qint32  row, qint32 count, const QModelIndex &parent = QModelIndex());

    //! Вставка строки
    QModelIndex insertLastRows(qint32  row, qint32 count, const QModelIndex &parent = QModelIndex());

    //! Получение индекса последней вставленной строки
    Q_DECL_DEPRECATED QModelIndex lastInsertRow();

    //! Проверка является ли строка вставленной
    bool isInsertRow(const QModelIndex &index) const;

    //! Установка скрытия удаленных строк
    void setHiddenRow(bool enabled);

    //! Проверка установлен ли режим скрытия удаленных строк
    bool isHiddenRow();

    //! Возвращает индекс модели buddy для элемента чей индекс передан параметром.
    //! Когда пользователь захочет отредактировать элемент, view (представление)
    //! вызывет эту функцию, чтобы проверить должен ли быть отредактирован другой
    //! элемент этой модели. Тогда view создаст делегат, используя индекс buddy элемента.
    //! По умолчанию каждый элемент является собственным buddy.
    QModelIndex buddy(const QModelIndex &index) const;

    //! Возвращает true, если имеются потомки у родителей, в противном случае возвращает ложь.
    //! По умолчанию всегда возвращает ложь.
    bool canFetchMore(const QModelIndex &parent) const;

public slots:

    //! Применение изменений сделанных в прокси модели к модели-источнику
    bool submitAll();

    //! Отмена изменений сделанных в прокси модели
    void revertAll();

    //! Слот обработки сигнала изменения данных источника
    void sourceDataChanged(const QModelIndex &left, const QModelIndex &right);

    //! Слот обработки сигнала удаления строк в источнике
    void sourceRowsRemoved(const QModelIndex &parent, qint32 start, qint32 end);

    //! Слот обработки сигнала перед удалением строк в источника
    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, qint32 start, qint32 end);

    //! Слот обработки сигнала вставки строк в источнике
    void sourceRowsInserted(const QModelIndex &parent, qint32 start, qint32 end);

private:

    //! Вставка строк в исходную модель
    bool insertSourceRows(const QPersistentModelIndex &parent,
                          const QPersistentModelIndex &sourceParent = QModelIndex());

    //! Список измененных значений
    QHash <QPersistentModelIndex, QHash<qint32, QVariant> > m_updatedRow;

    //! Список вставленных строк
    QHash <QPersistentModelIndex, QList<QPersistentModelIndex *> > m_insertedRow;

    //! Список удаленных строк
    QHash  <QPersistentModelIndex,QList<QPersistentModelIndex> > m_removedRow;

    //! Список названий заголовков
    QMap<qint32, QString> m_header;

    //! Флаг скрытия удаленных строк
    bool m_hiddenRow;

    //! Флаг разрешения на редактирование
    bool m_editable;

    //! Список строк для удаления в текущей модели
    QHash  <QModelIndex,QSet<qint32> > m_srcRemovedRow;
};

}}

#endif
