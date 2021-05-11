//
// Created by root on 2021/4/21.
//
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <ArmorDetector.hpp>
using namespace std;
/**
由于相邻两帧装甲板可能看起来极为相似，因此由于一些代码上的交互不足，会出现一些误标注的情况，所以不建议使用waitKey读入键盘值直接标注，而是建议在这
 次之后再手动将每个保存的图像展示再标注一次。对每个视频都需针对红蓝两种颜色分别识别一次，识别时改动detector.hpp中的bluetarget宏定义以及main函
 数中的保存路径保存时按下y键，丢弃时按下其它键.
**/

int FRAMEWIDTH;
int FRAMEHEIGHT;

int main()
{
    rm::ArmorDetector detector;

    ofstream output("../res/color/blue/param.txt" , ios::app);
    string base = "../res/color/blue/num_";
    string suffix = ".jpg";
    string diff;

    int count = 232;

    string videoPath;

    VideoCapture capture;
    Mat src,numImage;

    /**Initialization**/
    videoPath = "/home/ljh/视频/SWMU_剪辑/video_sichuan_SWMU_7_Sub_01.mp4";
    capture.open(videoPath);

    /**number variables**/
    int armorNumber;//装装甲板数字标识，即从waitKey读入的键值， 实际不推荐使用
    Size armorImgSize = Size(40, 40);//装甲板统一保存大小
    Mat warpPerspective_mat;
    vector<Point2f> dstPoints(4);
    vector<Point2f> srcPoints(4, Point2f(0, 0));

    dstPoints[0] = Point2f(0, 0);
    dstPoints[1] = Point2f(armorImgSize.width, 0);
    dstPoints[2] = Point2f(armorImgSize.width, armorImgSize.height);
    dstPoints[3] = Point2f(0, armorImgSize.height);

    Mat warpPerspective_dst;

    capture.read(src);
    FRAMEHEIGHT = src.rows;
    FRAMEWIDTH = src.cols;

    detector.Init();

    while (capture.isOpened())
    {
        capture.read(src);
        if(src.empty())
            break;
        if(detector.ArmorDetectTask(src))
        {
            numImage = src(detector.targetArmor.rect);

            warpPerspective_mat = findHomography(detector.targetArmor.pts, dstPoints, RANSAC);

            warpPerspective(src, warpPerspective_dst, warpPerspective_mat, armorImgSize, INTER_NEAREST, BORDER_CONSTANT, Scalar(0)); //warpPerspective to get armorImage

            diff = (count%2)?("@@@@@    "):("*****    ");

            cout<<"=============================update===========================    "<<diff<<count<<endl;
            cv::imshow("num",warpPerspective_dst);
            armorNumber = waitKey() - '0';
            if(armorNumber == 27 - '0')
                continue;
            cv::imwrite(base + std::to_string(count++) + suffix, warpPerspective_dst);
            output<<armorNumber<<endl;
        }
        pyrDown(src,src);
        imshow("src",src);
        waitKey(20);
    }
    output.close();

    return 0;
}