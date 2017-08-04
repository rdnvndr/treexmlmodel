#ifndef SETDATACOMMAND_H
#define SETDATACOMMAND_H

#include <QUndoCommand>
#include <QPersistentModelIndex>

//! Команда отмены установки значения в модели
/*! Класс предназначен для описания команды установки значения модели
 */

class SetDataCommand: public QUndoCommand
{

public:
    //! Конструктор класса
    SetDataCommand(const QPersistentModelIndex &index,
                   const QVariant &oldValue, const QVariant &newValue);

    //! Отмена команды
    void undo();

    //! Повтор команды
    void redo();

private:
    const QPersistentModelIndex m_index; ///< Ссылка на индекс модели
    const QVariant m_oldValue; ///< Хранение старого значения.
    const QVariant m_newValue; ///< Хранения нового значения.
};




#endif // SETDATACOMMAND_H
