//
// Created by root on 2021/4/21.
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

#include <FileProcess.h>
#include <opencv2/core/core_c.h>

using namespace std;
using namespace cv;

int FRAMEWIDTH = 640;
int FRAMEHEIGHT = 480;

int main()
{
    /**==============================================================================================================*
     * ============================================File Read Content=================================================*
     * ==============================================================================================================**/
    string base = "../res/number/";
    stack<string> subDirectories;//sub-directories stack

    subDirectories.push("5");
    subDirectories.push("4");
    subDirectories.push("3");
    subDirectories.push("2");
    subDirectories.push("1");

    string suffix = ".jpg";//suffix of target files
    FileReader fileScanner;

    string filePath;
    string command;
    string paramPath;

    ifstream paramReader;
    FILE *stream;

    int fileAmount = 0;//the amount of target files in directory
    char shellOutput[5];
    int fileReadCount = 0;// the amount of already read target files

    filePath = base + subDirectories.top() + "/";

    command = "ls " + base + subDirectories.top() + "/*.jpg|wc -l";
    stream = popen( command.c_str(), "r" );
    memset(shellOutput,'0',sizeof(shellOutput));
    fread(shellOutput, sizeof(char),sizeof(shellOutput), stream);
    fileAmount = stoi((shellOutput));
    pclose(stream);

    fileScanner = FileReader(filePath.c_str(),suffix);

     /**==============================================================================================================*
     * ============================================SVM Training Content===============================================*
     * ==============================================================================================================**/
    Mat classes;
    Mat trainingData;
    Mat trainingImages;
    vector<int> trainingLabels;

    Mat src,binary,bright,reshapedData;
    int armorNumber = 1;

    /**==========================================read training data==================================================**/
    while(!subDirectories.empty())//read files until all the target files in all the sub-directories has been read
    {
        if(!fileScanner.findNext())
            continue;
        cout<<"Directory: "<<filePath<<" | Read File "<<fileScanner.fileName<<endl;
        src = imread(fileScanner.fileName);
        if(src.empty())
            CV_Assert((fileScanner.fileName + " is Empty!").c_str());

        cvtColor(src,bright,COLOR_BGR2GRAY);

        //threshold(bright,binary,50, 255, CV_MINMAX);
        //Canny(bright,  binary, 50, 255);

       imshow("binary",binary);
        waitKey(20);

        reshapedData = binary.reshape(1, 1);
        trainingImages.push_back(reshapedData);

        trainingLabels.push_back(armorNumber);

        if(++fileReadCount == fileAmount)
        {
            cout<<"====================="<<filePath<<" | COMPLETED!====================="<<endl;
            subDirectories.pop();
            if(subDirectories.empty())
                break;
            fileReadCount = 0;
            filePath = base + subDirectories.top() + "/";

            command = "ls " + filePath + "/*.jpg|wc -l";
            stream = popen( command.c_str(), "r" );
            memset(shellOutput,'0',sizeof(shellOutput));
            fread(shellOutput, sizeof(char),sizeof(shellOutput), stream);
            fileAmount = stoi((shellOutput));
            pclose(stream);

            armorNumber++;

            fileScanner = FileReader(filePath.c_str(),suffix);
        }
    }
    trainingImages.copyTo(trainingData);
    trainingData.convertTo(trainingData,CV_32FC1);

    imshow("trainingImages",trainingImages);

    Mat(trainingLabels).copyTo(classes);

    /**=====================================SVM parameter configuration==============================================**/
    Ptr<cv::ml::SVM> svm = ml::SVM::create();
    svm->setType(ml::SVM::C_SVC);
    svm->setKernel(ml::SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::EPS, 1000, 1e-6));
    svm->setGamma(0.01);
    svm->setC(10.0);

    /**============================================train SVM=========================================================**/
    svm->train(trainingData,cv::ml::ROW_SAMPLE,classes);

    /**============================================save SVM==========================================================**/
    svm->save("../SVMTrain/models/svm.xml");

    return 0;
}