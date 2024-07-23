#ifndef DIALOGOPREVISUALIZACION_H
#define DIALOGOPREVISUALIZACION_H

#include <QDialog>
#include <QJsonObject>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QResizeEvent>

class DialogoPrevisualizacion : public QDialog
{
    Q_OBJECT

public:
    DialogoPrevisualizacion(const QStringList &rutas, QWidget *parent = nullptr);

    QJsonObject obtenerConfiguracionSeleccionada() const;

private slots:
    void seleccionarPlantilla(QListWidgetItem *item);
    void confirmar();
    void cancelar();

private:
    void cargarPlantillas(const QStringList &rutas);
    void aplicarConfiguracion(const QJsonObject &config);
    void resizeEvent(QResizeEvent *event) override;  // Override resizeEvent to adjust the window size

    QListWidget *listaPlantillas;
    QPushButton *confirmarButton;
    QPushButton *cancelarButton;
    QJsonObject plantillaSeleccionada;
};


#endif // DIALOGOPREVISUALIZACION_H
