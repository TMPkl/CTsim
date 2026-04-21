### Raport symulacja tomografu komputerowego
## 1. Skład grupy
Karol Leszyński 160269
## 2. Model tomografu
Zastosowany model tomografu: równoległy
## 3. Zastosowany język programowania oraz dodatkowe biblioteki

### Język programowania
- **C++** - standard używany w projekcie (wymagane dla współczesnych funkcji C++)

### Biblioteki

1. **Qt5** (Core, Gui, Widgets) - framework do budowy GUI 
   - Qt5::Core 
   - Qt5::Gui 
   - Qt5::Widgets 

2. **OpenCV** - biblioteka do przetwarzania obrazów i wizji komputerowej

3. **DCMTK** (DICOM Toolkit) - biblioteka do obsługi formatu DICOM

### Narzędzia budowania
- **CMake** - system zarządzania buildem projektu
- **Qt MOC** (Meta-Object Compiler) - automatyczne generowanie kodu dla Qt signals/slots

## 4. Opis głównych funkcji programu

### 4.1 Pozyskiwanie odczytów dla poszczególnych detektorów

Funkcja `radonTransform()` implementuje transformatę Radona dlamodelu tomografu równoległego. Dla każdego kąta rotacji i każdego detektora obliczana jest suma (lub średnia) wartości pikseli wzdłuż linii przez obraz, przy użyciu algorytmu Bresenham'a.

**Fragment kodu z `radon.cpp`:**
```cpp
for (int a = 0; a < num_angles; ++a) {
    float theta = a * delta_angle * CV_PI / 180.0f;
    
    float cos_t = std::cos(theta);
    float sin_t = std::sin(theta);
    
    for (int i = 0; i < number_of_emitters; ++i) {
        float emitter_step = detector_span / static_cast<float>(std::max(1, number_of_emitters - 1));
        float s = -detector_span / 2.0f + i * emitter_step;
        
        int x0 = centerX + static_cast<int>(std::lround(s * cos_t));
        int y0 = centerY + static_cast<int>(std::lround(s * sin_t));
        
        int x1 = x0 + static_cast<int>(std::lround(half_ray * (-sin_t)));
        int y1 = y0 + static_cast<int>(std::lround(half_ray * ( cos_t)));
        
        int x2 = x0 - static_cast<int>(std::lround(half_ray * (-sin_t)));
        int y2 = y0 - static_cast<int>(std::lround(half_ray * ( cos_t)));
        
        float mean = bresenhamMeanOfPixels(img, x1, y1, x2, y2);
        
        sinogram.at<float>(i, a) = mean;
    }
}
```

Pozycja kolejnych detektorów obliczana jest jako przesunięcie `s` w kierunku prostopadłym do promienia rentgena. Dla każdego detektora obliczana jest średnia wartość pikseli wzdłuż linii.

### 4.2 Filtrowanie sinogramu – Filtr Ram-Lak

Filtr Ram-Lak (ramp filter) zastosowany w dziedzinie częstotliwości za pomocą szybkiej transformaty Fouriera (FFT). Jest to filtr liniowy do tłumienia artefaktów w rekonstrukcji tomograficznej. **Rozmiar maski filtru: `n = cv::getOptimalDFTSize(num_emitters)`** - optymalny rozmiar dla szybkiej FFT.

**Fragment kodu z `ram_lak_filter.cpp`:**
```cpp
cv::Mat applyRamLakFilter(const cv::Mat& sinogram) {
    CV_Assert(sinogram.type() == CV_32F);
    
    int num_emitters = sinogram.rows;
    int num_angles = sinogram.cols;
    int n = cv::getOptimalDFTSize(num_emitters);  // Optymalny rozmiar maski
    
    cv::Mat filtered = cv::Mat::zeros(num_emitters, num_angles, CV_32F);
    
    for (int a = 0; a < num_angles; ++a) {
        cv::Mat padded = cv::Mat::zeros(n, 1, CV_32F);
        sinogram.col(a).copyTo(padded.rowRange(0, num_emitters));
        
        // Transformata Fouriera
        cv::Mat planes[] = {padded, cv::Mat::zeros(n, 1, CV_32F)};
        cv::Mat spectrum;
        cv::merge(planes, 2, spectrum);
        cv::dft(spectrum, spectrum);
        
        // Filtr Ram-Lak – mnożenie przez wartość liniowo rosnącą z częstotliwością
        for (int k = 0; k < n; ++k) {
            float freq = (k <= n / 2) ? static_cast<float>(k) : static_cast<float>(n - k);
            float ramp = freq / static_cast<float>(std::max(1, n / 2));
            
            cv::Vec2f& bin = spectrum.at<cv::Vec2f>(k, 0);
            bin[0] *= ramp;
            bin[1] *= ramp;
        }
        
        // Odwrotna transformata Fouriera
        cv::Mat inv;
        cv::idft(spectrum, inv, cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
        inv.rowRange(0, num_emitters).copyTo(filtered.col(a));
    }
    
    return filtered;
}
```

### 4.3 Rekonstrukcja obrazu – Odwrotna transformata Radona

Funkcja `inverseRadonTransform()` rekonstruuje obraz z sinogramu poprzez wzbogacenie (backprojection) wartości sinogramu na liniach w przestrzeni obrazu. **Normalizacja**: każdy piksel dzielony jest przez liczbę linii, które przeszły przez dany punkt, zapewniając jednakową jasność (normalizacja przez coverage).

**Fragment kodu z `iradon.cpp`:**
```cpp
cv::Mat inverseRadonTransform(const cv::Mat& sinogram, float delta_angle, int output_size) {
    cv::Mat reconstructed = cv::Mat::zeros(output_size, output_size, CV_32F);
    cv::Mat coverage = cv::Mat::zeros(output_size, output_size, CV_32F);
    
    int num_angles = sinogram.cols;
    int num_emitters = sinogram.rows;
    
    // Backprojection 
    for (int a = 0; a < num_angles; ++a) {
        float theta = a * delta_angle * CV_PI / 180.0f;
        
        for (int i = 0; i < num_emitters; ++i) {
            std::vector<cv::Point> line_points = bresenhamIterator(start, end);
            for (const auto& pt : line_points) {
                if (pt.x >= 0 && pt.x < output_size && pt.y >= 0 && pt.y < output_size) {
                    reconstructed.at<float>(pt.y, pt.x) += sinogram.at<float>(i, a);
                    coverage.at<float>(pt.y, pt.x) += 1.0f;
                }
            }
        }
    }
    
    // Normalizacja 
    for (int y = 0; y < output_size; ++y) {
        for (int x = 0; x < output_size; ++x) {
            float hits = coverage.at<float>(y, x);
            if (hits > 0.0f) {
                reconstructed.at<float>(y, x) /= hits;
            }
        }
    }
    
    return reconstructed;
}
```

### 4.4 Odczyt i zapis plików DICOM

Implementacja obsługi formatu DICOM przy użyciu biblioteki DCMTK. Program umożliwia zarówno zapis rekonstruowanego obrazu w formacie DICOM z informacjami pacjenta, jak i wczytywanie obrazów DICOM do dalszej przetwarzania.

**Zapis DICOM – najważniejsze fragmenty z `dicom_handler.cpp`:**
```cpp
bool DicomHandler::saveDICOM(const cv::Mat& image, const std::string& filename, const PatientInfo& patientInfo) {
    // Sprawdzenie poprawności obrazu
    if (image.empty() || image.type() != CV_8U) {
        std::cerr << "Błąd: Obraz musi być typu CV_8U" << std::endl;
        return false;
    }
    
    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();
    
    // Ustawienie UID i klasy SOP
    char uid[100];
    dcmGenerateUniqueIdentifier(uid);
    dataset->putAndInsertString(DCM_SOPInstanceUID, uid);
    dataset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
    
    // Informacje pacjenta
    dataset->putAndInsertString(DCM_PatientName, patientInfo.patientName.c_str());
    dataset->putAndInsertString(DCM_PatientID, patientInfo.patientID.c_str());
    dataset->putAndInsertString(DCM_StudyDate, dateStr);
    dataset->putAndInsertString(DCM_ImageComments, patientInfo.comments.c_str());
    
    // Wymiary i typ pikseli
    dataset->putAndInsertUint16(DCM_Rows, image.rows);
    dataset->putAndInsertUint16(DCM_Columns, image.cols);
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    dataset->putAndInsertUint16(DCM_SamplesPerPixel, 1);
    dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    dataset->putAndInsertUint16(DCM_BitsStored, 8);
    
    // Zapis danych pikseli
    uint8_t* pixelData = new uint8_t[image.rows * image.cols];
    memcpy(pixelData, image.ptr<uint8_t>(0), image.rows * image.cols);
    dataset->putAndInsertUint8Array(DCM_PixelData, pixelData, image.rows * image.cols);
    delete[] pixelData;
    
    // Zapis pliku
    OFCondition status = fileformat.saveFile(filename.c_str(), EXS_LittleEndianExplicit);
    return status.good();
}
```

**Odczyt DICOM:**
```cpp
bool DicomHandler::loadDICOM(const std::string& filename, cv::Mat& image, PatientInfo& patientInfo) {
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    
    if (!status.good()) {
        std::cerr << "Błąd przy wczytywaniu DICOM: " << status.text() << std::endl;
        return false;
    }
    
    DcmDataset *dataset = fileformat.getDataset();
    
    // Pobranie informacji pacjenta
    OFString patientNameStr, patientIDStr, studyDateStr;
    dataset->findAndGetOFString(DCM_PatientName, patientNameStr);
    dataset->findAndGetOFString(DCM_PatientID, patientIDStr);
    dataset->findAndGetOFString(DCM_StudyDate, studyDateStr);
    
    patientInfo.patientName = patientNameStr.c_str();
    patientInfo.patientID = patientIDStr.c_str();
    patientInfo.dateOfStudy = studyDateStr.c_str();
    
    // Wczytanie obrazu
    DicomImage *dicomImage = new DicomImage(filename.c_str());
    unsigned long frameSize = dicomImage->getWidth() * dicomImage->getHeight();
    uint8_t *pixelData = new uint8_t[frameSize];
    dicomImage->getOutputData(pixelData, frameSize, 8, 0);
    
    image = cv::Mat(dicomImage->getHeight(), dicomImage->getWidth(), CV_8U, pixelData);
    image = image.clone();
    
    delete[] pixelData;
    delete dicomImage;
    return true;
}
```
![Opis obrazu](images/nazwa_pliku.png)
