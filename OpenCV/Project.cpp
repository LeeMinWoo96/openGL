
/*
	이름 : 이민우
	프로젝트 :opencv를 사용한 손가락 인식을 통한 동영상 조작
*/
#define _CRT_SECURE_NO_WARNINGS
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


Mat getHandMask1(const Mat& image, int minCr = 128, int maxCr = 170, int minCb = 73, int maxCb = 158) {

	//휘도Y를 제외하고 Cr, Cb 만을 사용한다.
	// 보통 Cr 은 128~170
	// Cb는 73~158 사이의 값이다
	//컬러 공간 변환 BGR->YCrCb
	Mat YCrCb;
	cvtColor(image, YCrCb, CV_BGR2YCrCb);

	//각 채널별로 분리
	vector<Mat> planes;
	split(YCrCb, planes);

	//각 채널별로 화소마다 비교
	Mat mask(image.size(), CV_8U, Scalar(0));   //결과 마스크를 저장할 영상
	int nr = image.rows;    //전체 행의 수
	int nc = image.cols;

	for (int i = 0; i < nr; i++) {
		uchar* CrPlane = planes[1].ptr<uchar>(i);   //Cr채널의 i번째 행 주소
		uchar* CbPlane = planes[2].ptr<uchar>(i);   //Cb채널의 i번째 행 주소
		for (int j = 0; j < nc; j++) {
			if ((minCr < CrPlane[j]) && (CrPlane[j] < maxCr) && (minCb < CbPlane[j]) && (CbPlane[j] < maxCb))
				mask.at<uchar>(i, j) = 255;
		}// j for close
	}// i for close


	erode(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
	//이웃한 화소들 중 최소 화소값을 현재 화소값으로 대체
	//침식연산 노이즈 제거할 때에 사용
	//3,3 필터 적용 필터 크기를 크게할 수록 변화량 많아짐
	// 마지막 인자는 반복횟수
	Canny(mask, mask, 100, 127); // 엣지 검출 알고리즘
	// 입력영상 출력영성 낮은 경계값 높은경계감 소벨 커널크기(디폴트 3)
	// 우리가 지정한 낮은 경계값 이하는 인식 x
	// 우리가 지정한 높은 경계값 이하는 확실한 경계선으로 인식


	return mask;
}// func close



int main() {
	VideoCapture cam(0); // cam(0) --> web cam
	Mat frame;  //cam 에서 받아오는 영상을 담을 곳

	char Img_name[20]; // img에 표시할 문자열을 담을 배열 
	Mat img_sub;
	Mat img_result;
	Mat frame1; // 동영상 파일 담을 프레임

	int width = 400, height = 200; // 동영상의 크기
	if (!cam.isOpened()) {// cam open
		cout << "Cam is not opend ";
	}//if close 

	VideoCapture cap1("a.mp4"); // 동영상 파일 재생하기 위함 
	if (!cap1.isOpened())
	{
		printf("동영상 파일을 열수 없습니다. \n");
	}
	int num = 30; //  동영상 속도

	while (1) {
		int count = 0;
		cam.read(frame);

		cap1 >> frame1; // 동영상 

		if (frame.empty() || frame1.empty()) {
			cout << "cam or video file open error" << endl;
			break;
		}//if close

		cv::flip(frame, frame, 1);// 웹캡으로 들어오는 영상 반대로 보이니

		Rect cutImg(150, 150, 300, 300); //전체를 체킹 x 부분 만 짤라서 확인하는게 
		// 다른 곳에 영향을 덜 받을 수 있음 


		img_sub = frame(cutImg); // 원본 프레임을 cutImg 크기 만큼 추출 
		GaussianBlur(img_sub, img_sub, Size(3, 3), 0.0, 0); // 잡음 부분 블러처리 
		img_result = getHandMask1(img_sub);// 프레임 전처리 함수 호출 


		vector<vector<Point> >contours; //외곽선 배열
		vector<Vec4i>hierarchy; //외곽선간의 계층구조


		findContours(img_result, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
		// 외곽선 찾는 함수 , 이미지, 외곽선 벡터, 경계선, 어떤 외곽선할지 이번경우(외부), 
		// simple로 할경우 수평 수직 대각선 외곽선이 포함


		vector<vector<int> >hull(contours.size());
		vector<vector<Point> >hullPoint(contours.size());
		vector<vector<Vec4i> >defects(contours.size());

		size_t indexOfBiggestContour = -99; // 윤곽선이 여러개 저장되는데 그게 손이라는 가정 , 그것의 인덱스와 사이즈를 찾음
		size_t sizeOfBiggestContour = 0;
		
		for (size_t i = 0; i < contours.size(); i++) { // 손이 가장 큰 윤곽선임을 가정
			if (contours[i].size() > sizeOfBiggestContour) { // 가장 큰 윤곽선을 갖는 물체 찾기
				sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
			}
		}
	
		
		vector<vector<Point> >defectPoint(contours.size());
		vector<vector<Point> >contours_poly(contours.size());
		Point2f rect_point[4]; //point2f자료형으로 사각형 점 네개
		vector<RotatedRect>minRect(contours.size()); //회전된 사각형 그리기
		vector<Rect> boundRect(contours.size()); //사각형 그리기

		for (size_t i = 0; i < contours.size(); i++) {
			if (contourArea(contours[i]) > 1000) { //외곽선의 면적이 1000보다 크면 (잡음 제거 효과)

				convexHull(contours[i], hull[i], true);
				convexityDefects(contours[i], hull[i], defects[i]); //외곽선을 이어줌 
				minRect[i] = minAreaRect(contours[i]);
				if (indexOfBiggestContour == i) { // i 가 가장 큰 외곽선의 인덱스와 같다면

					for (size_t k = 0; k < hull[i].size(); k++) { //hull의 크기가 k보다 작을때 동안
						int ind = hull[i][k];
						hullPoint[i].push_back(contours[i][ind]);
					}
					count = 0;

					for (size_t k = 0; k < defects[i].size(); k++) {
						if (defects[i][k][3] > 14 * 256) {
							int p_start = defects[i][k][0];
							int p_end = defects[i][k][1];
							int p_far = defects[i][k][2];
							defectPoint[i].push_back(contours[i][p_far]);
							circle(img_sub, contours[i][p_end], 3, Scalar(0, 0, 255), 3); //손가락 끝 점에 원표시
							count++;
						}

					}

					if (count == 1) {
						strcpy(Img_name, "1");
						num += 100;

					}
					else if (count == 2) {
						strcpy(Img_name, "2");
						if (num > 100)
							num -= 100;

					}
					else if (count == 3) {
						strcpy(Img_name, "3");
						width += 50;
						height += 50;
						//resize(frame1, frame1, Size(width, height), 0, 0, CV_INTER_LINEAR);// 확때할댄 쌍선형


					}
					else if (count == 4) {
						strcpy(Img_name, "4");
						width -= 50;
						height -= 50;
						if (width < 10 || height < 10) {
							width = 50;
							height = 50;
						}
						//resize(frame1, frame1, Size(width, height), 0, 0, CV_INTER_AREA); // 축소할땐 영역보간법 


					}
					else if (count == 5) {
						strcpy(Img_name, "5");
						break;
					}
					else {
						strcpy(Img_name, "0"); //손가락 개수
					}
					putText(frame, Img_name, Point(70, 70), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, 8, false); //글씨 쓰기
					approxPolyDP(contours[i], contours_poly[i], 3, false);

					boundRect[i] = boundingRect(contours_poly[i]);
					rectangle(img_sub, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
					minRect[i].points(rect_point);
					for (size_t k = 0; k < 4; k++) {
						line(img_sub, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8); // 사각형의 point 읽고 선 그리기
					}
				}
			}
		}

		resize(frame1, frame1, Size(width, height), 0, 0, CV_INTER_AREA); 
		
		imshow("Video", frame1);
		imshow("Result img", frame);
		imshow("sub img", img_result);

		
		if (waitKey(num) == 27) // esc 입력 종료
			break;
	}//while close
}// main close
