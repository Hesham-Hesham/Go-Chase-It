#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include "sensor_msgs/Image.h"


using namespace ros;
using namespace ball_chaser;
using namespace sensor_msgs;
using namespace std;


// Define a global client that can request services
/*ros::*/
ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    DriveToTarget srv;
    srv.request.linear_x=lin_x;
    srv.request.angular_z=ang_z;
    if (!client.call(srv)){
        ROS_ERROR("Failed to call service safe_move");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback( const Image img)
{
    int white_pixels=255;
    int l = int(img.width / 3);		//left
    int c = int(2*(img.width) / 3);	//center
    int i=0;
    int steps=0;
    for ( i = 0; i < img.height * img.step; i++) {  // (img.height * img.step) size
        if(img.data[i]==white_pixels&&img.data[i+1]==white_pixels&&img.data[i+2]==white_pixels){
            steps=i % img.step;
            break;
        }
    }
     if ( steps<l )		//left ball
    {   
        drive_robot(0.0,2.0);
    }
    else if ( l<steps<c) 	//center ball
    {   
        drive_robot(1.0,0.0);
    }

    else if ( steps>c) 		//right ball
    {   
        drive_robot(0.0,-2.0);
    }

    else			//no ball  :"(
    {   
        drive_robot(0.0,0.0);        
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
