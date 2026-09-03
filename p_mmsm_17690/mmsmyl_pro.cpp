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
int f_mmsm_t83313_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmyl_pro)

int f_mmsmyl_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		s_formname = s.formname;
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		v_table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();
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
				
				tmmsmyl["REC_CREATE_TIME"] = nowTime;
				tmmsmyl["REC_CREATOR"] = s.userid;
				cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name+ " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
				tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			else if (v_proc_div == "U")
			{
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

			//特定画面发电文
			if (s_formname == "MMSMTLCFS2N")
			{
				if (v_proc_div == "I" || v_proc_div == "U" || v_proc_div == "S")
				{
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
