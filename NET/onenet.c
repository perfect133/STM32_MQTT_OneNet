/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
//#include "usart.h"
#include "Delay.h"
#include "LED.h"

//C库
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
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
//	u8g2_DrawStr(&u8g2,3,10,"OneNet_DevLink\r\n"
//							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
//                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				u8g2_ClearBuffer(&u8g2);
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:u8g2_DrawStr(&u8g2,3,10,"Tips:Connection success\r\n");status = 0;break;
					
					case 1:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:1\r\n");break;//协议错误
					case 2:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:2\r\n");break;//非法的clientid
					case 3:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:3\r\n");break;//服务器失败
					case 4:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:4\r\n");break;//用户名或密码错误
					case 5:u8g2_DrawStr(&u8g2,3,10,"WARN:Connection failure:5\r\n");break;//非法链接(比如token非法)
					
					default:u8g2_DrawStr(&u8g2,3,10,"ERR:Connection failure:6\r\n");break;//未知错误
				}
				u8g2_SendBuffer(&u8g2); 
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
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
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
//	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
//	for(; i < topic_cnt; i++)
//	{
//		u8g2_DrawStr(&u8g2,3,10,"Subscribe Topic: " );
//	}
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
//	u8g2_DrawStr(&u8g2,3,10,"Publish Topic:  ,Msg: \r\n" );
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
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
	switch(type)							//暂时只使用到一种情况
	{
//		case MQTT_PKT_CMD:															//命令下发
//			
//			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
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
//				MQTT_DeleteBuffer(&mqttPacket);									//删包
//			
//			}
//		
//		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				json = cJSON_Parse(req_payload);
				if(!json)
				{
//					u8g2_ClearBuffer(&u8g2);
//					u8g2_DrawStr(&u8g2,3,10,"Error before:\r\n");			//测试显示
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
			
//		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
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
//		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
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
//		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
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
//		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
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
//		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
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
//		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
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
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
