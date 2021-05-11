//
// Created by root on 2021/5/10.
//
#include <cstdio>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <ArmorDetector.hpp>

using namespace std;
using namespace cv;

int FRAMEWIDTH;
int FRAMEHEIGHT;

int main()
{
    /**parameter file directories**/
    string paramFileDirs[2] = {"../res/color/blue/param.txt", "../res/color/red/param.txt"};
    string imageColorBaseFileDirs[2] = {"../res/color/blue/num_", "../res/color/red/num_"};
    string imageNumberBaseFileDirs[6] = {" ","../res/number/1/num_","../res/number/2/num_","../res/number/3/num_","../res/number/4/num_","../res/number/5/num_",};

    /**image base directory**/
    string base = "../res/number/";

    /**image suffix**/
    string suffix = ".jpg";

    /**image counts**/
    int imageCounts[10] = {0};

    /**variables **/
    ifstream paramFile;//parameter file path
    int imageCount;//image read Counter
    int number;//image Number
    string imagePath;//image path
    string command;//shell command

    string directory;
    string imageBaseFileDir;
    for(int i = 0; i < 2; i++)
    {
        directory = paramFileDirs[i];
        imageBaseFileDir = imageColorBaseFileDirs[i];

        imageCount = 0;
        paramFile.open(directory);

        while(paramFile>>number)
        {
            imagePath = imageBaseFileDir + std::to_string(imageCount++) + suffix;
            command = "cp " + imagePath +  " " + imageNumberBaseFileDirs[number] + to_string(imageCounts[number]++) + suffix;

            popen(command.c_str(),"w");

        }
        paramFile.close();
    }
    return 0;
}

