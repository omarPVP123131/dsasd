#include "mainwindow.h"
#include "inventario.h"
#include "ui_mainwindow.h"  // Incluye este archivo generado por Qt
#include "reloj.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureWindow();
    setupTabs();
    applyStylesheet();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureWindow()
{
    setWindowTitle("Sistema de Punto de Venta");
    resize(1000, 600);
}

void MainWindow::setupTabs()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QTabWidget* tabWidget = new QTabWidget();
    addTabs(tabWidget);

    layout->addWidget(tabWidget);
    setCentralWidget(centralWidget);
}

void MainWindow::addTabs(QTabWidget* tabWidget)
{
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/venta.png"), "Punto de Venta");
    tabWidget->addTab(createTab(new InventarioWidget()), QIcon(":/styles/icons/inventario.png"), "Inventario");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/historial.png"), "Historial de Ventas");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/clientes.png"), "Clientes");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/proveedores.png"), "Proveedores");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/estadisticas.png"), "Estadísticas");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/notas.png"), "Bloc de Notas");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/calculadora.png"), "Calculadora");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/configuracion.png"), "Configuración");
    tabWidget->addTab(createTab(new QWidget()), QIcon(":/styles/icons/servidor.png"), "Servidor Local");
    tabWidget->addTab(createTab(new RelojWidget()), QIcon(":/styles/icons/reloj.png"), "Reloj");
}

QWidget* MainWindow::createTab(QWidget* widget)
{
    // Aquí puedes hacer más configuraciones específicas de cada pestaña si es necesario
    return widget;
}

void MainWindow::applyStylesheet()
{
    QFile styleFile(":/styles/stylesheet.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        QString style = stream.readAll();
        setStyleSheet(style);
    } else {
        qDebug() << "No se pudo abrir el archivo de estilo.";
    }
}
