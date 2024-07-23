#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

class Calculadora : public QWidget
{
    Q_OBJECT

public:
    explicit Calculadora(QWidget *parent = nullptr);
    ~Calculadora();

private slots:
    void numPressed();
    void opPressed();
    void equalPressed();
    void clearPressed();
    void calculateIVA();
    void calculateDiscount();
    void calculateMargin();
    void showHistory();
    void clearHistory();

private:
    void inicializarComponentes();
    void applyStylesheet();
    void updateHistoryDisplay();
    void fetchConversionRate(const QString &from, const QString &to);

    QLineEdit *display;
    double currentValue;
    QString pendingOperation;
    QStringList operationHistory;
};

#endif // CALCULADORA_H
