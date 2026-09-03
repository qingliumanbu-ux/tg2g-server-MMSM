
#include "stdafx.h"

BM2F_ENTERACE(mmsm01g3_inq)


int f_mmsm01g3_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	     
	
	
		CTracer log(__FUNCTION__);


		int doFlag = 0;
		CString sqlstr = " ";
		int		TotalRecordCount = 0;
		CString v_start_time_f = "";
		CString v_start_time_t = "";
		CString v_factory_div_p = "";
		int blkNum = 0;
		CString sqlstr_count;

		CDecimal BL_WT = 0;
		CDecimal BL_NUM = 0;
		CDecimal GG_WT = 0;
		CDecimal GG_NUM = 0;
		CDecimal WPD_WT = 0;
		CDecimal WPD_NUM = 0;



		try
		{
			//系统的分页类信息。
			CPageInfo pageInfo;
			CDbCommand cmd_inq(conn);

			try
			{//获取前台DEV控件传入的分页信息
				pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
			}
			catch (CException& ce)
			{
				pageInfo.RecordFrom = 0;
				pageInfo.PageSize = 1000;
			}
		
			//参数传入
			if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
				v_factory_div_p = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
				v_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
				v_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();


			if (v_start_time_f.Trim() == "")
				v_start_time_f = "00000000000000";
			else v_start_time_f += "000000";
			if (v_start_time_t.Trim() == "")
				v_start_time_t = "99999999999999";
			else v_start_time_t += "000000";

			Log::Info("", __FUNCTION__, "v_factory_div_p =[{0}]", v_factory_div_p);
			Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", v_start_time_f);
			Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", v_start_time_t);

			sqlstr = "select MAT_NO,SLAB_CUT_TIME,ST_NO,FIN_ST_NO,MAT_ACT_WT,JUDGE_TIME,STOCK_NO,DEST_FIN from TMMSM01 \
					  where judge_time between @time_f and @time_t  \
					  and INGOT_CODE <= ''";

			sqlstr_count = "select COUNT(*) from TMMSM01 \
						    where judge_time between @time_f and @time_t  \
			                and INGOT_CODE <= ''";

			cmd_inq.Parameters.Set("factory_div", v_factory_div_p);
			cmd_inq.Parameters.Set("time_f", v_start_time_f);
			cmd_inq.Parameters.Set("time_t", v_start_time_t);

			cmd_inq.SetCommandText(sqlstr_count);
			TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();


			blkNum = bcls_ret->Tables.IndexOf("MMSMTJ");
			if (blkNum < 0)
			{
				bcls_ret->Tables.Add("MMSMTJ");
			}

			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "BL_WT");
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "BL_NUM");
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "GG_WT");
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "GG_NUM");
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "WPD_WT");
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "WPD_NUM");
			bcls_ret->Tables["MMSMTJ"].Rows.Add();


			//计算保留钢锭量
			CDbCommand cmd_sql_1(conn);
			CString sql_cc_1 = "SELECT sum(MAT_ACT_WT),count(*) from TMMSM01  \
							   	WHERE  JUDGE_TIME between @time_f and @time_t \
								and fin_st_no = PREC_ST_NO \
								and INGOT_CODE <= ' ' \
								AND FIN_ST_NO<>''";
			//cmd_sql_1.Parameters.Set("factory_div", v_factory_div_p);
			cmd_sql_1.Parameters.Set("time_f", v_start_time_f);
			cmd_sql_1.Parameters.Set("time_t", v_start_time_t);
			cmd_sql_1.SetCommandText(sql_cc_1);

			Log::Trace("", __FUNCTION__, "sql_cc_1 = [{0}]", (const char*)sql_cc_1);

			cmd_sql_1.ExecuteReader();

			if (cmd_sql_1.Read())
			{
				BL_WT = cmd_sql_1.GetDecimal(1).ConvertToPrecScale(6, 3);
				BL_NUM = cmd_sql_1.GetDecimal(2).ConvertToPrecScale(6, 3);
			}

			bcls_ret->Tables["MMSMTJ"].Rows[0]["BL_WT"] = BL_WT;
			bcls_ret->Tables["MMSMTJ"].Rows[0]["BL_NUM"] = BL_NUM;
			cmd_sql_1.Close();

			//计算改钢锭量
			CDbCommand cmd_sql_2(conn);
			CString sql_cc_2 = "SELECT sum(MAT_ACT_WT),count(*) from TMMSM01  \
							   	WHERE  JUDGE_TIME between @time_f and @time_t  \
								and fin_st_no<>PREC_ST_NO  \
								and INGOT_CODE <= ' ' \
								AND FIN_ST_NO<>''";
			//cmd_sql_2.Parameters.Set("factory_div", v_factory_div_p);
			cmd_sql_2.Parameters.Set("time_f", v_start_time_f);
			cmd_sql_2.Parameters.Set("time_t", v_start_time_t);
			cmd_sql_2.SetCommandText(sql_cc_2);

			Log::Trace("", __FUNCTION__, "sql_cc_2 = [{0}]", (const char*)sql_cc_2);

			cmd_sql_2.ExecuteReader();
			if (cmd_sql_2.Read())
			{
				GG_WT = cmd_sql_2.GetDecimal(1).ConvertToPrecScale(6, 3);
				GG_NUM = cmd_sql_2.GetDecimal(2).ConvertToPrecScale(6, 3);
			}
			bcls_ret->Tables["MMSMTJ"].Rows[0]["GG_WT"] = GG_WT;
			bcls_ret->Tables["MMSMTJ"].Rows[0]["GG_NUM"] = GG_NUM;
			cmd_sql_2.Close();

			//计算未判钢锭量
			CDbCommand cmd_sql_3(conn);
			CString sql_cc_3 = "SELECT sum(MAT_ACT_WT),count(*) from TMMSM01  \
							   	WHERE  JUDGE_TIME between @time_f and @time_t \
							    and fin_st_no <= '' \
								and INGOT_CODE <= ' '";
			//cmd_sql_3.Parameters.Set("factory_div", v_factory_div_p);
			cmd_sql_3.Parameters.Set("time_f", v_start_time_f);
			cmd_sql_3.Parameters.Set("time_t", v_start_time_t);
			cmd_sql_3.SetCommandText(sql_cc_3);

			Log::Trace("", __FUNCTION__, "sql_cc_3 = [{0}]", (const char*)sql_cc_3);

			cmd_sql_3.ExecuteReader();
			if (cmd_sql_3.Read())
			{
				WPD_WT = cmd_sql_3.GetDecimal(1).ConvertToPrecScale(6, 3);
				WPD_NUM = cmd_sql_3.GetDecimal(2).ConvertToPrecScale(6, 3);
			}

			bcls_ret->Tables["MMSMTJ"].Rows[0]["WPD_WT"] = WPD_WT;
			bcls_ret->Tables["MMSMTJ"].Rows[0]["WPD_NUM"] = WPD_NUM;
			cmd_sql_3.Close();
			Log::Trace("", __FUNCTION__, "cmd_sql_3_finished");


			//返回分页总数量信息 
			bcls_ret->Tables.Add("PageInfo");
			bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
			bcls_ret->Tables["PageInfo"].Rows.Add();
			bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
		}
	
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error，sqlcode=[{0},{1}]", arguments, 2);
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
}


