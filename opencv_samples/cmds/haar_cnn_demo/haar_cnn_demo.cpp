#include "opencv2/objdetect.hpp"
#ifndef __EMBOX__
# include "opencv2/highgui.hpp"
#else
# include "opencv2/imgcodecs.hpp"
# include "opencv2/videoio.hpp"
#endif
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <unistd.h> 

#ifdef __EMBOX__
#include <cv_embox_imshowfb.hpp>
#include "face_cascade.inc"
#include "eye_cascade.inc"
#include "cnn_inference.h"
#endif

using namespace std;
using namespace cv;


#ifdef __EMBOX__
static bool loadCascadeFromMemory(
    CascadeClassifier& cc,
    const unsigned char* data,
    unsigned int size)
{
    string xml(reinterpret_cast<const char*>(data), size);
    FileStorage fs(xml, FileStorage::READ | FileStorage::MEMORY);
    if (!fs.isOpened()) {
        cerr << "ERROR: Failed to open cascade from memory" << endl;
        return false;
    }
    return cc.read(fs.getFirstTopLevelNode());
}
#endif

static void help()
{
    cout << "\nFace + eye detection demo (Embox compatible)\n"
         << "Usage:\n"
         << "  facedetect [image | image_sequence | camera]\n"
         << "Using OpenCV " << CV_VERSION << "\n";
}

void detectAndDraw(Mat& img,
                   CascadeClassifier& cascade,
                   CascadeClassifier& nestedCascade,
                   double scale,
                   bool tryflip);

int main(int argc, const char** argv)
{
    VideoCapture capture;
    Mat frame, image;
    string inputName;
    bool tryflip;
    double scale;
    CascadeClassifier cascade, nestedCascade;

    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{scale|1|}"
        "{try-flip||}"
        "{@filename||}"
    );

    if (parser.has("help")) {
        help();
        return 0;
    }

    scale = parser.get<double>("scale");
    if (scale < 1) scale = 1;
    tryflip = parser.has("try-flip");
    inputName = parser.get<string>("@filename");

#ifdef __EMBOX__
    if (!loadCascadeFromMemory(
            cascade,
            haarcascade_frontalface_alt_xml,
            haarcascade_frontalface_alt_xml_len))
    {
        cerr << "ERROR: face cascade load failed\n";
        return -1;
    }

    if (!loadCascadeFromMemory(
            nestedCascade,
            haarcascade_eye_tree_eyeglasses_xml,
            haarcascade_eye_tree_eyeglasses_xml_len))
    {
        cerr << "WARNING: eye cascade load failed\n";
    }
#else
    cascade.load("data/haarcascades/haarcascade_frontalface_alt.xml");
    nestedCascade.load("data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
#endif

    /* CAMERA */
    if (inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1)) {
        int cam = inputName.empty() ? 0 : inputName[0] - '0';
        capture.open(cam);
    }
    /* IMAGE OR IMAGE SEQUENCE */
    else {
        image = imread(inputName, IMREAD_COLOR);
        if (image.empty()) {
            if (!capture.open(inputName)) {
                cerr << "Could not read " << inputName << endl;
                return 1;
            }
        }
    }

    if (capture.isOpened()) {
        while (true) {
            capture >> frame;
            if (frame.empty()) break;
            detectAndDraw(frame, cascade, nestedCascade, scale, tryflip);
#ifdef __EMBOX__
            sleep(1);
#else
            if (waitKey(10) == 27) break;
#endif
        }
    } else if (!image.empty()) {
        detectAndDraw(image, cascade, nestedCascade, scale, tryflip);
#ifdef __EMBOX__
        sleep(5);
#else
        waitKey(0);
#endif
    }

    return 0;
}

void detectAndDraw(Mat& img,
                   CascadeClassifier& cascade,
                   CascadeClassifier& nestedCascade,
                   double scale,
                   bool tryflip)
{
    double t = 0;
    double total_start = (double)getTickCount();  // Start total timing
    vector<Rect> faces, faces2;
    Mat gray, smallImg;

    cvtColor(img, gray, COLOR_BGR2GRAY);
    resize(gray, smallImg, Size(), 1/scale, 1/scale, INTER_LINEAR_EXACT);
    equalizeHist(smallImg, smallImg);

    t = (double)getTickCount();
    cascade.detectMultiScale(
        smallImg, faces,
        1.1, 2, CASCADE_SCALE_IMAGE,
        Size(30, 30)
    );

    if (tryflip) {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale(smallImg, faces2, 1.1, 2);
        for (auto& r : faces2)
            faces.push_back(Rect(
                smallImg.cols - r.x - r.width, r.y, r.width, r.height));
    }

    t = (double)getTickCount() - t;
    printf("Haar detection time = %g ms\n", t * 1000 / getTickFrequency());
    printf("Haar found %zu potential faces\n", faces.size());
    fflush(stdout);

    for (size_t i = 0; i < faces.size(); i++) {
        Rect r = faces[i];

        // Extract ROI for CNN
        Mat roi = img(r);
        Mat gray_roi, cnn_input;
        cvtColor(roi, gray_roi, COLOR_BGR2GRAY);
        resize(gray_roi, cnn_input, Size(34, 34));

        // Time CNN inference
        double cnn_start = (double)getTickCount();
        float confidence = cnn_inference(cnn_input.data, 34, 34);
        double cnn_end = (double)getTickCount();

        double cnn_time = (cnn_end - cnn_start) * 1000 / getTickFrequency();

        printf("  ROI %zu: confidence = %.3f (CNN took %.2f ms)\n", i, confidence, cnn_time);

        // CNN decision: >0.3 = face, <0.3 = non-face
        bool is_face = (confidence > 0.3f);

        // Draw based on CNN decision
        Scalar face_color;
        if (is_face) {
            face_color = Scalar(0, 255, 0);  // Green for faces
            printf("    -> ACCEPTED as face\n");
        } else {
            face_color = Scalar(0, 0, 255);  // Red for non-faces
            printf("    -> REJECTED as non-face\n");
        }

        // Draw face rectangle (color based on CNN)
        rectangle(img,
                  Point(cvRound(r.x*scale), cvRound(r.y*scale)),
                  Point(cvRound((r.x+r.width)*scale),
                        cvRound((r.y+r.height)*scale)),
                  face_color, 3);

        // Only detect eyes if CNN says it's a face
        if (is_face && !nestedCascade.empty()) {
            Mat roi_small = smallImg(r);
            vector<Rect> eyes;
            nestedCascade.detectMultiScale(roi_small, eyes);

            for (auto& e : eyes) {
                Point ec(
                    cvRound((r.x + e.x + e.width*0.5)*scale),
                    cvRound((r.y + e.y + e.height*0.5)*scale));
                int rad = cvRound((e.width + e.height)*0.25*scale);
                circle(img, ec, rad, Scalar(255, 255, 0), 2);  // Yellow circles for eyes
            }
        }
    }

    // End total timing
    double total_end = (double)getTickCount();
    double total_time = (total_end - total_start) * 1000 / getTickFrequency();
    printf("Total detection time: %.2f ms\n", total_time);

#ifdef __EMBOX__
    imshowfb(img, 0);
#else
    imshow("result", img);
#endif
}
