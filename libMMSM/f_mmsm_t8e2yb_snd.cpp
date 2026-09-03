/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:
一给原料L2-原料成分
T8E2YB
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t8e2yb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int i = 0;
	int seq = 0;
	int blkNum = 0;
	int flag = 0;
	/* 业务变量 */
	CString tcNO = " ";
	CString action = " ";
	CString stationNo = " ";
	CString dealFlag = " ";
	CString tableName = " ";
	CString tableNameChild = " ";
	CString bunkerNo = " ";
	CString seqNo = " ";
	CString matCode = " ";
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm85("TMMSM85");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		blkNum = bcls_ret->Tables.IndexOf("QM_ELE");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("QM_ELE");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELEMENT"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_STRING, "ELEMENT");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELM_VALUE"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_DECIMAL, "ELM_VALUE");
		}
		
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
			action = bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
			stationNo = bcls_rec->Tables["MMLCSND"].Rows[0]["STATION_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
			tmmsm81al["QUALITY_BATCH_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
			tmmsm81al["MAT_CODE"] = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();
		//料蓝发X,还要给各设备发一份，料仓发具体工序
		if (stationNo.GetLength() == 1)
		{
			stationNo = stationNo + "X";
			flag = 1;
		}
		else if (stationNo.GetLength()>2)
		{
			//代表测试时 把地下料仓作为高位料仓使用，或者料蓝上料
			stationNo = " ";
		}
		if ("" == tmmsm81al["MAT_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "传入物料代码为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		Log::Trace("", __FUNCTION__, "===tcNO= [{0}]", tcNO);
		Log::Trace("", __FUNCTION__, "===QUALITY_BATCH_NO= [{0}]", tmmsm81al["QUALITY_BATCH_NO"].ToString());
		sqlstr = "  SELECT ELM_NAME ,ELM_VALUE FROM  TMMSM81AL    WHERE  QUALITY_BATCH_NO	= @tmmsm81.QUALITY_BATCH_NO";
		cmd_inq.Parameters.Set("tmmsm81.QUALITY_BATCH_NO", tmmsm81al["QUALITY_BATCH_NO"]);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		bcls_ret->Tables["QM_ELE"].Rows.Clear();
		while (cmd_inq.Read())
		{
			bcls_ret->Tables["QM_ELE"].Rows.Add();
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ELEMENT"] = cmd_inq.GetString(1);
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_VALUE"] = cmd_inq.GetDecimal(2);
			Log::Trace("", __FUNCTION__, "===ELEMENT= [{0}]", bcls_ret->Tables["QM_ELE"].Rows[seq]["ELEMENT"].ToString());
			seq++;
			
		}
		cmd_inq.Close();


		/* 查询主数据 */
		tmmsm50["MAT_CODE"] = tmmsm81al["MAT_CODE"];
		tmmsm50.Query("MAT_CODE");

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (action == "I")
		{
			action = "U";
		}

		if (epex.SetValue("INT_MAT_ANA", "ACTION", 0, action)<0 ||
			epex.SetValue("INT_MAT_ANA", "AGGREGATE_NAME", 0, stationNo) < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_NAME", 0, tmmsm50["MAT_SIMPLE_ENAME"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_CODE", 0, tmmsm50["MAT_CODE_L2"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "BATCH_NUMBER", 0, tmmsm50["LOT_NO"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_CLASS", 0, tmmsm50["BACK_C1"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "QUALITY_BATCH", 0, tmmsm81al["QUALITY_BATCH_NO"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_DESCRIPTION", 0, tmmsm50["MAT_NAME"].ToString()) < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_AOD", 0, " ") < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_BOF", 0, " ") < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_EAF", 0, " ") < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_LF", 0, " ") < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_RH", 0, " ") < 0 ||
			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_VOD", 0, " ") < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_ret->Tables["QM_ELE"].Rows.get_Count(); i++)
		{

			if (epex.SetValue("INT_MAT_ANA_DET", "ELEMENT", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELEMENT"].ToString())<0 ||
				epex.SetValue("INT_MAT_ANA_DET", "VALUE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal()) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		
		if (epex.SendTele() < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 释放
		epex.Uninitialize();

		//20240316 用户反馈 工序是0的是发送X 仅发送一条
		//if (flag)
		//{
		//	sqlstr = " SELECT DEV_CODE FROM TPSSMD1 WHERE STATION_ID= @STATION_ID";
		//	cmd_inq.Parameters.Set("STATION_ID", stationNo.SubstringNE(0,1));
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.ExecuteReader();
		//	while (cmd_inq.Read())
		//	{
		//		stationNo = cmd_inq.GetString(1);
		//		if (epex.Initialize(tcNO) < 0)
		//		{
		//			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}
		//		if (epex.SetValue("INT_MAT_ANA", "ACTION", 0, "U")<0 ||
		//			epex.SetValue("INT_MAT_ANA", "AGGREGATE_NAME", 0, stationNo) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_NAME", 0, tmmsm50["MAT_SIMPLE_ENAME"].ToString()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_CODE", 0, tmmsm50["MAT_CODE_L2"].ToString()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "BATCH_NUMBER", 0, tmmsm50["LOT_NO"].ToString()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_CLASS", 0, tmmsm50["BACK_C1"].ToString()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "QUALITY_BATCH", 0, tmmsm81al["QUALITY_BATCH_NO"].ToDecimal()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_DESCRIPTION", 0, tmmsm50["MAT_TYPE"].ToString()) < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_AOD", 0, " ") < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_BOF", 0, " ") < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_EAF", 0, " ") < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_LF", 0, " ") < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_RH", 0, " ") < 0 ||
		//			epex.SetValue("INT_MAT_ANA", "MATERIAL_GROUP_VOD", 0, " ") < 0)
		//		{
		//			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
		//			throw CApplicationException(-1, s.msg, s.svc_name);
		//		}
		//		for (int i = 0; i < bcls_ret->Tables["QM_ELE"].Rows.get_Count(); i++)
		//		{

		//			if (epex.SetValue("INT_MAT_ANA_DET", "ELEMENT", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELEMENT"].ToString())<0 ||
		//				epex.SetValue("INT_MAT_ANA_DET", "VALUE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal()) < 0)
		//			{
		//				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
		//				throw CApplicationException(-1, s.msg, s.svc_name);
		//			}
		//		}

		//		if (epex.SendTele() < 0)
		//		{
		//			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}

		//		// 释放
		//		epex.Uninitialize();

		//	}
		//	cmd_inq.Close();
		//}

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
