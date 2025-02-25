
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include "QString"

#include <stdio.h>

using namespace cv;
using namespace std;


class CascadeDetectorAdapter: public DetectionBasedTracker::IDetector
{
public:
    CascadeDetectorAdapter(Ptr<CascadeClassifier> detector):
        IDetector(),
        Detector(detector)
    {
        CV_Assert(detector);
    }

    void detect(const Mat &Image, vector<Rect> &objects) CV_OVERRIDE
    {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() CV_OVERRIDE
    {}

private:
    CascadeDetectorAdapter(); //evita que el constructor vacio sea invocado
    Ptr<CascadeClassifier> Detector;
};

DetectionBasedTracker InicializarDetector(){

    string cascadeFrontalfilename = "C:/Users/Usuario/Documents/Miniproyecto/lbpcascade_frontalface.xml";
    Ptr<CascadeClassifier> cascade = makePtr<CascadeClassifier>(cascadeFrontalfilename);
    Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(cascade);
    if ( cascade->empty() )
    {
        printf("Error: Cannot load %s\n", cascadeFrontalfilename.c_str());
    }

    cascade = makePtr<CascadeClassifier>(cascadeFrontalfilename);
    Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(cascade);
    if ( cascade->empty() )
    {
        printf("Error: Cannot load %s\n", cascadeFrontalfilename.c_str());
    }

    DetectionBasedTracker::Parameters params;
    return DetectionBasedTracker(MainDetector, TrackingDetector, params);

}


int CapturarCara(String NombreVentana,String archivo=""){

    VideoCapture VideoStream;

    if (archivo.empty()) {
        VideoStream.open(0);  // 0 es para la cámara
    } else {
        VideoStream.open(archivo);  // Abre el archivo de video
    }
    if (!VideoStream.isOpened())
    {
        printf("Error: Cannot open video stream from camera\n");
        return 1;
    }

    DetectionBasedTracker Detector= InicializarDetector();

    Mat ReferenceFrame;
    Mat GrayFrame;
    vector<Rect> Faces;

    do
    {
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_BGR2GRAY);
        Detector.process(GrayFrame);
        Detector.getObjects(Faces);

        for (size_t i = 0; i < Faces.size(); i++)
        {
            if(Faces.size()>1){
                rectangle(ReferenceFrame, Faces[i], Scalar(0,255,0));
            }
            else{
                rectangle(ReferenceFrame, Faces[i], Scalar(0,0,255));
            }

        }

        imshow(NombreVentana, ReferenceFrame);
    } while (waitKey(30) < 0);

    Detector.stop();
    return 0;
}


int CalcularDistancia(String NombreVentana,String archivo=""){

    VideoCapture VideoStream;

    if (archivo.empty()) {
        VideoStream.open(0);  // 0 es para la cámara
    } else {
        VideoStream.open(archivo);  // Abre el archivo de video
    }

    if (!VideoStream.isOpened())
    {
        printf("Error: Cannot open video stream from camera\n");
        return 1;
    }

    DetectionBasedTracker Detector= InicializarDetector();

    if (!Detector.run())
    {
        printf("Error: Detector initialization failed\n");
        return 2;
    }

    Mat ReferenceFrame;
    Mat GrayFrame;
    vector<Rect> Faces;

    String text = "Mi Rectangulo";
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 2;
    do
    {
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_BGR2GRAY);
        Detector.process(GrayFrame);
        Detector.getObjects(Faces);

        for (size_t i = 0; i < Faces.size(); i++)
        {
            Point textOrg(Faces[i].x, Faces[i].y + Faces[i].height + 20);  // 20 es la separación entre el rectángulo y el texto
            Scalar textColor(255, 0, 0); // Color del texto (blanco)

            if(Faces[i].area()>25000){
                rectangle(ReferenceFrame, Faces[i], Scalar(255,255,0));
                text = "Cerca";
            }
            else if(Faces[i].area()>12000){
                rectangle(ReferenceFrame, Faces[i], Scalar(0,255,0));
                text = "Medio";

            }
            else{
                rectangle(ReferenceFrame, Faces[i], Scalar(0,0,255));
                text = "Lejos";
            }

            text=QString::number(Faces[0].height).toStdString()+","+QString::number(Faces[0].width).toStdString();
            putText(ReferenceFrame, text, textOrg, fontFace, fontScale, textColor, thickness, LINE_AA);
        }

        imshow(NombreVentana, ReferenceFrame);
    } while (waitKey(30) < 0);

    Detector.stop();
    return 0;
}


int CensurarCaras(String NombreVentana,String archivo=""){
    VideoCapture VideoStream;

    if (archivo.empty()) {
        VideoStream.open(0);  // 0 es para la cámara
    } else {
        VideoStream.open(archivo);  // Abre el archivo de video
    }


    if (!VideoStream.isOpened())
    {
        printf("Error: Cannot open video stream from camera\n");
        return 1;
    }

    DetectionBasedTracker Detector= InicializarDetector();

    if (!Detector.run())
    {
        printf("Error: Detector initialization failed\n");
        return 2;
    }

    Mat ReferenceFrame;
    Mat GrayFrame;
    vector<Rect> Faces;

    do
    {
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_BGR2GRAY);
        Detector.process(GrayFrame);
        Detector.getObjects(Faces);

        // Si hay al menos dos caras detectadas

            // Extraer las dos primeras caras
        Mat CaraSinDistorsion[10];
        Mat CaraDistorsionada[10];

        for (size_t i = 0; i < Faces.size(); i++)
        {
            CaraSinDistorsion[i] = ReferenceFrame(Faces[i]).clone();  // Primer cara

            GaussianBlur(CaraSinDistorsion[i], CaraDistorsionada[i], Size(1, 61), 300);
            // Intercambiar las caras en la imagen
            CaraDistorsionada[i].copyTo(ReferenceFrame(Faces[i]));  // Copiar la primera cara a la segunda
        }
        imshow(NombreVentana, ReferenceFrame);

    } while (waitKey(30) < 0);

    Detector.stop();
    return 0;
}

int ResaltarCaras(String NombreVentana,String archivo=""){
    VideoCapture VideoStream;

    if (archivo.empty()) {
        VideoStream.open(0);  // 0 es para la cámara
    } else {
        VideoStream.open(archivo);  // Abre el archivo de video
    }


    if (!VideoStream.isOpened())
    {
        printf("Error: Cannot open video stream from camera\n");
        return 1;
    }

    DetectionBasedTracker Detector= InicializarDetector();

    if (!Detector.run())
    {
        printf("Error: Detector initialization failed\n");
        return 2;
    }

    Mat ReferenceFrame;
    Mat FrameDistorsionado;
    Mat GrayFrame;
    vector<Rect> Faces;

    do
    {
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_BGR2GRAY);
        Detector.process(GrayFrame);
        Detector.getObjects(Faces);

        // Si hay al menos dos caras detectadas

        // Extraer las dos primeras caras
        Mat CaraSinDistorsion[10];
        Mat CaraDistorsionada[10];
        GaussianBlur(ReferenceFrame, FrameDistorsionado, Size(1, 61), 300);

        for (size_t i = 0; i < Faces.size(); i++)
        {

            CaraSinDistorsion[i] = ReferenceFrame(Faces[i]).clone();  // Clonar para evitar problemas con ROI

            // Inicializar CaraDistorsionada con el tamaño de la cara y el mismo número de canales
            // Copiar cara distorsionada desde el FrameDistorsionado

            // Dibujar un rectángulo en la imagen distorsionada
            rectangle(FrameDistorsionado, Faces[i], Scalar(255, 255, 0));

            // Copiar la cara distorsionada de vuelta al área original
            CaraSinDistorsion[i].copyTo(FrameDistorsionado(Faces[i]));
        }


        imshow(NombreVentana, FrameDistorsionado);

    } while (waitKey(30) < 0);

    Detector.stop();
    return 0;
}



