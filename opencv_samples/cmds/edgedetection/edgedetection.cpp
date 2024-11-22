
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#ifndef __EMBOX__
#include "opencv2/highgui.hpp"
#endif

#include <iostream>
#include <unistd.h>

#ifdef __EMBOX__
#include <cv_embox_imshowfb.hpp>
#endif

using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges;

int lowThreshold = 50;
const int max_lowthreshold = 100;
const int canny_ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";

static void CannyThreshold() {
    blur(src_gray, detected_edges, Size(3, 3));
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * canny_ratio, kernel_size);
    dst = Scalar::all(0);
    src.copyTo(dst, detected_edges);
}

int main(int argc, char** argv) {
    static const char* names[] = { "data/objects/lotus.jpeg", "data/objects/lena.jpeg", "data/objects/car.jpeg", 0 };

    if( argc > 1) {
        names[0] = argv[1];
        names[1] = 0;
    }

    for( int i = 0; names[i] != 0; i++ ) {
        string filename = samples::findFile(names[i]);
        src = imread(filename, IMREAD_COLOR);
        if( src.empty() ) {
            cout << "Couldn't load " << filename << endl;
            continue;
        }

        cvtColor(src, src_gray, COLOR_BGR2GRAY);
        CannyThreshold();

        printf("Edge detection completed for %s.\n", filename.c_str());

#ifdef __EMBOX__
        imshowfb(dst, 0);
        sleep(1);
#else
        namedWindow(window_name, WINDOW_AUTOSIZE);
        imshow(window_name, dst);
        int c = waitKey();
        if( c == 27 )
            break;
#endif
    }

    return 0;
}
