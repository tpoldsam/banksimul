#include "donationwindow.h"
#include "ui_donationwindow.h"


DonationWindow::DonationWindow(QWidget *parent, MainMenu *ptr, DLLRestApi *api) :
    QDialog(parent),
    ui(new Ui::DonationWindow),
    pMainMenu(ptr)
{
    ui->setupUi(this);
    this->setWindowTitle("Turtle Software Banksimul - Donate");

    pRestApiInterfaceClass = api;

    donationWindowTimer = new QTimer();
    donationWindowTimer->setInterval(10000);  // 10 s timer
    donationWindowTimer->setSingleShot(false);

    donationWarningTimer = new QTimer();
    donationWarningTimer->setInterval(5000);  // 5 s timer
    donationWarningTimer->setSingleShot(false);

    // if the 10 s timer has ran out, this window will be closed
    connect(donationWindowTimer, SIGNAL(timeout()),
            this, SLOT(on_exitButton_clicked()));

    // if the 5 s timer has ran out, the warning will be closed
    connect(donationWarningTimer, SIGNAL(timeout()),
            this, SLOT(warningTimerFinished()));
}

DonationWindow::~DonationWindow()
{
    delete ui;
    ui = nullptr;

    delete donationWindowTimer;
    donationWindowTimer = nullptr;
}


/* customer info functions */
void DonationWindow::printName(QString name)
{
    ui->nameLabel->setText(name);
}


void DonationWindow::printAccountNumber(QString accountNumber)
{
    ui->accountNumberLabel->setText(accountNumber);
}


void DonationWindow::printType(QString type)
{
    cardType = type;
    ui->typeLabel->setText(type);
}


void DonationWindow::printBalance(QString balance)
{
    stringBalance = balance;
    ui->balanceLabel->setText(balance);
}


void DonationWindow::getCardNumber(QString cardnumber)
{
    cardNumber = cardnumber;
}


/* timer functions */
void DonationWindow::startDonationWindowTimer()
{
    donationWindowTimer->start();
}

void DonationWindow::reStartDonationWindowTimer()
{
    donationWindowTimer->stop();
    donationWindowTimer->start();
}


/* functions for different donation options (10e, 20e, 50e). */
void DonationWindow::on_tenButton_clicked()
{
    reStartDonationWindowTimer();
    donationAmount="10";
    ui->amountLine->setText(donationAmount);
}


void DonationWindow::on_twentyButton_clicked()
{
    reStartDonationWindowTimer();
    donationAmount="20";
    ui->amountLine->setText(donationAmount);
}


void DonationWindow::on_fiftyButton_clicked()
{
    reStartDonationWindowTimer();
    donationAmount="50";
    ui->amountLine->setText(donationAmount);
}


/* functions for manually choosing the amount which will be donated (numbers 0-9, enter, backspace) */

void DonationWindow::donateOtherAmount(QString i)
{
    reStartDonationWindowTimer();
    donationAmount.append(i);
    ui->amountLine->setText(donationAmount);
}


void DonationWindow::on_oneButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("1");
}


void DonationWindow::on_twoButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("2");
}


void DonationWindow::on_threeButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("3");
}


void DonationWindow::on_fourButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("4");
}


void DonationWindow::on_fiveButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("5");
}


void DonationWindow::on_sixButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("6");
}


void DonationWindow::on_sevenButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("7");
}


void DonationWindow::on_eightButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("8");
}


void DonationWindow::on_nineButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("9");
}


void DonationWindow::on_zeroButton_clicked()
{
    reStartDonationWindowTimer();
    donateOtherAmount("0");
}


/* backspace button */
void DonationWindow::on_cancelButton_clicked()
{
    reStartDonationWindowTimer();
    int i = donationAmount.count();
    donationAmount.remove((i-1),1);
    ui->amountLine->setText(donationAmount);
}


/* donation functions */
void DonationWindow::on_enterButton_clicked()
{
    donationWarningTimer->stop();

    donationCents = donationAmount.toInt() * 100;
    double remainder = donationCents % 1000;
    longCentsBalance = stringBalance.toDouble() * 100;

    double creditLimit = -500000; // credit limit is 5000 €

    /* debit card */
    if (cardType == debitType){

        if (donationCents < 1000){ // user tries to donate less than 10 €
            donateMessage("bad");
            donationWarningTimer->start();
        }
        else if (donationCents > 50000){ // user tries to donate more than 500 €
            donateMessage("bad");
            donationWarningTimer->start();
        }
        else if (donationCents >= 1000 && donationCents <= 50000){ // the amount is 10 - 500 €

            if (remainder != 0){ // the amount is not divisible by ten
                donateMessage("false");
                donationWarningTimer->start();
            }

            else if (remainder == 0){ // the amount is divisible by ten

                if (longCentsBalance > donationCents){ // user has enough money (and is using a debit card)
                    pRestApiInterfaceClass->debitDonation(cardNumber,charityAccount, donationCents);
                    donationCents = 0;
                    donationAmount = "0";
                    donateMessage("good");
                    donationWarningTimer->start();
                }

                else if (longCentsBalance < donationCents){ // the balance isn't enough
                    donateMessage("poor");
                    donationWarningTimer->start();
                }
            }
        }
    }

    /* credit card */
    if (cardType == creditType){

        if (donationCents < 1000){ // user tries to donate less than 10 €
            donateMessage("bad");
            donationWarningTimer->start();
        }
        else if (donationCents > 50000){ // user tries to donate more than 500 €
            donateMessage("bad");
            donationWarningTimer->start();
        }
        else if (donationCents >= 1000 && donationCents <= 50000){ // the amount is 10 - 500 €

            if (remainder != 0){ // the amount is not divisible by ten
                donateMessage("false");
                donationWarningTimer->start();
            }

            else if (remainder == 0){ // the amount is divisible by ten

                if (longCentsBalance > creditLimit){ // user has not exeeded the credit limit (and is using a credit card)
                    pRestApiInterfaceClass->creditDonation(cardNumber,charityAccount, donationCents);
                    donationCents = 0;
                    donationAmount = "0";
                    donateMessage("good");
                    donationWarningTimer->start();
                }

                else if (longCentsBalance <= creditLimit){ // the credit limit has been exeeded
                    donateMessage("poor");
                    donationWarningTimer->start();
                }
            }

        }
    }
}


void DonationWindow::donateMessage(QString message)
{
    if (message == "bad"){
        ui->amountLine->setText("Donation must be between 10 - 500 €");
    }

    else if (message == "good"){
        ui->amountLine->setText("Successful donation!");
    }

    else if (message == "poor"){
        ui->amountLine->setText("Not enough money on account!");
    }

    else if (message == "false"){
        ui->amountLine->setText("Amount must be divisible by ten.");
    }

    else {
        ui->amountLine->setText("Unexpected error occurred!");
    }
}


void DonationWindow::warningTimerFinished()
{
    ui->amountLine->setText(donationAmount);
    donationWarningTimer->start();
}


void DonationWindow::on_exitButton_clicked()
{
    donationWindowTimer->stop();
    this -> close();
}
