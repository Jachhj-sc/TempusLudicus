/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "fetchtimestamp.h"

#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <iostream>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_timeStamp(new QTextEdit(this)),
    m_time(new QTextEdit(this)),
    m_debug(new QTextEdit(this)),
    m_console(new Console),
    m_settings(new SettingsDialog),
    //! [1]
    m_serial(new QSerialPort(this))
//! [1]
{
    //! [0]
    m_ui->setupUi(this);
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);
    m_ui->centralWidget->layout()->addWidget(m_time);
    m_ui->centralWidget->layout()->addWidget(m_timeStamp);
    m_ui->centralWidget->layout()->addWidget(m_debug);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    //! [2]
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    //! [2]
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
    //! [3]
    //!     // Set up the initial text
    setText("Initial Value");

    // Create a QTimer to update the value at regular intervals
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateValue);

    // Set the update interval (in milliseconds)
    timer->start(1000); // Update every 1000 milliseconds (1 second)
}
//! [3]

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::buttonClicked(void)
{
    // Check if the serial port is open
    if (!m_serial->isOpen()) {
        QMessageBox::critical(this, tr("Error"), tr("Serial port is not open!"));
        return;
    }

    // Create an instance of FetchTimestamp
    FetchTimestamp fetcher;

    // Call the function to fetch the Unix timestamp
    qDebug() << "ButtonClicked, Fetching info through HTTP..";
    long long timestamp = fetcher.fetchUnixTimestamp();

    if (timestamp != -1) {
        // Print the Unix timestamp
        std::cout << "Unix Timestamp: " << timestamp << std::endl;

        // You can convert the timestamp to a human-readable format if needed
        std::time_t t = static_cast<std::time_t>(timestamp);
        qDebug() << "Unixtime is" << timestamp << "Sending time through UART!";

        // Send serial data
        m_serial->write("U");
        m_serial->write(QString::number(timestamp).toLocal8Bit());
        m_serial->write("/S");

        // Update m_time QTextEdit with the timestamp
        m_timeStamp->setPlainText("Sent Unix Timestamp: " + QString::number(timestamp) +
                                  "\nHuman-readable Time: " + QDateTime::fromSecsSinceEpoch(t).toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        std::cerr << "Failed to fetch Unix Timestamp." << std::endl;

        // If fetching fails, you may want to display an error message in m_time
        m_timeStamp->setPlainText("Error: Failed to fetch Unix Timestamp");
    }
}




//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("UART Terminal"),
                       tr("The <b>UART Terminal</b> fetches and sends UNIX timestamp \n"
                          "HAN ESE 2023 "));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

//! [6]

//! [7]
void MainWindow::readData()
{
    static QByteArray receivedData; // Static variable to accumulate data between calls

    const QByteArray newData = m_serial->readAll();
    m_console->putData(newData);
    qDebug() << "Received" << newData << "through UART!";

    receivedData += newData; // Accumulate received data

    // Check for the end-of-message character 'S'
    if (receivedData.contains('S')) {
        // Process the complete message
        processReceivedData(receivedData);

        // Clear the accumulated data for the next message
        receivedData.clear();
    }
}
void MainWindow::processReceivedData(const QByteArray &data)
{
    static QByteArray partialData; // Static variable to accumulate partial data between calls

    // Append the new data to the accumulated partial data
    partialData += data;

    // Find complete patterns in the accumulated data
    while (partialData.contains('S')) {
        int startIndexU = partialData.indexOf('U');
        int startIndexD = partialData.indexOf('D');
        int startIndexM = partialData.indexOf('M');
        int startIndexT = partialData.indexOf('T');

        // Find the minimum valid startIndex among 'U', 'D', 'M', and 'T'
        int startIndex = -1;
        if (startIndexU != -1) {
            startIndex = (startIndex == -1) ? startIndexU : qMin(startIndex, startIndexU);
        }
        if (startIndexD != -1) {
            startIndex = (startIndex == -1) ? startIndexD : qMin(startIndex, startIndexD);
        }
        if (startIndexM != -1) {
            startIndex = (startIndex == -1) ? startIndexM : qMin(startIndex, startIndexM);
        }
        if (startIndexT != -1) {
            startIndex = (startIndex == -1) ? startIndexT : qMin(startIndex, startIndexT);
        }

        if (startIndex != -1) {
            // Check if partialData contains a complete pattern
            QByteArray patternData = partialData.mid(startIndex, partialData.indexOf('S', startIndex) + 1);
            processPattern(patternData);
            // Remove the processed pattern from partialData
            partialData.remove(0, partialData.indexOf('S', startIndex) + 1);
        } else {
            // No valid identifier found, remove everything up to the first 'S'
            partialData.remove(0, partialData.indexOf('S') + 1);
        }
    }
}

void MainWindow::processPattern(const QByteArray &patternData)
{
    char pattern = patternData.at(0);

    // Process the data based on the specified patterns
    if (pattern == 'U')
    {
        QByteArray data = patternData.mid(1, patternData.length() - 2);
        m_debug->setPlainText("Recieved Unix Timestamp: " + QString::number(data.toLong()));
    }
    else if (pattern == 'D')
    {
        QByteArray data = patternData.mid(1, patternData.length() - 2);
        m_debug->setPlainText(m_debug->toPlainText() + "\nRecieved Distance state: " + QString::number(data.toInt()));
    }
    else if (pattern == 'M')
    {
        QByteArray data = patternData.mid(1, patternData.length() - 2);
        m_debug->setPlainText(m_debug->toPlainText() + "\nRecieved Mood setting: " + QString::number(data.toInt()));
    }
    else if (pattern == 'T')
    {
        QByteArray data = patternData.mid(1, patternData.length() - 2);
        m_debug->setPlainText(m_debug->toPlainText() + "\nRecieved Temperature state: " + QString::number(data.toDouble()));
    }
    else
    {
        // Print debug information for unrecognized patterns
        m_debug->setPlainText(m_debug->toPlainText() + "\nRecieved Unknown pattern: " + QString(pattern) + " Data: " + patternData);
    }
}



//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}


void MainWindow::updateValue() {
    // Update the displayed value (you would replace this with your variable)
    // For example, if your variable is the current time:
    QTime currentTime = QTime::currentTime();
    QString timeText = currentTime.toString("hh:mm:ss");

    // Set the text in the m_time label
    m_time->setText("Current Time: " + timeText);
}

void MainWindow::setText(const QString &text) {
    m_time->setText(text);
}
