#include <QObject>
#include <QtTest/QtTest>

#include <modeltest/modeltest.h>
#include <treexmlmodel/treexmlmodel.h>
#include <treexmlmodel/modifyproxymodel.h>
#include <QtXml/QDomDocument>

using namespace RTPTechGroup::XmlModel;

class tst_ModifyProxyModel : public QObject
{
    Q_OBJECT
public:
    explicit tst_ModifyProxyModel(QObject *parent = 0);
    virtual ~tst_ModifyProxyModel();
    
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void modelTest();

private:
    TreeXmlModel     *m_model;
    ModifyProxyModel *m_proxy;
};

tst_ModifyProxyModel::tst_ModifyProxyModel(QObject *parent) :
    m_model(0)
{
    Q_UNUSED(parent)
    m_proxy = NULL;
}

tst_ModifyProxyModel::~tst_ModifyProxyModel()
{

}

void tst_ModifyProxyModel::initTestCase()
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

            m_proxy = new ModifyProxyModel();
            m_proxy->setSourceModel(m_model);
            m_proxy->setHeaderData(0, Qt::Horizontal, tr("Имя атирибута"));
            m_proxy->setHeaderData(1, Qt::Horizontal, tr("Родитель"));
        }
    }
}

void tst_ModifyProxyModel::cleanupTestCase()
{
    delete m_proxy;
    delete m_model;
}

void tst_ModifyProxyModel::init()
{

}

void tst_ModifyProxyModel::cleanup()
{

}

void tst_ModifyProxyModel::modelTest()
{
//    new ModelTest(m_proxy, this);
}

QTEST_MAIN(tst_ModifyProxyModel)
#include "tst_modifyproxymodel.moc"
