#ifndef TABLEXMLPROXYMODEL_H
#define TABLEXMLPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include "treexmlmodelglobal.h"

namespace RTPTechGroup {
namespace XmlModel {

//! Класс прокси модель  для XML данных класса TreeXMLModel
/*! Данный класс предназначен для табличного отображения атрибутов
    класса TreeXMLModel.Позволяет отображать наследуемые тэги, сортировать
    и фильтровать данные. \n
    Пример использования:
    \code
    TableXMLProxyModel *m_attrModel = new TableXMLProxyModel();
    QStringList tags;
    tags << "Attribute";
    m_attrModel->setAttributeTags(tags);
    \endcode
*/
class XMLMODELLIB TableXMLProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    //! Конструктор модели
    explicit TableXMLProxyModel(QObject *parent = 0);

    //! Метод который определяет отображать ли указанную строку
    virtual bool filterAcceptsRow (qint32 row, const QModelIndex &parent) const;

    //! Метод который определяет сортировку
    virtual bool lessThan(const QModelIndex & left, const QModelIndex & right) const;

    //! Устанавливает индекс модели, в которой будут отображаться только атрибуты
    void setFilterIndex(const QModelIndex &parent);

    //! Получение индекса модели, в которой будут отображаться только атрибуты
    QModelIndex filterIndex();

    //! Получение тэгов, которые являются атрибутами
    QStringList attributeTags();

     //! Установка тэгов, которые являются атрибутами
    void setAttributeTags(QStringList list);

    //! Установка количества столбцов
    void setColumnCount(qint32 column);

    //! Количество столбцов
    virtual qint32 columnCount(const QModelIndex &parent) const;

    //! Устанавливает название заголовка
    bool setHeaderData(qint32 section, Qt::Orientation orientation, const QVariant &value,
                       qint32 role = Qt::EditRole);

    //! Установить фильтр для всех строк
    void setFilterAllRows(bool enabled);

    //! Проверка установлен ли фильтр для всех строк
    bool isFilterAllRows() const;

    //! Возращает название заголовка
    QVariant headerData(qint32 section, Qt::Orientation orientation, qint32 role) const;

private:
    //! Индекс модели, в котором отображаются только атрибуты
    QModelIndex m_index;

    //! Список тэгов, которые являются атрибутами
    QStringList m_tags;

    //! Количество столбцов
    qint32 m_columnCount;

    //! Список названий заголовков
    QMap<qint32, QString> m_header;

    //! Флаг фильтра для всех строк
    bool m_filterAllRows;
};

}}

#endif // TABLEXMLPROXYMODEL_H
