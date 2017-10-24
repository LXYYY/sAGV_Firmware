#include <math.h>

#include "TshapeVPlan.h"
#include "Rotating.h"

//#pragma CODE_SECTION(SmoothVelocityPlanning,"textafuncs"); 


//float tShapeVTarget = 0.0f;
//float tShapeWTarget = 0.0f;

// ֹͣ������ pathLength����0�����߹滮��TtotalС��һ���岹����
// ��������ֵΪ��ʣ����˶�ʱ�� Ttotal�� 
// ��һʱ�̵��ٶ�ֵ��ָ�����ʽ����
/* ���ڷ���������1. ָ���ٶ�vTarget; 2. ��ǰʵ���ٶ�Vcurr;
 */
float SmoothVelocityPlanning(float pathLength,float aB_acc,float aB_dec,float Vcurr,float Vend,float Vmax,float * vShapeTarget)
{
	  float EmergencyAcc = EmergencyAccRatio * aB_dec;
    float Vs = Vcurr;

    float Ve = Vend;

    float Ttotal = 0.0f;

    float res_t = 0.0f; 
    float Vout = 0.0f;  

    float Vpeak = Vmax;   // �滮�ķ�ֵ�ٶ�
    float T1 = 0.0f, T2 = 0.0f, T3 = 0.0f;


    float L_acc = 0.0f; // ����ٶγ���
    float L_dec = 0.0f; // ����ٶγ���
    float L_cri = 0.0f; // �ٽ糤��

    float aB_dec_Execute = aB_dec;


    if(pathLength <0){

        //Error, ����ģʽ
        Vout = Vs-EmergencyAcc*DELTA_T;

        T1 = 0; T2 = 0; T3 = (Vs-Ve)/EmergencyAcc;

        Vout = maximumAB(Vout,Ve);

        Ttotal = T1+T2+T3;

		* vShapeTarget = Vout;

        return Ttotal;
    }



    // �����ǰ�ٶȴ����趨�ٶ�
    if (Vs > (Vmax+aB_dec*DELTA_T)){

        L_cri = (Vs*Vs-Ve*Ve)/(2*aB_dec); 

        if (pathLength >= L_cri){  

            Vout = Vs-aB_dec*DELTA_T;

            T1 = 0; T2 = 0; T3 = (Vs-Ve)/aB_dec;

        }else{

          
            aB_dec_Execute = aB_dec*(L_cri/maximumAB(pathLength,NUM_ERROR));

            // Error, must be check
            if (aB_dec_Execute > EmergencyAcc){

                aB_dec_Execute = EmergencyAcc;

                // AGV�޷����趨��ͣ������������������ģʽ
            }

            Vout = Vs-aB_dec_Execute*DELTA_T;

            T1 = 0; T2 = 0; T3 = (Vs-Ve)/aB_dec_Execute;

        }

        Vout = maximumAB(Vout,Ve);

        Ttotal = T1+T2+T3;

        * vShapeTarget = Vout;

        return Ttotal;

    }


    // ֱ������ʽ�Ӽ��ٽṹ
    L_acc = (Vmax*Vmax-Vs*Vs)/(2*aB_acc);
	
    L_dec = (Vmax*Vmax-Ve*Ve)/(2*aB_dec);

    if (Vs > Ve){

        L_cri = (Vs*Vs-Ve*Ve)/(2*aB_dec);

    }else{

        L_cri = (Ve*Ve-Vs*Vs)/(2*aB_acc);

    }


    // ��·��
    if (pathLength >= L_acc+L_dec){

        T1 = (Vmax-Vs)/aB_acc;
        T2 = (pathLength-L_acc-L_dec)/Vmax;
        T3 = (Vmax-Ve)/aB_dec;

        Vpeak = Vmax;

    }

    // ��·��
    else if (pathLength < L_acc+L_dec && pathLength > L_cri+PathError){

        Vpeak = sqrt((2*aB_acc*aB_dec*pathLength+aB_dec*Vs*Vs+aB_acc*Ve*Ve)/(aB_acc+aB_dec));

        T1 = (Vpeak-Vs)/aB_acc;
        T2 = 0.0f;
        T3 = (Vpeak-Ve)/aB_dec;

    }

    // ��·��
    else if (pathLength <= L_cri+PathError){

        if (Ve > Vs){

            Vpeak = Ve;
            T1 = (Ve-Vs)/aB_acc; T2 = 0;T3 = 0;

        }else if (Ve < Vs-NUM_ERROR){

            Vpeak = Vs;

            if(pathLength >= L_cri){

                T1 = 0; T2 = 0;T3 = (Vs-Ve)/aB_dec;

            }else{

                aB_dec_Execute = aB_dec*(L_cri/maximumAB(pathLength,NUM_ERROR));

                // Error, must be check
                if (aB_dec_Execute > EmergencyAcc){

                    aB_dec_Execute = EmergencyAcc;

                    // AGV�޷����趨��ͣ������������������ģʽ
                }

                T1 = 0; T2 = 0;T3 = (Vs-Ve)/aB_dec;

            }

        }else
        {
            Vpeak = Vs;
            T1 = 0; T2 = 0; T3 = 0;
        }

    }


    Ttotal=T1+T2+T3;


    if(Ttotal>DELTA_T){

        if(T1>DELTA_T){

            Vout=Vs+aB_acc*DELTA_T;

        } else{

            if (T1<0){

                // ��ǰ�ٶȴ����趨�ٶ�
                Vout=Vs+aB_dec_Execute*maximumAB(T1,-DELTA_T);

            }else{

                res_t=DELTA_T-T1-T2;

                if (res_t<0){ 

                    Vout=Vs+aB_acc*T1;

                }else{  //�������ģʽ

                    Vout=Vs-aB_dec_Execute*minimumAB(DELTA_T,res_t);

                    Vout=maximumAB(Vout,Ve);

                }
            }
        }

    }else{

        Vout=Vs-aB_dec_Execute*DELTA_T;

        Vout=maximumAB(Vout,Ve);
    }


    * vShapeTarget = Vout;

    return Ttotal;

}


