#ifndef TreeXmlHashModel1_H
#define TreeXmlHashModel1_H

#include <QAbstractItemModel>
#include <QtXml/QDomDocument>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QIcon>
#include "treexmlmodelglobal.h"
#include "treexmlmodel.h"

namespace RTPTechGroup {
namespace XmlModel {

class TagXmlItem;

//! Класс модель дерева c хэшем для XML данных
/*! Данный класс предназначен для создания дерева на основе,
    XML данных. Позволяет отображать указанные наследуемые тэги. Узлы дерева
    (записи таблицы) связаны между указателем класса TagXMLItem, а также строить
    хэши (индексы) по полям для быстроко поиска данных.
*/

class XMLMODELLIB TreeXmlHashModel : public TreeXmlModel
{
    Q_OBJECT

public:

    //! Перечисления типов уникальности поля
    enum UniqueAttr {
        NoUnique                = 0, //!< Поле не уникальное
        UniqueRename            = 1, //!< Контроль уникальности путем переименования
        Unique                  = 2, //!< Поле уникальное
        UniqueUpperRename       = 3, //!< Контроль уникальности без учета регистра путем переименования
        UniqueUpper             = 4, //!< Поле уникальное без учета регистра
        UniqueParentRename      = 5, //!< Контроль уникальности путем переименования
        UniqueParent            = 6, //!< Поле уникальное
        UniqueParentUpperRename = 7, //!< Контроль уникальности без учета регистра путем переименования
        UniqueParentUpper       = 8, //!< Поле уникальное без учета регистра
        Uuid                    = 9  //!< Поле UUID
    };

     //! Конструктор модели
    TreeXmlHashModel(QDomNode document = QDomNode(), QObject *parent = 0);

    //! Деструктор модели
    virtual ~TreeXmlHashModel();

    //! Возращает хранимые данные
    QVariant data(const QModelIndex &index, qint32 role = Qt::DisplayRole ) const;

    //! Устанавливает значение для указанной записи
    bool setData(const QModelIndex &index, const QVariant &value, qint32 role = Qt::EditRole);

    //! Вставка строки
    bool insertRows (qint32 row, qint32 count, const QModelIndex & parent);

    //! Вставка строки
    QModelIndex insertLastRows (qint32 row, qint32 count,
                                const QModelIndex & parent, QString tag = "element");

    //! Удаление строки
    bool removeRows (qint32 row, qint32 count, const QModelIndex & parent);

    //! Перемещение элемента
    bool moveIndex(const QModelIndex &srcIndex, const QModelIndex &destIndex,
                   qint32 row, bool recursively = false);

    //! Добавления поля для хешеирования
    void addHashAttr(const QString &tag, const QString &value,
                      UniqueAttr unique = TreeXmlHashModel::NoUnique);

    //! Возращает индекс по значению хешеированного поля
    QModelIndex indexHashAttr(const QString &tag, const QString &attr,
                               const QVariant &value, qint32 number = 0) const;

    //! Обновление хэшей начиная с указанног индекса
    void refreshHashing(const QModelIndex &index = QModelIndex(), bool remove = false);

    //! Обновление хэша указанног индекса
    void refreshHashingOne(const QModelIndex &index, bool remove = false);

    //! Установка ссылки на другое поле
    void addRelation(const QString &tag, const QString &attr,
                     const QString &linkTag, const QString &linkAttr);

    //! Получить поля которые ссылаются на указанное поле
    QList<TagWithAttr> fromRelation(const QString &linkTag,
                                    const QString &linkAttr = QString()) const;

    //! Получить поле на которое ссылается указанное поле
    TagWithAttr toRelation(const QString &tag, const QString &attr) const;

    //! Возращает ссылку на индекс
    QModelIndex indexLink(const QString &tag, const QString &attr, const QVariant &value) const;

    //! Возращает ссылку на индекс
    QModelIndex indexLink(const QModelIndex &index) const;

    //! Возращает uuid поле
    QString uuidAttr(const QString &tag) const;

private:

    //! Преобразование в верхний регистр если не имеет значение регистр значения при контроле уникальности
    QString upperValue(const QString &tag, const QString &attr, const QString &value) const;

    //! Создание хэша для указанного индекса
    void makeHashing(TagXmlItem *item, bool remove = false);

    //! Создание хэшей начиная с указанног индекса
    void makeHashingOne(TagXmlItem *item, bool remove);

    //! Создание хэша при редактировании индекса
    bool makeHashingData(const QModelIndex &index, QString &dataValue);

    //! Заполнение UUID при вставке строки
    void insertUuid(const QModelIndex &index, QString tag);

    //! Список атрибутов для хэшеированя [тэг][атрибут][уникальность]
    QHash<QString, QHash<QString,UniqueAttr> > m_hashAttr;

    //! Список хэшей [тэг][атрибут][значение атрибута]
    QHash<QString, QHash<QString,QMultiHash<QString,TagXmlItem*> > > m_hashValue;

    //! Список зависимых полей [тэг][атрибут][ссылочныйТэг] = ссылочныйАтрибут
    QHash<QString, QHash<QString, QMap<QString, QString> > > m_linkAttr;
};

}}

#endif
