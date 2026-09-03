
#include "stdafx.h"

BM2F_ENTERACE(mmsm01g4_inq)


int f_mmsm01g4_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CDecimal good_wt_ch_cc = 0;
	CDecimal good_n_ch_cc = 0;
	CDecimal good_wt_y_cc = 0;
	CDecimal good_n_y_cc = 0;

	CDecimal good_wt_ch_ic = 0;
	CDecimal good_n_ch_ic = 0;
	CDecimal good_wt_y_ic = 0;
	CDecimal good_n_y_ic = 0;



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


		if (v_start_time_f.Trim() =="")
			v_start_time_f = "00000000000000";
		else v_start_time_f += "000000";
		if (v_start_time_t.Trim() == "")
			v_start_time_t = "99999999999999";
		else v_start_time_t += "000000";

		Log::Info("", __FUNCTION__, "v_factory_div_p =[{0}]", v_factory_div_p);
		Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", v_start_time_f);
		Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", v_start_time_t);

		sqlstr = "select t1.pono,t1.steel_wt as ms_total,t1.FACTORY_DIV,t2.dest_fin,t2.slab_num,t3.ST_NO as prec_st_no,t3.FIN_ST_NO,t3.JUDGE_TIME  from tmmsm31 t1,( \
                  select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num   \
                  from tmmsm33 t1,tqmts23 t3   \
                  where  t1.heat_no=t3.heat_no  \
                  and t3.judge_time between @time_f and @time_t  \
                  group by t1.heat_no, t3.heat_no) t2, tqmts23 t3 \
                  where  t1.heat_no = t2.heat_no \
                  and t1.heat_no = t3.HEAT_NO \
                  and t3.judge_time between @time_f and @time_t \
                  and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
                  and t3.st_no<>t3.fin_st_no";

		sqlstr_count = "select count(*)  from tmmsm31 t1,( \
                  select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num   \
                  from tmmsm33 t1,tqmts23 t3   \
                  where  t1.heat_no=t3.heat_no  \
                  and t3.judge_time between @time_f and @time_t  \
                  group by t1.heat_no, t3.heat_no) t2, tqmts23 t3 \
                  where  t1.heat_no = t2.heat_no \
                  and t1.heat_no = t3.HEAT_NO \
                  and t3.judge_time between @time_f and @time_t \
                  and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
                  and t3.st_no<>t3.fin_st_no"; 

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

		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_wt_ch_cc");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_n_ch_cc");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_wt_y_cc");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_n_y_cc");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_wt_ch_ic");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_n_ch_ic");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_wt_y_ic");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "good_n_y_ic");
		bcls_ret->Tables["MMSMTJ"].Rows.Add();


		//计算CC改炉量
		CDbCommand cmd_sql_1(conn);
		CString sql_cc_1 = "select sum(t1.STEEL_WT),sum(t2.slab_num)  from tmmsm31 t1,( \
			select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num \
			from tmmsm33 t1, tmmsm31 t2 \
		    where t2.heat_no = t1.HEAT_NO \
		    and t2.POUR_END_TIME between @time_f and @time_t \
			group by t1.heat_no, t2.heat_no) t2, tqmts23 t3 \
		    where  t1.heat_no = t2.heat_no \
		    and t1.heat_no = t3.HEAT_NO \
			and t1.POUR_END_TIME between @time_f and @time_t \
			and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
			and t3.FIN_ST_NO<>t3.ST_NO \
			and t1.STATION_ID = 'C'";
		cmd_sql_1.Parameters.Set("factory_div", v_factory_div_p);
		cmd_sql_1.Parameters.Set("time_f", v_start_time_f);
		cmd_sql_1.Parameters.Set("time_t", v_start_time_t);
		cmd_sql_1.SetCommandText(sql_cc_1);

		Log::Trace("", __FUNCTION__, "sql_cc_1 = [{0}]", (const char*)sql_cc_1);

		cmd_sql_1.ExecuteReader();

		if (cmd_sql_1.Read())
		{
			good_wt_ch_cc = cmd_sql_1.GetDecimal(1).ConvertToPrecScale(6, 3);
			good_n_ch_cc = cmd_sql_1.GetDecimal(2).ConvertToPrecScale(6, 3);
		}
		
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_WT_CH_CC"] = good_wt_ch_cc;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_N_CH_CC"] = good_n_ch_cc;
		cmd_sql_1.Close();

		//CC未判炉量
		CDbCommand cmd_sql_2(conn);
		CString sql_cc_2 = "select sum(t1.STEEL_WT),sum(t2.slab_num)  from tmmsm31 t1,( select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num \
			from tmmsm33 t1, tmmsm31 t2 \
		    where t2.heat_no = t1.HEAT_NO \
		    and t2.POUR_END_TIME between @time_f and @time_t \
			group by t1.heat_no, t2.heat_no) t2, tqmts23 t3 \
		    where  t1.heat_no = t2.heat_no \
		    and t1.heat_no = t3.HEAT_NO \
			and t1.POUR_END_TIME between @time_f and @time_t \
			and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
			and t3.FIN_ST_NO <= '' \
			and t1.STATION_ID = 'C'";
		cmd_sql_2.Parameters.Set("factory_div", v_factory_div_p);
		cmd_sql_2.Parameters.Set("time_f", v_start_time_f);
		cmd_sql_2.Parameters.Set("time_t", v_start_time_t);
		cmd_sql_2.SetCommandText(sql_cc_2);

		Log::Trace("", __FUNCTION__, "sql_cc_2 = [{0}]", (const char*)sql_cc_2);

		cmd_sql_2.ExecuteReader();
		if (cmd_sql_2.Read())
		{
			good_wt_y_cc = cmd_sql_2.GetDecimal(1).ConvertToPrecScale(6, 3);
			good_n_y_cc = cmd_sql_2.GetDecimal(2).ConvertToPrecScale(6, 3);
		}
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_WT_Y_CC"] = good_wt_y_cc;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_N_Y_CC"] = good_n_y_cc;
		cmd_sql_2.Close();

		//计算IC改炉量
		CDbCommand cmd_sql_3(conn);
		CString sql_cc_3 = "select sum(t1.STEEL_WT),sum(t2.slab_num)  from tmmsm31 t1,( \
			select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num \
			from tmmsm33 t1, tmmsm31 t2 \
		    where t2.heat_no = t1.HEAT_NO \
		    and t2.POUR_END_TIME between @time_f and @time_t \
			group by t1.heat_no, t2.heat_no) t2, tqmts23 t3 \
		    where  t1.heat_no = t2.heat_no \
		    and t1.heat_no = t3.HEAT_NO \
			and t1.POUR_END_TIME between @time_f and @time_t \
			and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
			and t3.FIN_ST_NO<>t3.ST_NO \
			and t1.STATION_ID = 'I'";
		cmd_sql_3.Parameters.Set("factory_div", v_factory_div_p);
		cmd_sql_3.Parameters.Set("time_f", v_start_time_f);
		cmd_sql_3.Parameters.Set("time_t", v_start_time_t);
		cmd_sql_3.SetCommandText(sql_cc_3);

		Log::Trace("", __FUNCTION__, "sql_cc_3 = [{0}]", (const char*)sql_cc_3);

		cmd_sql_3.ExecuteReader();
		if (cmd_sql_3.Read())
		{
			Log::Trace("", __FUNCTION__, "cmd_sql_3_read");
			good_wt_ch_ic = cmd_sql_3.GetDecimal(1).ConvertToPrecScale(6, 3);
			good_n_ch_ic = cmd_sql_3.GetDecimal(2).ConvertToPrecScale(6, 3);
		}

		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_WT_CH_IC"] = good_wt_ch_ic;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_N_CH_IC"] = good_n_ch_ic;
		cmd_sql_3.Close();
		Log::Trace("", __FUNCTION__, "cmd_sql_3_finished");
		//计算IC未判炉量
		CDbCommand cmd_sql_4(conn);
		CString sql_cc_4= "select sum(t1.STEEL_WT),sum(t2.slab_num)  from tmmsm31 t1,(select t1.heat_no, min(t1.SLAB_PLAN_DEST)as dest_fin, count(*) as slab_num \
			from tmmsm33 t1, tmmsm31 t2 \
	    	where t2.heat_no = t1.HEAT_NO \
		    and t2.POUR_END_TIME between @time_f and @time_t \
			group by t1.heat_no, t2.heat_no) t2, tqmts23 t3 \
		    where  t1.heat_no = t2.heat_no \
		    and t1.heat_no = t3.HEAT_NO \
			and t1.POUR_END_TIME between @time_f and @time_t \
			and t1.FACTORY_DIV = nvl(@factory_div, FACTORY_DIV) \
			and t3.FIN_ST_NO <= '' \
			and t1.STATION_ID = 'I'";
		cmd_sql_4.Parameters.Set("factory_div", v_factory_div_p);
		cmd_sql_4.Parameters.Set("time_f", v_start_time_f);
		cmd_sql_4.Parameters.Set("time_t", v_start_time_t);
		cmd_sql_4.SetCommandText(sql_cc_4);

		Log::Trace("", __FUNCTION__, "sql_cc_4 = [{0}]", (const char*)sql_cc_4);

		cmd_sql_4.ExecuteReader();
		if (cmd_sql_4.Read())
		{
			Log::Trace("", __FUNCTION__, "cmd_sql_4_read");
			good_wt_y_ic = cmd_sql_4.GetDecimal(1).ConvertToPrecScale(6, 3);
			good_n_y_ic = cmd_sql_4.GetDecimal(2).ConvertToPrecScale(6, 3);
		}

		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_WT_Y_IC"] = good_wt_y_ic;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["GOOD_N_Y_IC"] = good_n_y_ic;
		cmd_sql_4.Close();
		Log::Trace("", __FUNCTION__, "cmd_sql_4_finished");

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


