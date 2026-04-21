#include "dicom_handler.hpp"
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

bool DicomHandler::saveDICOM(const cv::Mat& image, const std::string& filename, const PatientInfo& patientInfo) {
    // Sprawdź czy obraz jest prawidłowy
    if (image.empty() || image.type() != CV_8U) {
        std::cerr << "Błąd: Obraz musi być typu CV_8U" << std::endl;
        return false;
    }

    // Utwórz nowy dataset DICOM
    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();

    // Ustaw UID instancji SOP
    char uid[100];
    dcmGenerateUniqueIdentifier(uid);
    dataset->putAndInsertString(DCM_SOPInstanceUID, uid);

    // Ustaw klasę SOP (Secondary Capture Image Storage)
    dataset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);

    // Metainformacje
    DcmMetaInfo *metainfo = fileformat.getMetaInfo();
    metainfo->putAndInsertString(DCM_FileMetaInformationVersion, "\x00\x01");
    metainfo->putAndInsertString(DCM_MediaStorageSOPClassUID, UID_SecondaryCaptureImageStorage);
    metainfo->putAndInsertString(DCM_MediaStorageSOPInstanceUID, uid);
    metainfo->putAndInsertString(DCM_ImplementationClassUID, "1.2.276.0.7230010.3.0.3.6.0");
    metainfo->putAndInsertString(DCM_TransferSyntaxUID, UID_LittleEndianExplicitTransferSyntax);

    // Transfer syntax jest ustawiony w saveFile()

    // Informacje o pacjencie
    dataset->putAndInsertString(DCM_PatientName, patientInfo.patientName.c_str());
    dataset->putAndInsertString(DCM_PatientID, patientInfo.patientID.c_str());

    // Data i czas badania
    char dateStr[20];
    char timeStr[20];
    
    if (patientInfo.dateOfStudy.empty()) {
        // Jeśli brak daty, użyj bieżącej daty
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        strftime(dateStr, sizeof(dateStr), "%Y%m%d", timeinfo);
        strftime(timeStr, sizeof(timeStr), "%H%M%S", timeinfo);
    } else {
        // Użyj podanej daty
        snprintf(dateStr, sizeof(dateStr), "%s", patientInfo.dateOfStudy.c_str());
        snprintf(timeStr, sizeof(timeStr), "000000");
    }

    dataset->putAndInsertString(DCM_StudyDate, dateStr);
    dataset->putAndInsertString(DCM_StudyTime, timeStr);
    dataset->putAndInsertString(DCM_ContentDate, dateStr);
    dataset->putAndInsertString(DCM_ContentTime, timeStr);

    // Komentarze
    if (!patientInfo.comments.empty()) {
        dataset->putAndInsertString(DCM_ImageComments, patientInfo.comments.c_str());
    }

    // Ustaw wymiary obrazu
    dataset->putAndInsertUint16(DCM_Rows, image.rows);
    dataset->putAndInsertUint16(DCM_Columns, image.cols);
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    dataset->putAndInsertUint16(DCM_SamplesPerPixel, 1);
    dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    dataset->putAndInsertUint16(DCM_BitsStored, 8);
    dataset->putAndInsertUint16(DCM_HighBit, 7);
    dataset->putAndInsertUint16(DCM_PixelRepresentation, 0);

    // Skopiuj dane pikseli
    uint8_t* pixelData = new uint8_t[image.rows * image.cols];
    memcpy(pixelData, image.ptr<uint8_t>(0), image.rows * image.cols);

    // Wstaw dane pikseli
    dataset->putAndInsertUint8Array(DCM_PixelData, pixelData, image.rows * image.cols);
    delete[] pixelData;

    // Ustaw dodatkowe informacje
    dataset->putAndInsertString(DCM_Modality, "OT"); // Other
    dataset->putAndInsertString(DCM_SeriesNumber, "1");
    dataset->putAndInsertString(DCM_InstanceNumber, "1");
    dataset->putAndInsertString(DCM_StudyDescription, "Tomography Reconstruction");

    // Zapisz plik
    OFCondition status = fileformat.saveFile(filename.c_str(), EXS_LittleEndianExplicit);
    
    if (status.good()) {
        std::cout << "DICOM zapisany pomyślnie: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Błąd przy zapisie DICOM: " << status.text() << std::endl;
        return false;
    }
}

bool DicomHandler::loadDICOM(const std::string& filename, cv::Mat& image, PatientInfo& patientInfo) {
    // Wczytaj plik DICOM
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());

    if (!status.good()) {
        std::cerr << "Błąd przy wczytywaniu DICOM: " << status.text() << std::endl;
        return false;
    }

    DcmDataset *dataset = fileformat.getDataset();

    // Odczytaj informacje pacjenta
    OFString patientNameStr;
    OFString patientIDStr;
    OFString studyDateStr;
    OFString imageCommentsStr;

    dataset->findAndGetOFString(DCM_PatientName, patientNameStr);
    dataset->findAndGetOFString(DCM_PatientID, patientIDStr);
    dataset->findAndGetOFString(DCM_StudyDate, studyDateStr);
    dataset->findAndGetOFString(DCM_ImageComments, imageCommentsStr);

    patientInfo.patientName = patientNameStr.c_str();
    patientInfo.patientID = patientIDStr.c_str();
    patientInfo.dateOfStudy = studyDateStr.c_str();
    patientInfo.comments = imageCommentsStr.c_str();

    // Wczytaj obraz ze DICOM
    DicomImage *dicomImage = new DicomImage(filename.c_str());

    if (dicomImage->getStatus() != EIS_Normal) {
        std::cerr << "Błąd: Nie można odczytać obrazu z DICOM" << std::endl;
        delete dicomImage;
        return false;
    }

    // Konwertuj do 8-bitowego greyscale
    if (dicomImage->isMonochrome()) {
        unsigned long frameSize = dicomImage->getWidth() * dicomImage->getHeight();
        uint8_t *pixelData = new uint8_t[frameSize];

        // Wczytaj piksele
        if (dicomImage->getOutputData(pixelData, frameSize, 8, 0) != 0) {
            // Stwórz Mat z danymi
            image = cv::Mat(dicomImage->getHeight(), dicomImage->getWidth(), CV_8U, pixelData);
            image = image.clone(); // Skopiuj dane
            delete[] pixelData;
            delete dicomImage;
            
            std::cout << "DICOM wczytany pomyślnie: " << filename << std::endl;
            return true;
        }
        delete[] pixelData;
    }

    delete dicomImage;
    return false;
}
