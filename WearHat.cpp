#include<opencv2/opencv.hpp>
#include<tchar.h>
#include<stdio.h>
#include<iostream>
#include<SDKDDKVer.h>
using namespace std;
using namespace cv;



void MapToHead(Mat &hat, Mat &srcImage, int x, int y)
{
	if (hat.channels() != 4)
	{
		cout << "通道不为4" << endl;
		return;
	}
	for (int i = 0; i < hat.rows; i++)
	{
		const uchar *P = hat.ptr<uchar>(i);
		uchar* head = NULL;
		//j指向帽子，h指向头
		for (int j = 0, h = 0; (j < hat.cols) && (h < srcImage.cols); ++j, ++h)
		{
			int alpha = P[4 * j + 3];
			if (alpha != 0)
			{
				if ((i + y >= 0) && (i + y < srcImage.rows) && (x + h >= 0) && (x + h < srcImage.cols))
				{
					head = srcImage.ptr<uchar>(i + y);
					head[3 * (x + h)] = P[4 * j];
					head[3 * (x + h) + 1] = P[4 * j + 1];
					head[3 * (x + h) + 2] = P[4 * j + 2];
				}
			}
		}
	}
	

}

void detectFace(Mat srcImage, CascadeClassifier face_cascade)
{
	vector<Rect> faces;
	Mat grayImage;
	Mat hat;

	hat = imread("hat.png", -1);
	if (!hat.data)
	{
		cout << "hat 不能打开" << endl;
	}
	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);

	//人脸检测函数
	face_cascade.detectMultiScale(grayImage, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE,Size(30,30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		resize(hat, hat, Size(faces[i].width, faces[i].height), 0, 0, INTER_LANCZOS4);
		MapToHead(hat, srcImage, faces[i].x, faces[i].y - 1.0*faces[i].height);
	}
	imshow("Christmas hat", srcImage);
}

int main()
{
	String face_cascade_name = "haarcascade_frontalface_alt.xml";
	CascadeClassifier face_cascade;
	//载入人脸检测的库haarcascade_frontalface_alt
	if (!face_cascade.load(face_cascade_name))
	{
		cout << "facecascade_frontalface_alt不能加载" << endl;
		return -1;
	}
	//打开默认摄像头
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "摄像头无法打开！";
		return -1;
	}
	while (1)
	{
		Mat frame;
		cap >> frame;
		detectFace(frame, face_cascade);
		if (waitKey(1) >= 0) break;
	}
	waitKey(0);
	return 0;

}