#include "patient_info_dialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDate>

PatientInfoDialog::PatientInfoDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Informacje o pacjencie");
    setModal(true);
    setGeometry(100, 100, 500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Imię i nazwisko pacjenta
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Imię i nazwisko:"));
    editPatientName = new QLineEdit(this);
    nameLayout->addWidget(editPatientName);
    mainLayout->addLayout(nameLayout);

    // ID pacjenta
    QHBoxLayout *idLayout = new QHBoxLayout();
    idLayout->addWidget(new QLabel("ID pacjenta:"));
    editPatientID = new QLineEdit(this);
    idLayout->addWidget(editPatientID);
    mainLayout->addLayout(idLayout);

    // Data badania
    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Data badania:"));
    editStudyDate = new QDateEdit(this);
    editStudyDate->setDate(QDate::currentDate());
    editStudyDate->setCalendarPopup(true);
    dateLayout->addWidget(editStudyDate);
    mainLayout->addLayout(dateLayout);

    // Komentarze
    mainLayout->addWidget(new QLabel("Komentarze:"));
    editComments = new QTextEdit(this);
    editComments->setMinimumHeight(150);
    mainLayout->addWidget(editComments);

    // Przyciski
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Anuluj", this);
    
    connect(okButton, &QPushButton::clicked, this, &PatientInfoDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

PatientInfo PatientInfoDialog::getPatientInfo() const {
    return patientInfo;
}

void PatientInfoDialog::onAccept() {
    patientInfo.patientName = editPatientName->text().toStdString();
    patientInfo.patientID = editPatientID->text().toStdString();
    
    // Konwertuj datę na format YYYYMMDD
    QDate date = editStudyDate->date();
    patientInfo.dateOfStudy = date.toString("yyyyMMdd").toStdString();
    
    patientInfo.comments = editComments->toPlainText().toStdString();

    accept();
}
