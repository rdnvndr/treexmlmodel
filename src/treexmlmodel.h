#ifndef TREEXMLMODEL_H
#define TREEXMLMODEL_H

#include <QAbstractItemModel>
#include <QtXml/QDomDocument>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QIcon>
#include "treexmlmodelglobal.h"

namespace RTPTechGroup {
namespace XmlModel {

class TagXmlItem;

//! Класс модель дерева для XML данных
/*! Данный класс предназначен для создания дерева на основе,
    XML данных. Позволяет отображать указанные наследуемые тэги. Узлы дерева
    (записи таблицы) связаны между указателем класса TagXmlItem. \n
    Пример использования:
    \code

    QString filePath = QFileDialog::getOpenFileName(NULL, tr("Открытие"),"", tr("Фаил модели (*.xml)"));
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if (document.setContent(&file)) {
            dbStructModel = new TreeXmlModel(document, this);

            dbStructModel->addTagFilter("Class");
            dbStructModel->addTagFilter("Attribute");

            QStringList propsClass;
            propsClass << "ClassName" << "parent";
            dbStructModel->addDisplayedAttr("Class",propsClass);

            QStringList propsAttr;
            propsAttr << "AttributeName" << "parent";
            dbStructModel->addDisplayedAttr("Attribute",propsAttr);

            dbStructModel->addAttrTag("Attribute");

            dbStructModel->setHeaderData(0, Qt::Horizontal, tr("Имя атирибута"));
            dbStructModel->setHeaderData(1, Qt::Horizontal, tr("Родитель"));

            QTreeView *treeView = new QTreeView( this );
            treeView->setModel(dbStructModel);
        }
        file.close();
    }
    \endcode

*/
class XMLMODELLIB TreeXmlModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    //! Структура для хранения тэга и аттрибута вместе
    struct TagWithAttr {
        QString tag;    //!< Поле с тэгом
        QString attr;   //!< Поле с атрибутом
    };

    //! Перечисления ролей
    enum AttrRole {
        TagRole =  Qt::UserRole + 1    //!< Роль Тэга
    };

     //! Конструктор модели
    TreeXmlModel(QDomNode document  = QDomNode(), QObject *parent = 0);

    //! Деструктор модели
    virtual ~TreeXmlModel();

    //! Возращает хранимые данные
    QVariant data(const QModelIndex &index, qint32 role = Qt::DisplayRole ) const;

    //! Устанавливает значение для указанной записи
    bool setData(const QModelIndex &index, const QVariant &value, qint32 role = Qt::EditRole);

    //! Возвращает флаг записи
    Qt::ItemFlags flags(const QModelIndex &index) const;

    //! Возращает название заголовка
    QVariant headerData(qint32 section, Qt::Orientation orientation,
                        qint32 role = Qt::DisplayRole) const;

    //! Устанавливает название заголовка
    bool setHeaderData(qint32 section, Qt::Orientation orientation,
                       const QVariant &value, qint32 role = Qt::EditRole);

    //! Возращает индекс модели для строки и колонки
    QModelIndex index(qint32 row, qint32 m_column,
                      const QModelIndex &parent = QModelIndex()) const;

    //! Возращает индекс родителя
    QModelIndex parent(const QModelIndex &child) const;

    //! Возращает количество строк в индексе родителя
    qint32 rowCount(const QModelIndex &parent = QModelIndex()) const;

    //! Возращает количество строк указанных тэгов в индексе родителя
    virtual qint32 rowCount(const QModelIndex &parent, const QStringList &tags) const;

    //! Возращает количество строк указанных тэгов в индексе родителя c
    //! учетом наследумых тэгов
    virtual qint32 rowCount(const QModelIndex &parent, const QStringList &tags,
                         const QStringList &attrTags) const;

    //! Возращает количество столбцов в индексе родителя
    qint32 columnCount(const QModelIndex &parent = QModelIndex()) const;

    //! Добавляет тэг для фильтрации
    void addTagFilter(const QString &tag);

    //! Получение списка тэгов для фильтрации
    QStringList tagsFilter();

    //! Очищает фильтр тэгов
    void clearTagsFilter();

    //! Добавляет тэг, который будет являться атрибутом
    void addAttrTag(const QString &classTag, const QString &attrTag);

    //! Получение списка тэгов атрибутов для родителя
    QStringList attrTags(const QModelIndex &parent) const;

    //! Очищает список тэгов, которые являются атрибутами
    void clearAttrTags();

    //! Возращает True если тэг является атрибутом для родителя
    bool isAttr(const QModelIndex &parent, QString tag) const;

    //! Возращает True если строка является атрибутом
    bool isAttr(const QModelIndex &index) const;

    //! Получает класс связку TagXMLItem по индексу модели
    virtual TagXmlItem *toItem(const QModelIndex &index) const;

    //! Получает  индекс модели по классу связке TagXMLItem
    virtual QModelIndex fromItem(TagXmlItem *item) const;

    //! Указывает xml атрибуты тэга для отображения
    void addDisplayedAttr(const QString &tag,const QStringList &value,QIcon icon = QIcon());

    //! Возращает иконку для тэга
    QIcon tagIcon(const QString &tag);

    //! Указывает тэги разрешенные для вставки
    void addInsertTags(const QString &tag, const QStringList &value);

    //! Получение индекса поля в тэге
    qint32 columnDisplayedAttr(const QString &tag, const QString &attr) const;

    //! Получение поля в тэге по индексу
    QString displayedAttr(const QString &tag, qint32 column) const;

    //! Удаляет xml атрибуты тэга для отображения
    void removeDisplayedAttr(const QString &tag);

    //! Получение корневой узла дерева
    virtual TagXmlItem *rootItem();

    //! Вставка строки
    bool insertRows (qint32 row, qint32 count, const QModelIndex & parent);

    //! Вставка строки
    virtual QModelIndex insertLastRows (qint32 row, qint32 count,
                                        const QModelIndex & parent, QString tag = "element");

    //! Удаление строки
    bool removeRows (qint32 row, qint32 count, const QModelIndex & parent);

    //! Обработчик окрнчания перетаскивания данных путем Drag and Drop
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      qint32 row, qint32 column, const QModelIndex &parent);

    //! Возращает объект с упакованными данными по списку индексов
    QMimeData *mimeData(const QModelIndexList &indexes) const;

    //! Возращает список типов, которые могут быть упакованы
    QStringList mimeTypes() const;

    //! Возращает поддерживаемые моделью операции Drop
    Qt::DropActions supportedDropActions() const;

    //! Возращает поддерживаемые моделью операции Drag
    Qt::DropActions supportedDragActions() const;

    //! Возращает True если можно вставить строку
    virtual bool isInsert(qint32 row, const QModelIndex &index, QString tag) const;

    //! Возращает True если имеются потомки указанных тэгов
    virtual bool hasChildren(const QModelIndex &parent, const QStringList &tags) const;

    //! Возращает True если имеются потомки
    bool hasChildren(const QModelIndex &parent) const;

    //! Возращает True если унаследован
    virtual bool isInherited(const QModelIndex &index) const;

    //! Перемещение элемента
    virtual bool moveIndex(const QModelIndex &srcIndex, const QModelIndex &destIndex,
                           qint32 row, bool recursively = false);

    //! Копирование элемента
    virtual bool copyIndex(const QModelIndex &srcIndex, const QModelIndex &destIndex,
                           qint32 row, bool recursively = false);

    //! Проверка наличия подчиненного элемента у родителя
    bool hasIndex(qint32 row, qint32 column, const QModelIndex &parent) const;

private:

    //! Обновление отредактированных унаследованных строк
    virtual void updateModifyRow(qint32 emptyRowAttr, const QModelIndex &parent, qint32 column);

    //! Обновление вставленных унаследованных строк
    virtual void updateInsertRows(qint32 row, qint32 count,
                                  const QModelIndex &parent, QString tag);

    //! Удаление вставленных унаследованных строк
    virtual void revertInsertRows(qint32 row, qint32 count,
                                  const QModelIndex &parent, QString tag);

    //! Обновление удаленных унаследованных строк
    virtual void updateRemoveRows(qint32 emptyRowAttr, qint32 count, const QModelIndex &parent);

    //! Корневой узел
    TagXmlItem *m_rootItem;

    //! Количество колонок
    qint32 m_column;

    //! Список тэгов для фильтрации
    QStringList m_filterTags;

    //! Список тэгов которые являются атрибутами
    QHash<QString, QStringList> m_attrTags;

    //! Список атрибутов тэга для отображения
    QHash<QString, QStringList> m_displayedAttr;

    //! Список иконок тэгов для отображения
    QHash<QString, QIcon> m_displayedIcon;

    //! Список названий заголовков
    QMap<qint32, QString> m_header;

    //! Список тэгов в которые нельзя вставлять строки
    QHash<QString, QStringList> m_insertTags;
};

}}

#endif
