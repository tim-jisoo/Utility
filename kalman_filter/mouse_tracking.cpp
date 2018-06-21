//Made by ji-soo.
//Date: 2017.05.12

//*********************************    <USER PARAMETER>   ***************************************//

// [:: set speed of frame interval ::]
#define SEC_PER_FRAME					0.1

// [:: set covariance of noise ::]																
// 1. system white noise
#define MATRIX_Q_VALUE					1.0

// 2. sensor noise
#define MATRIX_R_VALUE					100.0


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




























#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

typedef struct mypoint{
    int x;
    int y;
}MYPOINT;

MYPOINT kalmanFilter(MYPOINT measure)
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
    MYPOINT estimate;

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

void mouse_eventHandler(int event, int x, int y, int flags, void* param)
{
	Mat image = *((Mat *)param);
    MYPOINT input;
    MYPOINT output;

    switch(event)
    {
    case EVENT_MOUSEMOVE:
		input.x = x;
		input.y = y;
        circle(image, Point(x, y), 1, Scalar(0, 0, 255), 2);
        output = kalmanFilter(input);
        circle(image, Point(output.x,output.y), 1, Scalar(0,255,0),2);
        break;
    case EVENT_LBUTTONDOWN:
    case EVENT_RBUTTONDOWN:
        memset(image.data,0,sizeof(uchar)*image.rows*image.cols*3);
        break;
    }

	imshow("test", image);
}


int main(void)
{
	int i;
    Mat img(1024,1024, CV_8UC3, Scalar(0, 0, 0));

    printf("[green] : estimation of mouse pointer position (kalman filter output) \n");
    printf("[red  ] : measurement of mouse pointer position (kalman filter input) \n\n");
    printf("====[USAGE] ========================================\n\n");
    printf("any mouse button      >> claen up the background\n");
    printf("any keyboard button   >> termination of program\n\n");
	printf("====================================================\n\n");

	imshow("test", img);
	moveWindow("test", 800,0);
	setMouseCallback("test", mouse_eventHandler, (void *)&img);
	
	waitKey();
	printf("creator : jisoo heo\n");
	printf("date    : 17.05.12\n");
	printf("e-mail  : heo3063@naver.com\n");

	return 0;
}
