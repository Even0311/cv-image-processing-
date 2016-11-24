#include <iostream>
#include <opencv2/opencv.hpp>



// Please change your image path here 
const std::string PATH{"/Users/haoranzhi/Desktop/boats.jpg"};
int mask = 3;
using cv::Mat;
using cv::namedWindow;
using cv::Scalar;
using cv::Vec3b;
using cv::Point;
using cv::waitKey;
using cv::copyMakeBorder;
typedef std::pair<int, int> PAIR;
class Transfer{

public:
    Transfer();
    Mat togray(Mat &);
    Mat to_most_frequent(Mat &,  int);
    int get_most_frequ(std::map<int,int> &);
    Mat final_oil(Mat &,Mat &);
    Mat sharpen(Mat&);
    Mat smooth(Mat &);


};




Transfer::Transfer() {};
// This is the method which transfer a image to gray image;
Mat Transfer::togray(Mat &image1) {
    namedWindow( "Original Image" );
    namedWindow( "Gray Image");
    imshow( "Original Image", image1 );
    Mat image2(image1.rows,image1.cols, CV_8UC1, Scalar::all(0));

    for(auto i=0;i<image1.rows;++i){
        for(auto j=0;j<image1.cols;++j){
            Vec3b intensity = image1.at<Vec3b>(Point(j,i));
            uchar blue = intensity.val[0];
            uchar green = intensity.val[1];
            uchar red = intensity.val[2];

            uchar I = 0.299 * red + 0.587 * green  + 0.114 * blue;

            image2.at<uchar>(i,j) = I;
        }
    }

    imshow("Gray Image", image2);

    return image2;
}
// This transger the original image to oil-painting like image
// which replace the current pixel value with the most frequent
// gray value
Mat Transfer::to_most_frequent(Mat &image,  int mask) {
    namedWindow( "Most Frequent Image" );
    Mat image2;
    Mat image3;
    copyMakeBorder(image,image2,mask,mask,mask,mask,cv::BORDER_DEFAULT);
    copyMakeBorder(image,image3,mask,mask,mask,mask,cv::BORDER_DEFAULT);

    for(int i=mask;i<image2.rows - mask; ++i){
        for(int j=mask;j<image2.cols - mask;++j){
            std::map<int,int> frequency;

            for(int row = -mask; row<= mask; ++row){
                for(int col = -mask; col <=mask; ++col){
                    Scalar intensity = image2.at<uchar>(Point(j+col,i+row ));
                    int fre = intensity.val[0];
                    frequency[fre] += 1;

                }
            }
            auto most_fre = get_most_frequ(frequency);
            image3.at<uchar>(i,j) = most_fre;
        }
    }



    imshow("Most Frequent Image", image3);
    return image3;
}

int Transfer::get_most_frequ(std::map<int,int> & frequency){



    std::vector<PAIR> pair_vec;
    for(auto it=frequency.begin();it!=frequency.end();++it){
        pair_vec.push_back(std::make_pair(it->first,it->second));
    }

    sort(pair_vec.begin(),pair_vec.end(),[](PAIR& a,PAIR& b){
        return a.second > b.second;
    });

    int f = pair_vec[0].first;
    return f;
}
// This transfer the source image into the final oil-painting like image
Mat Transfer::final_oil(Mat &source_oil, Mat &source_gry) {

    Mat image_to_return;
    copyMakeBorder(source_oil,image_to_return,mask,mask,mask,mask,cv::BORDER_DEFAULT);
    for(int i=mask;i<source_gry.rows - mask;++i){
        for(int j=mask;j<source_gry.cols - mask;++j){
            std::vector<PAIR> cordinate;
            Scalar intensity = source_gry.at<uchar>(Point(j,i));
            int standard = intensity.val[0];

            for(int row = -mask; row<=mask; ++row){
                for(int col=-mask;col <=mask;++col){
                    Scalar cmp = source_gry.at<uchar>(Point(j+col,i+row));
                    int cmp_value = cmp.val[0];
                    if(cmp_value == standard){
                        cordinate.push_back(std::make_pair(j+col,i+row));
                    }
                }
            }

            int b =0,g=0,r=0;
            for(auto it = cordinate.begin();it!=cordinate.end();++it){
                Vec3b colour = source_oil.at<Vec3b>(Point(it->first,it->second));

                uchar blue = colour.val[0];
                uchar green = colour.val[1];
                uchar red = colour.val[2];

                b+=blue;
                g+=green;
                r +=red;

            }

            b /= cordinate.size();
            g /= cordinate.size();
            r /= cordinate.size();

            image_to_return.at<Vec3b>(Point(j,i))[0] = b;
            image_to_return.at<Vec3b>(Point(j,i))[1] = g;
            image_to_return.at<Vec3b>(Point(j,i))[2] = r;

        }
    }

    namedWindow("Final Oil");
    imshow("Final Oil", image_to_return);

    return image_to_return;

}
Mat Transfer::sharpen(Mat &image) {
    Mat border_image;
    Mat sharpen_image;

    copyMakeBorder(image,border_image,mask,mask,mask,mask,cv::BORDER_DEFAULT);
    copyMakeBorder(image,sharpen_image,mask,mask,mask,mask,cv::BORDER_DEFAULT);


    for(int i=mask;i<border_image.rows - mask;++i){
        for(int j=mask;j<border_image.cols - mask;++j){
            Vec3b colour = border_image.at<Vec3b>(Point(j,i));
            Vec3b colour1 = border_image.at<Vec3b>(Point(j,i - 1));
            Vec3b colour2 = border_image.at<Vec3b>(Point(j,i+1));
            Vec3b colour3 = border_image.at<Vec3b>(Point(j-1,i));
            Vec3b colour4 = border_image.at<Vec3b>(Point(j+1,i));

            sharpen_image.at<Vec3b>(Point(j,i))[0]  = cv::saturate_cast<uchar>(colour.val[0]*5 - colour1.val[0]- colour2.val[0]- colour3.val[0]- colour4.val[0]);
            sharpen_image.at<Vec3b>(Point(j,i))[1]  = cv::saturate_cast<uchar>(colour.val[1]*5 - colour1.val[1]- colour2.val[1]- colour3.val[1]- colour4.val[1]);
            sharpen_image.at<Vec3b>(Point(j,i))[2]  = cv::saturate_cast<uchar>(colour.val[2]*5 - colour1.val[2]- colour2.val[2]- colour3.val[2]- colour4.val[2]);

        }
    }

    namedWindow("Sharpen Image");
    imshow("Sharpen Image" , sharpen_image);

    return sharpen_image;
}

Mat Transfer::smooth(Mat &image) {
    Mat border_image;
    Mat smoth_image;


    copyMakeBorder(image,border_image,mask,mask,mask,mask,cv::BORDER_DEFAULT);
    copyMakeBorder(image,smoth_image,mask,mask,mask,mask,cv::BORDER_DEFAULT);


    for(int i=mask;i<border_image.rows - mask;++i){
        for(int j=mask;j<border_image.cols - mask;++j){
            Vec3b colour  = border_image.at<Vec3b>(Point(j,i));
            Vec3b colour1 = border_image.at<Vec3b>(Point(j,i - 1));
            Vec3b colour2 = border_image.at<Vec3b>(Point(j,i+1));
            Vec3b colour3 = border_image.at<Vec3b>(Point(j-1,i));
            Vec3b colour4 = border_image.at<Vec3b>(Point(j+1,i));

            smoth_image.at<Vec3b>(Point(j,i))[0]  = cv::saturate_cast<uchar>((colour.val[0] +colour1.val[0]+ colour2.val[0]+colour3.val[0]+ colour4.val[0]) / 5);
            smoth_image.at<Vec3b>(Point(j,i))[1]  = cv::saturate_cast<uchar>((colour.val[1]+colour1.val[1] +colour2.val[1] + colour3.val[1] + colour4.val[1])/ 5);
            smoth_image.at<Vec3b>(Point(j,i))[2]  = cv::saturate_cast<uchar>((colour.val[2]+colour1.val[2] +colour2.val[2]+colour3.val[2] + colour4.val[2]) / 5 );

        }
    }

    namedWindow("Smooth Image");
    imshow("Smooth Image" , smoth_image);

    return smoth_image;


}
int main() {
    Mat image1 = cv::imread(PATH);
    Transfer trs = Transfer{};
    auto image2 = trs.togray(image1);
    auto image_frequent_ = trs.to_most_frequent(image2,mask);


    auto oil_image = trs.final_oil(image1,image_frequent_);
    auto sharpen_image = trs.sharpen(oil_image);
    auto smooth_image  = trs.smooth(oil_image);


    waitKey(0);


    return 0;
}