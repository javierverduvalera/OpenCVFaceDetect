#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DeteccionRectangular.cpp"
#include "DeteccionDnn.h"
//#include <iostream>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Conectar la señal del QComboBox al slot personalizado
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onComboBoxIndexChanged);

    ui->OMedir->setVisible(true); //
    ui->OCensura->setVisible(false); // Oculta el frame
    ui->OPong->setVisible(false); // Oculta el frame
    ui->OOscu->setVisible(false);
    ui->ODir->setVisible(false);

}







MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onComboBoxIndexChanged(int index)
{
    // Obtener el texto actual del comboBox
    QString selectedText = ui->comboBox->itemText(index);

    // Mostrar u ocultar el frame según el texto seleccionado
    if (selectedText == "Censurador de caras") {
        ui->OCensura->setVisible(true);  // Muestra el frame
        ui->OMedir->setVisible(false); // Oculta el frame
        ui->OPong->setVisible(false); // Oculta el frame
        ui->OOscu->setVisible(false);
        ui->ODir->setVisible(false);

    }
    else if (selectedText == "Medir Distancia") {
        ui->OMedir->setVisible(true); // Muestra el frame
        ui->OCensura->setVisible(false); // Oculta el frame
        ui->OPong->setVisible(false); // Oculta el frame
        ui->OOscu->setVisible(false);
        ui->ODir->setVisible(false);


    }
    else if (selectedText == "Death Pong") {
        ui->OPong->setVisible(true); // Muestra el frame
        ui->OMedir->setVisible(false); // Oculta el frame
        ui->OCensura->setVisible(false); // Oculta el frame
        ui->OOscu->setVisible(false);
        ui->ODir->setVisible(false);

    }
    else if(selectedText=="Modo Oscuridad"){
        ui->OOscu->setVisible(true);
        ui->OPong->setVisible(false); // Muestra el frame
        ui->OMedir->setVisible(false); // Oculta el frame
        ui->OCensura->setVisible(false); // Oculta el frame
        ui->ODir->setVisible(false);

    }

    else if(selectedText=="Vector Direccion"){
        ui->ODir->setVisible(true);
        ui->OOscu->setVisible(false);
        ui->OPong->setVisible(false); // Muestra el frame
        ui->OMedir->setVisible(false); // Oculta el frame
        ui->OCensura->setVisible(false); // Oculta el frame

    }

    else{
        ui->OMedir->setVisible(false); // Oculta el frame
        ui->OCensura->setVisible(false); // Oculta el frame
        ui->OPong->setVisible(false); // Oculta el frame
        ui->OOscu->setVisible(false);
        ui->ODir->setVisible(false);


    }
}

void MainWindow::on_pushButton_clicked()
{
    QString opcion = ui->comboBox->currentText();
    QString nombreArchivo="";


    if(ui->radioButton_2->isChecked()){
        nombreArchivo= QFileDialog::getOpenFileName();
    }

    if(opcion=="Censurador de caras"){

        if (ui->radioButton_3->isChecked()) {
            TipoCensura = 1;
        } else if (ui->radioButton_4->isChecked()) {
            TipoCensura = 2;
        }
        Censurar(TipoCensura, nombreArchivo.toLatin1().data());
    }

    else if(opcion=="Medir Distancia"){
        int tiempo= ui->tiempoCal->value();
        MedirDistanciaDNN(tiempo, nombreArchivo.toLatin1().data());
    }
    else if(opcion=="Death Pong"){
        int dificultad =ui->comboBox_2->currentIndex();
        PingPong(dificultad, nombreArchivo.toLatin1().data());
    }
    else if(opcion=="Modo Oscuridad"){
        int nivelOscuridad = ui->horizontalSlider->value();
        Oscuridad(nivelOscuridad,nombreArchivo.toLatin1().data());
    }
    else if(opcion=="Vector Direccion"){
        RayCast(ui->checkBox->isChecked(),nombreArchivo.toLatin1().data());
    }
}

