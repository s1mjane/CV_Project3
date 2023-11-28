// 소리 X 터미널 출력 대체 (재인 code in Mac)
// Line256!까지 출력되고 멈춤 (라인 벗어남 끝)
// 다시 라인 잡히면 257부터 출력 ~ 438까지 하고 라인 벗어남 끝나면 다시 중지
// -> end of video 출력

#include <iostream>
#include <opencv2/opencv.hpp>
// #include <Windows.h> // Include Windows.h for Beep function

using namespace std;
using namespace cv;

int main() {

    //lane departure
    Mat frame;
    Mat canny_left, canny_right;
    VideoCapture cap;
    bool showText = false; // Variable that determines whether the "Lane Departure" message is displayed
    int frameCount = 0;    // Variable to store the number of last frames after displaying the message

    float rho, theta, a, b, x0, y0, total_rho, total_theta;
    Point p1, p2;
    Point p3, p4;
    vector<Vec2f> lines1, lines2;
    int i=1;

    // ROI to detect lane departure
    Rect lane_roi(600, 800, 400, 700); 


    if (cap.open("road2.mp4") == 0) {
        cout << "no such file!" << endl;
        waitKey(0);
    }

    int fps = cap.get(CAP_PROP_FPS);

    while (1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }

        rotate(frame, frame, ROTATE_90_CLOCKWISE);

        // lane departure
        canny_left = frame(lane_roi);

        cvtColor(canny_left, canny_left, COLOR_BGR2GRAY);
        GaussianBlur(canny_left, canny_left, Size(7, 7), 0);
        Canny(canny_left, canny_left, 50, 150, 3);
        rectangle(frame, lane_roi, Scalar(255, 0, 0), 2);

        imshow("canny", canny_left);
        HoughLines(canny_left, lines1, 1, CV_PI / 180, 250, 0, 0, -CV_PI / 180 * 15, CV_PI / 180 * 20);

        if (lines1.size()) {
            total_rho = 0;
            total_theta = 0;
            for (int i = 0; i < lines1.size(); i++) {
                rho = lines1[i][0];
                theta = lines1[i][1];

                total_rho += rho;
                total_theta += theta;
            }
            total_rho /= lines1.size();
            total_theta /= lines1.size();

            a = cos(total_theta);
            b = sin(total_theta);
            x0 = a * total_rho;
            y0 = b * total_rho;
            p1 = Point(cvRound(x0 + 1000 * (-b)) + 305, cvRound(y0 + 1000 * a) + 305);
            p2 = Point(cvRound(x0 - 1000 * (-b)) + 305, cvRound(y0 - 1000 * a) + 305);
            //line(frame,p1,p2,Scalar(0,0,255),3,8);

            showText = true;
            frameCount = 0;
        }

        if (showText) {
            // Beep(1000, 500);  // Beep sound for 500 milliseconds
            cout << "Line" << i << "!" << endl;

            frameCount++;
            i++;
            if (frameCount > fps) {
                showText = false;
            }
        }


        imshow("Frame", frame);
        waitKey(500 / fps);

    }
}