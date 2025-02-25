#ifndef DETECCIONDNN_H
#define DETECCIONDNN_H

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;
using namespace std;

int MedirDistanciaDNN(int tiempo, String nombreArchivo);
int PingPong(int dificultad, String nombreArchivo);
int Censurar(int modo, String nombreArchivo);
int RayCast(bool mostrar, String nombreArchivo);
int Oscuridad(int nivelOscuridad, String nombreArchivo);




#endif // DETECCIONDNN_H
