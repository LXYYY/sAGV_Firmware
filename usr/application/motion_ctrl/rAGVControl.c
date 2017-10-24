#include <stdlib.h>

#include "ucos_ii.h"

#include "common/tools.h"

#include "rAGVControl.h"
#include "DataBank.h"
#include "Tracingzq.h"
#include "Rotating.h"
#include "user_function.h"
#include "app_cfg.h"
#include "agv.h"
#include "Navigation.h"
#include "encoder.h"
#include "motion_ctrl.h"
#include "TshapeVPlan.h"
#include "common/PosixError.h"
#include "MCTools.h"
#include "angle.h"
#include "MotionEstimate.h" 

Uint16 reverseMode = REVERSE_MODE_HORIZONTAL;
Uint16 reverseCnt = 0;
Uint16 nReverseFlag = 0;


//�������ѹ�
static Uint16 GSMissing = 0x00 ; //������ʧ�� Ĭ��Ϊ����ʧ
static Uint16 OutOfTrack = 0x00 ; //�ѹ죬 Ĭ��Ϊ���ѹ�

//�ٶ����
static Uint16   nEvent = NO_EVENT;        // ��ǰ����Ӧ���¼�

float32  fFwdObsVel = 50.0f;   //ǰ�����ϰ��Զ��ʱ�������ٶ�
Uint16   uObClearCount = 0x00;     //Զ�����ϰ����־����Զ�����ϰ�����ٴ���

#if 0
Uint16 actionFinishSeq = 0x0;
Uint32 actionFinishReply = 0x0;
#endif
const float32 fOpenVel = 40.0f; 
const float32 wOpenVel = 1.0f; 


Uint16 	nAgvWorkMode = AGV_MODE_STANDBY;

float32 twoD_Progress = 0.0f;
float32 RealTwoDProgress = 0.0f;

float rotateR = 0.0f;

Uint16 twoDMissingCnt = 0x0;


static void error_event(int error);
//static void error_recovery(int error);

void AGV_Run(void)
{
   switch(nAgvWorkMode)
   {
      case AGV_MODE_STANDBY: AGV_Standby();
	                         break;
	  case AGV_MODE_RUNNING: AGV_Running();
	                         break;
	  case AGV_MODE_OP:      AGV_Operation();
							 break;
	  case AGV_MODE_SUSPENDED:AGV_SuspendRun();
	                          break;
      default: break;
   }
}

/*-----------------------------
   ������������
-----------------------------*/
void AGV_Standby(void)
{
	if(1)
	{
		nAgvWorkMode = AGV_MODE_RUNNING;

		wCurrent = 0.0f;
		fCurVel = 0.0f;
		fCurVelL = 0.0f;
		fCurVelR = 0.0f;
		fTargetVel = 0.0f;
		twoD_Progress = 0.0f;
		RealTwoDProgress = 0.0f;
	}
}

uint8_t dflag[3] = {0};

void AGV_Running(void)
{
	float32 fVelInc, fVelDec;
	float32  wCurInc, wCurDec;
  static  float s_max_vel = 0;
  float     fActVel;  // ��ʵ���ٶ�
  float     wActVel;  // ��ʵ���ٶ�
  //uint32_t  difftm;
  //uint32_t  tm;
  uint8_t   data[16];
  float     l;
  int32 deltaVL = 0;
  int32 deltaVR = 0;  //50ms�� �������̵ļ���ֵ��

#if 0
  static uint32_t old_tm = 0;
  tm = OSTimeGet();
  difftm = TIME_DIFFERENCE(tm, old_tm);
  old_tm = tm;

  //AGCRUN_CYCLE = difftm;
#endif
  //APP_TRACE("AGV run. difftm = %d\r\n", difftm);

	fVelInc = (AGCRUN_CYCLE/1000.0f) * fAccUp;              // AGCRUN_CYCLE���ڼ��ٲ���
	fVelDec = (AGCRUN_CYCLE/1000.0f) * fAccDn;               // AGCRUN_CYCLE���ڼ��ٲ���

	wCurInc = (AGCRUN_CYCLE/1000.0f) * wAccUp;
	wCurDec = (AGCRUN_CYCLE/1000.0f) * wAccUp;

  DISABLE_INT();
  deltaVL = get_count_and_clear(encoder_l);
  deltaVR = get_count_and_clear(encoder_r);
  ENABLE_INT();

  if( (Motionstyle == ACTION_MODE_GOAHEAD)
    || (Motionstyle == ACTION_MODE_GOBACK)
    || (Motionstyle == ACTION_MODE_TURNLEFT)
    || (Motionstyle == ACTION_MODE_TURNRIGHT) )
    APP_TRACE("encoder: left=%d, right=%d\r\n", deltaVL, deltaVR);

  //Check encoder error
  if(Motionstyle != ACTION_MODE_STOP)
  {
    int expect_cnt_l, expect_cnt_r;
    static int  encoder_errcnt_l=0, encoder_errcnt_r=0;

    expect_cnt_l = LENGTH_TO_ENCODERCNT(fCurVelL)*AGCRUN_CYCLE/1000;
    expect_cnt_r = LENGTH_TO_ENCODERCNT(fCurVelR)*AGCRUN_CYCLE/1000;

    if( (abs(abs(expect_cnt_l) - abs(deltaVL)) > 30)
        && ((abs(expect_cnt_l) > 2*abs(deltaVL))||((abs(deltaVL) > 2*abs(expect_cnt_l)))) )
    {
      ++encoder_errcnt_l;

      APP_TRACE("left wrong encoder cnt %d, %d is expected. err cnt = %d\r\n",
                deltaVL, expect_cnt_l, encoder_errcnt_l);

      deltaVL = expect_cnt_l;
    }
    else if(encoder_errcnt_l > 0)
      --encoder_errcnt_l;

    if( ((abs(abs(expect_cnt_r) - abs(deltaVR))) > 30)
        && ((abs(expect_cnt_r) > 2*abs(deltaVR))||((abs(deltaVR) > 2*abs(expect_cnt_r)))) )
    {
      ++encoder_errcnt_r;

      APP_TRACE("right wrong encoder cnt %d, %d is expected. err cnt = %d\r\n",
                deltaVR, expect_cnt_r, encoder_errcnt_r);

      deltaVR = expect_cnt_r;
    }
    else if(encoder_errcnt_r > 0)
      --encoder_errcnt_r;

    if(encoder_errcnt_l > 5)
    {
        APP_TRACE("left encoder error.\r\n");
        error_event(ENCODER_L_ERROR);
    }
    if(encoder_errcnt_r > 5)
    {
        APP_TRACE("right encoder error.\r\n");
        error_event(ENCODER_R_ERROR);
    }
  }

	if(NO_EVENT == nEvent)
  {
    if(ethCommBreakFlag == 0x1)   //�ɻָ�ͣ����־λ
		{
			nEvent = CONTROL_RESUMABLE_STOP_EVENT;
		}
		else if (OutOfTrack == 0x01)                              // ��鵼���ѹ��¼�
    {
      APP_TRACE("Error event: out of track\r\n");
			nEvent = OUT_OF_TRACK_EVENT;
      error_event(nEvent);
    }
		else if (GSMissing == 0x01)                    // ���CAN�����źŶ��¼�, check �˴��Ƿ���Ҫ�޸�
    {
      APP_TRACE("Error event: navi out of time\r\n");
			nEvent = GS_COMM_BREAK_EVENT;
      error_event(nEvent);
    }
		else if(ServoLeftInvaild())
		{
      APP_TRACE("Error event: sevo invalid\r\n");
			nEvent = SERVOL_INVALID_EVENT;
      //error_event(nEvent);
		}
		else if(ServoRightInvalid())
		{
      APP_TRACE("Error event: sevo invalid\r\n");
			nEvent = SERVOR_INVALID_EVENT;
      //error_event(nEvent);
		}

		else
		{
/*------------------------------------------ֱ��--------------------------------------------------*/
			if( (Motionstyle == ACTION_MODE_GOAHEAD) || (Motionstyle == ACTION_MODE_GOBACK) )
			{

        /* use encoder*/
#if 0
				RealTwoDProgress += (((abs(fCurVelL) + abs(fCurVelR))* 0.5f)* (AGCRUN_CYCLE/1000.0f));
				sRemain = sRemain - (((abs(fCurVelL) + abs(fCurVelR))* 0.5f)* (AGCRUN_CYCLE/1000.0f));
#else

        l = ENCODERCNT_TO_LENGTH((abs(deltaVL)+abs(deltaVR))*0.5f);

        RealTwoDProgress += l;
        sRemain -= l;
#endif

#if 0
        APP_TRACE("AGV go straight, iPath=%d, fCurVel = %f, remain=%f, fTargetVel=%f\r\n",
               iPath, fCurVel, sRemain,fTargetVel);
#endif
			// ����·������	
				UpdateAgvHeadDirToNew();
				Get_TwoDDev();

        if(Motionstyle == ACTION_MODE_GOBACK)
        {
            GS_DevFB = -GS_DevFB;
            GS_DEV = -GS_DEV;
        }

        /*TODO: tobe checked */
#if 1
        twoD_Progress += l;
#else
        twoD_Progress += fCurVel * AGCRUN_CYCLE/1000.0f;  //�������õ�λΪmm�� mm/s * ms ÿ��50ms�ż�һ�Σ�
#endif

        //fActVel = (ENCODERCNT_TO_LENGTH(abs(deltaVL+deltaVR)/2.0) * 1000)/AGCRUN_CYCLE;
        fActVel = l*1000.0/AGCRUN_CYCLE;

        if(fActVel > s_max_vel)
            s_max_vel = fActVel;

        float s_Open;

        s_Open = (s_max_vel*70.0)/1000.0;
        //if(s_Open < 40)
         //   s_Open = 40;

        APP_TRACE("ActVel = %.2f\r\n", fActVel);
        SmoothVelocityPlanning(sRemain-s_Open, fAccUp, fAccDn, fActVel, fOpenVel,
                               vPath, &fTargetVel);
#if 0
 				if ( sRemain < S_Vdown)
				{
					  if(fTargetVel > fOpenVel)   //��ԭ���Ĳ�����0��Ϊ�˴���fOpenVel
            {
              fTargetVel = fOpenVel ;
            }
				}
				else
				{
					  VPlan(sRemain,fAccUp,fAccDn,fCurVel,vPath);
				}
#else
#if 0
        if((sRemain < S_Vdown) & (sRemain >= (S_Vdown*0.67)) )   
        {
          if(dflag[0] == 0)
            fTargetVel = fCurVel * 0.9f; 

          fVelDec = fVelDec * 1.5;
          dflag[0] = 1;
        }
				else if((sRemain < (S_Vdown*0.67)) & (sRemain >= (S_Vdown*0.3)) )
        {
          if(dflag[1] == 0)
            fTargetVel = fCurVel * 0.6f;  

          fVelDec = fVelDec * 1.2;
          dflag[1] = 1;
        }
				else if((sRemain < (S_Vdown*0.3)) )
        {
          if(dflag[2] == 0)
            fTargetVel = fCurVel * 0.05f;  //original 0.05
          if(fTargetVel < 40)
            fTargetVel = 40.0;

          fVelDec = fVelDec * 1.2;
          dflag[2] = 1;
        }
				else if(sRemain >= S_Vdown)
        {
          VPlan((sRemain-0.0f),fAccUp,fAccDn,fCurVel,vPath); //original sRemain-120.0
          dflag[0]=0;
          dflag[1]=0;
          dflag[2]=0;
        }
#endif
#endif
				if(fCurVel < 0.01f)
        {
					  //pInitzq(GS_DEV * 0.001,GS_Angle * 0.01745);	
					  pInitzq(GS_DEV * 0.001, ANGLE_TO_RADIAN(GS_Angle));
				}
				else if( NewTag != 0x00 )
				{
            //APP_TRACE("%d: tag: %d\r\n", OSTimeGet(), twoD_TagCode);
            //APP_TRACE("DEV_FB = %f, DEV =%f, GS_Angle=%f\r\n", GS_DevFB, GS_DEV, GS_Angle);
            //APP_TRACE("GS_DevFB = %f\r\n", GS_DevFB);

            RealTwoDProgress = GS_DevFB;  //������ά��֮�侭����·��  Ϊ�������ۼ����
            twoD_Progress = GS_DevFB;  //������ά��֮�侭����·��  Ϊ�������ۼ����

						if( sRemain < 300.0f)  //Ŀ�ĵ�
						{
              //APP_TRACE("sRemain = %f\r\n", sRemain);
							sRemain = -GS_DevFB ;
							if(((absVar(sRemain)) < sStopDistance )||(GS_DevFB >0.0001f))
							{
								fTargetVel = 0.0f;
							}
						}
					  else				// ����վ
            {
              iPath -- ;  //�����������һ��·�εļ���
              sRemain = twoD_Distance * iPath - GS_DevFB;

              //pInitzq(GS_DEV * 0.001,GS_Angle * 0.01745);
              pInitzq(GS_DEV * 0.001, ANGLE_TO_RADIAN(GS_Angle));

              NewTag = 0x00;   //������Ϊ��������ά��֮����Ͻ�һ��
            }
				}
				else if (RealTwoDProgress > (twoD_Distance + twoD_Distance/2.0))
				{
					twoD_TagCodeOld = 0x0;  //����ֵ���㣬����ǿУ�飬�����Ƶ���ά���������ѹ�
          APP_TRACE("%d: OutOfTrack \r\n", OSTimeGet());
          OutOfTrack = 0x01 ;  //ͨ����ť���� ���� ��
				}
				else if (twoD_Progress > (twoD_Distance * 3.0f))  //�˴�Ӧ���ü���Sopen����
				{
            //APP_TRACE("Motors: power off");
            motors_power_onoff(0);
				}
#if 0
				if (fCurVel < fTargetVel)
				{
          FApproach(&fCurVel, fTargetVel, fVelInc); // ֱ�Ӳ���·����������ٶ�	
				}
				else
					FApproach(&fCurVel, fTargetVel, fVelDec); // 
#else
				fCurVel = fTargetVel;
#endif

				if(fCurVel < 0.01f)
				{
            Drive(0x0,0x0);
            fCurVelL = 0;
            fCurVelR = 0;

            NewTag = 0x00;
            iPath = 0x00;

            if(Motionstyle == ACTION_MODE_GOAHEAD)
              data[0] = ACTION_GOSTRAIGHT;
            else
              data[0] = ACTION_GOBACKWARD;

            motionctrl_event_callback(MOTIONCTRL_EVENT_ACTION_OVER, 1, data);

            s_max_vel = 0;
            RealTwoDProgress = 0;
            clear_encoder(encoder_l);
            clear_encoder(encoder_r);

            if(Motionstyle == ACTION_MODE_GOBACK)
              GlideReset();

            Motionstyle = ACTION_MODE_STOP ;
        }
        else
        {
          // When velocity is very low ��ʧ ���봫����ͨ�Ŷ�, don't follow guide tape��
          if (nEvent == OUT_OF_TRACK_EVENT || nEvent == GS_COMM_BREAK_EVENT)
          {
            fDiffVel = 0.0f;
          }
          else
          {
            if(Motionstyle == ACTION_MODE_GOBACK)
              fDiffVel = CalcPzq(deltaVR, deltaVL);	  
            else
              fDiffVel = CalcPzq(deltaVL, deltaVR);	  
          }

          if(fCurVel <= fOpenVel+0.01)
              fDiffVel = 0;

          if(fDiffVel != 0)
          {
            //APP_TRACE("fDiffVel = %f\r\n", fDiffVel);
          }

          VControl(fCurVel, fDiffVel);	// ����������ٶȻ��Ŀ�����

          if(Motionstyle == ACTION_MODE_GOBACK)
              Drive(fCurVelR, fCurVelL);
          else
              Drive(fCurVelL, fCurVelR);
        }
			}
/*---------------------------------------------��ת-----------------------------------------------*/
			else if(Motionstyle == ACTION_MODE_TURNLEFT)
			{
        if(wCurrent>1.0f)
        {
          ++twoDMissingCnt;
          if(twoDMissingCnt >100)
          {
            GSMissing = 0x01;
            twoD_TagCodeOld = 0x0;
          }
        }
				UpdateAgvHeadDirToNew();
				Get_TwoDDev();

        /*TODO:  use encoder*/
#if 0
				angleProcess +=  wCurrent * AGCRUN_CYCLE/1000.0f;  //�������õ�λΪ ��
#else
        angleProcess += LEGHTH_TO_ANGLE(ENCODERCNT_TO_LENGTH((abs(deltaVL)+abs(deltaVR))*0.5f));
#endif
				angleRemain = (minusL(AGV_Head_Dir , AGV_Dir_Dest))* 90.0f - GS_Angle;
				if((turnWholeCycleCnt == 0x1)&&(AGV_Head_Dir == AGV_Dir_Dest))
				{
				   angleRemain += 360.0f;
				}
				else
				{
					turnWholeCycleCnt = 0x0;
				}
                /*
				if(angleRemain < angleVDown)
				{
					if(wTarget > 5.0f)
					{
						wTarget = 5.0f ;
					}
				}
                */

				if( angleRemain < angleStopDistance )
				{
					wCurrent = wOpenVel;
					//angleProcess = 0;
				}
				
				else
				{
					//CyclePlan(angleRemain,wAccUp,wAccDn,wCurrent,wCycle);

                    wActVel = (ENCODERCNT_TO_LENGTH(deltaVR-deltaVL)/deltaT)/(AXLE_LENGTH);
					float wMax = 360; // the max angle speed, (degree per second)	
					SmoothVelocityPlanning(angleRemain - angleStopDistance, wAccUp, wAccDn, wActVel, wOpenVel,
                               wMax, &wCurrent);
				}
                /*
				if( angleRemain < angleStopDistance )
				{
					wCurrent = 0.0f ;
					angleProcess = 0;
				}
				*/

                /*
				if ( wCurrent <  wTarget )
					FApproach(&wCurrent, wTarget, wCurInc); // ֱ�Ӳ���·����������ٶ�
				else
					FApproach(&wCurrent, wTarget, wCurDec); // ֱ�Ӳ���·����������ٶ�
				*/

				if(wCurrent < 0.01f)
				{
				    Motionstyle = ACTION_MODE_STOP ;

            fCurVelL = 0;
            fCurVelR = 0;

            Drive(0x0,0x0);
            GlideReset();

            angleRemain = 0.0f ;
            angleProcess = 0.0f;

            data[0] = ACTION_TRUNLEFT;
            motionctrl_event_callback(MOTIONCTRL_EVENT_ACTION_OVER, 1, data);
        }

				// ɾ��
				#if 0
			  if(((270.0f < twoDAngleOld ) && (twoDAngleOld< 361.0f))&&((0.001f< twoDAngleConver)&&(twoDAngleConver < 90.0f))&&(acrossXAxisFlag == 0))
			  {
			  	 acrossXAxisFlag = 1;
           twoDAngleNew = twoDAngleConver + 360.0f;
           twoDAngleOld = twoDAngleConver;  //Old ʼ��Ϊ�ɼ������ĽǶ���Ϣ
			  }
			  else if((acrossXAxisFlag == 1)&&((twoDAngleConver - twoDAngleOld) > 180.0f))
			  {
			  	 //��ʱ�����Ѿ�����һ�����ֶ����ص������ޣ������κδ���
			  }
			  else if(acrossXAxisFlag == 1)
			  {
			  	 //��ʱ���ڿ�X����ĺ�����һֱ��Ҫ��������
			  	 twoDAngleNew = twoDAngleConver + 360.0f;
				 twoDAngleOld = twoDAngleConver;
			  }
			  else
			  {
			  	 //��ʱδ����
			  	 twoDAngleNew = twoDAngleConver;
           twoDAngleOld = twoDAngleConver;
			  }
#endif
			if(GSMissing == 0x01){
				
				POSE newPose= MotionEstimate(deltaVL, deltaVR);
				twoD_XP = newPose.x;
				twoD_YP = newPose.y;
				twoDAngleNew = newPose.theta;
		      
			}else{
                twoDAngleNew=ExtendAngleExecute(twoDAngleConver);
				MotionEstimateSet(twoD_XP, twoD_YP, twoDAngleNew);
			}
				
			  rotateR =  CalcRotateP(twoD_XP, twoD_YP, twoDAngleNew);
#if 0
        APP_TRACE("X=%f, Y=%f, anglenew=%f\r\n", twoD_XP, twoD_YP, twoDAngleNew);
        if(rotateR != 0)
          APP_TRACE("rotateR =%f \r\n", rotateR);
#endif
			  WControl(wCurrent, rotateR);	// ����������ٶȻ��Ŀ�����
        Drive(fCurVelL, fCurVelR);
			}
/*-------------------------------------------��ת------------------------------------------------*/
			else if(Motionstyle == MOTIONSTATE_TRUNRIGHT)
      {
			   if(wCurrent>1.0f)
			   {
				   ++twoDMissingCnt;
				   if(twoDMissingCnt >100)
				   {
				   		GSMissing = 0x01;
              twoD_TagCodeOld = 0x0;
				   }
			   }
			   UpdateAgvHeadDirToNew();
			   Get_TwoDDev();

        /*TODO:  use encoder*/
#if 0
			   angleProcess -=  wCurrent * AGCRUN_CYCLE/1000.0f;  //�������õ�λΪ ��
#else
         angleProcess -= LEGHTH_TO_ANGLE(ENCODERCNT_TO_LENGTH((abs(deltaVL)+abs(deltaVR))*0.5f));
#endif
			   angleRemain = (minusR(AGV_Head_Dir , AGV_Dir_Dest))* 90.0f + GS_Angle;
			   if((turnWholeCycleCnt == 0x1)&&(AGV_Head_Dir == AGV_Dir_Dest))
			   {
			   		angleRemain += 360.0f;
			   }
			   else
			   {
				    turnWholeCycleCnt = 0x0;
			   }

			   if( angleRemain < angleVDown )
			   {
					if(wTarget > 5.0f)
					{
						wTarget = 5.0f;
					}
			   }
			   else
			   {
			   		CyclePlan(angleRemain,wAccUp,wAccDn,wCurrent,wCycle);
			   }

			   if(angleRemain < angleStopDistance)
			   {
			   		wTarget = 0.0f;
					angleProcess = 0.0f;
			   }

				if ( wCurrent <  wTarget )
					FApproach(&wCurrent, wTarget, wCurDec); // ֱӲ���·����������ٶ?
				else
					FApproach(&wCurrent, wTarget, wCurInc); // ֱ�Ӳ���·����������ٶ�

				if(absVar(wCurrent) < 0.01f)
				{
            Motionstyle = ACTION_MODE_STOP ;

            fCurVelL = 0;
            fCurVelR = 0;

            Drive(0x0,0x0);

            GlideReset();
            angleRemain = 0.0f ;
            angleProcess = 0.0f;

            data[0] = ACTION_TRUNRIGHT;
            motionctrl_event_callback(MOTIONCTRL_EVENT_ACTION_OVER, 1, data);
				}

				// ɾ��
				#if 0
				if(((0.001f < twoDAngleOld)&&( twoDAngleOld< 90.0f))&&((270.0f< twoDAngleConver)&&(twoDAngleConver < 361.0f))&&(acrossXAxisFlag == 0))
				{
				   acrossXAxisFlag = 1;
				   twoDAngleNew = twoDAngleConver - 360.0f;
				   twoDAngleOld = twoDAngleConver;
				}
				else if((acrossXAxisFlag == 1)&&((twoDAngleOld-twoDAngleConver) > 180.0f))
				{
					//��ʱΪ���˳��Ѿ��ɵ�һ���޵��������ޣ��ֶ����ص�һ���޵����
				}
				else if(acrossXAxisFlag == 1)
				{
				   twoDAngleNew = twoDAngleConver - 360.0f;
				   twoDAngleOld = twoDAngleConver;
				}
				else
        {
          twoDAngleNew = twoDAngleConver;
          twoDAngleOld = twoDAngleConver;
        }

				#endif
				
				
				
				twoDAngleNew=ExtendAngleExecute(twoDAngleConver);
				
				rotateR =  CalcRotateP(twoD_XP, twoD_YP, twoDAngleNew);
#if 0
        APP_TRACE("X=%f, Y=%f, anglenew=%f\r\n", twoD_XP, twoD_YP, twoDAngleNew);
        if(rotateR != 0)
          APP_TRACE("rotateR =%f \r\n", rotateR);
#endif
				WControl(wCurrent, rotateR);	// ����������ٶȻ��Ŀ����� 

        Drive(fCurVelL, fCurVelR);
/*------------------------------------------����------------------------------------------------*/
		   }
	  }
  }
	if(NO_EVENT != nEvent)
	{
		if((nEvent == SERVOL_INVALID_EVENT)||(nEvent == SERVOR_INVALID_EVENT))
		{
			sRemain = 0.0f;
			angleRemain = 0.0f;
      fCurVelL = 0;
      fCurVelR = 0;
			Motionstyle = ACTION_MODE_STOP;
			nAgvWorkMode = AGV_MODE_SUSPENDED;
		}

		if((nEvent == GS_COMM_BREAK_EVENT )&&((Motionstyle == ACTION_MODE_TURNLEFT)||(Motionstyle == ACTION_MODE_TURNRIGHT)))
		{
			angleRemain = 0.0f;
      fCurVelL = 0;
      fCurVelR = 0;
			nAgvWorkMode = AGV_MODE_SUSPENDED;
			Motionstyle = ACTION_MODE_STOP ;
		}

    if( (Motionstyle == ACTION_MODE_GOAHEAD) || (Motionstyle == ACTION_MODE_GOBACK) )
		{
			UpdateAgvHeadDirToNew();
			Get_TwoDDev();
#if 0
			twoD_Progress += fCurVel * AGCRUN_CYCLE/1000.0f;
#else
      l = ENCODERCNT_TO_LENGTH((abs(deltaVL)+abs(deltaVR))*0.5f);
      twoD_Progress += l;
#endif

			if( NewTag != 0x00 )
			{
				RealTwoDProgress = GS_DevFB;  //������ά��֮�侭����·��  Ϊ�������ۼ����
				twoD_Progress = GS_DevFB;  //������ά��֮�侭����·��  Ϊ�������ۼ����

				if( sRemain < 300.0f )  //Ŀ�ĵ�
				{
					  sRemain = GS_DevFB ;				
				}
				else				// ����վ  
				{
					  iPath--;
					  sRemain = twoD_Distance * iPath - GS_DevFB;

				    //pInitzq(GS_DEV * 0.001,GS_Angle * 0.01745);
					  pInitzq(GS_DEV * 0.001, ANGLE_TO_RADIAN(GS_Angle));

					NewTag = 0x00;   //������Ϊ��������ά��֮����Ͻ�һ��
				}
			}

			if (FApproach(&fCurVel, 0.0f, fVelDec))
			{
				if(nEvent == OUT_OF_TRACK_EVENT )//�ѹ�����Ϊ�������ߣ��������ֵ
				{
					twoD_Progress = 0.0f;
					RealTwoDProgress = 0.0f;
					sRemain = 0.0f;	
					iPath = 0x00;
          fCurVelL = 0;
          fCurVelR = 0;

          /*�ȼ�����ֹͣ*/
					//Motionstyle = ACTION_MODE_STOP ;
				}
				else if (nEvent == CONTROL_RESUMABLE_STOP_EVENT)
				{
					nAgvWorkMode = AGV_MODE_SUSPENDED;
				}
			}

      /*modified by SunLiang   2017-04-12 Start*/
      if(fCurVel < 0.01f)
      {
          fCurVelL = 0;
          fCurVelR = 0;
					Drive(0x0,0x0);
          Motionstyle = ACTION_MODE_STOP ;
      }
      else
      {
        fDiffVel = CalcPzq(deltaVL, deltaVR);
        VControl(fCurVel, fDiffVel);	// ����������ٶȻ��Ŀ�����

        Drive(fCurVelL, fCurVelR);
      }
      /*modified by SunLiang   2017-04-12 End*/

			if(CONTROL_RESUMABLE_STOP_EVENT == nEvent)
			{
				if(ethCommBreakFlag == 0)
				{
					nAgvWorkMode = AGV_MODE_RUNNING;
					nEvent = NO_EVENT;
				}
			}
		}
		else if( Motionstyle == ACTION_MODE_STOP )
		{
			nAgvWorkMode = AGV_MODE_SUSPENDED;    // һ�������ת����SUSPENDEDģʽ
		}
	}
}

void AGV_SuspendRun(void)
{
	//�˴��ϵ�PWM�������ѡ��رն�ʱ����
//	EvaRegs.T1CON.bit.TENABLE	= 0;  //��ֹ��ʱ��
//	EvbRegs.T3CON.bit.TENABLE	= 0;

  /*TODO: Ӧ��*/
	Drive(0x0,0x0);
  motors_power_onoff(0);
	//DO_SERVO_POWERUP = 0x01;	// not power up	

	switch(nEvent)
	{

		case OUT_OF_TRACK_EVENT:
		{
			if (OutOfTrack == 0x00)
			{
        APP_TRACE("Error: OutOfTrack \r\n");

				nAgvWorkMode = AGV_MODE_RUNNING;
				fCurVel = 0.0f;
				fDiffVel = 0.0f;
				wCurrent = 0;
				iPath = 0;
				NewTag = 0x00;
				fCurVelL = 0;
				fCurVelR = 0;
				nEvent = NO_EVENT;
        motors_power_onoff(1);
			//	DO_SERVO_POWERUP = 0x00;  //������·�ϵ�
//				EvaRegs.T1CON.bit.TENABLE	= 1;  //ʹ�ܶ�ʱ��
			//	EvbRegs.T3CON.bit.TENABLE	= 1;
#if 0
				actionFinishReply =  (((Uint32)actionFinishSeq)<<24);
				actionFinishReply |= 0x00010100;
				actionFinishSeq ++;
		//		CAN_SendPacket(13,actionFinishReply,ACK_OK);
#endif
			}
			break;
		}
		case GS_COMM_BREAK_EVENT:
		{
			if (GSMissing  == 0x00)
			{
				nAgvWorkMode = AGV_MODE_RUNNING;
				fCurVel = 0.0f;
				fDiffVel = 0.0f;
				wCurrent = 0;
				iPath = 0;
				NewTag = 0x00;
				fCurVelL = 0;
				fCurVelR = 0;
				nEvent = NO_EVENT;
		//		DO_SERVO_POWERUP = 0x00;  //������·�ϵ�
        motors_power_onoff(1);
				twoDMissingCnt = 0x0;
		//		EvaRegs.T1CON.bit.TENABLE	= 1;  //ʹ�ܶ�ʱ��
		//		EvbRegs.T3CON.bit.TENABLE	= 1;
#if 0
				actionFinishReply =  (((Uint32)actionFinishSeq)<<24);
				actionFinishReply |= 0x00010200;
				actionFinishSeq ++;
		//		CAN_SendPacket(13,actionFinishReply,ACK_OK);
#endif
			}
			break;
		}
		case CONTROL_RESUMABLE_STOP_EVENT:
		{
			if (ethCommBreakFlag==0x00)
			{
				nAgvWorkMode = AGV_MODE_RUNNING;
				fCurVel = 0.0f;
				fDiffVel = 0.0f;
				wCurrent = 0;
				fCurVelL = 0;
				fCurVelR = 0;
				nEvent = NO_EVENT;
        motors_power_onoff(1);
		//		DO_SERVO_POWERUP = 0x00;  //������·�ϵ�
		//		EvaRegs.T1CON.bit.TENABLE	= 1;  //ʹ�ܶ�ʱ��
		//		EvbRegs.T3CON.bit.TENABLE	= 1;
			}
			break;
		}
		case SERVOL_INVALID_EVENT:
		{
			if(ServoLeftInvaild() == 0x00)
			{
				nAgvWorkMode = AGV_MODE_RUNNING;
				fCurVel = 0.0f;
				fDiffVel = 0.0f;
				wCurrent = 0;
				iPath = 0;
				fCurVelL = 0;
				fCurVelR = 0;
				nEvent = NO_EVENT;
        motors_power_onoff(1);
			//	DO_SERVO_POWERUP = 0x00;  //������·�ϵ�
			//	EvaRegs.T1CON.bit.TENABLE	= 1;  //ʹ�ܶ�ʱ��
		//		EvbRegs.T3CON.bit.TENABLE	= 1;
			}
			break;
		}
		case SERVOR_INVALID_EVENT:
		{
			if(ServoRightInvalid() == 0x00)
			{
				nAgvWorkMode = AGV_MODE_RUNNING;
				fCurVel = 0.0f;
				fDiffVel = 0.0f;
				wCurrent = 0;
				iPath = 0;
				fCurVelL = 0;
				fCurVelR = 0;
				nEvent = NO_EVENT;
        motors_power_onoff(1);
			}
			break;
		}
		default:break;
	}
}

void AGV_Operation(void)
{

}

void reverseTask(void)
{
  static uint32_t holdtm = 0;
  uint32_t  tm_now;
  int       err;

  reverseMode = (uint16_t)dumper_get_state();

 	switch(reverseMode)
	{
		case REVERSE_MODE_HORIZONTAL:
#if 0
        if(!is_dumper_homing())
        {
          APP_TRACE("Dumper not in place. Reset dumper.\r\n");
          err = dumper_down(DEFAULT_DUMPER_W/2);
        }
        else 
#endif
        if(0x1 == nReverseFlag )
        {
          dumper_up(DEFAULT_DUMPER_W);
          holdtm = 0;
          nReverseFlag = 0;
        }
        break;

    case REVERSE_MODE_TOP:
        if(holdtm == 0)
        {
            APP_TRACE("Dumper up ok. hold_tm =%d\r\n", holdtm);
            holdtm = OSTimeGet();
        }
        else
        {
            tm_now = OSTimeGet();
            if(TIME_DIFFERENCE(tm_now, holdtm) > DEFAULT_DUMPER_HOLDTIME_MS)
            {
                uint8_t data = ACTION_UNLOAD;

                holdtm = 0;
                err = dumper_down(DEFAULT_DUMPER_W);
                if(err == -PERR_EFAULT)
                  dumper_stop();

                motionctrl_event_callback(MOTIONCTRL_EVENT_ACTION_OVER, 1, &data);
            }
        }
			  break;

		case REVERSE_MODE_REVERSEUP :
        if(dumper_angle()>=DEFAULT_DUMPER_REVERSE_ANGLE)
            dumper_stop();

        break;

		case REVERSE_MODE_REVERSEDN:
        if(is_dumper_homing())
            dumper_stop();
        break;
		default:
        break;
  }
}

static void error_event(int error)
{
    uint8_t err = error;

    motionctrl_event_callback(MOTIONCTRL_EVENT_ERROR, 1, &err);
}
#if 0
static void error_recovery(int error)
{
    uint8_t err = error;

    motionctrl_event_callback(MOTIONCTRL_EVENT_ERROR_RECOVERY, 1, &err);
}
#endif
/*--------------------end line------------------------*/
