#include "signalinputpanel.h"
#include "Utils.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

SignalInputPanel::SignalInputPanel(const Message &msg, QMap<QString, qint64> &rawValues, QWidget *parent)
    : QWidget{parent},m_msg(msg),m_rawValues(rawValues)
{
    createInputs();
}

void SignalInputPanel::createInputs()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    for(const Signal &sig : m_msg.signalsList)
    {
        QHBoxLayout *rowLayout = new QHBoxLayout();

        QLabel *nameLabel = new QLabel(sig.name);
        nameLabel->setFixedWidth(80);

        QDoubleSpinBox *physicalSpin = new QDoubleSpinBox();
        physicalSpin->setRange(sig.minVal,sig.maxVal);
        physicalSpin->setDecimals(2);           // 小数位数，可按需调整

        double step = (sig.maxVal - sig.minVal) / 100.0;
        if(step <= 0)
            step = 0.1;
        physicalSpin->setSingleStep(step);
        physicalSpin->setValue(0.0);

        QLabel *unitLabel = new QLabel(sig.unit);
        unitLabel->setFixedWidth(60);

        QLabel *rawLabel = new QLabel("raw:--");
        rawLabel->setFixedWidth(60);

        QLabel *rangeLabel = new QLabel(QString("[%1~%2]").arg(sig.minVal).arg(sig.maxVal));
        rangeLabel->setFixedWidth(100);

        rowLayout->addWidget(nameLabel);
        rowLayout->addWidget(physicalSpin);
        rowLayout->addWidget(unitLabel);
        rowLayout->addWidget(rawLabel);
        rowLayout->addWidget(rangeLabel);
        mainLayout->addLayout(rowLayout);

        double factor = sig.factor;
        double offset = sig.offset;
        QString name = sig.name;

        connect(physicalSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                [=](double physicalValue) {
                    if(Utils::isZero(factor))return;
                    qint64 rawValue = Utils::physicalToRaw(physicalValue,factor,offset);

                    if (!rawLabel) { qDebug() << "rawLabel is null!"; return; }
                    rawLabel->setText(QString("raw: %1").arg(rawValue));
                    m_rawValues[name] = rawValue;
                });

        if (!Utils::isZero(factor))
        {
            double initPhysical = physicalSpin->value();
            qint64 initRaw = Utils::physicalToRaw(initPhysical, factor, offset);
            rawLabel->setText(QString("raw: %1").arg(initRaw));
            m_rawValues[name] = initRaw;
        }
        else
        {
            rawLabel->setText("raw: factor=0");
            m_rawValues[name] = 0;
        }
    }
}
