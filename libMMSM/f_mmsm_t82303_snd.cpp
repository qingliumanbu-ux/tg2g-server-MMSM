/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		MFJ
Version:    1.0
Date:		2024-01-18
Description:二切实绩接收 发送L4电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t82303_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString v_mat_no = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString cs_tc_no = "";//电文号
	EPEX epex;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm36("TMMSM36");

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("t82303");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 t82303 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		for (int i = 0; i < bcls_rec->Tables["t82303"].Rows.get_Count(); i++)
		{
			cs_tc_no = "T82303";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}

			tmmsm01.Reset();//将数据清空
			tmmsm01.MergeFrom(bcls_rec->Tables["t82303"].Rows[i]);//获取数据
			tmmsm01.Query();

			tmmsm36["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm36.Query("MAT_NO");//从33表获取部分数据


			if (epex.SetValue(0, tmmsm01) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		

			Log::Trace("", __FUNCTION__, "PROD_TIME[{0}]  ", tmmsm01["PROD_TIME"].ToString());

			if (epex.SetValue("DEAL_FLAG", 0, "1") < 0 //操作标记
				|| epex.SetValue( "SLAB_NO", 0, tmmsm01["MAT_NO"].ToString()) < 0  //板坯号
				|| epex.SetValue( "LG_ST", 0, tmmsm01["ST_NO"].ToString()) < 0     //钢种
				|| epex.SetValue( "HEAT_NO", 0, tmmsm01["HEAT_NO"].ToString()) < 0  //炉号
				|| epex.SetValue( "CUT_TIME", 0, tmmsm01["SLAB_CUT_TIME"].ToString()) < 0	//切割时间
				|| epex.SetValue( "VALUE_TYPE", 0, "2") < 0  //值类型
				|| epex.SetValue( "VALUE", 0, "水爆") < 0	 //值
				|| epex.SetValue("SEND_TIME", 0, dateNow) < 0	 //发送时间
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			


			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}
			epex.Uninitialize();
		}






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
