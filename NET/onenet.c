/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
//#include "usart.h"
#include "Delay.h"
#include "LED.h"

//C��
#include <string.h>
#include <stdio.h>

#include "u8g2.h"
#include "cJSON.h"

#define PROID		"Perfect"

#define AUTH_INFO	"1438438"

#define DEVID		"LTyOLrVTP1"


extern unsigned char esp8266_buf[128];
extern u8g2_t u8g2;

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
//	u8g2_DrawStr(&u8g2,3,10,"OneNet_DevLink\r\n"
//							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
//                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				u8g2_ClearBuffer(&u8g2);
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:u8g2_DrawStr(&u8g2,3,10,"Tips:Connection success\r\n");status = 0;break;
					
					case 1:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:1\r\n");break;//Э�����
					case 2:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:2\r\n");break;//�Ƿ���clientid
					case 3:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:3\r\n");break;//������ʧ��
					case 4:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:4\r\n");break;//�û������������
					case 5:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:5\r\n");break;//�Ƿ�����(����token�Ƿ�)
					
					default:u8g2_DrawStr(&u8g2,3,10,"ERR:Connection failure:6\r\n");break;//δ֪����
				}
				u8g2_SendBuffer(&u8g2); 
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
	{
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2,3,10,"WARN:	MQTT_PacketConnect Failed\r\n");
		u8g2_SendBuffer(&u8g2); 
	}
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
//	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
//	for(; i < topic_cnt; i++)
//	{
//		u8g2_DrawStr(&u8g2,3,10,"Subscribe Topic: " );
//	}
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
//	u8g2_DrawStr(&u8g2,3,10,"Publish Topic:  ,Msg: \r\n" );
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json,*json_value;
	
	type = MQTT_UnPacketRecv(cmd);
//	u8g2_ClearBuffer(&u8g2);
	switch(type)							//��ʱֻʹ�õ�һ�����
	{
//		case MQTT_PKT_CMD:															//�����·�
//			
//			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
//			if(result == 0)
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,2,10,"cmdid: %s, req: %s, req_len: %d\r\n");
////				u8g2_SendBuffer(&u8g2);
//				json = cJSON_Parse(req_payload);
//				if(!json)
//				{
//					u8g2_ClearBuffer(&u8g2);
//					u8g2_DrawStr(&u8g2,3,10,"Error before:\r\n");
//					u8g2_SendBuffer(&u8g2);
//				}
//				else
//				{
////					json_value = cJSON_GetObjectItem(json,"LED_SW");
////					if(json_value->valueint)
////					{
////						 LED1_ON();
////					}
////					else
////					{
////						LED1_OFF();
////					}
//				}
//				MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
//			
//			}
//		
//		break;
			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				json = cJSON_Parse(req_payload);
				if(!json)
				{
//					u8g2_ClearBuffer(&u8g2);
//					u8g2_DrawStr(&u8g2,3,10,"Error before:\r\n");			//������ʾ
//					u8g2_SendBuffer(&u8g2);
				}
				else
				{
					json_value = cJSON_GetObjectItem(json,"TooL_SW");
					u8 SW_Flag = json_value->valueint;
					if(SW_Flag == 10)
					{
						LED2_OFF();
					}
					else if(SW_Flag == 11)
					{
						LED2_ON();
					}
					else if(SW_Flag == 20)
					{
						LED1_OFF();
					}
					else if(SW_Flag == 21)
					{
						LED1_ON();
					}
				}
				cJSON_Delete(json);
				MQTT_DeleteBuffer(&mqttPacket);	
			}
		
		break;
			
//		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
//		
//			if(MQTT_UnPacketPublishAck(cmd) == 0)
//			{
//				u8g2_ClearBuffer(&u8g2);
//				u8g2_DrawStr(&u8g2,3,10,"Tips:	MQTT Publish Send OK\r\n");
//				u8g2_SendBuffer(&u8g2);
//			}
//			
//		break;
//			
//		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
//		
//			if(MQTT_UnPacketPublishRec(cmd) == 0)
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,3,10,"Tips:	Rev PublishRec\r\n");
////				u8g2_SendBuffer(&u8g2); 
//				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
//				{
////					u8g2_ClearBuffer(&u8g2);
////					u8g2_DrawStr(&u8g2,3,10,"Tips:	Send PublishRel\r\n");
////					u8g2_SendBuffer(&u8g2);
//					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
//					MQTT_DeleteBuffer(&mqttPacket);
//				}
//			}
//		
//		break;
//			
//		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
//			
//			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,3,10,"Tips:	Rev PublishRel\r\n");
////				u8g2_SendBuffer(&u8g2);
//				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
//				{
////					u8g2_ClearBuffer(&u8g2);
////					u8g2_DrawStr(&u8g2,3,10,"Tips:	Send PublishComp\r\n");
////					u8g2_SendBuffer(&u8g2);
//					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
//					MQTT_DeleteBuffer(&mqttPacket);
//				}
//			}
//		
//		break;
//		
//		case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
//		
//			if(MQTT_UnPacketPublishComp(cmd) == 0)
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,3,10,"Tips:	Rev PublishComp\r\n");
////				u8g2_SendBuffer(&u8g2);
//			}
//		
//		break;
//			
//		case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack
//		
//			if(MQTT_UnPacketSubscribe(cmd) == 0)
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,3,10,"Tips:	MQTT Subscribe OK\r\n");
////				u8g2_SendBuffer(&u8g2);
//			}
//			else
//			{
////				u8g2_ClearBuffer(&u8g2);
////				u8g2_DrawStr(&u8g2,3,10,"Tips:	MQTT Subscribe Err\r\n");
////				u8g2_SendBuffer(&u8g2);
//			}
//		
//		break;
//			
//		case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack
//		
//			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
//			{
//				u8g2_ClearBuffer(&u8g2);
//				u8g2_DrawStr(&u8g2,3,10,"Tips:	MQTT UnSubscribe OK\r\n");
//				u8g2_SendBuffer(&u8g2);
//			}
//			else
//			{
//				u8g2_ClearBuffer(&u8g2);
//				u8g2_DrawStr(&u8g2,3,10,"Tips:	MQTT UnSubscribe Err\r\n");
//				u8g2_SendBuffer(&u8g2);
//			}
//		
//		break;
		
		default:
			result = -1;
		break;
	}
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
