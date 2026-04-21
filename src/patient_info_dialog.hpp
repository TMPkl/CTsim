#ifndef PATIENT_INFO_DIALOG_HPP
#define PATIENT_INFO_DIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include "dicom_handler.hpp"

class PatientInfoDialog : public QDialog {
    Q_OBJECT

public:
    PatientInfoDialog(QWidget *parent = nullptr);
    PatientInfo getPatientInfo() const;

private slots:
    void onAccept();

private:
    QLineEdit *editPatientName;
    QLineEdit *editPatientID;
    QDateEdit *editStudyDate;
    QTextEdit *editComments;

    PatientInfo patientInfo;
};

#endif // PATIENT_INFO_DIALOG_HPP
