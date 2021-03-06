// object_countingandtracking.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<opencv2\opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;

VideoCapture cap;

//int init_velocity=0, final_velocity=0;

void distance(vector<Point> &current_center, vector<Point> &previous_center,Mat &imgOriginal,vector<vector<Point>> &points_in_last_frames,vector<Point> &last_frames2, vector<Point> &last_frames1,int *object_count,vector<int> &init_velocity,vector<int> &final_velocity)
{
	vector<int> object_id(current_center.size());
	vector<Point> current_center1(current_center.size());
	int minimum_distance1 = -1, minimum_distance2 = -1;
	int count;

	//if ((*object_still_been_tracked) == true)
	{
		count = (*object_count);
		for (int k = 0; k < object_id.size(); k++)
		{
			object_id[k] = count;
			count++;
		}
		count = 0;
	}
	Point small;
	int intMiniumumdistanceindex=-1;
	
	for (int i = 0; i < previous_center.size(); i++)
	{
		int temp1 = 50000;
		long x, y;
		for (int j = 0; j < current_center.size(); j++)
		{
			int temp;
			x = (abs(current_center[j].x - previous_center[i].x))*(abs(current_center[j].x - previous_center[i].x));
			y = (abs(current_center[j].y - previous_center[i].y))*(abs(current_center[j].y - previous_center[i].y));
			temp = sqrt((x + y));

			if (temp <= temp1)
			{
				temp1 = temp;
				intMiniumumdistanceindex = j;
			}
		}

		//cout << " min " << intMiniumumdistanceindex << endl;

		if (i < intMiniumumdistanceindex)
		{
			int c = 0;
			for (int a =current_center.size()-1;a>=0 ;a--)
			{
				current_center1[c]=current_center[a];
				c++;
			}
			for (int b = 0; b < current_center1.size(); b++)
			{
				current_center[b]=current_center1[b];
			}
			break;
		}
	}
	
	for (int i = 0; i < previous_center.size(); i++)
	{
		int temp1 = 50000;
		long x, y;
		for (int j = 0; j < current_center.size(); j++)
		{
			int temp;
			x = (abs(current_center[j].x - previous_center[i].x))*(abs(current_center[j].x - previous_center[i].x));
			y = (abs(current_center[j].y - previous_center[i].y))*(abs(current_center[j].y - previous_center[i].y));
			temp = sqrt((x + y));

			if (temp <= temp1)
			{
				temp1 = temp;
				intMiniumumdistanceindex = j;
				if (i == 0)
				{
					minimum_distance1 = temp1;
					
				}

				if (i == 1)
				{
					minimum_distance2 = temp1;
					//final_velocity.push_back(temp1);
				}
			}

		}

		final_velocity.push_back(temp1);

		if (i == intMiniumumdistanceindex)
		{
				Point Center_point2(current_center[intMiniumumdistanceindex]);
				Point center(30, 40);
				putText(imgOriginal, format("%d", object_id[intMiniumumdistanceindex]), Center_point2, FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 255), 2);
				putText(imgOriginal, format("Active objects: %d", object_id.size()), center, FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 255), 2);


				for (int c = 0; c <= i; c++)
				{
					vector<Point> temp;

					for (int d = 0; d < current_center.size(); d++)
					{
						temp.push_back(current_center[intMiniumumdistanceindex]);
					}

					points_in_last_frames.push_back(temp);
				}
				if (object_id[i] == (*object_count))
				{
					last_frames1.push_back(current_center[intMiniumumdistanceindex]);
				}

				if (object_id[i] == (*object_count+1))
				{
					last_frames2.push_back(current_center[intMiniumumdistanceindex]);
				}
				
					
					//final_velocity.push_back(minimum_distance2);


				if (minimum_distance1 > 100 || minimum_distance2 > 100)
				{
					last_frames1.clear();
					last_frames2.clear();
					current_center.clear();
					(*object_count) = (*object_count) + 2;
					final_velocity.clear();
					init_velocity.clear();
					cout << "infinite loop";
				}
				/*if ()
				{
					
				}*/
			}
	}

}

int main()
{
	
	cap.open("C:\\Users\\saurabh chandra\\Desktop\\video14.mp4");
	char chCheckForEscKey=113;
	if (!cap.isOpened())
	{
		cout << "cannot open the video stream";
		return -1;
	}

	int iLowH = 170;
	int iHighH = 179;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;
	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	cvCreateTrackbar("LowH", "Control", &iLowH, 179); 
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); 
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); 
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	vector<Point> previous_center;
	vector<Point> last_frames1, last_frames2;
	vector<vector<Point>> points_in_last_frames;
	vector<int> init_velocity, final_velocity, acceleration;
	
    
	Mat imgOriginal;
	int object_count=0;
	//bool object_still_been_tracked = true;
	while (cap.isOpened() && chCheckForEscKey != 27)
	{
		
		cap >> imgOriginal;
		//double fps = cap.get(CV_CAP_PROP_FPS);
		//cout << "fps " << cap.get(CV_CAP_PROP_FPS) <<"\n";
		Mat imgHSV;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		dilate(imgThresholded, imgThresholded, 0, Point(-1, -1), 2, 1, 1);

		vector<Point> current_center;
		vector<vector<Point>> contours;
		findContours(imgThresholded, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> mu(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{

			mu[i] = moments(contours[i]);
			double area = contourArea(contours[i]);
			Point center;

			if (area > 300)
			{
				center.x = int(mu[i].m10 / mu[i].m00);
				center.y = int(mu[i].m01 / mu[i].m00);
				Rect r = boundingRect(contours[i]);
				rectangle(imgOriginal, r, Scalar(255, 0, 0), 1, 8, 0);
				current_center.push_back(center);
			}
		}

		distance(current_center, previous_center, imgOriginal, points_in_last_frames, last_frames2, last_frames1,&object_count,init_velocity,final_velocity);
		previous_center = current_center;

		/*if (points_in_last_frames.size() == 0)
			continue;
		else
		{
			for (int i=0;i<points_in_last_frames.size();++i)
			{
				for (int  j = 0; j < points_in_last_frames[i].size(); ++j)
				{
					line(imgOriginal, points_in_last_frames[i][j], points_in_last_frames[i][j], Scalar(0, 0, 255));
				}
			}*/
	//}

		if (last_frames2.size() == 0 || last_frames2.size() == 1)
			continue;
		else
		{
			for (int k = 0; k < last_frames2.size()-1; k++)
			{
				line(imgOriginal, last_frames2[k], last_frames2[k + 1], Scalar(0, 0, 255));
			}

		}
		
		if (last_frames1.size() == 0 || last_frames1.size() == 1)
			continue;
		else
		{
			for (int k = 0; k < last_frames1.size() - 1; k++)
			{
				line(imgOriginal, last_frames1[k], last_frames1[k + 1], Scalar(0, 255, 0));
			}
		}

		if (init_velocity.size() != 0)
		{
			for (int i = 0; i < final_velocity.size(); i++)
			{
				int temp=0,displacemet=0;

				if (final_velocity.size() > init_velocity.size())
				{
					temp = final_velocity[i] - init_velocity[0];
					acceleration.push_back(temp);
					displacemet = init_velocity[0] + (temp / 2);
					cout << "displacement " << i << " "<<displacemet<<endl;
				}
				else
				{
					temp = final_velocity[i] - init_velocity[i];
					acceleration.push_back(temp);
					displacemet = init_velocity[i] + (temp / 2);
					cout << "displacement " << i << " " << displacemet<<endl;
				}

			}

		}

		 init_velocity = final_velocity;
		 final_velocity.clear();

			imshow("thresholded image", imgThresholded);
			imshow("imgOriginal", imgOriginal);
			chCheckForEscKey = waitKey(0);

			if(chCheckForEscKey!=27)
			{
			  waitKey(1);
			}
	}

	
	waitKey(0);
    return 0;
}

