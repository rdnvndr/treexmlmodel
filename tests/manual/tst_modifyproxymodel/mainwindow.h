#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <treexmlmodel/modifyproxymodel.h>
#include "ui_mainwindow.h"

class QAction;
class QTreeView;
class QWidget;

using namespace RTPTechGroup::XmlModel;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ModifyProxyModel *proxyModel;
public slots:
    void updateActions();
    void updateActions(const QItemSelection &, const QItemSelection &);

private slots:
    void insertChild();
    bool insertColumn(const QModelIndex &parent);
    bool insertColumn();
    void insertRow();
    bool removeColumn(const QModelIndex &parent);
    bool removeColumn();
    void removeRow();

    void insert();
    void remove();
    void submit();
};

#endif
