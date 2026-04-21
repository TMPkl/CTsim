#ifndef TOMOGRAPH_WINDOW_HPP
#define TOMOGRAPH_WINDOW_HPP

#include <QMainWindow>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <opencv2/opencv.hpp>

class TomographWindow : public QMainWindow {
    Q_OBJECT

public:
    TomographWindow(QWidget *parent = nullptr);
    ~TomographWindow();

private slots:
    void onLoadImage();
    void onParametersChanged();
    void onCompute();
    void onSliderChanged(int value);
    void onIterativeMode(int state);
    void onSaveDICOM();
    void onLoadDICOM();

private:
    void createControlPanel();
    void processTomography(float maxAngleDegrees = 180.0f);
    QPixmap matoQPixmap(const cv::Mat& mat);

    // Widgety kontrolne
    QPushButton* btnLoadImage;
    QDoubleSpinBox* spinDeltaAngle;
    QSpinBox* spinNumberOfDetectors;
    QDoubleSpinBox* spinDetectorSpan;
    QSpinBox* spinOutputSize;
    QCheckBox* checkMask;
    QCheckBox* checkSharpening;
    QDoubleSpinBox* spinSharpeningCoeff;
    QCheckBox* checkMedianBlur;
    QSpinBox* spinMedianKernel;
    QCheckBox* checkRamLakFilter;
    QCheckBox* checkIterativeMode;
    QSlider* sliderAngle;
    QLabel* labelAngleValue;
    QPushButton* btnSaveDICOM;
    QPushButton* btnLoadDICOM;

    // Widgety wyświetlające
    QLabel* labelInputImage;
    QLabel* labelSinogram;
    QLabel* labelFilteredSinogram;
    QLabel* labelReconstruction;
    QLabel* labelStatus;

    // Dane
    cv::Mat inputImage;
    cv::Mat sinogram;
    cv::Mat fullSinogram;
    cv::Mat filteredSinogram;
    cv::Mat reconstruction;

    // Parametry
    float deltaAngle;
    int numberOfDetectors;
    float detectorSpan;
    int outputSize;
    bool useMask;
    bool useSharpening;
    float sharpeningCoeff;
    bool useMedianBlur;
    int medianKernel;
    bool useRamLakFilter;
    bool isIterativeMode;
    int maxAngleValue;
};

#endif // TOMOGRAPH_WINDOW_HPP
