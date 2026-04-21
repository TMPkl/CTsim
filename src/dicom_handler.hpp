#ifndef DICOM_HANDLER_HPP
#define DICOM_HANDLER_HPP

#include <string>
#include <opencv2/opencv.hpp>

struct PatientInfo {
    std::string patientName;
    std::string patientID;
    std::string dateOfStudy;
    std::string comments;
};

class DicomHandler {
public:
    /**
     * Zapisz obraz jako plik DICOM z informacjami pacjenta
     * @param image Obraz do zapisania (CV_8U)
     * @param filename Ścieżka do pliku wyjściowego
     * @param patientInfo Informacje o pacjencie
     * @return true jeśli zapis się powiódł, false w przeciwnym razie
     */
    static bool saveDICOM(const cv::Mat& image, const std::string& filename, const PatientInfo& patientInfo);

    /**
     * Wczytaj obraz z pliku DICOM
     * @param filename Ścieżka do pliku DICOM
     * @param image Wczytany obraz (CV_8U)
     * @param patientInfo Informacje o pacjencie z pliku
     * @return true jeśli odczyt się powiódł, false w przeciwnym razie
     */
    static bool loadDICOM(const std::string& filename, cv::Mat& image, PatientInfo& patientInfo);
};

#endif // DICOM_HANDLER_HPP
