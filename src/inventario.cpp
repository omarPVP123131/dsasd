#include "inventario.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QTableView>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

InventarioWidget::InventarioWidget(QWidget *parent) : QWidget(parent), db(QSqlDatabase::addDatabase("QSQLITE")) {
    db.setDatabaseName("inventario.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Error", "No se pudo abrir la base de datos.");
        return;
    }

    // Crear la tabla si no existe
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS productos (nombre TEXT PRIMARY KEY, cantidad INTEGER, precio REAL)")) {
        QMessageBox::critical(this, "Error", "No se pudo crear la tabla de productos.");
        return;
    }

    setupUi();
    setupConnections();
}

void InventarioWidget::setupUi() {
    QVBoxLayout* inventarioLayout = new QVBoxLayout(this);

    // Formulario de agregar/modificar producto
    formLayout = new QFormLayout();
    nombreProductoEdit = new QLineEdit();
    cantidadProductoEdit = new QLineEdit();
    precioProductoEdit = new QLineEdit();
    formLayout->addRow("Nombre del Producto:", nombreProductoEdit);
    formLayout->addRow("Cantidad:", cantidadProductoEdit);
    formLayout->addRow("Precio:", precioProductoEdit);

    // Botones
    buttonLayout = new QHBoxLayout();
    agregarProductoButton = new QPushButton("Agregar Producto");
    modificarProductoButton = new QPushButton("Modificar Producto");
    eliminarProductoButton = new QPushButton("Eliminar Producto");
    buscarProductoButton = new QPushButton("Buscar Producto");
    refrescarButton = new QPushButton("Refrescar");

    buttonLayout->addWidget(agregarProductoButton);
    buttonLayout->addWidget(modificarProductoButton);
    buttonLayout->addWidget(eliminarProductoButton);
    buttonLayout->addWidget(buscarProductoButton);
    buttonLayout->addWidget(refrescarButton);

    // Configurar la tabla
    inventarioTable = new QTableView();
    model = new QSqlTableModel(this, db);
    model->setTable("productos");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // Permite modificar la tabla
    model->select();
    inventarioTable->setModel(model);
    inventarioTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Agregar todo al layout principal
    inventarioLayout->addLayout(formLayout);
    inventarioLayout->addLayout(buttonLayout);
    inventarioLayout->addWidget(new QLabel("Lista de Productos"));
    inventarioLayout->addWidget(inventarioTable);
    setLayout(inventarioLayout);

    // Inicialmente deshabilitar los botones de modificar y eliminar
    modificarProductoButton->setEnabled(false);
    eliminarProductoButton->setEnabled(false);
}

void InventarioWidget::setupConnections() {
    connect(agregarProductoButton, &QPushButton::clicked, this, &InventarioWidget::addProduct);
    connect(modificarProductoButton, &QPushButton::clicked, this, &InventarioWidget::modifyProduct);
    connect(eliminarProductoButton, &QPushButton::clicked, this, &InventarioWidget::removeProduct);
    connect(buscarProductoButton, &QPushButton::clicked, this, &InventarioWidget::searchProduct);
    connect(refrescarButton, &QPushButton::clicked, this, &InventarioWidget::refresh);

    connect(inventarioTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &InventarioWidget::onSelectionChanged);
}

void InventarioWidget::addProduct() {
    QString nombre = nombreProductoEdit->text();
    QString cantidad = cantidadProductoEdit->text();
    QString precio = precioProductoEdit->text();

    if (nombre.isEmpty() || cantidad.isEmpty() || precio.isEmpty()) {
        QMessageBox::warning(this, "Error", "Todos los campos son obligatorios.");
        return;
    }

    bool cantidadValida = false;
    bool precioValido = false;
    cantidad.toInt(&cantidadValida);
    precio.toDouble(&precioValido);

    if (!cantidadValida || !precioValido) {
        QMessageBox::warning(this, "Error", "Cantidad y Precio deben ser valores numéricos.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO productos (nombre, cantidad, precio) VALUES (?, ?, ?)");
    query.addBindValue(nombre);
    query.addBindValue(cantidad.toInt());
    query.addBindValue(precio.toDouble());

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "No se pudo agregar el producto: " + query.lastError().text());
        return;
    }

    model->select();  // Recargar la tabla
    QMessageBox::information(this, "Éxito", "Producto agregado correctamente.");
    refresh();
}

void InventarioWidget::modifyProduct() {
    QModelIndexList selectedRows = inventarioTable->selectionModel()->selectedRows();

    if (selectedRows.empty()) {
        QMessageBox::warning(this, "Error", "Debe seleccionar un producto para modificar.");
        return;
    }

    QString nombre = nombreProductoEdit->text();
    QString cantidad = cantidadProductoEdit->text();
    QString precio = precioProductoEdit->text();

    if (nombre.isEmpty() || cantidad.isEmpty() || precio.isEmpty()) {
        QMessageBox::warning(this, "Error", "Todos los campos son obligatorios.");
        return;
    }

    bool cantidadValida = false;
    bool precioValido = false;
    cantidad.toInt(&cantidadValida);
    precio.toDouble(&precioValido);

    if (!cantidadValida || !precioValido) {
        QMessageBox::warning(this, "Error", "Cantidad y Precio deben ser valores numéricos.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    QString oldName = model->data(model->index(selectedIndex.row(), 0)).toString();

    QSqlQuery query;
    query.prepare("UPDATE productos SET nombre = ?, cantidad = ?, precio = ? WHERE nombre = ?");
    query.addBindValue(nombre);
    query.addBindValue(cantidad.toInt());
    query.addBindValue(precio.toDouble());
    query.addBindValue(oldName);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "No se pudo modificar el producto: " + query.lastError().text());
        return;
    }

    model->select();  // Recargar la tabla
    QMessageBox::information(this, "Éxito", "Producto modificado correctamente.");
    refresh();
}

void InventarioWidget::removeProduct() {
    QModelIndexList selectedRows = inventarioTable->selectionModel()->selectedRows();

    if (selectedRows.empty()) {
        QMessageBox::warning(this, "Error", "Debe seleccionar un producto para eliminar.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    QString nombre = model->data(model->index(selectedIndex.row(), 0)).toString();

    QSqlQuery query;
    query.prepare("DELETE FROM productos WHERE nombre = ?");
    query.addBindValue(nombre);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "No se pudo eliminar el producto: " + query.lastError().text());
        return;
    }

    model->select();  // Recargar la tabla
    QMessageBox::information(this, "Éxito", "Producto eliminado correctamente.");
    refresh();
}

void InventarioWidget::searchProduct() {
    QString nombre = nombreProductoEdit->text();

    if (nombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe ingresar un nombre para buscar.");
        return;
    }

    model->setFilter(QString("nombre = '%1'").arg(nombre));
    model->select();
}

void InventarioWidget::refresh() {
    model->select(); // Recargar la tabla
}

void InventarioWidget::onSelectionChanged() {
    QModelIndexList selectedRows = inventarioTable->selectionModel()->selectedRows();

    bool enableButtons = !selectedRows.empty();
    modificarProductoButton->setEnabled(enableButtons);
    eliminarProductoButton->setEnabled(enableButtons);
}
