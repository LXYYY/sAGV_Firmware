#include <stdint.h>

#include "ucos_ii.h"

#include "common/PosixError.h"
#include "common/tools.h"
#include "Navigation.h"
#include "porting.h"
#include "bsp_can.h"
#include "user_function.h"
#include "can_dispatcher.h"
#include "app_cfg.h"
#include "motion_ctrl.h"
#include "agv.h"

#define SCANNER_CANID   0x08

//���ά�����ȫ�ֱ�������
float32  twoD_YP = 0;      //Y���ϵ�ƫ�ƣ���λΪmm
float32  twoD_XP = 0;      //X���ϵ�ƫ�ƣ���λΪmm
float32  twoD_Angle = 0;   //�Ƕ�ƫ�ƣ�Ϊ��ά��X���������ά��������˳ʱ��нǣ��ܷ�ΧΪ0~359
Uint32 twoD_TagCode = 0; //��ǩ����
Uint32 twoD_TagCodeOld = 0; //��һ�εı�ǩ����
Uint16 NewTag = 0 ;

static int canUser;

typedef struct
{
    float       x;
    float       y;
    float       angle;
    uint32_t    tag;

    uint16_t    warn;
}navi_data_t;

typedef struct
{
    int can_user;
    int state;

    navi_data_t data;
}navi_t;

navi_data_t navi_data;

static int CanMessageProcess(int usr, CanRxMsg* RxMessage);

int NavigationInit(void)
{
    canUser = can_dispatcher_register_user("navi", CanMessageProcess);

    return 0;
}

int NavigationOnOff(int on)
{
    CanTxMsg txmsg;

    txmsg.StdId = 0;
    txmsg.IDE = CAN_Id_Standard;
    txmsg.RTR = CAN_RTR_Data;

    txmsg.DLC = 2;
    if(on)
      txmsg.Data[0] = 0x01;
    else
      txmsg.Data[0] = 0x02;

    txmsg.Data[1] = SCANNER_CANID;

    //memset(&txmsg.Data[2], 0, 6);
    can_dispatcher_send_msg(canUser, &txmsg);

    return 0;
}

extern float32 detY0;
extern float32 detTheta0;

void Calibrate(void)
{
    UpdateAgvHeadDirToNew();
    Get_TwoDDev();

    detY0 = -GS_DEV;
    detTheta0 = -GS_Angle;
}

static int CanMessageProcess(int usr, CanRxMsg* RxMessage)
{
  int       ret = 0;
  int32_t  temp32=0;
  uint16_t  temp16=0;
  Uint16 twoD_TAG = 0;     //��ά���״̬��TAGλ�����㵽��ǩ��ʱ��λΪ1

  if(RxMessage->IDE != CAN_Id_Standard)
    return -PERR_ENOTSUP;

  switch(RxMessage->StdId)
  {
    //�յ���ά���TxPDO1
    //case 0x188:
    case 0x180|SCANNER_CANID:
        reverse_copy(&RxMessage->Data[0], &temp32, 4);
        navi_data.y = temp32 * 0.1f;

        if(temp32 != 0)
        {
          twoD_YP = navi_data.y;

          if(Motionstyle != MOTIONSTATE_ONSTOPING)
            APP_TRACE("%d: y = %f \r\n", OSTimeGet(), twoD_YP);
        }
        break;

    //�յ���ά���TxPDO2
    //case 0x288:
    case 0x280|SCANNER_CANID:
        reverse_copy(&RxMessage->Data[0], &temp32, 4);
        reverse_copy(&RxMessage->Data[4], &temp16, 2);
        navi_data.x = temp32*0.1f;
        navi_data.angle = temp16*0.1f;

        if( (temp32 !=0) || (temp16 != 0) )
        {
            twoD_XP = navi_data.x;
            twoD_Angle = navi_data.angle;
            twoDAngleConver = 360.0f-twoD_Angle;

            if(Motionstyle != MOTIONSTATE_ONSTOPING)
              APP_TRACE("%d: x = %f, angle=%f\r\n", OSTimeGet(), twoD_XP, twoD_Angle);
        }

        twoDMissingCnt = 0;

        break;

    //�յ���ά���TxPDO3
    //case 0x388:
    case 0x380|SCANNER_CANID:
        twoD_TAG = (RxMessage->Data[0]>>3)&0x01;
        if(twoD_TAG == 0x1)  // ?twoD_TAG
        {
            //APP_TRACE("new tag\r\n");
            reverse_copy(&RxMessage->Data[2], &navi_data.warn, 2);
            reverse_copy(&RxMessage->Data[4], &navi_data.tag, 4);
            /*
             * ����motion-ctrl���ִ��룬�Ե������ݵķ��ʱȽ���ɢ��
             * ͨ�����������ѽ��б������ʷ�����Щ����ʱ���ý�ֹ���ȵķ�ʽ�����ݽ��б�����
             *  ��������ǰ���ǣ�
             *  Motion-ctrl �̵߳����ȼ�һ��Ҫ����can-dispatcher�߳�(����ǰ��һ�γ��������е�������)
             * */
            OSSchedLock();

            twoD_YP = navi_data.y;
            twoD_XP = navi_data.x;
            twoD_Angle = navi_data.angle;
            twoDAngleConver = 360.0f-twoD_Angle;
            twoD_TagCode = navi_data.tag;

            OSSchedUnlock();

            if(twoD_TagCode != twoD_TagCodeOld)
            {
                UpdateAgvHeadDirToNew();

                uint8_t buf[8];

                HTONL_COPY(&twoD_TagCode, &buf[0]);
                HTONL_COPY(&AGV_Head_Dir , &buf[4]);
                /*  Notice:
                 *  �˴������ȱ��㣬����NewTag��
                 *  ������������̨�ȱ���ɣ��󱨵�����
                 */
                if(twoD_TagCodeOld != 0)
                    motionctrl_event_callback(MOTIONCTRL_EVENT_POINT_ARRIVAL, 8, buf);

                twoD_TagCodeOld = twoD_TagCode;
                NewTag = 0x01;

                APP_TRACE("%d: new tag: %d\r\n", OSTimeGet(), twoD_TagCode);
					  	  if(Motionstyle != MOTIONSTATE_ONSTOPING)
                    APP_TRACE("x = %f, y =%f, angle=%f\r\n", twoD_XP, twoD_YP, twoD_Angle);
            }
        }
        break;

    //case 0x488:
    case 0x480|SCANNER_CANID:
      break;

    ///case 0x588:
    case 0x580|SCANNER_CANID:
      break;

    default:
      ret = -PERR_ENOTSUP;
      break;
  }

	return ret;
}
