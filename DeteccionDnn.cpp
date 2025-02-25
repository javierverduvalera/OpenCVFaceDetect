#include <chrono> // Para medir el tiempo
#include "DeteccionDnn.h"

#include <iostream>

static bool colisionRecCir(int x,int y, int w, int h,int cx,int cy,int r, Mat& frame){

    int px = cx;
    if ( px < x ){
        px = x;
    }
    if ( px > x + w ){
      px = x + w;
    }
    int py = cy;
    if ( py < y ){
       py = y;
    }
    if ( py > y + h ){
       py = y + h;
    }
    double distancia = sqrt( (cx - px)*(cx - px) + (cy - py)*(cy - py) );
    circle(frame, Point(px, py), 5, Scalar(0, 0, 255), 3);

    if ( distancia < r ) {
        return true;
    }
    return false;
}


static void visualizeFlecha(Mat& input, Mat& faces,bool mostrar)
{
    int thickness=8;

    for (int i = 0; i < faces.rows; i++)
    {
        // Draw landmarks
        if(mostrar){
            circle(input, Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5))), 2, Scalar(255, 0, 0), thickness);
            circle(input, Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7))), 2, Scalar(0, 0, 255), thickness);
            circle(input, Point2i(int(faces.at<float>(i, 8)), int(faces.at<float>(i, 9))), 2, Scalar(0, 255, 0), thickness);
            circle(input, Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11))), 2, Scalar(255, 0, 255), thickness);
            circle(input, Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13))), 2, Scalar(0, 255, 255), thickness);
        }

        Point V1=Point(int(faces.at<float>(i, 4)),int(faces.at<float>(i, 5)));
        Point V2=Point(int(faces.at<float>(i, 6)),int(faces.at<float>(i, 7)));
        Point V3=Point(int(faces.at<float>(i, 10)),int(faces.at<float>(i, 11)));
        Point V4=Point(int(faces.at<float>(i, 12)),int(faces.at<float>(i, 13)));
        Point V5=(V2+V1+V3+V4)-Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9)))*4;
        Point V6=Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9)))-V5;

        /*
         DIBUJAR VECTORES:

        Vectores entre la nariz y los landmarks exteriores
        arrowedLine(input, V1, Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9))), Scalar(0, 255, 0), 2, LINE_8, 0, 0.2);
        arrowedLine(input, V2, Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9))), Scalar(0, 255, 255), 2, LINE_8, 0, 0.2);
        arrowedLine(input, V3, Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9))), Scalar(255, 255, 255), 2, LINE_8, 0, 0.2);
        arrowedLine(input, V4, Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9))), Scalar(0, 0, 255), 2, LINE_8, 0, 0.2);

        Vector resultante
        arrowedLine(input, Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9))),V6 , Scalar(0, 0, 255), 2, LINE_8, 0, 0.2);

        */
        Scalar verde(0, 255, 0); // Verde en formato BGR

        line(input,  Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5))), V6, verde, 3, LINE_AA);
        line(input,  Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7))), V6, verde, 3, LINE_AA);
        line(input,  Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11))), V6, verde, 3, LINE_AA);
        line(input,  Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13))), V6, verde, 3, LINE_AA);
    }

}


static void CensurarGaus(Mat& input, Mat& faces)
{
    Mat frameFiltrado = input.clone();
    GaussianBlur(frameFiltrado, frameFiltrado, Size(155, 105), 0);
    Mat mascara = Mat::zeros(input.size(), CV_8UC1);

    for (int i = 0; i < faces.rows; i++)
    {
        //rectangle(result, Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))), Scalar(0, 255, 0), 3);
        Point Centro1=Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9)));
        //circle(result, Centro1, 5, Scalar(0, 0, 255), 3);

        // Centro de la elipse
        int ejeMayor = int(faces.at<float>(i, 3)+20);     // Longitud del eje mayor
        int ejeMenor =int(faces.at<float>(i, 2)+40);     // Longitud del eje menor
        double anguloRotacion = 90; // Rotación en grados
        Scalar color(255);
        ellipse(mascara, Centro1, Size(ejeMayor / 2, ejeMenor / 2), anguloRotacion, 0, 360,color, -1);

        Mat regionFiltrada;
        frameFiltrado.copyTo(regionFiltrada, mascara);

        // Combinar la región filtrada con el frame original
        input.setTo(Scalar(0, 0, 0), mascara); // Limpia el área en el frame original
        add(input, regionFiltrada, input);     // Combina la región filtrada en el frame original
    }

    //imshow ("mascara",mascara);
    //imshow("filtado", frameFiltrado);

    flip(input, input, 1); //invertimos la imagen para que sea como debe verse
    imshow("Censurado",input);
}

static void CensurarRectangulos(Mat& input, Mat& faces)
{
    int offsetX=40;
    int offsetY=16;

    for (int i = 0; i < faces.rows; i++)
    {
        Point P1=Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5)));
        Point P2=Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7)));
        int vy=P2.y-P1.y;
        int vx=P2.x-P1.x;
        Point P3(P1.x-0.2*vy,P1.y+0.4*vx);
        Point P4(P2.x-0.2*vy,P2.y+0.4*vx);

        /*
         DIBUJAR LANDMARKS

        int thickness =8;
        circle(input, P1, 2, Scalar(255, 0, 0), thickness);
        circle(input, P2, 2, Scalar(0, 0, 255), thickness);
        circle(input, P3, 2, Scalar(255, 255, 0), thickness);
        circle(input, P4, 2, Scalar(255, 0, 255), thickness);
        */



        Point P5=Point(P1.x-offsetX,P1.y-offsetY);
        Point P6=Point(P2.x+offsetX,P2.y-offsetY);
        Point P7=Point(P3.x-offsetX,P3.y+offsetY/2);
        Point P8=Point(P4.x+offsetX,P4.y+offsetY/2);


        Point polygon[1][4] = {
            {P5, P6, P8, P7} // Puntos en orden
        };


        const Point* ppt[1] = { polygon[0] };
        int npt[] = { 4 }; // Número de puntos del polígono

        // Dibujar el censurado de los ojos
        fillPoly(input, ppt, npt, 1, Scalar(0, 0, 0), LINE_8);

        //ahora repetimos el proceso para la censura de la boca
         P1=Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11)));
         P2=Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13)));
         vy=P2.y-P1.y;
         vx=P2.x-P1.x;
         P3=Point(P1.x-0.2*vy,P1.y+0.4*vx);
         P4=Point(P2.x-0.2*vy,P2.y+0.4*vx);

         P5=Point(P1.x-offsetX/2,P1.y-offsetY/2);
         P6=Point(P2.x+offsetX/2,P2.y-offsetY/2);
         P7=Point(P3.x-offsetX/2,P3.y+offsetY/3);
         P8=Point(P4.x+offsetX/2,P4.y+offsetY/3);

         Point polygon2[1][4] = {
             {P5, P6, P8, P7} // Puntos en orden
         };

         const Point* ppt2[1] = { polygon2[0] };
         int npt2[] = { 4 };

         fillPoly(input, ppt2, npt2, 1, Scalar(0, 0, 0), LINE_8);
    }
    flip(input, input, 1); //invertimos la imagen para que sea correctamente
    imshow("Censurado",input);

}


static void visualizeMedirDistancia(Mat& input, Mat& faces, int thickness, float K)
{

    for (int i = 0; i < faces.rows; i++)
    {
        // Draw bounding box
        rectangle(input, Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))), Scalar(0, 255, 0), thickness);
        // Draw landmarks
        float tamano=faces.at<float>(i, 2)*faces.at<float>(i, 3);
        float distancia=K/tamano;
        String texto=to_string(distancia);
        Point posicionTexto=Point(faces.at<float>(i, 0),faces.at<float>(i, 1)+faces.at<float>(i, 3)+15);
        putText(input, texto, posicionTexto, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
    }
}

static float visualizeCalibracion(Mat& input, Mat& faces, int thickness) //metodo solo disponible para una cara
{
    if(faces.rows>0)
    {
        // Draw bounding box
        rectangle(input, Rect2i(int(faces.at<float>(0, 0)), int(faces.at<float>(0, 1)), int(faces.at<float>(0, 2)), int(faces.at<float>(0, 3))), Scalar(0, 0, 255), thickness);

        String texto="Calibrando, no te muevas";
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int baseline = 0; // Se usa para calcular el espacio extra debajo del texto
        Size textSize = getTextSize(texto, fontFace, fontScale, thickness, &baseline);
        int x = (input.cols - textSize.width) / 2; // Centrado en el eje X
        int y = 30; //
        putText(input, texto, Point(x,y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

        return faces.at<float>(0, 2)*faces.at<float>(0, 3);
    }
    return 0;
}

static void visualizePrepararacion(Mat& input, Mat& faces, int thickness) //metodo solo disponible para una cara
{
    if(faces.rows>0)
    {
        // Draw bounding box
        rectangle(input, Rect2i(int(faces.at<float>(0, 0)), int(faces.at<float>(0, 1)), int(faces.at<float>(0, 2)), int(faces.at<float>(0, 3))), Scalar(255, 255, 0), thickness);

        String texto="colocate a un metro de la pantalla";

        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int baseline = 0; // Se usa para calcular el espacio extra debajo del texto
        Size textSize = getTextSize(texto, fontFace, fontScale, thickness, &baseline);
        int x = (input.cols - textSize.width) / 2; // Centrado en el eje X
        int y = 30; //
        putText(input, texto, Point(x,y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0), 2);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int MedirDistanciaDNN(int tiempo, String nombreArchivo)
{
    String fd_modelPath ="../../face_detection_yunet_2023mar.onnx";
    float scoreThreshold = 0.9;
    float nmsThreshold =0.3;
    int topK = 5000;
    float scale = 1.0;


    Ptr<FaceDetectorYN> detector = FaceDetectorYN::create(fd_modelPath, "", Size(320, 320), scoreThreshold, nmsThreshold, topK);

    int frameWidth, frameHeight;
    VideoCapture capture;
    string video ="0";
    if (nombreArchivo.empty())
        capture.open(0);
    else
        capture.open(nombreArchivo);

    if (capture.isOpened())
    {
        frameWidth = int(capture.get(CAP_PROP_FRAME_WIDTH) * scale);
        frameHeight = int(capture.get(CAP_PROP_FRAME_HEIGHT) * scale);
    }
    else
    {
        cout << "Could not initialize video capturing: " << video << "\n";
        return 1;
    }

    detector->setInputSize(Size(frameWidth, frameHeight));

    int nFrame = 0;
    auto startTime = chrono::steady_clock::now();
    float VariableObtenida;
    float VariableAcumulada=0;
    float K; //constante de calibracion

    for (;;)
    {
        // Get frame
        Mat frame;
        if (!capture.read(frame))
        {
            cerr << "Can't grab frame! Stop\n";
            break;
        }

        resize(frame, frame, Size(frameWidth, frameHeight));

        // Inference
        Mat faces;
        detector->detect(frame, faces);

        Mat result = frame.clone();
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();

        if (elapsedTime<tiempo){
            visualizePrepararacion(result,faces,2);
        }
        else if (elapsedTime<tiempo+3){
            VariableObtenida=visualizeCalibracion(result,faces,2);

            if(VariableObtenida!=0){
                ++nFrame;
                VariableAcumulada+=VariableObtenida;
            }
        }
        else{
            K=VariableAcumulada/nFrame;
            visualizeMedirDistancia(result,faces,2,K);
        }

        // Visualize results
        imshow("Live", result);

        int key = waitKey(1);


        if (key > 0)
            break;
    }

    return 0;
}

int PingPong(int dificultad, String nombreArchivo)
{
    String fd_modelPath ="../../face_detection_yunet_2023mar.onnx";

    float scoreThreshold = 0.9;
    float nmsThreshold =0.3;
    int topK = 5000;
    float scale = 1.0;

    //! [initialize_FaceDetectorYN]
    // Initialize FaceDetectorYN
    Ptr<FaceDetectorYN> detector = FaceDetectorYN::create(fd_modelPath, "", Size(320, 320), scoreThreshold, nmsThreshold, topK);
    //! [initialize_FaceDetectorYN]

    int frameWidth, frameHeight;
    VideoCapture capture;
    string video ="0";
    if (nombreArchivo.empty())
        capture.open(0);
    else
        capture.open(nombreArchivo);
    if (capture.isOpened())
    {
        frameWidth = int(capture.get(CAP_PROP_FRAME_WIDTH) * scale);
        frameHeight = int(capture.get(CAP_PROP_FRAME_HEIGHT) * scale);
    }
    else
    {
        cout << "Could not initialize video capturing: " << video << "\n";
        return 1;
    }

    detector->setInputSize(Size(frameWidth, frameHeight));
    int radio=30;
    int CposX=200;
    int CposY=50;
    int Cvx=5+dificultad;
    int Cvy=6+dificultad;
    int velocidadMax=12+(dificultad+1)*2;
    int vidas=100-dificultad*10;
    int puntos=0;

    Scalar color(0, 255, 0);
    while(vidas>0)
    {
        // Get frame
        Mat frame;
        if (!capture.read(frame))
        {
            cerr << "Can't grab frame! Stop\n";
            break;
        }
        // Inference

        circle(frame, Point(CposX, CposY), radio, Scalar(0, 255, 0), 3);
        CposX=CposX+Cvx;
        CposY=CposY+Cvy;

        if(CposX+radio>frameWidth || CposX-radio<0){
            if(Cvx>0 && Cvx<velocidadMax){Cvx++;}
            else if(Cvx<0 && Cvx>-velocidadMax){Cvx--;}
            Cvx*=-1;
        }

        if(CposY+radio>frameHeight || CposY-radio<0){
            if(Cvy>0 && Cvy<velocidadMax){Cvy++;}
            else if(Cvy<0 && Cvy>-velocidadMax){Cvy--;}
            Cvy*=-1;
        }


        Mat faces;
        detector->detect(frame, faces);
        Mat result = frame.clone();
        // Draw results on the input image
        putText(result, "Puntos: "+to_string(puntos), Point(10,55), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

        if(vidas>1){
            rectangle(result,Point(10,10),Point(10+vidas*3,40),Scalar(0,0,255),-1);
        }
        else{
            putText(result, "Has perdido", Point(75,255), FONT_HERSHEY_SIMPLEX, 2.5, Scalar(0, 0, 255), 5);

        }

        for (int i = 0; i < faces.rows; i++)
        {
            if(colisionRecCir( int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)),int(faces.at<float>(i, 3)), CposX, CposY, radio, result)){
                color=Scalar(0, 0, 255);
                vidas--;
            }
            else{
                color=Scalar(0,255,0);
            }
            rectangle(result, Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))),color, 3);
        }
        puntos+=faces.rows;


        // Visualize results
        imshow("Live", result);

        int key = waitKey(1);
        if (key > 0)
            break;
    }
    return 0;
}

int Censurar(int modo, String nombreArchivo)
{

    String fd_modelPath ="../../face_detection_yunet_2023mar.onnx";

    float scoreThreshold = 0.9;
    float nmsThreshold =0.3;
    int topK = 5000;
    float scale = 1.0;


    Ptr<FaceDetectorYN> detector = FaceDetectorYN::create(fd_modelPath, "", Size(320, 320), scoreThreshold, nmsThreshold, topK);

    int frameWidth, frameHeight;
    VideoCapture capture;
    string video ="0";
    if (nombreArchivo.empty())
        capture.open(0);
    else
        capture.open(nombreArchivo);
    if (capture.isOpened())
    {
        frameWidth = int(capture.get(CAP_PROP_FRAME_WIDTH) * scale);
        frameHeight = int(capture.get(CAP_PROP_FRAME_HEIGHT) * scale);
    }
    else
    {
        cout << "Could not initialize video capturing: " << video << "\n";
        return 1;
    }

    detector->setInputSize(Size(frameWidth, frameHeight));

    for (;;)
    {
        // Get frame
        Mat frame;
        if (!capture.read(frame))
        {
            cerr << "Can't grab frame! Stop\n";
            break;
        }
        // Inference
        Mat faces;
        detector->detect(frame, faces);

        Mat result = frame.clone();
        // Draw results on the input image

        switch (modo) {
        case 1:
            CensurarGaus(result,faces);
            break;
        case 2:
            CensurarRectangulos(result,faces);
            break;
        }


        // para ver el original
        //imshow("original",frame);


        int key = waitKey(1);
        if (key > 0)
            break;
    }
    return 0;
}

int Oscuridad(int nivelOscuridad, String nombreArchivo)
{
    String fd_modelPath ="../../face_detection_yunet_2023mar.onnx";

    float scoreThreshold = 0.9;
    float nmsThreshold =0.3;
    int topK = 5000;
    float scale = 1.0;

    Ptr<FaceDetectorYN> detector = FaceDetectorYN::create(fd_modelPath, "", Size(320, 320), scoreThreshold, nmsThreshold, topK);


    int frameWidth, frameHeight;
    VideoCapture capture;
    string video ="0";
    if (nombreArchivo.empty())
        capture.open(0);
    else
        capture.open(nombreArchivo);

    if (capture.isOpened())
    {
        frameWidth = int(capture.get(CAP_PROP_FRAME_WIDTH) * scale);
        frameHeight = int(capture.get(CAP_PROP_FRAME_HEIGHT) * scale);
    }
    else
    {
        cout << "Could not initialize video capturing: " << video << "\n";
        return 1;
    }

    detector->setInputSize(Size(frameWidth, frameHeight));

    for (;;)
    {
        Mat frame;
        if (!capture.read(frame))
        {
            cerr << "Can't grab frame! Stop\n";
            break;
        }
        Mat faces;
        detector->detect(frame, faces);

        Mat mascara = Mat::zeros(frame.size(), CV_8UC1);
        Mat imagenConElipse;
        Mat mascara3C;
        int radio;
        double anguloRotacion = 90; // Rotación en grados
        Scalar blanco(255);
        Point Centro1;
        int tamBase=201; //tamaño base utilizado por el filtro gausiano

        for (int i = 0; i < faces.rows; i++)
        {
            Point Centro=Point(int(faces.at<float>(i, 8)),int(faces.at<float>(i, 9)));

            // Centro de la elipse
            radio = int(faces.at<float>(i, 3)+20);     // Longitud del eje mayor

            int tam=radio-nivelOscuridad;
                if(tam<30){
                    //nos aseguramos que el tamaño de la elipse sea al menos 30
                    tam=30;
                }
            ellipse(mascara, Centro, Size(tam,tam), anguloRotacion, 0, 360,blanco, -1);
            frame.copyTo(imagenConElipse, mascara);
        }
            //imshow("mascara", mascara);
            GaussianBlur(mascara, mascara, Size(tamBase, tamBase), 0);
            cvtColor(mascara, mascara3C, COLOR_GRAY2BGR); //ajustamos los canales de la mascara para poder hacer la multiplicacion, pasamos de 1 a 3
            multiply(frame, mascara3C, frame, 1.0/255.0);

        // Visualize results
        flip(frame, frame, 1); //invertimos la imagen para que sea como debe verse
        imshow("Live", frame);

        int key = waitKey(1);
        if (key > 0)
            break;
    }

    return 0;
}

int RayCast(bool mostrar, String nombreArchivo)
{

    String fd_modelPath ="../../face_detection_yunet_2023mar.onnx";


    float scoreThreshold = 0.9;
    float nmsThreshold =0.3;
    int topK = 5000;

    float scale = 1.0;

    Ptr<FaceDetectorYN> detector = FaceDetectorYN::create(fd_modelPath, "", Size(320, 320), scoreThreshold, nmsThreshold, topK);

    int frameWidth, frameHeight;
    VideoCapture capture;
    string video ="0";
    if (nombreArchivo.empty())
        capture.open(0);
    else
        capture.open(nombreArchivo);
    if (capture.isOpened())
    {
        frameWidth = int(capture.get(CAP_PROP_FRAME_WIDTH) * scale);
        frameHeight = int(capture.get(CAP_PROP_FRAME_HEIGHT) * scale);
    }
    else
    {
        cout << "Could not initialize video capturing: " << video << "\n";
        return 1;
    }

    detector->setInputSize(Size(frameWidth, frameHeight));

    for (;;)
    {
        // Get frame
        Mat frame;
        if (!capture.read(frame))
        {
            cerr << "Can't grab frame! Stop\n";
            break;
        }

        resize(frame, frame, Size(frameWidth, frameHeight));

        // Inference
        Mat faces;
        detector->detect(frame, faces);

        Mat result = frame.clone();
        // Draw results on the input image
        visualizeFlecha(result, faces, mostrar);              //aqui está el visualize

        // Visualize results
        imshow("Live", result);

        int key = waitKey(1); 

        if (key > 0)
            break;
    }

    return 0;
}
