#include <QObject>
#include <QtTest/QtTest>

#include <modeltest/modeltest.h>
#include <treexmlmodel/treexmlmodel.h>
#include <treexmlmodel/tablexmlproxymodel.h>
#include <QtXml/QDomDocument>

using namespace RTPTechGroup::XmlModel;

class tst_TableXmlProxyModel : public QObject
{
    Q_OBJECT
public:
    explicit tst_TableXmlProxyModel(QObject *parent = 0);
    virtual ~tst_TableXmlProxyModel();
    
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void modelTest();

private:
    TreeXmlModel *m_model;
    TableXMLProxyModel *m_proxy;
};

tst_TableXmlProxyModel::tst_TableXmlProxyModel(QObject *parent) :
    m_model(0)
{
    Q_UNUSED(parent)
    m_proxy = NULL;
}

tst_TableXmlProxyModel::~tst_TableXmlProxyModel()
{

}

void tst_TableXmlProxyModel::initTestCase()
{
    QFile file(":/test.xml");
    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if (document.setContent(&file)) {
            m_model = new TreeXmlModel(document, this);

            m_model->addTagFilter("RTPClass");
            m_model->addTagFilter("RTPAttribute");

            QStringList propsClass;
            propsClass << "name" << "parent";
            m_model->addDisplayedAttr("RTPClass",propsClass);
            QStringList propsAttr;
            propsAttr << "name" << "parent";
            m_model->addDisplayedAttr("RTPAttribute",propsAttr);
            m_model->setHeaderData(0, Qt::Horizontal, tr("Имя атирибута"));
            m_model->setHeaderData(1, Qt::Horizontal, tr("Родитель"));
            m_model->addAttrTag("RTPClass", "RTPAttribute");

            m_proxy = new TableXMLProxyModel();
            QStringList tags;
            tags << "RTPAttribute";
            m_proxy->setAttributeTags(tags);
            m_proxy->setSourceModel(m_model);
            m_proxy->setHeaderData(0, Qt::Horizontal, tr("Имя атирибута"));
            m_proxy->setHeaderData(1, Qt::Horizontal, tr("Родитель"));
        }
    }
}

void tst_TableXmlProxyModel::cleanupTestCase()
{
    delete m_proxy;
    delete m_model;
}

void tst_TableXmlProxyModel::init()
{

}

void tst_TableXmlProxyModel::cleanup()
{

}

void tst_TableXmlProxyModel::modelTest()
{
//    new ModelTest(m_proxy, this);
}

QTEST_MAIN(tst_TableXmlProxyModel)
#include "tst_tablexmlproxymodel.moc"
