///////////////////////////////////////////////////////////////////////////////////////////////////////
/// Robot_s_head.cpp
/// 
/// Description:
/// This program is based on the "DemoTracker.cpp. It gives a simple head that will be applied to a robot.
/// The head expression changes with the user's expression
///
/// Dependencies: None. OpenCV DLLs and include folders are copied.
///
/// Author: Oriane Dermy
///
/// Creation Date: 17/12/2015
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <time.h>
#include <FaceAlignment.h>
#include <XXDescriptor.h>
#include <fstream>

using namespace std;

bool compareRect(cv::Rect r1, cv::Rect r2) { return r1.height < r2.height; }

// 2 modes: REALTIME,VIDEO
#define REALTIME//VIDEO

void drawPose(cv::Mat& img, const cv::Mat& rot, float lineL)
{
	int loc[2] = {70, 70};
	int thickness = 2;
	int lineType  = 8;

	cv::Mat P = (cv::Mat_<float>(3,4) << 
		0, lineL, 0,  0,
		0, 0, -lineL, 0,
		0, 0, 0, -lineL);
	P = rot.rowRange(0,2)*P;
	P.row(0) += loc[0];
	P.row(1) += loc[1];
	cv::Point p0(P.at<float>(0,0),P.at<float>(1,0));

	line(img, p0, cv::Point(P.at<float>(0,1),P.at<float>(1,1)), cv::Scalar( 255, 0, 0 ), thickness, lineType);
	line(img, p0, cv::Point(P.at<float>(0,2),P.at<float>(1,2)), cv::Scalar( 0, 255, 0 ), thickness, lineType);
	line(img, p0, cv::Point(P.at<float>(0,3),P.at<float>(1,3)), cv::Scalar( 0, 0, 255 ), thickness, lineType);
}

int main()
{
	char detectionModel[] = "./models/DetectionModel-v1.5.bin";
	char trackingModel[]  = "./models/TrackingModel-v1.10.bin";
	string faceDetectionModel("./models/haarcascade_frontalface_alt2.xml");

	// initialize a XXDescriptor object
	INTRAFACE::XXDescriptor xxd(4);
	// initialize a FaceAlignment object
	INTRAFACE::FaceAlignment fa(detectionModel, trackingModel, &xxd);
	if (!fa.Initialized()) {
		cerr << "FaceAlignment cannot be initialized." << endl;
		return -1;
	}
	// load OpenCV face detector model
	cv::CascadeClassifier face_cascade;
	if( !face_cascade.load( faceDetectionModel ) )
	{ 
		cerr << "Error loading face detection model." << endl;
		return -1; 
	}
	

#ifdef REALTIME
	// use the first camera it finds
	cv::VideoCapture cap(0); 
#endif 

#ifdef VIDEO
	string filename("./data/vid.wmv");
	cv::VideoCapture cap(filename); 
#endif

	if(!cap.isOpened())  
		return -1;

	int key = 0, count=0;
	bool isDetect = true;
	bool eof = false;
	float score, notFace = 0.5;
	cv::Mat X,X0, XPREVIOUS;
	cv::Point EL,ER,P1,P2;
    string winname("Demo IntraFace Tracker");
    const char* name= "Demo IntraFace Tracker";
    cv::namedWindow(winname,CV_WINDOW_NORMAL);
	
	cv::Mat Xbase(2,49,cv::DataType<float>::type);



	/*Add "basic head of the robot"*/
	ifstream fichier("teteEnreg.txt", ios::in);
	if(fichier)
	{
		char msg;
		int cols;
		fichier >> cols >> msg;
		//cout << cols << endl;
		for(int di=0;di<2;di++)
		{
			for(int dj=0;dj<cols;dj++)
			{
				fichier >> Xbase.at<float>(di,dj) >> msg;
			}
		}
		fichier.close();
	}else
	{
		cerr << "impossible d'ouvrir le fichier"<< endl;
		return 0;
	}
	/*End Add*/
	
	while (key!=1048603) // Press Esc to quit
    {

		//if 'f';, we go to fullscreen
        if(key==1048678) cvSetWindowProperty(name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		//if delete we delete fullscreen
        /*if(key==1114111){
			cout << "supr" << endl;
			cvSetWindowProperty(name, CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
		}*/
		cv::Mat frame;
		cap >> frame; // get a new frame from camera
		if (frame.rows == 0 || frame.cols == 0)
			break;
		//initialize the frame with color
		cv::Mat frame2(frame.rows, frame.cols, CV_32FC3, cv::Scalar(255,255,255));
		
		if (isDetect)
		{
			// face detection
			vector<cv::Rect> faces;
			face_cascade.detectMultiScale(frame, faces, 1.2, 2, 0, cv::Size(50, 50));
			// if no face found, do nothing
			if (faces.empty()) {
				//cv::imshow(winname,frame);
				key = cv::waitKey(5);
				continue ;
			}
			// facial feature detection on largest face found
			if (fa.Detect(frame,*max_element(faces.begin(),faces.end(),compareRect),X0,score) != INTRAFACE::IF_OK)
				break;
			isDetect = false;
		}
		else
		{
			// facial feature tracking
			if (fa.Track(frame,X0,X,score) != INTRAFACE::IF_OK)
				break;
		
			X0 = X;
		}/*
		if (score > 0.9)
		{
			for(int di=0;di<2;di++)
			{
			for(int dj=0;dj<X.cols;dj++)
			{
				Xbase= X;
			}
		}
		}*/
		if (score < notFace){ // detected face is not reliable
			isDetect = true;
			//TODO
			for(int di=0;di<2;di++)
			{
				for(int dj=0;dj<49;dj++)
				{
					X0.at<float>(di,dj) = Xbase.at<float>(di,dj);
				}
			}
			//X=Xbase;
			//cout << score << endl;
			//X0=Xbase;
			//X=XPREVIOUS;
		}
		//else
		//{
			XPREVIOUS = X0;
		//TEST
		//X0=Xbase;
			// plot facial landmarks
			for (int i = 0; i < 9; i++) //eyebrows
			{
				if (i==4) continue;
				P1 = cv::Point((int)(frame2.cols/2)+ (int)X0.at<float>(0,i) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i)-(int)X0.at<float>(1,10) );
				P2 = cv::Point((int)(frame2.cols/2) + (int)X0.at<float>(0,i+1) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i+1)-(int)X0.at<float>(1,10) );
				cv::line(frame2/*image*/,P1,P2 , cv::Scalar(0,0,0)/*couleur*/, 3);
				
			}for(int i=15; i<17;i++) //noze
			{
				P1 = cv::Point((int)(frame2.cols/2)+ (int)X0.at<float>(0,i) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i)-(int)X0.at<float>(1,10) );
				P2 = cv::Point((int)(frame2.cols/2) + (int)X0.at<float>(0,i+1) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i+1)-(int)X0.at<float>(1,10) );
				cv::line(frame2/*image*/,P1,P2 , cv::Scalar(0,0,0)/*couleur*/,3);
			}
			for(int i=43; i<48; i++)
			{
				P1 = cv::Point((int)(frame2.cols/2)+ (int)X0.at<float>(0,i) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i)-(int)X0.at<float>(1,10) );
				P2 = cv::Point((int)(frame2.cols/2) + (int)X0.at<float>(0,i+1) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i+1)-(int)X0.at<float>(1,10) );
				cv::line(frame2/*image*/,P1,P2 , cv::Scalar(0,0,0)/*couleur*/,3);
			}
			
				P1 = cv::Point((int)(frame.cols/2)+ (int)X0.at<float>(0,48) - (int)X0.at<float>(0,10), (int)(frame.rows/2)+ (int)X0.at<float>(1,48)-(int)X0.at<float>(1,10) );
				P2 = cv::Point((int)(frame.cols/2) + (int)X0.at<float>(0,43) - (int)X0.at<float>(0,10), (int)(frame.rows/2)+ (int)X0.at<float>(1,43)-(int)X0.at<float>(1,10) );
				cv::line(frame2/*image*/,P1,P2 , cv::Scalar(0,0,0)/*couleur*/,3);
			
			//creation des yeux
			if(count > 200)
			{
			P1= cv::Point((int)(frame.cols/2) + 7 - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,19)+(int)X0.at<float>(0,22))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,19) + (int)X0.at<float>(1,19))/2);
			P2 = cv::Point((int)(frame.cols/2)+ 7 - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,25)+(int)X0.at<float>(0,28))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,25) + (int)X0.at<float>(1,28))/2);
			EL = cv::Point((int)(frame.cols/2) - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,19)+(int)X0.at<float>(0,22))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,19) + (int)X0.at<float>(1,19))/2);
			ER = cv::Point((int)(frame.cols/2) - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,25)+(int)X0.at<float>(0,28))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,25) + (int)X0.at<float>(1,28))/2);
			cv::line(frame2/*image*/,EL/*centre*/,P1, cv::Scalar(0,0,0)/*couleur*/, 3);
			cv::line(frame2/*image*/,ER/*centre*/,P2, cv::Scalar(0,0,0)/*couleur*/, 3);
			if (count > 205) count=0;
			}else{
			EL = cv::Point((int)(frame.cols/2) - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,19)+(int)X0.at<float>(0,22))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,19) + (int)X0.at<float>(1,19))/2);
			ER = cv::Point((int)(frame.cols/2) - (int)X0.at<float>(0,10) +((int)X0.at<float>(0,25)+(int)X0.at<float>(0,28))/2, (int)(frame.rows/2)-(int)X0.at<float>(1,10) +((int)X0.at<float>(1,25) + (int)X0.at<float>(1,28))/2);
			cv::circle(frame2/*image*/,EL/*centre*/, 10/*radian*/, cv::Scalar(0,0,0)/*couleur*/, -1);
			cv::circle(frame2/*image*/,ER/*centre*/, 10/*radian*/, cv::Scalar(0,0,0)/*couleur*/, -1);
			}
			
			// head pose estimation
			//INTRAFACE::HeadPose hp;
			//fa.EstimateHeadPose(X0,hp);
			// plot head pose
			//drawPose(frame, hp.rot, 50);
		//}
		
		/*Ajout pour retenir la "tête de base" à dessiner*/
		/*ofstream fichier("teteEnregResult.txt", ios::out | ios::trunc);
		if(!fichier)
		{
			cerr << "Erreur à l'ouverture !" << endl;
			return 0;
		}
		else{
			char msg=',';
			fichier << X0.cols << msg;
			for(int di=0;di<X0.rows;di++)
			{
				for(int dj=0;dj<X0.cols;dj++)
				{
					fichier << X0.at<float>(di,dj) << msg;
				}
			}
			fichier.close();
		}*/
		/*End add*/
		
		cv::imshow(winname,frame2);	
		frame2.release();
		key = cv::waitKey(5);
	//	cout << key << endl;
		count = count +1;
	}

	return 0;

}
