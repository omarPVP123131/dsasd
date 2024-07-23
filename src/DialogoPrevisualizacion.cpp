#include "DialogoPrevisualizacion.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFont>
#include <QResizeEvent>

DialogoPrevisualizacion::DialogoPrevisualizacion(const QStringList &rutas, QWidget *parent)
    : QDialog(parent), listaPlantillas(new QListWidget(this)), confirmarButton(new QPushButton("Confirmar", this)),
    cancelarButton(new QPushButton("Cancelar", this))
{
    setWindowTitle("Explorador de Plantillas");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listaPlantillas);
    layout->addWidget(confirmarButton);
    layout->addWidget(cancelarButton);
    setLayout(layout);

    cargarPlantillas(rutas);

    connect(listaPlantillas, &QListWidget::itemClicked, this, &DialogoPrevisualizacion::seleccionarPlantilla);
    connect(confirmarButton, &QPushButton::clicked, this, &DialogoPrevisualizacion::confirmar);
    connect(cancelarButton, &QPushButton::clicked, this, &DialogoPrevisualizacion::cancelar);
}

void DialogoPrevisualizacion::cargarPlantillas(const QStringList &rutas)
{
    listaPlantillas->clear();
    for (const QString &ruta : rutas) {
        QListWidgetItem *item = new QListWidgetItem(ruta);
        listaPlantillas->addItem(item);
    }
    adjustSize();  // Ajusta el tamaño de la ventana al contenido
}

void DialogoPrevisualizacion::seleccionarPlantilla(QListWidgetItem *item)
{
    QString rutaArchivo = item->text();
    QFile archivo(rutaArchivo);
    if (archivo.open(QIODevice::ReadOnly)) {
        QByteArray contenido = archivo.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(contenido));
        QJsonObject obj = doc.object();
        aplicarConfiguracion(obj);
        plantillaSeleccionada = obj;
    }
}

void DialogoPrevisualizacion::aplicarConfiguracion(const QJsonObject &config)
{
    QString colorTexto = config.value("colorTexto").toString("#76FF03");
    QString colorFondo = config.value("colorFondo").toString("#0099ff");
    QFont fuente;
    fuente.fromString(config.value("fuente").toString());

    // Aplicar los estilos para la previsualización
    listaPlantillas->setStyleSheet(QString("background-color: %1; color: %2;").arg(colorFondo, colorTexto));
    listaPlantillas->setFont(fuente);
}

QJsonObject DialogoPrevisualizacion::obtenerConfiguracionSeleccionada() const
{
    return plantillaSeleccionada;
}

void DialogoPrevisualizacion::confirmar()
{
    accept();
}

void DialogoPrevisualizacion::cancelar()
{
    reject();
}

void DialogoPrevisualizacion::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    adjustSize();  // Ajusta el tamaño de la ventana cuando cambia su tamaño
}
