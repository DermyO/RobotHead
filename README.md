# RobotHead
Based on the intraface tool, it allows to have a simple robotic head that changes its expression following its partner's one.

# INSTALL
This requires to have install intraface that are no longer available on their website (?)
In a terminal, go to the intraface folder and then:
"geany makefile"
(geany or an other application) then, replace the code with the one I give you. Save and quit.
"cd src"
here, paste the robot_s_head.cpp file.
"cd ../"
"make"

# USE
You just have to launch "./Robot_s_head" in the intraface folder, and then enjoy!

# Wich modifications has been done 
You have to install intraface, following these instructions http://humansensing.cs.cmu.edu/intraface/download_functions_cpp.html.
Then, you can add the "DemoTracker_modified_OnlySomePointsDisplay.cpp" and launch it.

#Changes between files DemoTracker and "DemoTracker_modified_OnlySomePointsDisplay":
First of all, the DemoTracker file allow to have two modes, one to read picture, and one to read our camera. In my programm I take into account camera's pictures. (#define REALTIME)

1. I create a new white frame, where will be the head robot
Code:
//initialize the frame with color
cv::Mat frame2(frame.rows, frame.cols, CV_32FC3, cv::Scalar(255,255,255));
	

2. The detection points are defined in X0 matrix as following: 
a. X0[0]--> X0[9] : eyebrows (linked by lines)
b. X0[10]--> X0[18] : noze (high part hidden)
c. X0[19]--> X0[30] : eyes
d. X0[31]--> X0[42] : external mouth (hidden)
e. X0[43]--> X0[48] : internal mouth

3. I Create lines between the interesting points (for example 0 to 4 and 5 to 9 to draw eyebrows);
Example : 
for (int i = 0; i < 9; i++) //eyebrows
{
	if (i==4) continue; //between the two eyebrows
	P1 = cv::Point((int)(frame2.cols/2) - (int)X0.at<float>(0,10) /*to center the face with respect to its noze*/ + (int)X0.at<float>(0,i) , (int)(frame2.rows/2)-(int)X0.at<float>(1,10)/*to center the face*/ +  (int)X0.at<float>(1,i) );
	P2 = cv::Point((int)(frame2.cols/2) + (int)X0.at<float>(0,i+1) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i+1)-(int)X0.at<float>(1,10) );
	cv::line(frame2/*picture we create*/,P1/*begin of the line*/,P2/*end of the line*/ , cv::Scalar(0,0,0)/*color*/, 3/*size*/);
}

4. I create a simple big point in the center of eye points

5. I add the possibility to blink sometimes
Code:
//Eyes creation
if(count > 200) //blink
{
	[...]
	cv::line(frame2,EL,P1, cv::Scalar(0,0,0), 3);
	cv::line(frame2,ER,P2, cv::Scalar(0,0,0), 3);
	if (count > 205) count=0;
}else //no blink
{
	[...]
        cv::circle(frame2/*image*/,EL/*centre*/, 10/*radian*/, cv::Scalar(0,0,0)/*couleur*/, -1);
	cv::circle(frame2/*image*/,ER/*centre*/, 10/*radian*/, cv::Scalar(0,0,0)/*couleur*/, -1);
}

5. If the person is not recognized (i.e. if his head is not totally visible on the screen), the head robot doesn't change, 
Code:
[...]
if (faces.empty()) {
	//cv::imshow(winname,frame); // there is the change
	key = cv::waitKey(5);
	continue ;
}
/*and I had this part*/
if (score < notFace){ // detected face is not reliable
	isDetect = true;
	X=XPREVIOUS; // previous X values
}
