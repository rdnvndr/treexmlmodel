#include <QObject>
#include <QtTest/QtTest>

#include <modeltest/modeltest.h>
#include <treexmlmodel/treexmlmodel.h>
#include <QtXml/QDomDocument>

using namespace RTPTechGroup::XmlModel;

class tst_TreeXmlModel : public QObject
{
    Q_OBJECT
public:
    explicit tst_TreeXmlModel(QObject *parent = 0);
    virtual ~tst_TreeXmlModel();
    
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void modelTest();

private:
    TreeXmlModel *m_model;
    
};

tst_TreeXmlModel::tst_TreeXmlModel(QObject *parent) :
    m_model(0)
{
    Q_UNUSED(parent)
}

tst_TreeXmlModel::~tst_TreeXmlModel()
{

}

void tst_TreeXmlModel::initTestCase()
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

            m_model->setHeaderData(0, Qt::Horizontal, tr("Имя атрибута"));
            m_model->setHeaderData(1, Qt::Horizontal, tr("Родитель"));

            m_model->addAttrTag("RTPClass", "RTPAttribute");
        }
    }
}

void tst_TreeXmlModel::cleanupTestCase()
{
    delete m_model;
}

void tst_TreeXmlModel::init()
{

}

void tst_TreeXmlModel::cleanup()
{

}

void tst_TreeXmlModel::modelTest()
{
//    new ModelTest(m_model, this);
}

QTEST_MAIN(tst_TreeXmlModel)
#include "tst_treexmlmodel.moc"
