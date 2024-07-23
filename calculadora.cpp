#include "calculadora.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>

Calculadora::Calculadora(QWidget *parent)
    : QWidget(parent), currentValue(0.0)
{
    inicializarComponentes();
    applyStylesheet();
}

Calculadora::~Calculadora() {}

void Calculadora::inicializarComponentes()
{
    QGridLayout *layout = new QGridLayout(this);

    display = new QLineEdit(this);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    layout->addWidget(display, 0, 0, 1, 4);

    QStringList numButtons = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0"};
    int row = 1, col = 0;
    for (const QString &text : numButtons) {
        QPushButton *button = new QPushButton(text, this);
        connect(button, &QPushButton::clicked, this, &Calculadora::numPressed);
        layout->addWidget(button, row, col);
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }

    QStringList opButtons = {"+", "-", "*", "/", "IVA", "Descuento", "Margen"};
    col = 3;
    row = 1;
    for (const QString &text : opButtons) {
        QPushButton *button = new QPushButton(text, this);
        if (text == "IVA") connect(button, &QPushButton::clicked, this, &Calculadora::calculateIVA);
        else if (text == "Descuento") connect(button, &QPushButton::clicked, this, &Calculadora::calculateDiscount);
        else if (text == "Margen") connect(button, &QPushButton::clicked, this, &Calculadora::calculateMargin);
        else connect(button, &QPushButton::clicked, this, &Calculadora::opPressed);
        layout->addWidget(button, row, col);
        row++;
    }

    QPushButton *equalButton = new QPushButton("=", this);
    connect(equalButton, &QPushButton::clicked, this, &Calculadora::equalPressed);
    layout->addWidget(equalButton, 4, 2, 1, 2);

    QPushButton *clearButton = new QPushButton("C", this);
    connect(clearButton, &QPushButton::clicked, this, &Calculadora::clearPressed);
    layout->addWidget(clearButton, 4, 0, 1, 2);

    QPushButton *historyButton = new QPushButton("Historial", this);
    connect(historyButton, &QPushButton::clicked, this, &Calculadora::showHistory);
    layout->addWidget(historyButton, 5, 0);

    QPushButton *clearHistoryButton = new QPushButton("Limpiar Historial", this);
    connect(clearHistoryButton, &QPushButton::clicked, this, &Calculadora::clearHistory);
    layout->addWidget(clearHistoryButton, 5, 1);

    setLayout(layout);
}

void Calculadora::numPressed()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString buttonText = button->text();
        display->setText(display->text() + buttonText);
    }
}

void Calculadora::opPressed()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString buttonText = button->text();
        currentValue = display->text().toDouble();
        pendingOperation = buttonText;
        display->clear();
    }
}

void Calculadora::equalPressed()
{
    double operand = display->text().toDouble();
    double result = 0.0;

    if (pendingOperation == "+") {
        result = currentValue + operand;
    } else if (pendingOperation == "-") {
        result = currentValue - operand;
    } else if (pendingOperation == "*") {
        result = currentValue * operand;
    } else if (pendingOperation == "/") {
        if (operand != 0) {
            result = currentValue / operand;
        } else {
            display->setText("Error");
            return;
        }
    }

    display->setText(QString::number(result));
    currentValue = result;
    operationHistory.append(display->text());
    updateHistoryDisplay();
}

void Calculadora::clearPressed()
{
    display->clear();
    currentValue = 0.0;
    pendingOperation.clear();
}

void Calculadora::calculateIVA()
{
    bool ok;
    double ivaRate = QInputDialog::getDouble(this, "Calcular IVA", "Ingrese el porcentaje de IVA:", 16.0, 0, 100, 1, &ok);
    if (ok) {
        double value = display->text().toDouble();
        double iva = value * (ivaRate / 100.0);
        double result = value + iva;
        display->setText(QString::number(result));
        operationHistory.append(QString("IVA: %1 + %2%% = %3").arg(value).arg(ivaRate).arg(result));
        updateHistoryDisplay();
    }
}

void Calculadora::calculateDiscount()
{
    bool ok;
    double discountRate = QInputDialog::getDouble(this, "Calcular Descuento", "Ingrese el porcentaje de descuento:", 10.0, 0, 100, 1, &ok);
    if (ok) {
        double value = display->text().toDouble();
        double discount = value * (discountRate / 100.0);
        double result = value - discount;
        display->setText(QString::number(result));
        operationHistory.append(QString("Descuento: %1 - %2%% = %3").arg(value).arg(discountRate).arg(result));
        updateHistoryDisplay();
    }
}

void Calculadora::calculateMargin()
{
    bool ok;
    double cost = display->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Cálculo de Margen", "Ingrese un costo válido.");
        return;
    }
    double marginRate = QInputDialog::getDouble(this, "Calcular Margen", "Ingrese el porcentaje de margen:", 20.0, 0, 100, 1, &ok);
    if (ok) {
        double price = cost / (1 - (marginRate / 100.0));
        display->setText(QString::number(price));
        operationHistory.append(QString("Margen: %1 / (1 - %2%%) = %3").arg(cost).arg(marginRate).arg(price));
        updateHistoryDisplay();
    }
}

void Calculadora::showHistory()
{
    QString historyText;
    for (const QString &entry : operationHistory) {
        historyText += entry + "\n";
    }
    QMessageBox::information(this, "Historial de Operaciones", historyText);
}

void Calculadora::clearHistory()
{
    operationHistory.clear();
    updateHistoryDisplay();
}

void Calculadora::applyStylesheet()
{
    QFile styleFile(":/styles/calculadora.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        QString style = stream.readAll();
        setStyleSheet(style);
    } else {
        qDebug() << "No se pudo abrir el archivo de estilo.";
    }
}

void Calculadora::updateHistoryDisplay()
{
    // Esta función se encarga de actualizar la vista de historial en la interfaz
}
