/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给铁区-质量数据
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int seq = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString dealFlag = " ";
	CString tcNO = " ";
	CString tableName = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString voucherId = " ";
	CString shipName = " ";
	CString mat_code = "";
	EPEX epex;

	/* 实体类定义 */

	// 数据库SQL操作字符串
	CString  sqlstr("");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm50("TMMSM50");
	CString weigh_no = "";
	CString quality_batch_no = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
			weigh_no = bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
			quality_batch_no = bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();

		blkNum = bcls_ret->Tables.IndexOf("QM_ELE");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("QM_ELE");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ITEM_CODE"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_STRING, "ITEM_CODE");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ITEM_NAME"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_STRING, "ITEM_NAME");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELM_VALUE"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_DECIMAL, "ELM_VALUE");
		}
		Log::Trace("", __FUNCTION__, "===tableName= [{0}]", tableName);
		Log::Trace("", __FUNCTION__, "===mat_code= [{0}]", mat_code);
		/* 查询主数据 */
		//tmmsm81.Query("WEIGH_NO");
		//tmmsm81.TrimOrBlank();

		tmmsm50["MAT_CODE"] = mat_code;
		tmmsm50.Query("MAT_CODE");

		Log::Trace("", __FUNCTION__, "===QUALITY_BATCH_NO= [{0}]", quality_batch_no);
		sqlstr = "  SELECT ELM_NAME ,ELM_CODE,ELM_VALUE FROM  TMMSM81AL    WHERE  QUALITY_BATCH_NO	= @QUALITY_BATCH_NO";
		cmd_inq.Parameters.Set("QUALITY_BATCH_NO", quality_batch_no);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		bcls_ret->Tables["QM_ELE"].Rows.Clear();
		while (cmd_inq.Read())
		{
			bcls_ret->Tables["QM_ELE"].Rows.Add();
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ITEM_NAME"] = cmd_inq.GetString(1);
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ITEM_CODE"] = cmd_inq.GetString(2);
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_VALUE"] = cmd_inq.GetDecimal(3);
			Log::Trace("", __FUNCTION__, "===ITEM_NAME= [{0}]", bcls_ret->Tables["QM_ELE"].Rows[seq]["ITEM_NAME"].ToString());
			seq++;

		}
		cmd_inq.Close();


		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 后补充，C_FLAG_DIF 3:  汽运4：部分数据来源计量电文
		if (epex.SetValue("21B004", "DEAL_FLAG", 0, dealFlag) < 0 ||
			epex.SetValue("21B004", "SAMPLE_ENTR_NO", 0, quality_batch_no) < 0 ||
			epex.SetValue("21B004", "TCP_NO", 0, " ") < 0 ||
			epex.SetValue("21B004", "TPC_SEQ", 0, " ") < 0 ||
			epex.SetValue("21B004", "TPC_NO", 0, " ") < 0 ||
			epex.SetValue("21B004", "SAMPLE_NO", 0, " ") < 0 ||
			epex.SetValue("21B004", "INSPECT_TYPE", 0, " ") < 0 ||
			epex.SetValue("21B004", "MAT_CODE", 0, mat_code) < 0 ||
			epex.SetValue("21B004", "MAT_CNAME", 0, tmmsm50["MAT_NAME"].ToString()) < 0 ||
			epex.SetValue("21B004", "SAMPLE_POS_CODE", 0, " ") < 0 ||
			epex.SetValue("21B004", "SAMPLE_POS_CNAME", 0, " ") < 0 ||
			epex.SetValue("21B004", "SAMPLE_TIME", 0, datetime) < 0 ||
			epex.SetValue("21B004", "ANALYSE_STD", 0, " ") < 0 ||
			epex.SetValue("21B004", "ANALYSE_TIME", 0, " ") < 0 ||
			epex.SetValue("21B004", "ANALYSE_BY", 0, " ") < 0 ||
			epex.SetValue("21B004", "UPDATE_TIME", 0, " ") < 0 ||
			epex.SetValue("21B004", "UPDATE_BY", 0, " ") < 0 ||
			epex.SetValue("21B004", "ANALYSE_REMARK", 0, " ") < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===count= [{0}]", bcls_ret->Tables["QM_ELE"].Rows.get_Count());
		for (int i = 0; i < bcls_ret->Tables["QM_ELE"].Rows.get_Count(); i++)
		{
			Log::Trace("", __FUNCTION__, "===count= [{0}]", bcls_ret->Tables["QM_ELE"].Rows.get_Count());
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ITEM_CODE"].ToString()) < 0 ||
				epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ITEM_NAME"].ToString()) < 0 ||
				epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", i, " ") < 0 ||
				epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal()) < 0)

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
