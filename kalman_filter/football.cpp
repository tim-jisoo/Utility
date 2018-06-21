//*********************************    <USER PARAMETER>   ***************************************//
#define PATH_VIDEO										"football.mp4"
#define PATH_TEMPLATE									"football.jpg"

#define ROI_WIDTH										150
#define ROI_HEIGHT										250
#define ROI_SIZE										(ROI_WIDTH)*(ROI_HEIGHT)

#define NUMBER_OF_INITIAL_DATA							3
#define BUTTON_FOR_WAIT_DATAREAD_SECTION				1
#define BUTTON_FOR_WAIT_KALMANFIL_SECTION				1

// [:: set speed of frame interval ::]
#define SEC_PER_FRAME					1/30

// [:: set covariance of noise ::]																
// 1. system white noise
#define MATRIX_Q_VALUE					1.0

// 2. sensor noise
#define MATRIX_R_VALUE					10.0


// [:: initiate parameters ::]
// 1. vector_x_prediction (Px, Py, Vx, Vy)
#define VECTORXPEDICTION_P_X_AXIS		0.0
#define VECTORXPEDICTION_V_X_AXIS		0.0
#define VECTORXPEDICTION_P_Y_AXIS		0.0
#define VECTORXPEDICTION_V_Y_AXIS		0.0

// 2. error covariance matrix										
#define MATRIX_P_VALUE_1				100.0   
#define MATRIX_P_VALUE_2				100.0   
#define MATRIX_P_VALUE_3				100.0
#define MATRIX_P_VALUE_4				100.0

//***********************************************************************************************//

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

void ball_detector(Mat* pFrame, Mat* pTemplate, Point * pos_by_kalman , Point * pos_by_sensor);
Point kalmanFilter(Point measure);

int main()
{
	const char path_video[] = PATH_VIDEO;
	const char path_template[] = PATH_TEMPLATE;
	const Scalar color_red = Scalar(0,0,255);
	const Scalar color_blue = Scalar(255,0,0);
	const int count = NUMBER_OF_INITIAL_DATA;
	const int radius = 2;
	const int thickness = 10;

	int x, y, width, height;
	Point pos_by_sensor;
	Point pos_by_kalman;
	Mat ball,frame;
	Mat padding;
	VideoCapture vc;
	FILE* fp;

	if((fp = fopen("data.txt","rt")))
	{
		ball = imread(path_template,CV_LOAD_IMAGE_COLOR);
		vc = VideoCapture(path_video);
		vc.read(frame);
		padding = Mat(frame.rows+ROI_HEIGHT,frame.cols+ROI_WIDTH,CV_8UC3);
		memset(padding.data,0,sizeof(uchar)*3*(frame.cols+ROI_WIDTH)*(frame.rows+ROI_HEIGHT));
		imshow("template",ball);
		waitKey();
	}
	else
	{
		printf("txt file read error\n");
		exit(1);
	}
	
	//tracking by reading data
	for(int i = 0; i < count ; i++)
	{
		//Get frame from video.
		vc.read(frame);

		//read data and calculate center position of object.
		fscanf(fp,"%d",&x); fscanf(fp,"%d",&y); fscanf(fp,"%d",&width); fscanf(fp,"%d",&height);
		pos_by_sensor.x = (x + x+width)/2;
		pos_by_sensor.y = (y + y+height)/2;

		//mark circle which indicate position of object to the source image.
		circle(frame,Point(pos_by_sensor.x, pos_by_sensor.y),radius, color_red, thickness);

		//insert the position of object to the kalman algorithm.
		//must consider padding size. because, the sensing algorithm uses roi-size image and could need padding image.
		//and initial data are obtained without considertion of padding size.
		pos_by_kalman = kalmanFilter(Point(pos_by_sensor.x + ROI_WIDTH/2, pos_by_sensor.y + ROI_HEIGHT/2));

		//mark circle which indicate estimation of object positoin to the padding image.
		circle(frame, Point(pos_by_kalman.x - ROI_WIDTH/2, pos_by_kalman.y - ROI_HEIGHT/2), radius, color_blue, thickness);
	
		//console out
		imshow("image", frame);
		waitKey(BUTTON_FOR_WAIT_DATAREAD_SECTION);
	}

	fclose(fp);

	//tracking by sensing
	while(vc.read(frame))
	{
		//add padding to frame.
		for(int j = 0; j < frame.rows; j++)
			memcpy(&padding.data[3*( ROI_WIDTH/2 + (ROI_HEIGHT/2 + j)*(frame.cols+ROI_WIDTH))], &frame.data[3*(j*frame.cols)], sizeof(uchar)*3*frame.cols);

		//get position of object by sensing algorithm.
		ball_detector(&padding, &ball, &pos_by_kalman, &pos_by_sensor);

		//mark circle which indicate position of object to the padding image.
		circle(frame,Point(pos_by_sensor.x - ROI_WIDTH/2, pos_by_sensor.y-ROI_HEIGHT/2),radius,color_red,thickness);

		//insert the position of object to the kalman algorithm.
		pos_by_kalman = kalmanFilter(pos_by_sensor);

		//mark circle which indicate estimation of object positoin to the padding image.
		circle(frame, Point(pos_by_kalman.x - ROI_WIDTH/2, pos_by_kalman.y - ROI_HEIGHT/2), radius, color_blue, thickness);
		
		//console out
		imshow("image",frame);
		waitKey(BUTTON_FOR_WAIT_KALMANFIL_SECTION);
	}

	vc.release();

	return 0;
}

void ball_detector(Mat* pFrame, Mat* pTemplate, Point * pos_by_kalman , Point * pos_by_sensor)
{
	const Scalar color_red = Scalar(0,0,255);
	const int thickness = 2;
	const int kalman_x = pos_by_kalman->x;
	const int kalman_y = pos_by_kalman->y;
	double minVal;
	Point minLoc, reference;
	Mat math_area;
	Mat roi;
	
	//set roi size.
	roi = Mat(ROI_HEIGHT, ROI_WIDTH, CV_8UC3);
	reference.x = kalman_x - ROI_WIDTH/2;
	reference.y = kalman_y - ROI_HEIGHT/2;
	
	//get roi_image from frame.
	for(int r = 0; r < ROI_HEIGHT; r++)
		memcpy(&roi.data[3*(r*ROI_WIDTH)], &pFrame->data[3*(reference.x+(reference.y+r)*(pFrame->cols))], sizeof(uchar)*3*ROI_WIDTH);

	//search ball by using template matching method.
	matchTemplate(roi, *pTemplate,math_area, TM_SQDIFF);
	minMaxLoc(math_area, &minVal, NULL, &minLoc, NULL);

	rectangle(roi, minLoc, Point(minLoc.x+ (pTemplate->cols), minLoc.y+ (pTemplate->rows)), color_red, thickness);
	imshow("roi",roi);

	pos_by_sensor->x = (reference.x + minLoc.x) + (pTemplate->cols)/2;
	pos_by_sensor->y = (reference.y + minLoc.y) + (pTemplate->rows)/2;
}

Point kalmanFilter(Point measure)
{
    //vecotr_X[0] : position-x
    //vector_X[1] : velocity-x
    //vector_X[2] : position-y
    //vector_X[3] : velocity-y
    static float vector_X[4] = {VECTORXPEDICTION_P_X_AXIS,
				VECTORXPEDICTION_V_X_AXIS,
				VECTORXPEDICTION_P_Y_AXIS,
				VECTORXPEDICTION_V_Y_AXIS};

    static float matrix_P[4][4] = {{MATRIX_P_VALUE_1, 0.0, 0.0, 0.0},
                                {0.0, MATRIX_P_VALUE_2, 0.0, 0.0},
                                {0.0, 0.0, MATRIX_P_VALUE_3, 0.0},
                                {0.0, 0.0, 0.0, MATRIX_P_VALUE_4}};

    float vector_X_prediction[4], matrix_P_prediction[4][4], matrix_K[4][2], vector_Z[2];
    float fxdt, hxdt, nxdt, pxdt, inv_det, _a, _k, ci;
    Point estimate;

    //step1: prediction
    vector_X_prediction[0] = vector_X[0] + SEC_PER_FRAME * vector_X[1];
    vector_X_prediction[1] = vector_X[1];
    vector_X_prediction[2] = vector_X[2] + SEC_PER_FRAME * vector_X[3];
    vector_X_prediction[3] = vector_X[3];

    fxdt = matrix_P[1][1]*SEC_PER_FRAME;
    hxdt = matrix_P[1][3]*SEC_PER_FRAME;
    nxdt = matrix_P[3][1]*SEC_PER_FRAME;
    pxdt = matrix_P[3][3]*SEC_PER_FRAME;
    matrix_P_prediction[0][0] = matrix_P[0][0] + (matrix_P[1][0]+matrix_P[0][1])*SEC_PER_FRAME + fxdt*SEC_PER_FRAME;
    matrix_P_prediction[0][1] = matrix_P[0][1] + fxdt;
    matrix_P_prediction[0][2] = matrix_P[0][2] + (matrix_P[0][3]+matrix_P[1][2])*SEC_PER_FRAME + hxdt*SEC_PER_FRAME;
    matrix_P_prediction[0][3] = matrix_P[0][3] + hxdt;
    matrix_P_prediction[1][0] = matrix_P[1][0] + fxdt;
    matrix_P_prediction[1][1] = matrix_P[1][1];
    matrix_P_prediction[1][2] = matrix_P[1][2] + hxdt;
    matrix_P_prediction[1][3] = matrix_P[1][3];
    matrix_P_prediction[2][0] = matrix_P[2][0] + (matrix_P[3][0]+matrix_P[2][1])*SEC_PER_FRAME + nxdt*SEC_PER_FRAME;
    matrix_P_prediction[2][1] = matrix_P[2][1] + nxdt;
    matrix_P_prediction[2][2] = matrix_P[2][2] + (matrix_P[3][2]+matrix_P[2][3])*SEC_PER_FRAME + pxdt*SEC_PER_FRAME;
    matrix_P_prediction[2][3] = matrix_P[2][3] + pxdt;
    matrix_P_prediction[3][0] = matrix_P[3][0] + nxdt;
    matrix_P_prediction[3][1] = matrix_P[3][1];
    matrix_P_prediction[3][2] = matrix_P[3][2] + pxdt;
    matrix_P_prediction[3][3] = matrix_P[3][3];
    matrix_P_prediction[0][0] += MATRIX_Q_VALUE;
    matrix_P_prediction[1][1] += MATRIX_Q_VALUE;
    matrix_P_prediction[2][2] += MATRIX_Q_VALUE;
    matrix_P_prediction[3][3] += MATRIX_Q_VALUE;

    //step2: get Kalman gain
    _a=matrix_P_prediction[0][0] + MATRIX_R_VALUE;
    _k=matrix_P_prediction[2][2] + MATRIX_R_VALUE;
    ci = matrix_P_prediction[0][2]*matrix_P_prediction[2][0];
    inv_det = 1/((_a*_k)-ci);
    matrix_K[0][0] = inv_det * (matrix_P_prediction[0][0]*_k - ci);
    matrix_K[0][1] = inv_det * (MATRIX_R_VALUE * matrix_P_prediction[0][2]);
    matrix_K[1][0] = inv_det * (matrix_P_prediction[1][0]*_k - matrix_P_prediction[1][2]*matrix_P_prediction[2][0]);
    matrix_K[1][1] = inv_det * (matrix_P_prediction[1][2]*_a - matrix_P_prediction[1][0]*matrix_P_prediction[0][2]);
    matrix_K[2][0] = inv_det * (MATRIX_R_VALUE * matrix_P_prediction[2][0]);
    matrix_K[2][1] = inv_det * (matrix_P_prediction[2][2]*_a - ci);
    matrix_K[3][0] = inv_det * (matrix_P_prediction[3][0]*_k - matrix_P_prediction[3][2]*matrix_P_prediction[2][0]);
    matrix_K[3][1] = inv_det * (matrix_P_prediction[3][2]*_a - matrix_P_prediction[3][0]*matrix_P_prediction[0][2]);

    //step3: get estimate value
    vector_Z[0] = (float)measure.x;
    vector_Z[1] = (float)measure.y;
    vector_Z[0] -= vector_X_prediction[0];
    vector_Z[1] -= vector_X_prediction[2];
    vector_X[0] = vector_X_prediction[0] + matrix_K[0][0]*vector_Z[0] + matrix_K[0][1]*vector_Z[1];
    vector_X[1] = vector_X_prediction[1] + matrix_K[1][0]*vector_Z[0] + matrix_K[1][1]*vector_Z[1];
    vector_X[2] = vector_X_prediction[2] + matrix_K[2][0]*vector_Z[0] + matrix_K[2][1]*vector_Z[1];
    vector_X[3] = vector_X_prediction[3] + matrix_K[3][0]*vector_Z[0] + matrix_K[3][1]*vector_Z[1];

    //update P_matrix
    matrix_P[0][0] = matrix_P_prediction[0][0] - (matrix_K[0][0]*matrix_P_prediction[0][0] + matrix_K[0][1]*matrix_P_prediction[2][0]);
    matrix_P[0][1] = matrix_P_prediction[0][1] - (matrix_K[0][0]*matrix_P_prediction[0][1] + matrix_K[0][1]*matrix_P_prediction[2][1]);
    matrix_P[0][2] = matrix_P_prediction[0][2] - (matrix_K[0][0]*matrix_P_prediction[0][2] + matrix_K[0][1]*matrix_P_prediction[2][2]);
    matrix_P[0][3] = matrix_P_prediction[0][3] - (matrix_K[0][0]*matrix_P_prediction[0][3] + matrix_K[0][1]*matrix_P_prediction[2][3]);
    matrix_P[1][0] = matrix_P_prediction[1][0] - (matrix_K[1][0]*matrix_P_prediction[0][0] + matrix_K[1][1]*matrix_P_prediction[2][0]);
    matrix_P[1][1] = matrix_P_prediction[1][1] - (matrix_K[1][0]*matrix_P_prediction[0][1] + matrix_K[1][1]*matrix_P_prediction[2][1]);
    matrix_P[1][2] = matrix_P_prediction[1][2] - (matrix_K[1][0]*matrix_P_prediction[0][2] + matrix_K[1][1]*matrix_P_prediction[2][2]);
    matrix_P[1][3] = matrix_P_prediction[1][3] - (matrix_K[1][0]*matrix_P_prediction[0][3] + matrix_K[1][1]*matrix_P_prediction[2][3]);
    matrix_P[2][0] = matrix_P_prediction[2][0] - (matrix_K[2][0]*matrix_P_prediction[0][0] + matrix_K[2][1]*matrix_P_prediction[2][0]);
    matrix_P[2][1] = matrix_P_prediction[2][1] - (matrix_K[2][0]*matrix_P_prediction[0][1] + matrix_K[2][1]*matrix_P_prediction[2][1]);
    matrix_P[2][2] = matrix_P_prediction[2][2] - (matrix_K[2][0]*matrix_P_prediction[0][2] + matrix_K[2][1]*matrix_P_prediction[2][2]);
    matrix_P[2][3] = matrix_P_prediction[2][3] - (matrix_K[2][0]*matrix_P_prediction[0][3] + matrix_K[2][1]*matrix_P_prediction[2][3]);
    matrix_P[3][0] = matrix_P_prediction[3][0] - (matrix_K[3][0]*matrix_P_prediction[0][0] + matrix_K[3][1]*matrix_P_prediction[2][0]);
    matrix_P[3][1] = matrix_P_prediction[3][1] - (matrix_K[3][0]*matrix_P_prediction[0][1] + matrix_K[3][1]*matrix_P_prediction[2][1]);
    matrix_P[3][2] = matrix_P_prediction[3][2] - (matrix_K[3][0]*matrix_P_prediction[0][2] + matrix_K[3][1]*matrix_P_prediction[2][2]);
    matrix_P[3][3] = matrix_P_prediction[3][3] - (matrix_K[3][0]*matrix_P_prediction[0][3] + matrix_K[3][1]*matrix_P_prediction[2][3]);


    estimate.x = (int)vector_X[0];
    estimate.y = (int)vector_X[2];

    return estimate;
}


	/*
	double distance, criteria;
	int buff1,buff2;
	Point globalPos;
	//check if the object position is valid or not.
	globalPos.x = reference.x + minLoc.x + (pTemplate->cols)/2;
	globalPos.y = reference.y + minLoc.y + (pTemplate->rows)/2;
	buff1 = globalPos.x - kalman_x;
	buff2 = globalPos.y - kalman_y;
	distance = sqrt((double)(buff1*buff1+buff2*buff2));
	buff1 = (pTemplate->cols)*(pTemplate->cols);
	buff2 = (pTemplate->rows)*(pTemplate->rows);
	criteria = sqrt((double)(buff1 + buff2))/2.0;

	if(criteria * 3.0 < distance)
	{
		*result_x = kalman_x - (pTemplate->cols)/2;
		*result_y = kalman_y - (pTemplate->rows)/2;
		*result_width = pTemplate->cols;
		*result_height = pTemplate->rows;
		rectangle(roi, Point(*result_x, *result_y), Point(*result_x + (pTemplate->cols), *result_y + (pTemplate->rows)), color_blue, thickness);
		imshow("roi",roi);
		waitKey();
	}
	else
	{
		*result_x = reference.x + minLoc.x;
		*result_y = reference.y + minLoc.y;
		*result_width = pTemplate->cols;
		*result_height = pTemplate->rows;
		rectangle(roi, minLoc, Point(minLoc.x+ (pTemplate->cols), minLoc.y+ (pTemplate->rows)), color_blue, thickness);
		imshow("roi",roi);
	}
	*/
