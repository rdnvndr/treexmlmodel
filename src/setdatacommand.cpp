#include "setdatacommand.h"

SetDataCommand::SetDataCommand(const QPersistentModelIndex &index,
                               const QVariant &oldValue, const QVariant &newValue)
    : m_index(index), m_oldValue(oldValue), m_newValue(newValue)
{

}


void SetDataCommand::undo()
{
    if (m_index.isValid())
        const_cast<QAbstractItemModel *>(m_index.model())->setData(m_index, m_oldValue);
}

void SetDataCommand::redo()
{
    if (m_index.isValid())
        const_cast<QAbstractItemModel *>(m_index.model())->setData(m_index, m_newValue);
}

