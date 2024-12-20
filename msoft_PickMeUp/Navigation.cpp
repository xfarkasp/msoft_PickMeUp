﻿#include <QMessageBox>
#include <QLabel>
#include <qlayout.h>
#include "Navigation.h"

// USE CASE 1: Odošli balík a USE CASE 2: Doruč nový balík
void Navigation::navigate()
{
    QDialog* imageDialog = new QDialog();
    imageDialog->setWindowTitle("Image Viewer");
    QLabel* imageLabel = new QLabel();
    QPixmap pixmap("C:\\Users\\lordp\\Pictures\\Screenshots\\map.png");
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap);
        imageLabel->setScaledContents(true);
    }
    else {
        imageLabel->setText("Failed to load image.");
    }
    QVBoxLayout* layout = new QVBoxLayout(imageDialog);
    layout->addWidget(imageLabel);
    imageDialog->resize(pixmap.size());
    imageDialog->exec();
}

//---------------------------------------------------------------------------