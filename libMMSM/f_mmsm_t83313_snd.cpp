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
#include "math.h"

int f_mmrqjd(CDecimal elm_act, int elm_accu,CString &r_elm_act)//成分修约
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		if (elm_accu > 0)
		{
			CDecimal v_elm_format = pow(10.0, elm_accu);
			elm_act = elm_act * v_elm_format;
			int z_value = floor(elm_act.ToDouble());//取传入长度的整数部分
			//Log::Trace("", __FUNCTION__, " z_value [{0}] elm_act[{1}] ", z_value, elm_act);
			float x_value = (elm_act.ToDouble() - z_value);//取传入长度的小数部分
			//Log::Trace("", __FUNCTION__, " x_value [{0}]  ", x_value);
			if (x_value == 0.5)
			{
				if (z_value % 2 == 0)
				{
					elm_act = z_value;
				}
				else
				{
					elm_act = z_value + 1;
				}
			}
			else
			{
				elm_act = floor(elm_act.ToDouble() + 0.5);
			}
			r_elm_act = (elm_act / v_elm_format).Round(elm_accu).ToString();
			Log::Trace("", __FUNCTION__, " 修约后elm_act [{0}]  ", r_elm_act, elm_accu);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}//修约

BM2_FUNCTION_EXPORT
int f_mmsm_t83313_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 实体类定义 */
	CModel tmmsmwq("TMMSMWQ");;
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
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELM_CODE"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_STRING, "ELM_CODE");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELM_NAME"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_STRING, "ELM_NAME");
		}
		if (!bcls_ret->Tables["QM_ELE"].Columns.Contains("ELM_VALUE"))
		{
			bcls_ret->Tables["QM_ELE"].Columns.Add(DT_DECIMAL, "ELM_VALUE");
		}

		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
			action = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PROD_DATE"))
			tmmsmwq["PROD_DATE"] = bcls_rec->Tables["MMLCSND"].Rows[0]["PROD_DATE"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
			tmmsmwq["LOT_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
			tmmsmwq["PROC_COUNT"] = bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"].ToString().Trim();
		
		if ("" == tmmsmwq["LOT_NO"].ToString().Trim())
		{
			strcpy(s.msg, "传入批次号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmmsmwq.Query("PROD_DATE,PROC_COUNT,LOT_NO");

		Log::Trace("", __FUNCTION__, "===tcNO= [{0}]", tcNO);
		Log::Trace("", __FUNCTION__, "===LOT_NO= [{0}]", tmmsmwq["LOT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "===PROC_COUNT= [{0}]", tmmsmwq["PROC_COUNT"].ToString());
		sqlstr = 
		"	WITH T1 AS(SELECT DECODE(ELM_NAME, 'C_VALUE', 'C', 'SI_VALUE', 'Si', 'MN_VALUE', 'Mn', 'P_VALUE', 'P', 'S_VALUE', 'S',\
				'CR_VALUE', 'Cr', 'NI_VALUE', 'Ni', 'MO_VALUE', 'Mo', 'CU_VALUE', 'Cu', 'PB_VALUE', 'PB',\
				'SN_VALUE',\
				'SN', 'SB_VALUE', 'SB', 'AS_VALUE', 'AS', 'BI_VALUE', 'BI') ELM_NAME1,\
				DECODE(ELM_NAME, 'C_VALUE', 'Y001', 'SI_VALUE', 'Y002', 'MN_VALUE', 'Y003', 'P_VALUE', 'Y004',\
					'S_VALUE', 'Y005', 'CR_VALUE', 'Y015', 'NI_VALUE', 'Y022', 'MO_VALUE', 'Y019', 'CU_VALUE',\
					'Y016',\
					'PB_VALUE', 'Y024', 'SN_VALUE', 'Y026', 'SB_VALUE', 'Y025', 'AS_VALUE', 'Y008', 'BI_VALUE',\
					'Y011')                                                     ELM_CODE,\
				ELM_VALUE\
				FROM(SELECT LOT_NO, ELM_VALUE, ELM_NAME\
					FROM(SELECT *\
						FROM TMMSMWQ\
						WHERE PROC_COUNT = "+tmmsmwq["PROC_COUNT"].ToString() + "\
						AND PROD_DATE = '"+ tmmsmwq["PROD_DATE"].ToString() +"') UNPIVOT(ELM_VALUE FOR ELM_NAME IN(C_VALUE, SI_VALUE, MN_VALUE, P_VALUE, S_VALUE, CR_VALUE, NI_VALUE, MO_VALUE, CU_VALUE, PB_VALUE, SN_VALUE, SB_VALUE, AS_VALUE, BI_VALUE)))\
				WHERE LOT_NO = '"+ tmmsmwq["LOT_NO"].ToString() +"')\
			SELECT T1.*, nvl(T2.CODE_DESC_1_CONTENT,0) CODE_DESC_1_CONTENT\
			FROM t1\
			left join tep0002 T2 ON UPPER(T1.ELM_NAME1) = T2.CODE AND T2.CODE_CLASS = 'MMRQJD' ";
		cmd_inq.Parameters.Set("LOT_NO", tmmsmwq["LOT_NO"]);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		bcls_ret->Tables["QM_ELE"].Rows.Clear();
		while (cmd_inq.Read())
		{
			bcls_ret->Tables["QM_ELE"].Rows.Add();
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_NAME"] = cmd_inq.GetString(1);
			bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_CODE"] = cmd_inq.GetString(2);
			if (cmd_inq.GetDecimal(4) > 0)
			{
				CDecimal elm_act = cmd_inq.GetDecimal(3);
				int elm_accu = cmd_inq.GetInt32(4);
				CString r_elm_act;
				int ret = 0;
				ret= f_mmrqjd(elm_act, elm_accu, r_elm_act);
				if (ret == 0)
				{
					bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_VALUE"] = r_elm_act;
					Log::Trace("", __FUNCTION__, "===ELM_VALUE= [{0}]", r_elm_act);
				}
				else {
					bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_VALUE"] = cmd_inq.GetDecimal(3);
				}
			}
			else {
				bcls_ret->Tables["QM_ELE"].Rows[seq]["ELM_VALUE"] = cmd_inq.GetDecimal(3);
			}
			
			seq++;

		}
		cmd_inq.Close();
		CString v_res_type = Db::QueryCString("select CODE_DESC_1_CONTENT from tep0002 where CODE_CLASS='MMRQLX' AND CODE ='" + tmmsmwq["RES_TYPE"].ToString() + "'");

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (epex.SetValue("DEAL_FLAG", 0, action)<0 ||
			epex.SetValue("QUALITY_BATCH_NO", 0, " ") < 0 ||
			epex.SetValue("SAMPLE_ENTR_NO", 0, " ") < 0 ||
			epex.SetValue("MAT_INSPECT_TYPE", 0, "配送") < 0 ||
			epex.SetValue("PURCH_NO", 0, tmmsmwq["LOT_NO"].ToString()) < 0 ||
			epex.SetValue("LOT_NO", 0, tmmsmwq["LOT_NO"].ToString()) < 0 ||
			epex.SetValue("MAT_CODE", 0, tmmsmwq["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("MAT_NAME", 0, tmmsmwq["MAT_NAME"].ToString()) < 0 ||
			epex.SetValue("DATI_ANALYSIS_TAKEN", 0, tmmsmwq["PROD_DATE"].ToString()) < 0 ||
			epex.SetValue("ANALYSE_TIME", 0, datetime) < 0||
			epex.SetValue("ANALYSE_ITEM_COUNT", 0, bcls_ret->Tables["QM_ELE"].Rows.get_Count()) < 0 ||
			epex.SetValue("REMARK_1", 0, v_res_type) < 0 ||
			epex.SetValue("REMARK_2", 0, tmmsmwq["EAF_HEAT_NO"].ToString()) < 0 ||
			epex.SetValue("REMARK_3", 0, tmmsmwq["COM_FLAG"].ToString()) < 0 ||
			epex.SetValue("REMARK_4", 0, tmmsmwq["PRODUCE_DATE"].ToString()) < 0 ||
			epex.SetValue("REMARK_5", 0, tmmsmwq["SERIAL_NO"].ToString()) < 0 ||
			epex.SetValue("REMARK_11", 0, tmmsmwq["EMPTY_LADLE_WEIGHT"].ToDecimal()) < 0 ||
			epex.SetValue("REMARK_12", 0, tmmsmwq["SLAG_STEEL_WT"].ToDecimal()) < 0 ||
			epex.SetValue("REMARK_13", 0, tmmsmwq["SLAG_THICK"].ToDecimal()) < 0 ||
			epex.SetValue("REMARK_14", 0, tmmsmwq["ACTRESULT"].ToDecimal()) < 0 ||
			epex.SetValue("REMARK_15", 0, tmmsmwq["MAT_AMOUNT1"].ToDecimal()) < 0 ||
			epex.SetValue("REMARK_16", 0, tmmsmwq["METAL_YIELD_RATE"].ToDecimal()) < 0 
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_ret->Tables["QM_ELE"].Rows.get_Count(); i++)
		{

			if (epex.SetValue("ELM_CODE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_CODE"].ToString())<0 ||
				epex.SetValue("ELM_NAME", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_NAME"].ToString())<0 ||
				epex.SetValue("ANALYSE_DATA_TYPE", i, "Y")<0 ||
				epex.SetValue("ANALYSE_MEASURE_UNIT", i, "%")<0 ||
				epex.SetValue("ELM_VALUE", i, bcls_ret->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToString()) < 0)
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
