#include <math.h>

static float angleMeasuredOld;
static float angleReal;
static int   count;

void ExtendAngleInit(float angleMeasured)
{
    angleMeasuredOld=angleMeasured;
    count=0;
    angleReal=angleMeasured;
}

// �Ƕ���
float ExtendAngleExecute(float angleMeasured){


    float deltaAngle=angleMeasured-angleMeasuredOld;

    // ��Ϊ���β������ڣ�ת���ǶȲ����ܳ���180����������ζ�Ŵ��ڻ���
    if(fabs(deltaAngle)>180){

        if (deltaAngle>0){

            count=count-1;

        }else{
            count=count+1;
        }

    }

    angleReal=count*360+angleMeasured;

    angleMeasuredOld=angleMeasured; // ����


    return angleReal;

}

