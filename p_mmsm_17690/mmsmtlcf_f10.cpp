/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_t83313_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
// service入口
BM2F_ENTERACE(mmsmtlcf_f10)

int f_mmsmtlcf_f10(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_name = "TMMSMWQ";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsmwq_ll("TMMSMWQ_LL");
	CString v_prod_desc = "";
	try
	{
		s_formname = s.formname;
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		
		CModel tmmsmyl(v_table_name);
		Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);
		Log::Trace("", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmyl.Reset();
			tmmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "PROD_DATE =[{0}]", tmmsmyl["PROD_DATE"].ToString());
			if (v_proc_div == "I")
			{
				v_prod_desc = "新增";
				tmmsmyl["REC_CREATE_TIME"] = nowTime;
				tmmsmyl["REC_CREATOR"] = s.userid;
				tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0,8);
				cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
				tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			else if (v_proc_div == "U")
			{
				v_prod_desc = "修改";
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM 	" + v_table_name + " "
					"   WHERE  PROD_DATE	= @PROD_DATE  AND  PROC_COUNT	= @PROC_COUNT ";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("PROD_DATE", tmmsmyl["PROD_DATE"].ToString());
				cmd_sql.Parameters.Set("PROC_COUNT", tmmsmyl["PROC_COUNT"].ToDecimal());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmyl["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmyl["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmyl["REC_REVISE_TIME"] = nowTime;
				tmmsmyl["REC_REVISOR"] = s.userid;

				tmmsmyl.Delete("PROC_COUNT,PROD_DATE");
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			else if (v_proc_div == "D")
			{
				if (s_formname == "MMSMTLCFS2N")
				{
					v_prod_desc = "删除";
					//发资源电文
					blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("MMLCSND");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROD_DATE"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROD_DATE");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_COUNT");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					bcls_rec->Tables["MMLCSND"].Rows.Clear();
					bcls_rec->Tables["MMLCSND"].Rows.Add();
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T83313";
					bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = v_proc_div;
					bcls_rec->Tables["MMLCSND"].Rows[0]["PROD_DATE"] = tmmsmyl["PROD_DATE"];
					bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsmyl["LOT_NO"];
					bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"] = tmmsmyl["PROC_COUNT"];
					doFlag = f_mmsm_t83313_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t83313_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Delete("PROC_COUNT,PROD_DATE");
			}
			else if (v_proc_div == "ISD")
			{
				v_prod_desc = "新增";
				sqlstr = " WITH MM1 AS (select PROC_NO, SUM(CASE WHEN B.MAT_CODE IS NOT NULL THEN A.DEVO_WT ELSE 0 END) DEVO_WT\
					from TMMSM2A_YL A\
					LEFT JOIN tmmsm50 b on a.MAT_CODE = b.MAT_CODE AND B.MAT_TYPE = '2'\
				where A.LOT_NO in\
				(select distinct LOT_NO from TMMSM2A_YL where 1 = 1 and MAT_CODE = '"+bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString() + "' AND PROC_NO = '" + bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString() + "')\
					GROUP BY PROC_NO)\
					SELECT '加权平均'                                               EAF_HEAT_NO,\
					MAX(LOT_NO)                                              LOT_NO,\
					sum(DEVO_WT)                                             DEVO_WT,\
					'加权平均'                                               ST_SAMPLE_NO,\
					round(sum(METAL_YIELD_RATE * DEVO_WT) / sum(DEVO_WT), 1) METAL_YIELD_RATE,\
					round(sum(C_VALUE * DEVO_WT) / sum(DEVO_WT), 4)          C_VALUE,\
					round(sum(SI_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         SI_VALUE,\
					round(sum(MN_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         MN_VALUE,\
					round(sum(P_VALUE * DEVO_WT) / sum(DEVO_WT), 4)          P_VALUE,\
					round(sum(S_VALUE * DEVO_WT) / sum(DEVO_WT), 4)          S_VALUE,\
					round(sum(CR_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         CR_VALUE,\
					round(sum(NI_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         NI_VALUE,\
					round(sum(MO_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         MO_VALUE,\
					round(sum(CU_VALUE * DEVO_WT) / sum(DEVO_WT), 4)         CU_VALUE\
					FROM TMMSMWQ A\
					LEFT JOIN MM1 ON A.EAF_HEAT_NO = MM1.PROC_NO\
					WHERE LOT_NO = '" + bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString() + "'\
					AND MM1.PROC_NO IS NOT NULL ";

				tmmsmyl["REC_CREATE_TIME"] = nowTime;
				tmmsmyl["REC_CREATOR"] = s.userid;
				tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0, 8);
				cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
				tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			
			//特定画面发电文
			if (s_formname == "MMSMTLCFS2N")
			{
				if (v_proc_div == "I" || v_proc_div == "U" || v_proc_div == "S" || v_proc_div == "ISD")
				{
					if (v_proc_div == "S")
					{
						v_prod_desc = "发送";
					}
					//发资源电文
					blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("MMLCSND");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROD_DATE"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROD_DATE");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_COUNT");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					bcls_rec->Tables["MMLCSND"].Rows.Clear();
					bcls_rec->Tables["MMLCSND"].Rows.Add();
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T83313";
					bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = v_proc_div;
					bcls_rec->Tables["MMLCSND"].Rows[0]["PROD_DATE"] = tmmsmyl["PROD_DATE"];
					bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsmyl["LOT_NO"];
					bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"] = tmmsmyl["PROC_COUNT"];
					doFlag = f_mmsm_t83313_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t83313_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsmyl["SEND_FLAG"] = "1";
					tmmsmyl.Update("SEND_FLAG", "PROD_DATE,PROC_COUNT");
				}

			}

			tmmsmyl.Query("PROC_COUNT,PROD_DATE");
			tmmsmwq_ll.CopyFrom(tmmsmyl);
			tmmsmwq_ll["EVENT_ID"] = v_proc_div;
			tmmsmwq_ll["EVENT_DESC"] = v_prod_desc;
			tmmsmwq_ll["EVENT_MAKER"] = s.userid;
			tmmsmwq_ll["EVENT_TIME"] = nowTime;
			tmmsmwq_ll.Insert();
			if (v_proc_div == "ISD") {
				tmmsmyl.Delete("PROC_COUNT,PROD_DATE");
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
