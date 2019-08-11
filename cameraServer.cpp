
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include "OpenVideo.hpp"

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 

#define PORT 8888 
#define BUFFER_SIZE 3*800*600
using namespace cv;
using namespace std;

int main()
{
    //tcp settings
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[BUFFER_SIZE] = {0}; 

    //opencv data vars
    VideoCapture capture; //camera feed
    Mat currImg;

     // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Forcefully attaching socket to the port 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 

    bind(server_fd, (struct sockaddr *)&address,sizeof(address));
   
	cout<< "listening for host"; 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
	cout<< "got host";
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
  

    //timing variables to check performace
    clock_t startTime=clock(), currTime=clock();
    
    struct timeval currFrameTime,lastFrameTime;
    gettimeofday(&currFrameTime, NULL);

    OpenVideo myVideo(0);
   
        cout << "Capture is opened" << endl;
        while(waitKey(10) != 'q')
        {
            //grab image from camera
            currImg = myVideo.getImage();
            if(currImg.empty())
                break;

            //send images 
            Size imageSize = currImg.size();

            int datalen = imageSize.width * imageSize.height * 3, currPos = 0;
            int packetSize = imageSize.width;
            int currPacket; 
            while(currPos < datalen)
            {
                if(currPos + packetSize > datalen)
                    currPacket = datalen - currPos;
                else
                    currPacket = packetSize;
            
                send(new_socket, currImg.data + currPos, currPacket, 0);
                currPos += currPacket;
            }

        }
    

    return 0;
}


