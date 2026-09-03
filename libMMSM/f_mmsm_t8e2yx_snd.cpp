/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:
一给原料L2-各工序料槽信息 
T8E2Y2
T8E2Y4
T8E2Y7
T8E2YJ
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t8e2yx_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString action = " ";
	CString tcNO = " ";
	CString dealFlag = " ";
	CString tableName = " ";
	CString tableNameChild = " ";
	CString bunkerNo = " ";
	CString seqNo = " ";
	CString matCode = " ";
	CDecimal requestId = 0;
	EPEX epex;
	EIClass EITable;
	/* 实体类定义 */
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm81("TMMSM81");
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

		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
			action = bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
			tableName = bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME_CHILD"))
			tableNameChild = bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME_CHILD"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
			tmmsm81["BUNKER_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_NO"].ToString().Trim();
		//if(bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
		//	tmmsm81["LOT_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
		//	tmmsm81["QUALITY_BATCH_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		//	tmmsm81["MAT_CODE"] = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
		//	tmmsm81["STOCK_WT"] = bcls_rec->Tables["MMLCSND"].Rows[0]["STOCK_WT"].ToDecimal();

		if ("" == tableName)
		{
			strcpy(s.msg, "选择的料仓类型有误！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == tmmsm81["BUNKER_NO"].ToString())
		{
			strcpy(s.msg, "传入料仓号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===tcNO= [{0}]", tcNO);

		sqlstr = "  SELECT * FROM TMMSM85 WHERE bunker_no = @bunker_no ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("bunker_no", tmmsm81["BUNKER_NO"]);
		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		cmd_inq.Close();
		if (EITable.Tables[0].Rows.get_Count() > 0)
		{
			requestId = EITable.Tables[0].Rows[0]["SEQ_NO_TM"].ToDecimal();
		}

		tmmsm60["BUNKER_NO"] = tmmsm81["BUNKER_NO"];
		tmmsm60.Query("BUNKER_NO");

		if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(3, 3) == "BOX")
		{
			tmmsm60["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) + "X";
		}

		if (tmmsm60["BUNKER_TYPE"].ToString().GetLength()>2)
		{
			tmmsm60["BUNKER_TYPE"] = " ";
		}
		
		Log::Trace("", __FUNCTION__, "===BUNKER_TYPE= [{0}]", tmmsm60["BUNKER_TYPE"].ToString());
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (action == "I")
		{
			action = "U";
		}
		if (epex.SetValue(tableName, "ACTION", 0, action)<0 ||
			epex.SetValue(tableName, "REQUEST_ID", 0, requestId) < 0 ||
			epex.SetValue(tableName, "AGGREGATE_NAME", 0, tmmsm60["BUNKER_TYPE"].ToString()) < 0 ||
			epex.SetValue(tableName, "CHUTE_NUMBER", 0, tmmsm60["BUNKER_NO"].ToString()) < 0 ||
			epex.SetValue(tableName, "ORDER_NUMBER", 0, "0") < 0 ||
			epex.SetValue(tableName, "SPLIT_INDICATION", 0, 0) < 0 ||
			epex.SetValue(tableName, "TREATMENT_COUNTER", 0, 0) < 0 ||
			epex.SetValue(tableName, "BOX_SEQUENCE", 0, 1) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (action=="D")
		{
		
			
			if (epex.SetValue(tableNameChild, "CNT", 0, 0)<0 ||
				epex.SetValue(tableNameChild, "MATERIAL_CODE", 0, " ") < 0 ||
				epex.SetValue(tableNameChild, "BATCH_NUMBER", 0, " ") < 0 ||
				epex.SetValue(tableNameChild, "WEIGHT", 0, 0) < 0 ||
				epex.SetValue(tableNameChild, "QUALITY_BATCH", 0, " ") < 0
				)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			if (epex.SendTele() < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			// 释放
			epex.Uninitialize();
		}
		else
		{
			if (EITable.Tables[0].Rows.get_Count()>0)
			{
				for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
				{

					tmmsm50["MAT_CODE"] = EITable.Tables[0].Rows[i]["MAT_CODE"];
					tmmsm50.Query("MAT_CODE");
					if (epex.SetValue(tableNameChild, "CNT", i, i+1)<0 ||
						epex.SetValue(tableNameChild, "MATERIAL_CODE", i, tmmsm50["MAT_CODE_L2"].ToString()) < 0 ||
						epex.SetValue(tableNameChild, "BATCH_NUMBER", i, tmmsm50["LOT_NO"].ToString()) < 0 ||
						epex.SetValue(tableNameChild, "WEIGHT", i, (EITable.Tables[0].Rows[i]["STOCK_WT"].ToDecimal()+EITable.Tables[0].Rows[i]["BUNKER_DEDUCT_WT"].ToDecimal())) < 0 ||
						epex.SetValue(tableNameChild, "QUALITY_BATCH", i, EITable.Tables[0].Rows[i]["QUALITY_BATCH_NO"].ToString()) < 0
						)
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
			}
		}
		
		/*if (epex.SetValue(tableNameChild, "CNT", 0, tmmsm2a["PROC_COUNT"].ToDecimal())<0 ||
			epex.SetValue(tableNameChild, "MATERIAL_CODE", 0, tmmsm50["MAT_CODE_L2"].ToString()) < 0 ||
			epex.SetValue(tableNameChild, "BATCH_NUMBER", 0, tmmsm50["LOT_NO"].ToString()) < 0 ||
			epex.SetValue(tableNameChild, "WEIGHT", 0, tmmsm81["STOCK_WT"].ToDecimal()) < 0 ||
			epex.SetValue(tableNameChild, "QUALITY_BATCH", 0, tmmsm81["QUALITY_BATCH_NO"].ToString()) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

		

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
