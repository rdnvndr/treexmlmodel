#include <QtGui>
#include <QAction>

#include "mainwindow.h"
#include "treemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

#ifdef Q_WS_MAEMO_5
    // Alternating row colors look bad on Maemo
    view->setAlternatingRowColors(false);
#endif

    QStringList headers;
    headers << tr("Title") << tr("Description");

    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();

    view->setModel(model);
    for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

    proxyModel = new ModifyProxyModel();
    proxyModel->setSourceModel(model);
    proxyView->setModel(proxyModel);


    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, static_cast<void (MainWindow::*)
            (const QItemSelection &, const QItemSelection &)>
            (&MainWindow::updateActions));

    connect(actionsMenu, &QMenu::aboutToShow, this,
            static_cast<void (MainWindow::*)()>(&MainWindow::updateActions));
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(insertColumnAction, &QAction::triggered, this,
            static_cast<bool (MainWindow::*)()>(&MainWindow::insertColumn));
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);

    connect(actionInsert, &QAction::triggered, this, &MainWindow::insert);
    connect(actionDelete, &QAction::triggered, this, &MainWindow::remove);
    connect(actionSubmit, &QAction::triggered, this, &MainWindow::submit);

    connect(removeColumnAction, &QAction::triggered, this,
            static_cast<bool (MainWindow::*)()>(&MainWindow::removeColumn));
    connect(insertChildAction,  &QAction::triggered, this, &MainWindow::insertChild);

    updateActions();
}

void MainWindow::insertChild()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"),
                                 Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1, parent);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"),
                             Qt::EditRole);

    updateActions();

    return changed;
}

bool MainWindow::insertColumn()
{
    return insertColumn(QModelIndex());
}

void MainWindow::insertRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool MainWindow::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

bool MainWindow::removeColumn()
{
    return removeColumn(QModelIndex());
}

void MainWindow::removeRow()
{
    QModelIndex index = view->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::insert()
{
    QModelIndex index = proxyView->currentIndex();
    QAbstractItemModel *model = proxyView->model();
    model->insertRow(0,index);
    updateActions();
}

void MainWindow::remove()
{
    QModelIndex index = proxyView->currentIndex();
    QAbstractItemModel *model = proxyView->model();
    proxyView->setCurrentIndex(QModelIndex());
    model->removeRow(index.row(),index.parent());
    updateActions();
}

void MainWindow::submit()
{
    proxyModel->submitAll();
    updateActions();
}

void MainWindow::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void MainWindow::updateActions(const QItemSelection &, const QItemSelection &)
{
    updateActions();
}
