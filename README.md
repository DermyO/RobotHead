# RobotHead
Based on the intraface tool, it allows to have a simple robotic head that changes its expression following its partner's one.

#How to use it
You have to install intraface, following these instructions http://humansensing.cs.cmu.edu/intraface/download_functions_cpp.html.
Then, you can add the "DemoTracker_modified_OnlySomePointsDisplay.cpp" and launch it.

#Changes between files DemoTracker and "DemoTracker_modified_OnlySomePointsDisplay":
First of all, the DemoTracker file allow to have two modes, one to read picture, and one to read our camera. In my programm I take into account camera's pictures. (#define REALTIME)

1. The detection points are defined in X0 matrix as following: 
a. X0[0]--> X0[9] : eyebrows (linked by lines)
b. X0[10]--> X0[18] : noze (high part hidden)
c. X0[19]--> X0[30] : eyes
d. X0[31]--> X0[42] : external mouth (hidden)
e. X0[43]--> X0[48] : internal mouth
2. I Create lines between the interesting points (for example 0 to 4 and 5 to 9 to draw eyebrows);
Example : 
			for (int i = 0; i < 9; i++) //eyebrows
			{
				if (i==4) continue; //between the two eyebrows
				P1 = cv::Point((int)(frame2.cols/2) - (int)X0.at<float>(0,10) /*to center the face with respect to its noze*/ + (int)X0.at<float>(0,i) , (int)(frame2.rows/2)-(int)X0.at<float>(1,10)/*to center the face*/ +  (int)X0.at<float>(1,i) );
				P2 = cv::Point((int)(frame2.cols/2) + (int)X0.at<float>(0,i+1) - (int)X0.at<float>(0,10), (int)(frame2.rows/2)+ (int)X0.at<float>(1,i+1)-(int)X0.at<float>(1,10) );
				cv::line(frame2/*picture we create*/,P1/*begin of the line*/,P2/*end of the line*/ , cv::Scalar(0,0,0)/*color*/, 3/*size*/);
			}

3. I create a simple big point in the center of eye points
4. I add the possibility to blink sometimes
Example:
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
