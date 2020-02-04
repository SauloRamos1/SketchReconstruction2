#include "slider_dialog.h"

#include<QDebug>
#include<QHBoxLayout>


#include "include/input_sketch2.h"

SliderDialog::SliderDialog()
{
}

int SliderDialog::updateRotAxis(const int nptsSampledSize){

    QWidget *window = new QWidget;
    window->setWindowTitle("Rotate");

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, nptsSampledSize);
    slider->setTickInterval(1);


    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    window->setLayout(layout);
    window->show();

    return slider->value();
}
