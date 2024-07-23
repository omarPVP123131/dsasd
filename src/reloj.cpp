#include "reloj.h"
#include "DialogoPrevisualizacion.h"
#include <QTime>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QTimeZone>
#include <QColorDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonParseError>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QGridLayout>

RelojWidget::RelojWidget(QWidget *parent) : QWidget(parent), formato24Horas(true), borderStyle("solid")
{
    QDir directorio("plantillas");
    if (!directorio.exists()) {
        directorio.mkpath(".");
    }

    tiempoLabel = new QLabel(this);
    tiempoLabel->setAlignment(Qt::AlignCenter);

    inicializarComponentes();
    cargarConfiguracion();
    aplicarEstilosGlobales();

    // Inicializar la configuración original
    configuracionOriginal.insert("formato24Horas", formato24Horas);
    configuracionOriginal.insert("zonaHoraria", comboZonaHoraria->currentText());
    configuracionOriginal.insert("colorTexto", colorTexto);
    configuracionOriginal.insert("colorFondo", colorFondo);
    configuracionOriginal.insert("estiloBorde", borderStyle);
    configuracionOriginal.insert("fuente", tiempoLabel->font().toString());
    configuracionOriginal.insert("mostrarFecha", checkBoxMostrarFecha->isChecked());

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RelojWidget::actualizarHora);
    timer->start(1000);

    actualizarDisplay();
}


void RelojWidget::inicializarComponentes()
{
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);

    QGroupBox *grupoReloj = new QGroupBox("Reloj", this);
    QVBoxLayout *layoutReloj = new QVBoxLayout();
    layoutReloj->addWidget(tiempoLabel);

    comboZonaHoraria = new QComboBox(this);
    foreach (const QByteArray &zona, QTimeZone::availableTimeZoneIds()) {
        comboZonaHoraria->addItem(QString::fromUtf8(zona));
    }
    connect(comboZonaHoraria, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RelojWidget::actualizarZonaHoraria);
    layoutReloj->addWidget(comboZonaHoraria);

    grupoReloj->setLayout(layoutReloj);

    QGroupBox *grupoColores = new QGroupBox("Colores", this);
    QGridLayout *layoutColores = new QGridLayout();

    botonColorTexto = new QToolButton(this);
    botonColorTexto->setIcon(QIcon::fromTheme("color-text"));
    botonColorTexto->setText("Cambiar Color de Texto");
    botonColorTexto->setToolTip("Cambiar el color del texto");
    connect(botonColorTexto, &QToolButton::clicked, this, &RelojWidget::cambiarColorTexto);
    layoutColores->addWidget(botonColorTexto, 0, 0);

    botonColorFondo = new QToolButton(this);
    botonColorFondo->setIcon(QIcon::fromTheme("color-background"));
    botonColorFondo->setText("Cambiar Color de Fondo");
    botonColorFondo->setToolTip("Cambiar el color de fondo");
    connect(botonColorFondo, &QToolButton::clicked, this, &RelojWidget::cambiarColorFondo);
    layoutColores->addWidget(botonColorFondo, 0, 1);

    grupoColores->setLayout(layoutColores);

    QGroupBox *grupoPersonalizacion = new QGroupBox("Personalización", this);
    QGridLayout *layoutPersonalizacion = new QGridLayout();

    botonCambiarFuente = new QToolButton(this);
    botonCambiarFuente->setIcon(QIcon::fromTheme("font"));
    botonCambiarFuente->setText("Cambiar Fuente y Tamaño");
    botonCambiarFuente->setToolTip("Cambiar la fuente y el tamaño del texto");
    connect(botonCambiarFuente, &QToolButton::clicked, this, &RelojWidget::cambiarFuente);
    layoutPersonalizacion->addWidget(botonCambiarFuente, 0, 0);

    botonCambiarEstiloBorde = new QToolButton(this);
    botonCambiarEstiloBorde->setIcon(QIcon::fromTheme("border-style"));
    botonCambiarEstiloBorde->setText("Cambiar Estilo del Borde");
    botonCambiarEstiloBorde->setToolTip("Cambiar el estilo del borde del reloj");
    connect(botonCambiarEstiloBorde, &QToolButton::clicked, this, &RelojWidget::cambiarEstiloBorde);
    layoutPersonalizacion->addWidget(botonCambiarEstiloBorde, 0, 1);

    grupoPersonalizacion->setLayout(layoutPersonalizacion);

    QGroupBox *grupoConfiguracion = new QGroupBox("Configuración y Plantillas", this);
    QGridLayout *layoutConfiguracion = new QGridLayout();

    checkBoxMostrarFecha = new QCheckBox("Mostrar Fecha", this);
    connect(checkBoxMostrarFecha, &QCheckBox::toggled, this, &RelojWidget::actualizarEstadoCheckBox);
    layoutConfiguracion->addWidget(checkBoxMostrarFecha, 0, 0, 1, 2);

    botonAbrirConfiguracion = new QToolButton(this);
    botonAbrirConfiguracion->setIcon(QIcon::fromTheme("settings"));
    botonAbrirConfiguracion->setText("Abrir Configuración Manual");
    botonAbrirConfiguracion->setToolTip("Abrir configuración manual del reloj");
    connect(botonAbrirConfiguracion, &QToolButton::clicked, this, &RelojWidget::abrirConfiguracionManual);
    layoutConfiguracion->addWidget(botonAbrirConfiguracion, 1, 0);

    botonPlantillas = new QToolButton(this);
    botonPlantillas->setIcon(QIcon::fromTheme("template"));
    botonPlantillas->setText("Seleccionar Plantilla");
    botonPlantillas->setToolTip("Seleccionar una plantilla para el reloj");
    connect(botonPlantillas, &QToolButton::clicked, this, &RelojWidget::abrirPlantilla);
    layoutConfiguracion->addWidget(botonPlantillas, 1, 1);

    botonFormato = new QToolButton(this);
    botonFormato->setIcon(QIcon::fromTheme("format-time"));
    botonFormato->setText("Alternar Formato");
    botonFormato->setToolTip("Alternar entre formato de 12 y 24 horas");
    connect(botonFormato, &QToolButton::clicked, this, &RelojWidget::alternarFormato);
    layoutConfiguracion->addWidget(botonFormato, 2, 0, 1, 2);

    grupoConfiguracion->setLayout(layoutConfiguracion);

    layoutPrincipal->addWidget(grupoReloj);
    layoutPrincipal->addWidget(grupoColores);
    layoutPrincipal->addWidget(grupoPersonalizacion);
    layoutPrincipal->addWidget(grupoConfiguracion);

    setLayout(layoutPrincipal);
}

void RelojWidget::aplicarEstilosGlobales()
{
    QString estiloBoton =
        "QToolButton {"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 5px;"
        "    padding: 10px;"
        "    font-size: 16px;"
        "    min-width: 200px;"
        "}"
        "QToolButton:hover {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "}"
        "QToolButton:pressed {"
        "    background-color: #45a049;"
        "}";
    this->setStyleSheet(estiloBoton);
}

void RelojWidget::abrirPlantilla()
{
    QString rutaDirectorio = "plantillas/";
    QDir directorio(rutaDirectorio);
    QStringList archivos = directorio.entryList(QStringList() << "*.json", QDir::Files);
    QStringList rutasCompleta;
    for (const QString &archivo : archivos) {
        rutasCompleta.append(directorio.filePath(archivo));
    }

    DialogoPrevisualizacion dialogo(rutasCompleta, this);
    if (dialogo.exec() == QDialog::Accepted) {
        QJsonObject obj = dialogo.obtenerConfiguracionSeleccionada();

        formato24Horas = obj.value("formato24Horas").toBool(formato24Horas);

        QString zonaHoraria = obj.value("zonaHoraria").toString(comboZonaHoraria->currentText());
        int index = comboZonaHoraria->findText(zonaHoraria);
        if (index != -1) {
            comboZonaHoraria->setCurrentIndex(index);
        } else {
            qWarning() << "Zona horaria no encontrada en el combo box:" << zonaHoraria;
        }

        colorTexto = obj.value("colorTexto").toString(colorTexto);
        colorFondo = obj.value("colorFondo").toString(colorFondo);
        borderStyle = obj.value("estiloBorde").toString(borderStyle);

        QFont font;
        font.fromString(obj.value("fuente").toString());
        tiempoLabel->setFont(font);

        checkBoxMostrarFecha->setChecked(obj.value("mostrarFecha").toBool(checkBoxMostrarFecha->isChecked()));

        establecerEstilo();

        QFile archivoConfig("configuracion.json");
        if (archivoConfig.open(QIODevice::WriteOnly)) {
            QJsonObject config;
            config.insert("formato24Horas", formato24Horas);
            config.insert("zonaHoraria", comboZonaHoraria->currentText());
            config.insert("colorTexto", colorTexto);
            config.insert("colorFondo", colorFondo);
            config.insert("estiloBorde", borderStyle);
            config.insert("fuente", tiempoLabel->font().toString());
            config.insert("mostrarFecha", checkBoxMostrarFecha->isChecked());

            QJsonDocument docConfig(config);
            archivoConfig.write(docConfig.toJson());
            archivoConfig.close();
        } else {
            qWarning() << "No se pudo guardar la configuración en configuracion.json.";
        }
    }
}



void RelojWidget::guardarPlantilla()
{
    QString nombrePlantilla = QInputDialog::getText(this, "Nombre de Plantilla", "Nombre para la plantilla:");
    if (!nombrePlantilla.isEmpty()) {
        QString rutaArchivo = QString("plantillas/%1.json").arg(nombrePlantilla);
        QFile archivo(rutaArchivo);
        if (archivo.open(QIODevice::WriteOnly)) {
            QJsonObject obj;
            obj.insert("formato24Horas", formato24Horas);
            obj.insert("zonaHoraria", comboZonaHoraria->currentText());
            obj.insert("colorTexto", colorTexto.isEmpty() ? "#76FF03" : colorTexto);
            obj.insert("colorFondo", colorFondo.isEmpty() ? "#0099ff" : colorFondo);
            obj.insert("fuente", tiempoLabel->font().toString());
            obj.insert("estiloBorde", borderStyle);
            obj.insert("mostrarFecha", checkBoxMostrarFecha->isChecked());

            QJsonDocument doc(obj);
            archivo.write(doc.toJson());
            archivo.close();
            qDebug() << "Plantilla guardada:" << rutaArchivo;
        } else {
            qWarning() << "No se pudo guardar la plantilla.";
        }
    }
}

void RelojWidget::abrirConfiguracionManual()
{
    QString archivoPath = QFileDialog::getOpenFileName(this, "Abrir Configuración de Tema", "", "Archivos JSON (*.json)");
    if (!archivoPath.isEmpty()) {
        QFile archivo(archivoPath);
        if (archivo.open(QIODevice::ReadOnly)) {
            QByteArray contenido = archivo.readAll();
            QJsonDocument doc(QJsonDocument::fromJson(contenido));
            QJsonObject obj = doc.object();

            formato24Horas = obj.value("formato24Horas").toBool(formato24Horas);
            QString zonaHoraria = obj.value("zonaHoraria").toString(comboZonaHoraria->currentText());
            int index = comboZonaHoraria->findText(zonaHoraria);
            if (index != -1) {
                comboZonaHoraria->setCurrentIndex(index);
            }
            colorTexto = obj.value("colorTexto").toString(colorTexto);
            colorFondo = obj.value("colorFondo").toString(colorFondo);
            borderStyle = obj.value("estiloBorde").toString(borderStyle);
            QFont font;
            font.fromString(obj.value("fuente").toString());
            tiempoLabel->setFont(font);
            checkBoxMostrarFecha->setChecked(obj.value("mostrarFecha").toBool(checkBoxMostrarFecha->isChecked()));

            establecerEstilo();

            archivo.close();
        } else {
            qWarning() << "No se pudo abrir el archivo de configuración.";
        }
    }
}



void RelojWidget::establecerEstilo()
{
    QString estilo = QString("background-color: %1; color: %2; border: 2px %3;").arg(colorFondo, colorTexto, borderStyle);
    tiempoLabel->setStyleSheet(estilo);
}


void RelojWidget::actualizarHora()
{
    actualizarDisplay();
}

void RelojWidget::alternarFormato()
{
    QMessageBox::StandardButton respuesta = QMessageBox::question(
        this,
        "Confirmar Cambio de Formato",
        "¿Desea cambiar entre formato de 12 y 24 horas?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (respuesta == QMessageBox::Yes) {
        formato24Horas = !formato24Horas;
        actualizarDisplay();
        guardarConfiguracion();
    }
}

void RelojWidget::actualizarZonaHoraria(int index)
{
    guardarConfiguracion();
    actualizarDisplay();
}

bool RelojWidget::tieneCambiosNoGuardados() const
{
    return formato24Horas != configuracionOriginal.value("formato24Horas").toBool() ||
           comboZonaHoraria->currentText() != configuracionOriginal.value("zonaHoraria").toString() ||
           colorTexto != configuracionOriginal.value("colorTexto").toString() ||
           colorFondo != configuracionOriginal.value("colorFondo").toString() ||
           borderStyle != configuracionOriginal.value("estiloBorde").toString() ||
           tiempoLabel->font() != QFont(configuracionOriginal.value("fuente").toString()) ||
           checkBoxMostrarFecha->isChecked() != configuracionOriginal.value("mostrarFecha").toBool();
}

void RelojWidget::restablecerZonaHoraria()
{
    int index = comboZonaHoraria->findText("America/Mexico_City");
    if (index != -1) {
        comboZonaHoraria->setCurrentIndex(index);
    }
    guardarConfiguracion();
    actualizarDisplay();
}

void RelojWidget::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton respuesta = QMessageBox::question(
        this,
        "Confirmar Cierre",
        "¿Desea guardar la configuración antes de cerrar?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Cancel
        );

    if (respuesta == QMessageBox::Yes) {
        guardarConfiguracion();
        event->accept();
    } else if (respuesta == QMessageBox::No) {
        event->accept();
    } else {
        event->ignore();
    }
}

void RelojWidget::actualizarDisplay()
{
    QTimeZone zonaHoraria(comboZonaHoraria->currentText().toUtf8());
    QDateTime horaLocal = QDateTime::currentDateTime().toTimeZone(zonaHoraria);
    QString horaFormateada = horaLocal.time().toString(formato24Horas ? "HH:mm:ss" : "hh:mm:ss AP");

    if (checkBoxMostrarFecha->isChecked()) {
        horaFormateada += " " + horaLocal.date().toString("dd/MM/yyyy");
    }

    tiempoLabel->setText(horaFormateada);
    establecerEstilo();
    qDebug() << "Display actualizado. Mostrar Fecha:" << checkBoxMostrarFecha->isChecked();
}

void RelojWidget::guardarConfiguracion()
{
    QJsonObject obj;
    obj.insert("formato24Horas", formato24Horas);
    obj.insert("zonaHoraria", comboZonaHoraria->currentText());
    obj.insert("colorTexto", colorTexto.isEmpty() ? "#76FF03" : colorTexto);
    obj.insert("colorFondo", colorFondo.isEmpty() ? "#2d2d2d" : colorFondo);
    obj.insert("fuente", tiempoLabel->font().toString());
    obj.insert("estiloBorde", borderStyle);
    obj.insert("mostrarFecha", checkBoxMostrarFecha->isChecked());

    QJsonDocument doc(obj);
    QFile archivo("configuracion.json");
    if (archivo.open(QIODevice::WriteOnly)) {
        archivo.write(doc.toJson());
        archivo.close();
        qDebug() << "Configuración guardada:" << obj;
    } else {
        qWarning() << "No se pudo abrir el archivo para escritura.";
    }
}


void RelojWidget::cargarConfiguracion()
{
    QFile archivo("configuracion.json");
    if (archivo.open(QIODevice::ReadOnly)) {
        QByteArray contenido = archivo.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(contenido));

        if (!doc.isObject()) {
            qWarning() << "El documento JSON no es un objeto.";
            return;
        }

        QJsonObject obj = doc.object();

        formato24Horas = obj.value("formato24Horas").toBool(false);
        QString zonaHoraria = obj.value("zonaHoraria").toString("America/Mexico_City");
        int index = comboZonaHoraria->findText(zonaHoraria);
        if (index != -1) {
            comboZonaHoraria->setCurrentIndex(index);
        }
        colorTexto = obj.value("colorTexto").toString("#76FF03");
        colorFondo = obj.value("colorFondo").toString("#2d2d2d");
        QFont font;
        font.fromString(obj.value("fuente").toString());
        tiempoLabel->setFont(font);
        borderStyle = obj.value("estiloBorde").toString("solid");
        checkBoxMostrarFecha->setChecked(obj.value("mostrarFecha").toBool(true));

        establecerEstilo();
        actualizarDisplay();
        archivo.close();
    } else {
        qWarning() << "No se pudo abrir el archivo para lectura, usando valores por defecto.";
        colorTexto = "#76FF03";
        colorFondo = "#2d2d2d";
        formato24Horas = false;
        QString zonaHorariaPorDefecto = "America/Mexico_City";
        int index = comboZonaHoraria->findText(zonaHorariaPorDefecto);
        if (index != -1) {
            comboZonaHoraria->setCurrentIndex(index);
        }
        borderStyle = "solid";
        checkBoxMostrarFecha->setChecked(true);

        establecerEstilo();
        actualizarDisplay();
    }
}


void RelojWidget::cambiarColorTexto()
{
    QColor color = QColorDialog::getColor(QColor(colorTexto), this, "Seleccione Color de Texto");
    if (color.isValid()) {
        colorTexto = color.name();
        establecerEstilo();
        if (tieneCambiosNoGuardados()) {
            QMessageBox::StandardButton respuesta = QMessageBox::question(
                this,
                "Confirmar Cambios",
                "¿Desea guardar los cambios en la configuración?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
                );
            if (respuesta == QMessageBox::Yes) {
                guardarConfiguracion();
            }
        }
    }
}

void RelojWidget::cambiarColorFondo()
{
    QColor color = QColorDialog::getColor(QColor(colorFondo), this, "Seleccione Color de Fondo");
    if (color.isValid()) {
        colorFondo = color.name();
        establecerEstilo();
        if (tieneCambiosNoGuardados()) {
            QMessageBox::StandardButton respuesta = QMessageBox::question(
                this,
                "Confirmar Cambios",
                "¿Desea guardar los cambios en la configuración?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
                );
            if (respuesta == QMessageBox::Yes) {
                guardarConfiguracion();
            }
        }
    }
}

void RelojWidget::cambiarFuente()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, tiempoLabel->font(), this, "Seleccionar Fuente");
    if (ok) {
        tiempoLabel->setFont(font);
        if (tieneCambiosNoGuardados()) {
            QMessageBox::StandardButton respuesta = QMessageBox::question(
                this,
                "Confirmar Cambios",
                "¿Desea guardar los cambios en la configuración?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
                );
            if (respuesta == QMessageBox::Yes) {
                guardarConfiguracion();
            }
        }
    }
}

void RelojWidget::actualizarEstadoCheckBox()
{
    bool mostrarFecha = checkBoxMostrarFecha->isChecked();

    actualizarDisplay();

    if (mostrarFecha) {
        tiempoLabel->setText(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    } else {
        tiempoLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    }
}


void RelojWidget::cambiarEstiloBorde()
{
    QString estilo = QInputDialog::getItem(this, "Seleccionar Estilo de Borde", "Estilo:", {"solid", "dashed", "dotted", "double"}, 0, false);
    if (!estilo.isEmpty()) {
        borderStyle = estilo;
        establecerEstilo();
        if (tieneCambiosNoGuardados()) {
            QMessageBox::StandardButton respuesta = QMessageBox::question(
                this,
                "Confirmar Cambios",
                "¿Desea guardar los cambios en la configuración?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
                );
            if (respuesta == QMessageBox::Yes) {
                guardarConfiguracion();
            }
        }
    }
}
