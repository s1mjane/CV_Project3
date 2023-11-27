
#include <iostream>
#include <opencv2/opencv.hpp>

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

    Rect lane_roi(705, 500, 200, 200);  // ROI to detect lane departure

    if (cap.open("road.mp4") == 0) {
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

        Mat ycrcb;
        cvtColor(frame, ycrcb, COLOR_BGR2YCrCb);

        // Split into channels
        vector<Mat> channels;
        split(ycrcb, channels);

        // Apply histogram equalization to the Y channel
        equalizeHist(channels[0], channels[0]);

        // Merge the channels
        merge(channels, ycrcb);

        // Convert back to BGR color space
        cvtColor(ycrcb, frame, COLOR_YCrCb2BGR);


        // lane departure
        canny_left = frame(lane_roi);

        cvtColor(canny_left, canny_left, COLOR_BGR2GRAY);
        GaussianBlur(canny_left, canny_left, Size(7, 7), 0);
        Canny(canny_left, canny_left, 50, 150, 3);
        rectangle(frame, lane_roi, Scalar(255, 0, 0), 2);

        imshow("canny", canny_left);
        HoughLines(canny_left, lines1, 1, CV_PI / 180, 60, 0, 0, -CV_PI / 180 * 15, CV_PI / 180 * 20);

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
            putText(frame, "Lane Departure", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
            frameCount++;
            if (frameCount > fps) {
                showText = false;
            }
        }


        imshow("Frame", frame);
        waitKey(1000 / fps);

    }
}