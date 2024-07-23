#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>

class InventarioWidget : public QWidget {
    Q_OBJECT

public:
    explicit InventarioWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void setupConnections();

    // Miembros de la clase
    QSqlDatabase db;
    QSqlTableModel *model;
    QFormLayout *formLayout;
    QLineEdit *nombreProductoEdit;
    QLineEdit *cantidadProductoEdit;
    QLineEdit *precioProductoEdit;
    QTableView *inventarioTable;
    QHBoxLayout *buttonLayout;
    QPushButton *agregarProductoButton;
    QPushButton *modificarProductoButton;
    QPushButton *eliminarProductoButton;
    QPushButton *buscarProductoButton;
    QPushButton *refrescarButton;

private slots:
    void addProduct();
    void modifyProduct();
    void removeProduct();
    void searchProduct();
    void refresh();
    void onSelectionChanged();
};

#endif // INVENTARIO_H

