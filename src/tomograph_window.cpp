#include "tomograph_window.hpp"
#include "phantom_generator.hpp"
#include "radon.hpp"
#include "iradon.hpp"
#include "ram_lak_filter.hpp"
#include "dicom_handler.hpp"
#include "patient_info_dialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QImage>
#include <QPixmap>
#include <QScrollArea>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QThread>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <algorithm>

TomographWindow::TomographWindow(QWidget *parent)
    : QMainWindow(parent),
      deltaAngle(0.1f),
      numberOfDetectors(300),
      detectorSpan(300.0f),
      outputSize(256),
      useMask(true),
      useSharpening(true),
      sharpeningCoeff(0.02f),
      useMedianBlur(true),
      medianKernel(5),
      useRamLakFilter(true),
      isIterativeMode(false),
      maxAngleValue(180) {
    
    setWindowTitle("Tomograf - Wizualizacja");
    
    // Ustawienie rozmiaru okna responsywnie - 90% ekranu
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int windowWidth = static_cast<int>(screenGeometry.width() * 0.80);
    int windowHeight = static_cast<int>(screenGeometry.height() * 0.50);
    setGeometry((screenGeometry.width() - windowWidth) / 2, 
                (screenGeometry.height() - windowHeight) / 2,
                windowWidth, windowHeight);

    // Tworzenie centralnego widgetu
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    rootLayout->addWidget(scrollArea);

    QWidget *contentWidget = new QWidget(this);
    scrollArea->setWidget(contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Panel kontrolny na górze
    createControlPanel();
    QWidget *controlPanelWidget = new QWidget(this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanelWidget);
    
    // Layout dla przycisków ładowania/wczytywania
    QHBoxLayout *fileButtonsLayout = new QHBoxLayout();
    fileButtonsLayout->addWidget(btnLoadImage);
    fileButtonsLayout->addWidget(btnLoadDICOM);
    fileButtonsLayout->addWidget(btnSaveDICOM);
    fileButtonsLayout->addStretch();
    controlLayout->addLayout(fileButtonsLayout);
    
    QGroupBox *groupParams1 = new QGroupBox("Parametry tomografii", this);
    QHBoxLayout *paramsLayout1 = new QHBoxLayout(groupParams1);
    paramsLayout1->addWidget(new QLabel("Delta Angle (°):"));
    paramsLayout1->addWidget(spinDeltaAngle);
    paramsLayout1->addWidget(new QLabel("Liczba detektorów:"));
    paramsLayout1->addWidget(spinNumberOfDetectors);
    paramsLayout1->addWidget(new QLabel("Rozpiętość detektora:"));
    paramsLayout1->addWidget(spinDetectorSpan);
    paramsLayout1->addWidget(new QLabel("Rozmiar obrazu:"));
    paramsLayout1->addWidget(spinOutputSize);
    paramsLayout1->addStretch();
    controlLayout->addWidget(groupParams1);

    QGroupBox *groupOptions = new QGroupBox("Opcje przetwarzania", this);
    QHBoxLayout *optionsLayout = new QHBoxLayout(groupOptions);
    optionsLayout->addWidget(checkMask);
    optionsLayout->addWidget(checkRamLakFilter);
    optionsLayout->addWidget(checkSharpening);
    optionsLayout->addWidget(new QLabel("Wsp. sharpening:"));
    optionsLayout->addWidget(spinSharpeningCoeff);
    optionsLayout->addWidget(checkMedianBlur);
    optionsLayout->addWidget(new QLabel("Kernel:"));
    optionsLayout->addWidget(spinMedianKernel);
    optionsLayout->addStretch();
    controlLayout->addWidget(groupOptions);

    // Tryb iteracyjny i suwak kąta
    QGroupBox *groupIterative = new QGroupBox("Tryb iteracyjny", this);
    QVBoxLayout *iterativeLayout = new QVBoxLayout(groupIterative);
    iterativeLayout->addWidget(checkIterativeMode);
    
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(new QLabel("Kąt obrotu:"));
    sliderLayout->addWidget(sliderAngle, 1);
    sliderLayout->addWidget(labelAngleValue);
    iterativeLayout->addLayout(sliderLayout);
    controlLayout->addWidget(groupIterative);
    
    // Przycisk Compute
    QWidget *computeWidget = new QWidget(this);
    QHBoxLayout *computeLayout = new QHBoxLayout(computeWidget);
    computeLayout->addStretch();
    QPushButton* btnCompute = new QPushButton("Oblicz", this);
    btnCompute->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; font-size: 12pt;");
    btnCompute->setMinimumWidth(150);
    btnCompute->setMinimumHeight(40);
    connect(btnCompute, &QPushButton::clicked, this, &TomographWindow::onCompute);
    computeLayout->addWidget(btnCompute);
    computeLayout->addStretch();
    controlLayout->addWidget(computeWidget);
    
    mainLayout->addWidget(controlPanelWidget);

    // Label statusu
    labelStatus = new QLabel("Gotowy", this);
    labelStatus->setStyleSheet("background-color: #e8f5e9; color: #2e7d32; padding: 8px; font-weight: bold; border: 1px solid #4CAF50; border-radius: 4px;");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setMaximumWidth(200);
    mainLayout->addWidget(labelStatus);

    // Layout główny (2x2) dla wyników
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    // Obraz wejściowy
    QGroupBox *groupInput = new QGroupBox("Obraz wejściowy", this);
    QVBoxLayout *layoutInput = new QVBoxLayout(groupInput);
    labelInputImage = new QLabel(this);
    labelInputImage->setMinimumSize(200, 200);
    labelInputImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelInputImage->setAlignment(Qt::AlignCenter);
    labelInputImage->setStyleSheet("border: 1px solid black; background-color: #f0f0f0;");
    layoutInput->addWidget(labelInputImage);
    gridLayout->addWidget(groupInput, 0, 0);

    // Sinogram
    QGroupBox *groupSinogram = new QGroupBox("Sinogram", this);
    QVBoxLayout *layoutSinogram = new QVBoxLayout(groupSinogram);
    labelSinogram = new QLabel(this);
    labelSinogram->setMinimumSize(200, 200);
    labelSinogram->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelSinogram->setAlignment(Qt::AlignCenter);
    labelSinogram->setStyleSheet("border: 1px solid black; background-color: #f0f0f0;");
    layoutSinogram->addWidget(labelSinogram);
    gridLayout->addWidget(groupSinogram, 0, 1);

    // Przefiltrowany sinogram
    QGroupBox *groupFilteredSinogram = new QGroupBox("Sinogram (przefiltrowany - Ram-Lak)", this);
    QVBoxLayout *layoutFilteredSinogram = new QVBoxLayout(groupFilteredSinogram);
    labelFilteredSinogram = new QLabel(this);
    labelFilteredSinogram->setMinimumSize(200, 200);
    labelFilteredSinogram->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelFilteredSinogram->setAlignment(Qt::AlignCenter);
    labelFilteredSinogram->setStyleSheet("border: 1px solid black; background-color: #f0f0f0;");
    layoutFilteredSinogram->addWidget(labelFilteredSinogram);
    gridLayout->addWidget(groupFilteredSinogram, 1, 0);

    // Rekonstrukcja
    QGroupBox *groupReconstruction = new QGroupBox("Rekonstrukcja (Filtered Back-Projection)", this);
    QVBoxLayout *layoutReconstruction = new QVBoxLayout(groupReconstruction);
    labelReconstruction = new QLabel(this);
    labelReconstruction->setMinimumSize(200, 200);
    labelReconstruction->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelReconstruction->setAlignment(Qt::AlignCenter);
    labelReconstruction->setStyleSheet("border: 1px solid black; background-color: #f0f0f0;");
    layoutReconstruction->addWidget(labelReconstruction);
    gridLayout->addWidget(groupReconstruction, 1, 1);

    mainLayout->addLayout(gridLayout, 1);
}

TomographWindow::~TomographWindow() {
}

void TomographWindow::createControlPanel() {
    btnLoadImage = new QPushButton("Wczytaj obraz", this);
    connect(btnLoadImage, &QPushButton::clicked, this, &TomographWindow::onLoadImage);

    spinDeltaAngle = new QDoubleSpinBox(this);
    spinDeltaAngle->setMinimum(0.01);
    spinDeltaAngle->setMaximum(5.0);
    spinDeltaAngle->setValue(deltaAngle);
    spinDeltaAngle->setSingleStep(0.1);
    connect(spinDeltaAngle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &TomographWindow::onParametersChanged);

    spinNumberOfDetectors = new QSpinBox(this);
    spinNumberOfDetectors->setMinimum(50);
    spinNumberOfDetectors->setMaximum(1000);
    spinNumberOfDetectors->setValue(numberOfDetectors);
    connect(spinNumberOfDetectors, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TomographWindow::onParametersChanged);

    spinDetectorSpan = new QDoubleSpinBox(this);
    spinDetectorSpan->setMinimum(10.0);
    spinDetectorSpan->setMaximum(500.0);
    spinDetectorSpan->setValue(detectorSpan);
    spinDetectorSpan->setSingleStep(10.0);
    connect(spinDetectorSpan, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TomographWindow::onParametersChanged);

    spinOutputSize = new QSpinBox(this);
    spinOutputSize->setMinimum(64);
    spinOutputSize->setMaximum(512);
    spinOutputSize->setValue(outputSize);
    connect(spinOutputSize, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TomographWindow::onParametersChanged);

    checkMask = new QCheckBox("Maska (okrąg)", this);
    checkMask->setChecked(useMask);
    connect(checkMask, &QCheckBox::stateChanged, this, &TomographWindow::onParametersChanged);

    checkRamLakFilter = new QCheckBox("Filtr Ram-Lak", this);
    checkRamLakFilter->setChecked(useRamLakFilter);
    connect(checkRamLakFilter, &QCheckBox::stateChanged, this, &TomographWindow::onParametersChanged);

    checkSharpening = new QCheckBox("Sharpening", this);
    checkSharpening->setChecked(useSharpening);
    connect(checkSharpening, &QCheckBox::stateChanged, this, &TomographWindow::onParametersChanged);

    spinSharpeningCoeff = new QDoubleSpinBox(this);
    spinSharpeningCoeff->setMinimum(0.0);
    spinSharpeningCoeff->setMaximum(1.0);
    spinSharpeningCoeff->setValue(sharpeningCoeff);
    spinSharpeningCoeff->setSingleStep(0.01);
    spinSharpeningCoeff->setMaximumWidth(80);
    connect(spinSharpeningCoeff, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TomographWindow::onParametersChanged);

    checkMedianBlur = new QCheckBox("Median Blur", this);
    checkMedianBlur->setChecked(useMedianBlur);
    connect(checkMedianBlur, &QCheckBox::stateChanged, this, &TomographWindow::onParametersChanged);

    spinMedianKernel = new QSpinBox(this);
    spinMedianKernel->setMinimum(1);
    spinMedianKernel->setMaximum(21);
    spinMedianKernel->setValue(medianKernel);
    spinMedianKernel->setSingleStep(2);
    spinMedianKernel->setMaximumWidth(50);
    connect(spinMedianKernel, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TomographWindow::onParametersChanged);

    checkIterativeMode = new QCheckBox("Tryb iteracyjny (suwak kąta)", this);
    checkIterativeMode->setChecked(false);
    connect(checkIterativeMode, &QCheckBox::stateChanged, this, &TomographWindow::onIterativeMode);

    sliderAngle = new QSlider(Qt::Horizontal, this);
    sliderAngle->setMinimum(1);
    sliderAngle->setMaximum(180);
    sliderAngle->setValue(180);
    sliderAngle->setTickPosition(QSlider::TicksBelow);
    sliderAngle->setTickInterval(10);
    sliderAngle->setEnabled(false);
    connect(sliderAngle, &QSlider::valueChanged, this, &TomographWindow::onSliderChanged);

    labelAngleValue = new QLabel("180°", this);
    labelAngleValue->setMaximumWidth(50);

    // Przyciski DICOM
    btnSaveDICOM = new QPushButton("Zapisz DICOM", this);
    btnSaveDICOM->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold; padding: 5px;");
    connect(btnSaveDICOM, &QPushButton::clicked, this, &TomographWindow::onSaveDICOM);

    btnLoadDICOM = new QPushButton("Wczytaj DICOM", this);
    btnLoadDICOM->setStyleSheet("background-color: #FF9800; color: white; font-weight: bold; padding: 5px;");
    connect(btnLoadDICOM, &QPushButton::clicked, this, &TomographWindow::onLoadDICOM);
}

void TomographWindow::onLoadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Wczytaj obraz", "", 
        "Obrazy (*.png *.jpg *.jpeg *.bmp *.tiff);;Wszystkie pliki (*)");
    
    if (!fileName.isEmpty()) {
        labelStatus->setText(" Ładowanie obrazu...");
        labelStatus->setStyleSheet("background-color: #e3f2fd; color: #1565c0; padding: 8px; font-weight: bold; border: 1px solid #2196F3; border-radius: 4px;");
        QApplication::processEvents();

        inputImage = cv::imread(fileName.toStdString(), cv::IMREAD_GRAYSCALE);
        if (inputImage.empty()) {
            labelInputImage->setText("Błąd: Nie można wczytać obrazu!");
            labelStatus->setText(" Błąd: Nie można wczytać obrazu!");
            labelStatus->setStyleSheet("background-color: #ffebee; color: #c62828; padding: 8px; font-weight: bold; border: 1px solid #f44336; border-radius: 4px;");
            return;
        }
        inputImage.convertTo(inputImage, CV_64F, 1.0 / 255.0);
        labelStatus->setText("✓ Obraz wczytany - Kliknij 'Oblicz'");
        labelStatus->setStyleSheet("background-color: #e8f5e9; color: #2e7d32; padding: 8px; font-weight: bold; border: 1px solid #4CAF50; border-radius: 4px;");
    }
}

void TomographWindow::onParametersChanged() {
    deltaAngle = spinDeltaAngle->value();
    numberOfDetectors = spinNumberOfDetectors->value();
    detectorSpan = spinDetectorSpan->value();
    outputSize = spinOutputSize->value();
    useMask = checkMask->isChecked();
    useSharpening = checkSharpening->isChecked();
    sharpeningCoeff = spinSharpeningCoeff->value();
    useMedianBlur = checkMedianBlur->isChecked();
    medianKernel = spinMedianKernel->value();
    useRamLakFilter = checkRamLakFilter->isChecked();
}

void TomographWindow::onIterativeMode(int state) {
    isIterativeMode = (state == Qt::Checked);
    sliderAngle->setEnabled(isIterativeMode);
    if (isIterativeMode) {
        sliderAngle->setValue(180);
    }
}

void TomographWindow::onSliderChanged(int value) {
    maxAngleValue = value;
    labelAngleValue->setText(QString::number(value) + "°");
    // obliczenia będą wykonane po kliknięciu "Oblicz"
}

void TomographWindow::onCompute() {
    deltaAngle = spinDeltaAngle->value();
    numberOfDetectors = spinNumberOfDetectors->value();
    detectorSpan = spinDetectorSpan->value();
    outputSize = spinOutputSize->value();
    useMask = checkMask->isChecked();
    useSharpening = checkSharpening->isChecked();
    sharpeningCoeff = spinSharpeningCoeff->value();
    useMedianBlur = checkMedianBlur->isChecked();
    medianKernel = spinMedianKernel->value();
    useRamLakFilter = checkRamLakFilter->isChecked();
    isIterativeMode = checkIterativeMode->isChecked();

    // Wyświetl status "Obliczanie..."
    labelStatus->setText(" Obliczanie... Proszę czekać");
    labelStatus->setStyleSheet("background-color: #fff3e0; color: #e65100; padding: 8px; font-weight: bold; border: 1px solid #ff9800; border-radius: 4px;");
    QApplication::processEvents();

    if (isIterativeMode) {
        // W trybie iteracyjnym: generuj sinogram dla wybranego kąta (ze suwaka)
        processTomography(static_cast<float>(maxAngleValue));
        labelStatus->setText(" Gotowy ");
        labelStatus->setStyleSheet("background-color: #e8f5e9; color: #2e7d32; padding: 8px; font-weight: bold; border: 1px solid #4CAF50; border-radius: 4px;");
    } else {
        // Tryb normalny: generuj wszystko od razu
        processTomography(180.0f);
        labelStatus->setText(" Gotowy");
        labelStatus->setStyleSheet("background-color: #e8f5e9; color: #2e7d32; padding: 8px; font-weight: bold; border: 1px solid #4CAF50; border-radius: 4px;");
    }
}

void TomographWindow::processTomography(float maxAngleDegrees) {
    // Jeśli nie ma obrazu, wygeneruj fantom
    if (inputImage.empty()) {
        inputImage = generatePhantom(256);
    }

    if (inputImage.empty()) {
        labelInputImage->setText("Błąd: Nie można wygenerować obrazu!");
        labelStatus->setText("Błąd: Nie można wygenerować obrazu!");
        labelStatus->setStyleSheet("background-color: #ffebee; color: #c62828; padding: 8px; font-weight: bold; border: 1px solid #f44336; border-radius: 4px;");
        return;
    }

    // Konwertuj do uint8 do wyświetlenia
    cv::Mat inputDisplay;
    inputImage.convertTo(inputDisplay, CV_8U, 255.0);
    labelInputImage->setPixmap(matoQPixmap(inputDisplay));

    // Transformata Radona - licz tylko gdy pełny zakres lub pierwszy raz
    bool needsNewSinogram = maxAngleDegrees >= 179.9f || fullSinogram.empty();
    
    if (needsNewSinogram) {
        // Licz sinogram dla pełnego zakresu 180°
        fullSinogram = radonTransform(inputImage, deltaAngle, numberOfDetectors, detectorSpan);
    }
    
    // Użyj wybranego zakresu kąta z fullSinogram
    cv::Mat workingSinogram;
    if (maxAngleDegrees >= 179.9f) {
        workingSinogram = fullSinogram.clone();
    } else {
        // W trybie iteracyjnym: użyj wybranego zakresu kąta
        int maxCols = static_cast<int>(maxAngleDegrees / deltaAngle);
        maxCols = std::min(maxCols, fullSinogram.cols);
        workingSinogram = fullSinogram.colRange(0, maxCols).clone();
    }
    
    sinogram = workingSinogram;

    // Normalizuj i wyświetl surowy sinogram
    cv::Mat sinogramDisplay;
    cv::normalize(sinogram, sinogramDisplay, 0.0, 255.0, cv::NORM_MINMAX);
    sinogramDisplay.convertTo(sinogramDisplay, CV_8U);
    labelSinogram->setPixmap(matoQPixmap(sinogramDisplay));

    // Zastosuj filtr Ram-Lak jeśli włączony
    if (useRamLakFilter) {
        filteredSinogram = applyRamLakFilter(sinogram);
    } else {
        filteredSinogram = sinogram.clone();
    }

    // Wyświetl przefiltrowany sinogram
    cv::Mat filteredSinogramDisplay;
    cv::normalize(filteredSinogram, filteredSinogramDisplay, 0.0, 255.0, cv::NORM_MINMAX);
    filteredSinogramDisplay.convertTo(filteredSinogramDisplay, CV_8U);
    labelFilteredSinogram->setPixmap(matoQPixmap(filteredSinogramDisplay));

    // Odwrotna transformata Radona
    reconstruction = inverseRadonTransform(filteredSinogram, deltaAngle, outputSize);

    // Przetwarzanie rekonstrukcji
    cv::Mat reconstructedDisplay;

    if (useMask) {
        cv::Mat mask = cv::Mat::zeros(reconstruction.size(), CV_8U);
        cv::circle(mask, {reconstruction.cols / 2, reconstruction.rows / 2}, reconstruction.cols / 2, 255, -1);
        cv::normalize(reconstruction, reconstructedDisplay, 0.0, 255.0, cv::NORM_MINMAX, -1, mask);
    } else {
        cv::normalize(reconstruction, reconstructedDisplay, 0.0, 255.0, cv::NORM_MINMAX);
    }

    if (useSharpening) {
        cv::Mat reconstructedF64;
        reconstructedDisplay.convertTo(reconstructedF64, CV_64F);

        cv::Mat lap;
        cv::Laplacian(reconstructedF64, lap, CV_64F, 5);

        cv::Mat sharpened = reconstructedF64 - sharpeningCoeff * lap;
        sharpened.convertTo(reconstructedDisplay, CV_8U);
    } else {
        reconstructedDisplay.convertTo(reconstructedDisplay, CV_8U);
    }

    if (useMedianBlur) {
        cv::medianBlur(reconstructedDisplay, reconstructedDisplay, medianKernel);
    }

    labelReconstruction->setPixmap(matoQPixmap(reconstructedDisplay));
}

QPixmap TomographWindow::matoQPixmap(const cv::Mat& mat) {
    if (mat.empty()) {
        return QPixmap();
    }

    cv::Mat rgbMat;

    if (mat.channels() == 1) {
        cv::cvtColor(mat, rgbMat, cv::COLOR_GRAY2RGB);
    } else if (mat.channels() == 3) {
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
    } else {
        return QPixmap();
    }

    QImage qimg(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
    return QPixmap::fromImage(qimg);
}

void TomographWindow::onSaveDICOM() {
    if (reconstruction.empty()) {
        QMessageBox::warning(this, "Błąd", "Brak obrazu do zapisania. Wykonaj najpierw obliczenia.");
        return;
    }

    // Okno dialogu dla informacji pacjenta
    PatientInfoDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    PatientInfo patientInfo = dialog.getPatientInfo();

    // Okno dialogu do wyboru pliku
    QString filename = QFileDialog::getSaveFileName(this,
        "Zapisz obraz w formacie DICOM", "",
        "DICOM Files (*.dcm);;All Files (*)");

    if (filename.isEmpty()) {
        return;
    }

    // Przygotuj obraz do zapisania - upewnij się że jest CV_8U
    cv::Mat imageToSave;
    if (reconstruction.type() == CV_8U) {
        imageToSave = reconstruction.clone();
    } else {
        cv::normalize(reconstruction, imageToSave, 0.0, 255.0, cv::NORM_MINMAX);
        imageToSave.convertTo(imageToSave, CV_8U);
    }

    // Zapisz DICOM
    bool success = DicomHandler::saveDICOM(imageToSave, filename.toStdString(), patientInfo);

    if (success) {
        QMessageBox::information(this, "Sukces", "Obraz DICOM zapisany pomyślnie:\n" + filename);
        labelStatus->setText("DICOM zapisany: " + filename);
        labelStatus->setStyleSheet("color: green; font-weight: bold;");
    } else {
        QMessageBox::critical(this, "Błąd", "Nie udało się zapisać pliku DICOM.");
        labelStatus->setText("Błąd: Nie udało się zapisać DICOM");
        labelStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}

void TomographWindow::onLoadDICOM() {
    // Okno dialogu do wyboru pliku
    QString filename = QFileDialog::getOpenFileName(this,
        "Otwórz obraz DICOM", "",
        "DICOM Files (*.dcm);;All Files (*)");

    if (filename.isEmpty()) {
        return;
    }

    cv::Mat loadedImage;
    PatientInfo patientInfo;

    // Wczytaj DICOM
    bool success = DicomHandler::loadDICOM(filename.toStdString(), loadedImage, patientInfo);

    if (success) {
        reconstruction = loadedImage.clone();
        
        // Wyświetl wczytany obraz
        labelReconstruction->setPixmap(matoQPixmap(loadedImage));

        // Pokaż informacje pacjenta
        QString patientInfoStr = QString::fromStdString(
            "Pacjent: " + patientInfo.patientName + "\n" +
            "ID: " + patientInfo.patientID + "\n" +
            "Data badania: " + patientInfo.dateOfStudy + "\n" +
            "Komentarze: " + patientInfo.comments
        );

        QMessageBox::information(this, "Informacje pacjenta", patientInfoStr);
        labelStatus->setText("DICOM wczytany: " + filename);
        labelStatus->setStyleSheet("color: blue; font-weight: bold;");
    } else {
        QMessageBox::critical(this, "Błąd", "Nie udało się wczytać pliku DICOM.");
        labelStatus->setText("Błąd: Nie udało się wczytać DICOM");
        labelStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}
