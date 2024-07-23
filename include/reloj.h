#ifndef RELOJ_H
#define RELOJ_H

#include <QWidget>
#include <QLCDNumber>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QTimer>
#include <QColor>
#include <QGridLayout>      // Incluido para QGridLayout
#include <QJsonObject>
#include <QFormLayout>      // Incluido para QFormLayout
#include <QGroupBox>        // Incluido para QGroupBox
#include <QToolButton>

class RelojWidget : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override;
    bool tieneCambiosNoGuardados() const;

public:
    explicit RelojWidget(QWidget *parent = nullptr);

private slots:
    void actualizarHora();
    void alternarFormato();
    void actualizarZonaHoraria(int index);
    void cambiarColorTexto();
    void cambiarColorFondo();
    void cambiarFuente();
    void cambiarEstiloBorde();
    void abrirConfiguracionManual();
    void actualizarDisplay();
    void restablecerZonaHoraria();
    void actualizarEstadoCheckBox();

private:
    void inicializarComponentes();
    void establecerEstilo();
    void guardarConfiguracion();
    void cargarConfiguracion();
    void abrirPlantilla();
    void guardarPlantilla();
    void aplicarEstilosGlobales(); // Añadir la declaración de la función aquí

    QLCDNumber *lcd;  // Comentado si no se usa
    QLabel *tiempoLabel;
    QToolButton *botonFormato;
    QToolButton *botonColorTexto;
    QToolButton *botonColorFondo;
    QToolButton *botonCambiarFuente;
    QToolButton *botonCambiarEstiloBorde;
    QToolButton *botonAbrirConfiguracion;
    QToolButton *botonPlantillas;
    QToolButton *botonGuardarPlantilla;
    QCheckBox *checkBoxMostrarFecha;
    QComboBox *comboZonaHoraria;
    QTimer *timer;
    bool formato24Horas;
    QString colorTexto;
    QString colorFondo;
    QString borderStyle;
    QFont fuente;
    QJsonObject configuracionOriginal;  // Añadido aquí
};

#endif // RELOJ_H
