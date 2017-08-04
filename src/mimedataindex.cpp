#include "mimedataindex.h"

namespace RTPTechGroup {
namespace XmlModel {

MimeDataIndex::MimeDataIndex(): QMimeData ()
{

}

void MimeDataIndex::setIndexes(PersistentIndexes indexes)
{
    m_indexes =indexes;
}

PersistentIndexes MimeDataIndex::indexes() const
{
    return m_indexes;
}

bool MimeDataIndex::hasIndexes() const
{
    if (m_indexes.isEmpty())
        return false;
    return true;
}

QStringList MimeDataIndex::formats() const
{
    QStringList format = QMimeData::formats();
    if (hasIndexes()) format << "application/classxmlmodel";
    return format;
}

}}
