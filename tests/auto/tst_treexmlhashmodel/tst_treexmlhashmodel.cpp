#include <QObject>
#include <QtTest/QtTest>

#include <modeltest/modeltest.h>
#include <treexmlmodel/treexmlhashmodel.h>
#include <QtXml/QDomDocument>

using namespace RTPTechGroup::XmlModel;

class tst_TreeXmlHashModel : public QObject
{
    Q_OBJECT
public:
    explicit tst_TreeXmlHashModel(QObject *parent = 0);
    virtual ~tst_TreeXmlHashModel();
    
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void modelTest();

private:
     TreeXmlHashModel *m_model;
};

tst_TreeXmlHashModel::tst_TreeXmlHashModel(QObject *parent) :
    QObject(parent)
{
    m_model = NULL;
}

tst_TreeXmlHashModel::~tst_TreeXmlHashModel()
{
    delete m_model;
}

void tst_TreeXmlHashModel::initTestCase()
{
    QFile file(":/test.xml");
    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if (document.setContent(&file)) {
            m_model = new TreeXmlHashModel(document, this);

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
        }
    }
}

void tst_TreeXmlHashModel::cleanupTestCase()
{

}

void tst_TreeXmlHashModel::init()
{

}

void tst_TreeXmlHashModel::cleanup()
{

}

void tst_TreeXmlHashModel::modelTest()
{
//    new ModelTest(m_model, this);
}

QTEST_MAIN(tst_TreeXmlHashModel)
#include "tst_treexmlhashmodel.moc"
