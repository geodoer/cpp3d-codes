#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

//实现de Casteljau 算法来返回Bézier 曲线上对应点的坐标
cv::Point2f recursive_bezier(const std::vector<cv::Point2f>& control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if(control_points.size() == 2)
    {
        return control_points[0] + t * (control_points[1] - control_points[0]);
    }

    std::vector<cv::Point2f> tmp;

    for (int i = 0; i < control_points.size() - 1; ++i)
    {
        tmp.push_back(control_points[i] + t * (control_points[i + 1] - control_points[i]));
    }

    return recursive_bezier(tmp, t);
}

//采用反走样
#define ANTIALIASING

//绘制Bezier曲线
//@function: t在0到1范围内进行迭代，并在每次迭代中使t增加一个微小值
//              对于每个需要计算的t，将调用另一个函数recursive_bezier
//              然后该函数将返回在Bezier曲线上t处的点
//              最后将点绘制在window上
#ifdef ANTIALIASING
//反走样
void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    //Bilinear
    
    for (double t= 0.0; t<=1.0; t+=0.001)
    {
        auto p = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(p.y,p.x)[1] =255; 
        
        //左下角的坐标
        double x00 = std::floor(p.x);
        double y00 = std::floor(p.y);

        //与左下角的距离
        float dx = p.x - x00;
        float dy = p.y - y00;

        //距离采样点最近的坐标点
        cv::Point2f near;

        //采样点周围的4个点
        cv::Point2f p00;
        cv::Point2f p01;
        cv::Point2f p10;
        cv::Point2f p11;

        if(dx<=0.5f && dy<=0.5f)
        {
            p00 = cv::Point2f(x00,y00);
            p01 = cv::Point2f(x00,y00+1);
            p10 = cv::Point2f(x00+1,y00);
            p11 = cv::Point2f(x00+1,y00+1);
            near = p00;
        }
        else if(dx<=0.5f && dy>0.5f)
        {
            p00 = cv::Point2f(x00-1,y00);
            p01 = cv::Point2f(x00-1,y00+1);
            p10 = cv::Point2f(x00,y00);
            p11 = cv::Point2f(x00,y00+1);
            near = p10;
        }
        else if(dx>0.5f && dy<=0.5f)
        {
            p00 = cv::Point2f(x00,y00-1);
            p01 = cv::Point2f(x00,y00);
            p10 = cv::Point2f(x00+1,y00-1);
            p11 = cv::Point2f(x00+1,y00);
            near = p01;
        }
        else if(dx>0.5f && dy>0.5f)
        {
            p00 = cv::Point2f(x00-1,y00-1);
            p01 = cv::Point2f(x00-1,y00);
            p10 = cv::Point2f(x00,y00-1);
            p11 = cv::Point2f(x00,y00);
            near = p11;
        }

        std::vector<cv::Point2f> vec;
        vec.push_back(p00);
        vec.push_back(p01);
        vec.push_back(p10);
        vec.push_back(p11);

        //最近点 与 采样点 的距离
        float distance = sqrt((p-near).x * (p-near).x + (p-near).y * (p-near).y);
        
        for(auto point : vec)
        {
            //与采样点的距离
            float d = sqrt((p-point).x * (p-point).x + (p-point).y * (p-point).y);
            //权重
            float percnet = distance/d;

            cv::Vec3b color = window.at<cv::Vec3b>(p.y,p.x);
            window.at<cv::Vec3b>(point.y,point.x) = percnet * color;
        }
    }   
}
#else
//普通方法
void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for (float t = 0.0; t <= 1.0; t += 0.001)
    {
        auto p = recursive_bezier(control_points, t);

        // window.at<cv::Vec3b>(p.y, p.x)[0] = 255;  //B
        window.at<cv::Vec3b>(p.y, p.x)[1] = 255;  //G
        // window.at<cv::Vec3b>(p.y, p.x)[2] = 255;  //R
    }
}
#endif

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
#ifndef ANTIALIASING
            naive_bezier(control_points, window);
#endif
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}
