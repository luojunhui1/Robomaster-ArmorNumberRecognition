//
// Created by root on 2021/5/10.
//
#include <cstdio>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <ArmorDetector.hpp>
#include <random>

using namespace std;
using namespace cv;

int FRAMEWIDTH;
int FRAMEHEIGHT;

//盐噪声
void salt(const cv::Mat &image, cv::Mat &dst, int n)
{
    dst = image.clone();

    int i,j;
    for (int k=0; k<n; k++) {

        // rand() is the random number generator
        i = std::rand()%dst.cols; // % 整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        j = std::rand()%dst.rows;

        if (dst.type() == CV_8UC1) { // gray-level image

            dst.at<uchar>(j,i)= 255; //at方法需要指定Mat变量返回值类型,如uchar等

        } else if (dst.type() == CV_8UC3) { // color image

            dst.at<cv::Vec3b>(j,i)[0]= 255; //cv::Vec3b为opencv定义的一个3个值的向量类型
            dst.at<cv::Vec3b>(j,i)[1]= 255; //[]指定通道，B:0，G:1，R:2
            dst.at<cv::Vec3b>(j,i)[2]= 255;
        }
    }
}

//椒噪声
void pepper(const cv::Mat& image, cv::Mat &dst, int n)
{
    dst = image.clone();

    int i,j;
    for (int k=0; k<n; k++) {

        // rand() is the random number generator
        i = std::rand()%dst.cols; // % 整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        j = std::rand()%dst.rows;

        if (dst.type() == CV_8UC1) { // gray-level image

            dst.at<uchar>(j,i)= 0; //at方法需要指定Mat变量返回值类型,如uchar等

        } else if (dst.type() == CV_8UC3) { // color image

            dst.at<cv::Vec3b>(j,i)[0]= 0; //cv::Vec3b为opencv定义的一个3个值的向量类型
            dst.at<cv::Vec3b>(j,i)[1]= 0; //[]指定通道，B:0，G:1，R:2
            dst.at<cv::Vec3b>(j,i)[2]= 0;
        }
    }
}

void Pinch(Mat& img, Mat& dst, int degree)
{
    if (degree < 1) degree = 1;
    else if (degree > 32) degree = 32;

    if (dst.empty())
        dst.create(img.rows, img.cols, img.type());
    dst = cv::Scalar::all(0);

    int chns = img.channels();
    int height = img.rows;
    int width = img.cols;

    int midX = width / 2;
    int midY = height / 2;

    int i, j, k;
    int X, Y, offsetX, offsetY;
    double radian, radius;  //弧和半径

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            offsetX = j - midX;
            offsetY = i - midY;

            radian = atan2((double)offsetY, (double)offsetX);

            // 半径
            radius = sqrtf((float)(offsetX*offsetX + offsetY * offsetY));
            radius = sqrtf(radius)*degree;

            X = (int)(radius*cos(radian)) + midX;
            Y = (int)(radius*sin(radian)) + midY;

            if (X < 0) X = 0;
            if (X >= width) X = width - 1;
            if (Y < 0) Y = 0;
            if (Y >= height) Y = height - 1;

            for (k = 0; k < chns; k++)
            {
                dst.at<Vec3b>(i, j)[k] = img.at<Vec3b>(Y, X)[k];
            }
        }
    }
}

int main()
{
    string imageNumberBaseFileDirs[6] = {" ","../res/number/1","../res/number/2","../res/number/3","../res/number/4","../res/number/5",};
    FILE *stream;

    string command;
    string dir;
    int imageCounts[10] = {0};

    int maxImageCount = 0;
    char shellOutput[5];
    for(int i = 1; i < 6; i++)
    {
        dir = imageNumberBaseFileDirs[i];
        command = "ls " + dir + "/*.jpg|wc -l";
        stream = popen(command.c_str(), "r");
        memset(shellOutput,'0',sizeof(shellOutput));
        fread(shellOutput, sizeof(char),sizeof(shellOutput), stream);

        *(imageCounts + i) = stoi(shellOutput);

        pclose(stream);
        if(*(imageCounts + i) > maxImageCount)
                maxImageCount = *(imageCounts + i);
    }

    /**==============================================IMAGE TRANSFORM=================================================**/
    /**Due to the large difference in the number of data sets of each label, it is necessary to manually make some data
     * and carry out some operations on the image, such as blurring, adding noise, shifting, distorting and so on
     * ==============================================================================================================**/


    int curAddCount;
    int curTranResCount;
    string imagePath;
    string suffix = ".jpg";

    Mat src,dst;

    uniform_int_distribution<unsigned> u(0,2);
    uniform_int_distribution<unsigned> u1(0,20);
    uniform_int_distribution<unsigned> u2(3,6);

    default_random_engine randomNumEngine;

    for(int i = 0; i < 6; i++)
    {
        if(imageCounts[i] == 0)
            continue;

        curAddCount = *(imageCounts + i);
        curTranResCount = 0;
        while(curAddCount < maxImageCount)
        {
            imagePath = imageNumberBaseFileDirs[i] + "/num_" + to_string(curTranResCount) + suffix;
            src = imread(imagePath);

            /**=====================================Average Blurring Image============================================**/
            blur(src,dst,Size(3 + u(randomNumEngine),3 + u(randomNumEngine)),Point(-1,-1),BORDER_REPLICATE);
            imwrite(imageNumberBaseFileDirs[i] + "/num_" + to_string(curAddCount) + suffix, dst);

            curAddCount += 1;

            /**=====================================Adding pepper-salt Noise On Image=================================**/
            pepper(src,dst,15 + u1(randomNumEngine));
            salt(dst,dst,15 + u1(randomNumEngine));

            imwrite(imageNumberBaseFileDirs[i] + "/num_" + to_string(curAddCount) + suffix, dst);
            curAddCount += 1;

            /**===========================================Shifting On Image=========================================**/
            int exchangelines = u2(randomNumEngine);
            dst  = Mat::zeros(src.size(), CV_8UC3);
            switch ((int)(u(randomNumEngine) >= 1)) {
                case 1:
                    src.colRange(src.cols - exchangelines,src.cols).copyTo(dst.colRange(0, exchangelines) );
                    src.colRange(0, src.cols - exchangelines).copyTo(dst.colRange(exchangelines,dst.cols));
                    break;
                case 0:
                    src.rowRange(src.rows - exchangelines,src.rows).copyTo(dst.rowRange(0, exchangelines));
                    src.rowRange(0, src.rows - exchangelines).copyTo(dst.rowRange(exchangelines,dst.rows));
                    break;
                default:
                    dst.colRange(0, exchangelines) = src.colRange(src.cols - exchangelines,src.cols);
                    dst.colRange(exchangelines,dst.cols) = src.colRange(0, src.cols - exchangelines);
                    break;
            }

            imwrite(imageNumberBaseFileDirs[i] + "/num_" + to_string(curAddCount) + suffix, dst);
            curAddCount += 1;

            /**=========================================Distorting On Image==========================================**/
            Pinch(src,dst,5);

            imwrite(imageNumberBaseFileDirs[i] + "/num_" + to_string(curAddCount) + suffix, dst);
            curAddCount += 1;

        }
    }
    return 0;
}
