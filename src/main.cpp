#include "ardrone/ardrone.h"

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        return -1;
    }

    // Battery
    std::cout << "Battery = " << ardrone.getBatteryPercentage() << "[%]" << std::endl;

    // Instructions
    std::cout << "***************************************" << std::endl;
    std::cout << "*       CV Drone sample program       *" << std::endl;
    std::cout << "*           - How to play -           *" << std::endl;
    std::cout << "***************************************" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Controls -                        *" << std::endl;
    std::cout << "*    'Space' -- Takeoff/Landing       *" << std::endl;
    std::cout << "*    'Up'    -- Move forward          *" << std::endl;
    std::cout << "*    'Down'  -- Move backward         *" << std::endl;
    std::cout << "*    'Left'  -- Turn left             *" << std::endl;
    std::cout << "*    'Right' -- Turn right            *" << std::endl;
    std::cout << "*    'Q'     -- Move upward           *" << std::endl;
    std::cout << "*    'A'     -- Move downward         *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Others -                          *" << std::endl;
    std::cout << "*    'T'     -- Track marker          *" << std::endl;
    std::cout << "*    'C'     -- Change camera         *" << std::endl;
    std::cout << "*    'Esc'   -- Exit                  *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "***************************************" << std::endl;

    while (1) {
        double cx = 0;
        double cy = 0;
        cv::Rect trackRect;
        // Key input
        int key = cv::waitKey(33);
        if (key == 0x1b) break;

        // Get an image
        cv::Mat image = ardrone.getImage();

        // Take off / Landing 
        if (key == ' ') {
            if (ardrone.onGround()) ardrone.takeoff();
            else                    ardrone.landing();
        }

        // Move
        double vx = 0.0, vy = 0.0, vz = 0.0, vr = 0.0;
        if (key == 'i' || key == CV_VK_UP)    vx =  1.0;
        if (key == 'k' || key == CV_VK_DOWN)  vx = -1.0;
        if (key == 'u' || key == CV_VK_LEFT)  vr =  1.0;
        if (key == 'o' || key == CV_VK_RIGHT) vr = -1.0;
        if (key == 'j') vy =  1.0;
        if (key == 'l') vy = -1.0;
        if (key == 'q') vz =  1.0;
        if (key == 'a') vz = -1.0;
        ardrone.move3D(vx, vy, vz, vr);

        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode % 4);

        // Switch tracking ON/OFF
        static int track = 0;
        if (key == 't') track = !track;

        // People detect
        trackRect = ardrone.detectHuman(image);

        cx = trackRect.x + (trackRect.width / 2);
        cy = trackRect.y + (trackRect.height / 2); 
        cv::Point2f mc = cv::Point2f(cx, cy);

        //std::cout << "cx: " << cx << " cy: " << cy <<std::endl;
        cv::circle(image, mc, 5, cv::Scalar(0,0,255));
        //std::cout << "rect size: " << trackRect.width * trackRect.height << std::endl;

        // Tracking
        if (track) {
            if (cx == 0 && cy == 0)
            {
                vx = 0.0;
                vy = 0.0;
                vr = 0.0;
                vz = 0.0;
            } else {
                const double kp = 0.005;
                const double ka = 0.005;
                const double first_area = 30000;
                double rec_area = trackRect.width * trackRect.height;
                vx = ka * (first_area - rec_area);
                vy = 0.0;
                vr = kp * (image.cols / 2 - mc.x);
                vz = kp * (image.rows / 2 - mc.y);
                // const double kp = 0.005;
                // vx = 0.1;
                // vy = 0.0;
                // vz = kp * (image.rows / 2 - cy);
                // vr = kp * (image.cols / 2 - cx);
            }
        }
        
        // Display the image
        cv::putText(image, (track) ? "track on" : "track off", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, (track) ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        cv::imshow("camera", image);
        ardrone.move3D(vx, vy, vz, vr);

    }

    // See you
    ardrone.close();

    return 0;
}
