#ifndef TAGXMLITEM_H
#define TAGXMLITEM_H

#include <QObject>
#include <QDomNode>
#include <QMap>
#include <QStringList>

#include "treexmlmodelglobal.h"

namespace RTPTechGroup {
namespace XmlModel {

//! Класс узла дерева TreeXMLModel
/*! Класс предназначен для построения дерева xml модели
 */

class XMLMODELLIB TagXmlItem: public QObject
{
    Q_OBJECT
public:
    //! Конструктор узла дерева
    TagXmlItem(QDomNode &node, TagXmlItem *parent = 0);

    //! Деструктор узла дерева
    virtual ~TagXmlItem();

    //! Возвращает родителя
    TagXmlItem *parent();

    //! Возвращает тэг
    QDomNode node() const;

    //! Возвращает имя узла
    QString nodeName() const;

    //! Устанавливает имя узла
    void setNodeName(const QString& tagName);

    //! Возвращает значение атрибута тэга
    QVariant value(const QString& attr);

    //! Устанавливает значение атрибута тэга
    void setValue(const QString& attr, const QVariant& val);

    //! Возвращает количество записей
    qint32 count(QStringList tags = QStringList(),
              QStringList parentTags = QStringList());

    //! Возвращает True при наличии записей
    qint32 hasChildren(QStringList tags = QStringList(),
                    QStringList parentTags = QStringList());

    //! Возвращает потомка
    TagXmlItem *child(qint32 i, QStringList tags = QStringList(),
                             QStringList parentTags = QStringList());

    //! Возвращает номер потомка
    qint32 childNumber(TagXmlItem *child, QStringList tags, QStringList parentTags);

    //! Вставка узла
    bool insertChild(const QString &tagName);

    //! Вставка узла
    bool insertChild(const QString &tagName, qint32 i,
                     QStringList tags = QStringList(),
                     QStringList parentTags = QStringList());

    //! Удаление узла
    bool removeChild(const qint32 &row);

    //! Проверка на возможность удаление узла
    bool checkRemoveChild(const qint32 &row);

    //! Возвращает True если унаследован
    bool isInherited();

signals:
    //! Сигнал об уничтожении объекта
    void destroyedItem(TagXmlItem *item);

private slots:
    //! Удаление наследуемого узла из списка
    void removeInheritedItem(TagXmlItem *parent);

private:
    //! Тэг
    QDomNode m_domNode;

    //! Родитель
    TagXmlItem *m_parentItem;

    //! Список потомков
    QList<TagXmlItem*> m_childItems;

    //! Список наследников
    QMap<TagXmlItem*, TagXmlItem*> m_inherited;
};

}}

#endif
