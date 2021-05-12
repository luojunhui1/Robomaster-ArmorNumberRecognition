//
// Created by root on 2021/5/12.
//

#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <stack>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/dnn//dnn.hpp>

#include <FileProcess.h>
#include <opencv2/core/core_c.h>
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;

int FRAMEWIDTH = 640;
int FRAMEHEIGHT = 480;

int main()
{
    Ptr<cv::ml::SVM> model;
    model = cv::ml::StatModel::load<cv::ml::SVM>("../SVMTrain/models/svm.xml");

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

    string imagePath;
    string suffix = ".jpg";

    Mat src,dst;
    Mat bright,binary,reshapedData;

    int predictCount = 100;

    for(int i = 1; i < 6; i++)
    {
       for(int j = 0; j < predictCount; j++)
       {
           imagePath = imageNumberBaseFileDirs[i] + "/num_" + to_string(j) + suffix;
           src = imread(imagePath);

           cvtColor(src,bright,COLOR_BGR2GRAY);

           threshold(bright,binary,50, 255, CV_MINMAX);
           pyrDown(binary,binary);

           imshow("binary",binary);
           waitKey(20);

           reshapedData = binary.reshape(1, 1);
           reshapedData.convertTo(reshapedData,CV_32FC1);

           int output;
           output = (int)((model->predict(reshapedData)) + 0.5);

           cout<<"output: "<<output<<endl;

       }
    }

    return 0;
}