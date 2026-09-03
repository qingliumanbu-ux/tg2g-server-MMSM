/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给物流-采购退货装货确认
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21c007_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString dealFlag = " ";
	CString tableName = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	EPEX epex;

	/* 实体类定义 */

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tcNO = "21C007";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("DEAL_FLAG", 0, "A") < 0 ||
			epex.SetValue("WORK_DATE", 0, datetime) < 0 ||
			epex.SetValue("PLANT_CODE", 0, "S2N") < 0 ||
			epex.SetValue("PRODUCT", 0, "AT000385") < 0 ||
			epex.SetValue("PRODUCT_NUM", 0, "1000") < 0 
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		////21C008
		//if (epex.SetValue("DEAL_FLAG", 0, "A") < 0 ||
		//	epex.SetValue("WEIGH_NO", 0, "cs24013110141601") < 0 ||
		//	epex.SetValue("SAMPLE_NO", 0, "2401190957") < 0 ||
		//	epex.SetValue("MAT_PILE_NO", 0, "222-J71B2") < 0 
		//	)
		//{
		//	sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////21C009
		//if (epex.SetValue("DEAL_FLAG", 0, "A") < 0 ||
		//	epex.SetValue("WORK_DATE", 0, datetime) < 0 ||
		//	epex.SetValue("MAT_PROD_CODE", 0, "AT000385") < 0 ||
		//	epex.SetValue("MAT_PROD_CNAME", 0, "镍生铁-印尼（高镍）I级") < 0 ||
		//	epex.SetValue("STOCK_VALUE", 0, 1000) < 0||
		//	epex.SetValue("STOCK_XH_VALUE", 0, 1000) < 0
		//	)
		//{
		//	sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////21C010
		//if (epex.SetValue("DEAL_FLAG", 0, "A") < 0 ||
		//	epex.SetValue("WEIGH_NO", 0, "cs24013110141601") < 0 ||
		//	epex.SetValue("WORK_DATE", 0, datetime.SubstringNE(0, 8)) < 0 ||
		//	epex.SetValue("CAR_NO", 0, "晋A0001") < 0 ||
		//	epex.SetValue("MAT_CODE", 0, "1000") < 0||
		//	epex.SetValue("SRC_STOCK_CODE", 0, "AT000385") < 0 ||
		//	epex.SetValue("LOAD_CODE", 0, "6240") < 0 ||
		//	epex.SetValue("DST_STOCK_CODE", 0, "6240") < 0 ||
		//	epex.SetValue("UNLOAD_CODE", 0, "1000") < 0||
		//	epex.SetValue("WEIGH_APP_NO", 0, "JLCN2312140216") < 0 ||
		//	epex.SetValue("WEIGH_TIME", 0, datetime) < 0 ||
		//	epex.SetValue("TARE_WGT", 0, 11.1) < 0 ||
		//	epex.SetValue("GROSS_WGT", 0, 99.9) < 0||
		//	epex.SetValue("BALANCE_WGT", 0, 88.8) < 0 
		//	)
		//{
		//	sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//if (epex.SendTele() < 0)
		//{
		//	sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		// 释放
		epex.Uninitialize();

		/* ********* 程序处理结束 ********** */
		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
