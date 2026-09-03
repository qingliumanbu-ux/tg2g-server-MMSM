/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 取炉次的开始信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82c_inq)
int f_mmsm_gyins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyhl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t80r91_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString sqlstr_temp = "";
	CString heat_no = "";
	CString min_heat_no = "";
	CString max_heat_no = "";
	CString jump_heat_no = "";
	CString tab = "1";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString vtable = "";
	
	CModel tmmsmgy05("TMMSMGY05");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);
	CModel tmmsm56("TMMSM56");
	EIClass tiaoHao2Tab;
	try
	{ 	

			begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
			end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
			tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			

			if (bcls_rec->Tables.Contains("tab"))   //确认tab页面
			{
				tab = bcls_rec->Tables["tab"].Rows[0]["TAB"].ToString();
			}

			if ((begin_time.Trim() == "" || end_time.Trim() == "")&&heat_no.Trim()=="")
			{
				sprintf(s.msg, "必须传入时间或是炉号。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			} 
			Log::Info("", __FUNCTION__, "tab =[{0}],dev_code=[{1}]", tab, tmmsmgy05["DEV_CODE"].ToString());
			
			
			

			if (end_time.Trim() != "")
			{
				sqlstr_where = sqlstr_where + " and start_time <=@end_time";
			}
			if (begin_time.Trim() != "")
			{
				sqlstr_where = sqlstr_where + " and start_time >=@begin_time";
			}

			if (tab == "1")
			{ 
				if (tmmsmgy05["SM_PLAN_NOL2"].ToString().Trim() != "")
				{
					sqlstr_where = sqlstr_where + " and SM_PLAN_NOL2 =@sm_plan_nol2";
				}
				if (tmmsmgy05["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_where = sqlstr_where + " and ST_NO LIKE trim(@st_no)||'%'";
				}

				if (tmmsmgy05["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_where = sqlstr_where + " and heat_no LIKE trim(@heat_no)||'%'";
				}
			

				//根据炉号信息取
				sqlstr = " select t1.sm_plan_nol2,t1.heat_no,t1.proc_no,t1.L2_PROC_NO,t1.DEV_CODE,t1.START_TIME,t1.end_time, t1.st_no,t1.HEATNO_PREMELT1,t1.HEATNO_PREMELT2,t1.HEATNO_PREMELT3,t1.HEATNO_PREMELT4,t1.HEATNO_PREMELT5,t1.HEATNO_PREMELT6,t1.WEIGHT_PREMELT1,t1.WEIGHT_PREMELT2,t1.WEIGHT_PREMELT3,MOLTIRON_WT "
					" ,nvl(t2.AFFIRM_FLAG,' ') AFFIRM_FLAG,nvl(t2.AFFIRM_TIME,' ') AFFIRM_TIME"
					" ,nvl(t2.RECV_MAT_TIME,' ') RECV_MAT_TIME"
					" ,nvl((select sum(mat_act_wt) from  (select sum(mat_act_wt) mat_act_wt from tmmsm01 where CONCESS_CON_FLAG!='1' and heat_no=t1.heat_no union all  select sum(mat_act_wt) mat_act_wt from hmmsm01 where mat_no not in (select IN_MAT_NO from tmmsm35 ) and CONCESS_CON_FLAG!='1' and heat_no=t1.heat_no)),0) mat_act_wt"
					" from ( "
					" select sm_plan_nol2,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3 "
					" from tmmsm27 "
					" where 1=1"
					+ sqlstr_where +
					" union all"
					" select sm_plan_nol2,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3"
					" from tmmsm21"
					" where 1=1 "
					" and st_no != 'DeP'  and st_no not like '1%'" + sqlstr_where +
					") t1"
					" left join tmmsmgy05 t2 on t1.heat_no=t2.heat_no"
					" where 1=1"
					;
				if (tmmsmgy05["PROC_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and ( t1.l2_PROC_NO =@proc_no or t1.HEATNO_PREMELT1=@proc_no or t1.HEATNO_PREMELT2=@proc_no or t1.HEATNO_PREMELT3=@proc_no  or t1.HEATNO_PREMELT4=@proc_no or t1.HEATNO_PREMELT5=@proc_no or t1.HEATNO_PREMELT6=@proc_no)";
				}
				if (tmmsmgy05["AFFIRM_FLAG"].ToString().Trim() == "1")
				{
					sqlstr = sqlstr  +" and t2.AFFIRM_FLAG = '1'";
				}
				if (tmmsmgy05["AFFIRM_FLAG"].ToString().Trim() == "0")
				{
					sqlstr = sqlstr  +" and t2.AFFIRM_FLAG in ( '0',' ')";
				}
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.Parameters.Set("st_no", tmmsmgy05["ST_NO"].ToString());
				cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("proc_no", tmmsmgy05["PROC_NO"].ToString());
				cmd_inq.Parameters.Set("sm_plan_nol2", tmmsmgy05["SM_PLAN_NOL2"].ToString());
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
			}

			if (tab == "2")	 //重复预熔液
			{
				sqlstr = " SELECT HEATNO_PREMELT1,sum(WEIGHT_PREMELT1) as WEIGHT_PREMELT1,count(1) count_num"
					" FROM(	"
					" SELECT HEATNO_PREMELT1 HEATNO_PREMELT1, WEIGHT_PREMELT1"
					" FROM tmmsm27  t2	"
					" WHERE 1 = 1  AND HEATNO_PREMELT1 <> ' ' " + sqlstr_where +
					" UNION ALL"
					" SELECT HEATNO_PREMELT2 HEATNO_PREMELT1, WEIGHT_PREMELT2 as WEIGHT_PREMELT1"
					" FROM tmmsm27 t2"
					" WHERE 1 = 1 AND HEATNO_PREMELT2 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT3 HEATNO_PREMELT1, WEIGHT_PREMELT3 as WEIGHT_PREMELT1"
					" FROM tmmsm27 t2"
					" WHERE 1 = 1 AND HEATNO_PREMELT3 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT4 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1"
					" FROM tmmsm27  t2"
					" WHERE 1 = 1 AND HEATNO_PREMELT4 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT5 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1  "
					" FROM tmmsm27 t2"
					" WHERE 1 = 1 AND HEATNO_PREMELT5 <> ' '" + sqlstr_where +
					" UNION ALL	 "
					" SELECT HEATNO_PREMELT6 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1 "
					" FROM tmmsm27 t2"
					" WHERE 1 = 1 AND HEATNO_PREMELT6 <> ' '" + sqlstr_where +
					" union all"
					" SELECT HEATNO_PREMELT1 HEATNO_PREMELT1, WEIGHT_PREMELT1"
					" FROM tmmsm21  t2	"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%'  AND HEATNO_PREMELT1 <> ' ' " + sqlstr_where +
					" UNION ALL"
					" SELECT HEATNO_PREMELT2 HEATNO_PREMELT1, WEIGHT_PREMELT2 as WEIGHT_PREMELT1"
					" FROM tmmsm21 t2"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%'  AND HEATNO_PREMELT2 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT3 HEATNO_PREMELT1, WEIGHT_PREMELT3 as WEIGHT_PREMELT1"
					" FROM tmmsm21 t2"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT3 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT4 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1"
					" FROM tmmsm21 t2"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT4 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT5 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1"
					" FROM tmmsm21 t2"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT5 <> ' ' " + sqlstr_where +
					" UNION ALL	"
					" SELECT HEATNO_PREMELT6 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1"
					" FROM tmmsm21 t2"
					" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT6 <> ' ' " + sqlstr_where +
					")  GROUP BY HEATNO_PREMELT1 HAVING  COUNT(1)  > 1"
					" order by HEATNO_PREMELT1"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr); 			
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.Parameters.Set("st_no", tmmsmgy05["ST_NO"].ToString());
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
				//取开始时间、结束时间和钢种
				if (!bcls_ret->Tables[0].Columns.Contains("START_TIME"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "START_TIME");
				}
				if (!bcls_ret->Tables[0].Columns.Contains("END_TIME"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "END_TIME");
				}
				if (!bcls_ret->Tables[0].Columns.Contains("ST_NO"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
				}
				for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
				{
					sqlstr = " select START_TIME,END_TIME,ST_NO"
						" from tmmsm21"
						" where 1=1" 						
						" and l2_proc_no = @l2_proc_no"
						" union "
						" select START_TIME,END_TIME,ST_NO"
						" from tmmsm27"
						" where l2_proc_no = @l2_proc_no"
						" union "
						" select START_TIME,END_TIME,ST_NO"
						" from tmmsm20"
						" where l2_proc_no = @l2_proc_no"
						" union "
						" select START_TIME,END_TIME,ST_NO"
						" from tmmsm26"
						" where l2_proc_no = @l2_proc_no"
						" union "
						" select START_TIME,END_TIME,ST_NO"
						" from tmmsm19"
						" where l2_proc_no = @l2_proc_no"
						" union "
						" select START_TIME,END_TIME,ST_NO"
						" from tmmsm14"
						" where l2_proc_no = @l2_proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("l2_proc_no", bcls_ret->Tables[0].Rows[i]["HEATNO_PREMELT1"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						bcls_ret->Tables[0].Rows[i]["START_TIME"] = cmd_inq.GetString(1);
						bcls_ret->Tables[0].Rows[i]["END_TIME"] = cmd_inq.GetString(2);
						bcls_ret->Tables[0].Rows[i]["ST_NO"] = cmd_inq.GetString(3);
					}
					cmd_inq.Close();
				}

			}

			if (tab == "3")	 //跳号
			{
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "")
				{
					sprintf(s.msg, "传入的工位不能为空。");
					//strcpy(s.sysmsg,s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "F")
				{
				
				sqlstr = "  WITH table1 AS (select heat_no, heatno_premelt1 as l2_proc_no from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  @dev_code||'%' and start_time <=  @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt2   as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
				" union	"
				" select heat_no, heatno_premelt1  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt2  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	 "
				" union	 "
				" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
				" )	"
				" select heat_no, l2_proc_no, st_no, START_TIME, END_TIME from(	"
				" select t1.heat_no, t1.l2_proc_no, nvl(t2.ST_NO, ' ') st_no, nvl(START_TIME, ' ') START_TIME, nvl(END_TIME, ' ') END_TIME from table1 t1 left join tmmsm19 t2 on t1.l2_proc_no = t2.L2_PROC_NO where t1.l2_proc_no in(select l2_proc_no from table1 group by l2_proc_no having count(1)>1)	"
				" union	 "
				" select ' ' heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
				" from tmmsm19 "
				" where L2_PROC_NO not in ( "
				" select  heatno_premelt1  from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  @dev_code||'%' and start_time <=  @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt2  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt3  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt4  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt5  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt6  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	"
				" select  heatno_premelt1  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt2  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	 "
				" union	 "
				" select  heatno_premelt3  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt4  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt5  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt6  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" )"
				" and start_time>to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')" 
				" ) order by END_TIME ,l2_proc_no	"
				;

				//Log::Info("", __FUNCTION__, "sqlstr =[{0}],dev_code=[{1}]", sqlstr, tmmsmgy05["DEV_CODE"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
				}
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "E")
				{
					sqlstr = "  WITH table1 AS (select heat_no, heatno_premelt1 as l2_proc_no from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  @dev_code||'%' and start_time <=  @end_time  and start_time >= @begin_time "
						" union	 "
						" select heat_no, heatno_premelt2   as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
						" union	 "
						" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" union	 "
						" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
						" union	 "
						" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
						" union	 "
						" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time "
						" union	"
						" select heat_no, heatno_premelt1  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" union	 "
						" select heat_no, heatno_premelt2  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	 "
						" union	 "
						" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" union	 "
						" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" union	 "
						" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" union	 "
						" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like @dev_code || '%' and start_time <= @end_time  and start_time >= @begin_time	"
						" )	"
						" select heat_no, l2_proc_no, st_no, START_TIME, END_TIME from(	"
						" select t1.heat_no, t1.l2_proc_no, nvl(t2.ST_NO, ' ') st_no, nvl(START_TIME, ' ') START_TIME, nvl(END_TIME, ' ') END_TIME from table1 t1 left join tmmsm20 t2 on t1.l2_proc_no = t2.L2_PROC_NO where t1.l2_proc_no in(select l2_proc_no from table1 group by l2_proc_no having count(1)>1)	"
						" union	 "
						" select ' ' heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
						" from tmmsm20"
						" where L2_PROC_NO not in ( " 
						" select  heatno_premelt1  from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  @dev_code||'%' and start_time <=  @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
						" union	 "
						" select  heatno_premelt2  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
						" union	 "
						" select  heatno_premelt3  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" union	 "
						" select  heatno_premelt4  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
						" union	 "
						" select  heatno_premelt5  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
						" union	 "
						" select  heatno_premelt6  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
						" union	"
						" select  heatno_premelt1  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" union	 "
						" select  heatno_premelt2  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	 "
						" union	 "
						" select  heatno_premelt3  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" union	 "
						" select  heatno_premelt4  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" union	 "
						" select  heatno_premelt5  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" union	 "
						" select  heatno_premelt6  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like @dev_code || '%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
						" ) and start_time>to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')"
						" ) order by END_TIME ,l2_proc_no	"
						;

					//Log::Info("", __FUNCTION__, "sqlstr =[{0}],dev_code=[{1}]", sqlstr, tmmsmgy05["DEV_CODE"].ToString());
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
				}
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "A")	 //不在连铸或是不在回炉或是
				{
					sqlstr = " select heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
						" from tmmsm27"
						" where   1=1"
						" and l2_proc_no not in (select l2_proc_no from tmmsmgy06 where HANDLE_DIV='Y')"
						" and l2_proc_no not in (select heat_no from tmmsm31 where start_time < to_char(to_date(@end_time,'yyyy-MM-dd HH24:MI:SS')+2,'yyyyMMddHHmmss') and start_time>@begin_time)"
						" and l2_proc_no not in (select heat_no from tpssm35 where RET_HEAT_NO !=' ')"
						" and START_TIME<=@end_time"
						" and START_TIME>=@begin_time"
						" order by END_TIME "
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
				}
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "B")	 //不在连铸或是不在回炉或是
				{
					sqlstr = " select heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
						" from tmmsm21"
						" where   1=1"
						" and l2_proc_no not in (select l2_proc_no from tmmsmgy06 where HANDLE_DIV='Y')"
						" and l2_proc_no not in (select heat_no from tmmsm31 where start_time < to_char(to_date(@end_time,'yyyy-MM-dd HH24:MI:SS')+2,'yyyyMMddHHmmss') and start_time>@begin_time)"
						" and l2_proc_no not in (select heat_no from tpssm35 where RET_HEAT_NO !=' ')"
						" and START_TIME<=@end_time"
						" and START_TIME>=@begin_time"
						" order by END_TIME "
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
				}
				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "AB")
				{
					sqlstr = " select heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
						" from tmmsm27"
						" where   1=1"
						" and l2_proc_no not in (select l2_proc_no from tmmsmgy06 where HANDLE_DIV='Y')"
						" and l2_proc_no not in (select heat_no from tmmsm31 where start_time < to_char(to_date(@end_time,'yyyy-MM-dd HH24:MI:SS')+2,'yyyyMMddHHmmss') and start_time>@begin_time)"
						" and l2_proc_no not in (select heat_no from tpssm35 where RET_HEAT_NO !=' ')"
						" and START_TIME<=@end_time"
						" and START_TIME>=@begin_time"
						" union all"
						" select heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
						" from tmmsm21"
						" where   1=1"
						" and l2_proc_no not in (select l2_proc_no from tmmsmgy06 where HANDLE_DIV='Y')"
						" and l2_proc_no not in (select heat_no from tmmsm31 where start_time < to_char(to_date(@end_time,'yyyy-MM-dd HH24:MI:SS')+2,'yyyyMMddHHmmss') and start_time>@begin_time)"
						" and l2_proc_no not in (select heat_no from tpssm35 where RET_HEAT_NO !=' ')"
						" and START_TIME<=@end_time"
						" and START_TIME>=@begin_time"
						" order by END_TIME "
						;
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
				}

				if (tmmsmgy05["DEV_CODE"].ToString().Trim() == "EF")
				{
					sqlstr = "  WITH table1 AS (select heat_no, heatno_premelt1 as l2_proc_no from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  (heatno_premelt1 like 'E%' or  heatno_premelt1 like 'F%') and start_time <=  @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt2   as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   (heatno_premelt2 like 'E%' or  heatno_premelt2 like 'F%') and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   (heatno_premelt3 like 'E%' or  heatno_premelt3 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   (heatno_premelt4 like 'E%' or  heatno_premelt4 like 'F%') and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   (heatno_premelt5 like 'E%' or  heatno_premelt5 like 'F%') and start_time <= @end_time  and start_time >= @begin_time "
				" union	 "
				" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   (heatno_premelt6 like 'E%' or  heatno_premelt6 like 'F%') and start_time <= @end_time  and start_time >= @begin_time "
				" union	"
				" select heat_no, heatno_premelt1  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   (heatno_premelt1 like 'E%' or  heatno_premelt1 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt2  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND  (heatno_premelt2 like 'E%' or  heatno_premelt2 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	 "
				" union	 "
				" select heat_no, heatno_premelt3  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   (heatno_premelt3 like 'E%' or  heatno_premelt3 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt4  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   (heatno_premelt4 like 'E%' or  heatno_premelt4 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt5  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   (heatno_premelt5 like 'E%' or  heatno_premelt5 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" union	 "
				" select heat_no, heatno_premelt6  as l2_proc_no from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   (heatno_premelt6 like 'E%' or  heatno_premelt6 like 'F%') and start_time <= @end_time  and start_time >= @begin_time	"
				" )	"
				" select heat_no, l2_proc_no, st_no, START_TIME, END_TIME from(	"
				" select t1.heat_no, t1.l2_proc_no, nvl(t2.ST_NO, ' ') st_no, nvl(START_TIME, ' ') START_TIME, nvl(END_TIME, ' ') END_TIME "
				" from table1 t1 left join tmmsm19 t2 on t1.l2_proc_no = t2.L2_PROC_NO "
				" where t1.l2_proc_no in ( select l2_proc_no from table1 where l2_proc_no like 'F%' group by l2_proc_no having count(1)>1 ) and t1.l2_proc_no like 'F%'"
				" union	 "
				" select ' ' heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
				" from tmmsm19 "
				" where L2_PROC_NO not in ( "
				" select  heatno_premelt1  from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  'F%' and start_time <=  @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt2  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt3  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt4  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt5  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt6  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	"
				" select  heatno_premelt1  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt2  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	 "
				" union	 "
				" select  heatno_premelt3  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	" 
				" union	 "
				" select  heatno_premelt4  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt5  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt6  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  'F%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" )"
				" and start_time>to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')" 				
				" union "  				
				" select t1.heat_no, t1.l2_proc_no, nvl(t2.ST_NO, ' ') st_no, nvl(START_TIME, ' ') START_TIME, nvl(END_TIME, ' ') END_TIME from table1 t1 "
				" left join tmmsm20 t2 on t1.l2_proc_no = t2.L2_PROC_NO "
				" where t1.l2_proc_no in (select l2_proc_no from table1 WHERE l2_proc_no like 'E%' group by l2_proc_no having count(1)>1)	and t1.l2_proc_no like 'E%'"
				" union	 "
				" select ' ' heat_no, l2_proc_no, st_no, START_TIME, END_TIME"
				" from tmmsm20"
				" where L2_PROC_NO not in ( "
				" select  heatno_premelt1  from tmmsm27 where 1=1   AND HEATNO_PREMELT1 <> ' ' AND  heatno_premelt1 like  'E%' and start_time <=  @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt2  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt3  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt4  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt5  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	 "
				" select  heatno_premelt6  from tmmsm27 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss') "
				" union	"
				" select  heatno_premelt1  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT1 <> ' ' AND   heatno_premelt1 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt2  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT2 <> ' ' AND   heatno_premelt2 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	 "
				" union	 "
				" select  heatno_premelt3  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT3 <> ' ' AND   heatno_premelt3 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt4  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT4 <> ' ' AND   heatno_premelt4 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt5  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT5 <> ' ' AND   heatno_premelt5 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" union	 "
				" select  heatno_premelt6  from tmmsm21 where 1 = 1   AND HEATNO_PREMELT6 <> ' ' AND   heatno_premelt6 like  'E%' and start_time <= @end_time  and start_time >= to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')	"
				" ) and start_time>to_char(to_date(@begin_time,'yyyy-MM-dd HH24:MI:SS')-1,'yyyyMMddHHmmss')"
				" )"
				" order by END_TIME ,l2_proc_no	"
				;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.Parameters.Set("dev_code", tmmsmgy05["DEV_CODE"].ToString().Trim());
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
				}

			}

			if (tab == "4")	 //AOD,转炉是否去连铸
			{ 				
				sqlstr = " select L2_PROC_NO,max(START_TIME) START_TIME,max(END_TIME) END_TIME,max(devo_time) devo_time from (  "
					" select L2_PROC_NO, START_TIME, END_TIME, ' ' devo_time from tmmsm21 where st_no != 'DeP'  and st_no not like '1%' AND START_TIME<=@end_time AND START_TIME>=@begin_time	"
					" union	 "
					" select l2_proc_no, START_TIME, END_TIME, ' '  devo_time from tmmsm27 where 1 = 1  AND START_TIME<=@end_time AND START_TIME>=@begin_time "
					" union "
					" select  l2_proc_no, ' ' START_TIME, ' ' END_TIME, max(devo_time) devo_time from tmmsm2a t1 where   EXISTS(select 1 from tmmsm21 t2 where t2.ST_NO != 'DeP'  and st_no not like '1%' and t1.l2_proc_no = t2.L2_PROC_NO)"
					" and DEV_CODE LIKE 'B%' AND devo_time<@end_time  AND devo_time>@begin_time	"
					" group by l2_proc_no   "
					" ) where L2_PROC_NO not in( select heat_no from tmmsm31 UNION"
					" SELECT HEAT_NO FROM TPSSM35  GROUP BY  HEAT_NO HAVING  SUM(RATE) >= 1) "
					" group by L2_PROC_NO ORDER BY END_TIME	 " 
					;

					//Log::Info("", __FUNCTION__, "sqlstr =[{0}],dev_code=[{1}]", sqlstr, tmmsmgy05["DEV_CODE"].ToString());
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();
			}

			if (tab == "5")	 //跳号2，预熔液之间是否有跳号的
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_START_TIME");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "L2_PROC_NO");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "TIAOHAO_SEQ");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "START_TIME");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "END_TIME");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "L2_PROC_NO_E");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO_E");
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO_S");
				int k = 0;

				sqlstr = 
					" SELECT MIN(L2_PROC_NO) STR_HEAT_NO,MAX(L2_PROC_NO) FIN_HEAT_NO,SUBSTR(L2_PROC_NO,1,2) DEV_CODE  FROM TMMSMGY06 WHERE HEAT_NO IN ("
					" SELECT HEAT_NO FROM TMMSMGY05 WHERE 1=1 AND START_TIME>=@begin_time AND END_TIME<=@end_time)"
					" AND (SUBSTR(L2_PROC_NO,1,1)='E'  OR SUBSTR(L2_PROC_NO,1,1)='F')"
					" GROUP BY SUBSTR(L2_PROC_NO,1,2)"
					" ORDER BY SUBSTR(L2_PROC_NO,1,2) ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					Log::Info("", __FUNCTION__, "0107sqlstr =[{0}]", sqlstr);
					min_heat_no = cmd_inq.GetString(1);
					max_heat_no = cmd_inq.GetString(2);
					Log::Info("", __FUNCTION__, "min_heat_no =[{0}]", min_heat_no);
					Log::Info("", __FUNCTION__, "max_heat_no =[{0}]", max_heat_no);
					Log::Info("", __FUNCTION__, "begin_time =[{0}]", begin_time);
					Log::Info("", __FUNCTION__, "end_time =[{0}]", end_time);
					sqlstr =
						" SELECT * FROM ("
						" SELECT PRE_HEAT_NO,NEXT_HEAT_NO,CAST(SUBSTR(PRE_HEAT_NO,3) AS INT) AS PRE1,CAST(SUBSTR(NEXT_HEAT_NO,3) AS INT) AS PRE2 FROM ("
						" SELECT L2_PROC_NO AS PRE_HEAT_NO,LEAD(L2_PROC_NO,1,' ') OVER (ORDER BY L2_PROC_NO) NEXT_HEAT_NO FROM ( "
						" SELECT DISTINCT L2_PROC_NO FROM TMMSMGY06 WHERE L2_PROC_NO <=@max_heat_no and L2_PROC_NO >=@min_heat_no"
						" ORDER BY L2_PROC_NO))) "
						" WHERE PRE2-PRE1<>1"
						;
					tiaoHao2Tab.Tables[0].Rows.Clear();
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("min_heat_no", min_heat_no);
					cmd_inq_1.Parameters.Set("max_heat_no", max_heat_no);
					cmd_inq_1.ExecuteQuery(tiaoHao2Tab.Tables[0]);
					cmd_inq_1.Close();
					
					for (int i = 0; i < tiaoHao2Tab.Tables[0].Rows.get_Count(); i++)
					{
						CString dev_code1 = tiaoHao2Tab.Tables[0].Rows[i]["PRE_HEAT_NO"].ToString().SubstringNE(0, 2);
						CString dev_code2 = tiaoHao2Tab.Tables[0].Rows[i]["NEXT_HEAT_NO"].ToString().SubstringNE(0, 2);
						int pre_heat_no1 = tiaoHao2Tab.Tables[0].Rows[i]["PRE1"].ToDouble();
						int pre_heat_no2 = tiaoHao2Tab.Tables[0].Rows[i]["PRE2"].ToDouble();
						//从跳号前一炉开始显示
						for (int j = pre_heat_no1; j < pre_heat_no2; j++)
						{
							bcls_ret->Tables[0].Rows.Add();
							sqlstr =
								" SELECT START_TIME,END_TIME,ST_NO FROM TMMSM19 WHERE L2_PROC_NO=@L2_PROC_NO "
								" UNION "
								" SELECT START_TIME,END_TIME,ST_NO FROM TMMSM20 WHERE L2_PROC_NO=@L2_PROC_NO "
								;
							cmd_inq_1.SetCommandText(sqlstr);
							cmd_inq_1.Parameters.Set("L2_PROC_NO", dev_code1 + CConvert::ToString(j));
							cmd_inq_1.ExecuteReader();
							if (cmd_inq_1.Read())
							{
								bcls_ret->Tables[0].Rows[k]["START_TIME"] = cmd_inq_1.GetString(1);
								bcls_ret->Tables[0].Rows[k]["END_TIME"] = cmd_inq_1.GetString(2);
								bcls_ret->Tables[0].Rows[k]["ST_NO"] = cmd_inq_1.GetString(3);
							}
							cmd_inq_1.Close();
							if (pre_heat_no2 - pre_heat_no1 > 10)
							{
								//取最大处理号的炉号和流通处理号
								sqlstr = " select heat_no,l2_proc_no"
									" from tmmsmgy06"
									" where l2_proc_no=@l2_proc_no"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("l2_proc_no", tiaoHao2Tab.Tables[0].Rows[i]["NEXT_HEAT_NO"].ToString());
								cmd_inq_1.ExecuteReader();
								if (cmd_inq_1.Read())
								{
									bcls_ret->Tables[0].Rows[k]["HEAT_NO_E"] = cmd_inq_1.GetString(1);
									bcls_ret->Tables[0].Rows[k]["L2_PROC_NO_E"] = cmd_inq_1.GetString(2);
								}
								cmd_inq_1.Close();
								//取最小处理号的炉号
								sqlstr = " select heat_no"
									" from tmmsmgy06"
									" where l2_proc_no=@l2_proc_no"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("l2_proc_no", tiaoHao2Tab.Tables[0].Rows[i]["PRE_HEAT_NO"].ToString());
								cmd_inq_1.ExecuteReader();
								if (cmd_inq_1.Read())
								{
									bcls_ret->Tables[0].Rows[k]["HEAT_NO_S"] = cmd_inq_1.GetString(1);
								}
								cmd_inq_1.Close();

								bcls_ret->Tables[0].Rows[k]["L2_PROC_NO"] = dev_code1 + CConvert::ToString(j);
								bcls_ret->Tables[0].Rows[k]["TIAOHAO_SEQ"] = pre_heat_no2 - pre_heat_no1-1;
								k++;
								break;
							}
							else
							{
								bcls_ret->Tables[0].Rows[k]["L2_PROC_NO"] = dev_code1 + CConvert::ToString(j);
								bcls_ret->Tables[0].Rows[k]["TIAOHAO_SEQ"] = 1;
								k++;
							}
						}
					}


					//重复号的
					sqlstr = " select t1.HEAT_NO,t1.START_TIME,t1.END_TIME,t2.ST_NO,t2.START_TIME,t1.L2_PROC_NO from tmmsmgy06 t1"
						" left join tmmsmgy05 t2 on t1.heat_no=t2.heat_no"
						" where t1.L2_PROC_NO in ( select L2_PROC_NO from tmmsmgy06 where L2_PROC_NO in (select L2_PROC_NO from tmmsmgy06 where L2_PROC_NO <=@max_heat_no and L2_PROC_NO >=@min_heat_no) group by l2_proc_no having count(1)>1) "
						" order by t1.END_TIME"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("min_heat_no", min_heat_no);
					cmd_inq_1.Parameters.Set("max_heat_no", max_heat_no);
					cmd_inq_1.ExecuteReader();
					while (cmd_inq_1.Read())
					{
						bcls_ret->Tables[0].Rows.Add();
						bcls_ret->Tables[0].Rows[k]["HEAT_NO"] = cmd_inq_1.GetString(1);
						bcls_ret->Tables[0].Rows[k]["START_TIME"] = cmd_inq_1.GetString(2);
						bcls_ret->Tables[0].Rows[k]["END_TIME"] = cmd_inq_1.GetString(3);
						bcls_ret->Tables[0].Rows[k]["ST_NO"] = cmd_inq_1.GetString(4);
						bcls_ret->Tables[0].Rows[k]["HEAT_START_TIME"] = cmd_inq_1.GetString(5);
						bcls_ret->Tables[0].Rows[k]["L2_PROC_NO"] = cmd_inq_1.GetString(6);
						k++;
					}
					cmd_inq_1.Close();

				}
				cmd_inq.Close();

				

			}

			if (tab == "6")	 //无消耗
			{
				sqlstr = " select heat_no,slab_cut_time,recv_mat_time from VMMSMCPCL_BB1 "
				" where RECEIVE_WEIGHT !=0 "
				" and heat_no not in ( select heat_no from tmmsmgy05 )"
				" AND REC_CREATE_TIME<=@end_time AND REC_CREATE_TIME>=@begin_time"
				;  
				//Log::Info("", __FUNCTION__, "sqlstr =[{0}],dev_code=[{1}]", sqlstr, tmmsmgy05["DEV_CODE"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
			}


			if (bcls_rec->Tables.Contains("gyadd"))   //工艺路径新增
			{ 
				if (begin_time.Trim() == "" || end_time.Trim() == "")
				{ 
					sprintf(s.msg, "传入的时间不能为空。");
					//strcpy(s.sysmsg,s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}],st_no=[{2}]", begin_time, end_time, tmmsmgy05["ST_NO"].ToString());
				if (tmmsmgy05["ST_NO"].ToString() == "111111")
				{  				

				EIClass bcls_ret2;
				EIClass bcls_rec2;
				sqlstr = "select heat_no,sm_plan_nol2 from tmmsm21 where   st_no != 'DeP'  and st_no not like '1%'   and start_time<=@end_time and start_time>=@begin_time "
					" union all"
					" select heat_no,sm_plan_nol2 from tmmsm27 where   start_time<=@end_time and start_time>=@begin_time  "
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("begin_time", begin_time);
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();

				EIClass bcls_ret1;
				EIClass bcls_rec1;
				bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);


				for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
				{
					tmmsmgy05.Reset();
					tmmsmgy05.MergeFrom(bcls_ret->Tables[0].Rows[i]);
					bcls_rec1.Tables[0].Rows.Clear();
					bcls_rec1.Tables[0].Rows.Add();
					bcls_rec1.Tables[0].Rows[0].Merge(bcls_ret->Tables[0].Rows[i]);
					//tmmsmgy05.MergeTo(bcls_rec1.Tables[0]);
					doFlag = f_mmsm_gyins2(&bcls_rec1, &bcls_ret1, conn);
					
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}
				}

				if (tmmsmgy05["ST_NO"].ToString() == "222222")
				{

					EIClass bcls_ret2;
					EIClass bcls_rec2;
					sqlstr = "select heat_no,sm_plan_nol2 from tmmsmgy05 "
					" where 1=1"
					//" and LOCK_FLAG = 'Y' "
						//" and VALID_FLAG_1!='1' "
						" and  recv_mat_time<=@end_time and recv_mat_time>=@begin_time  "
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
					cmd_inq.Close();

					EIClass bcls_ret1;
					EIClass bcls_rec1;
					bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);


					for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
					{
						tmmsmgy05.Reset();
						tmmsmgy05.MergeFrom(bcls_ret->Tables[0].Rows[i]);
						bcls_rec1.Tables[0].Rows.Clear();
						bcls_rec1.Tables[0].Rows.Add();
						bcls_rec1.Tables[0].Rows[0].Merge(bcls_ret->Tables[0].Rows[i]);
						//tmmsmgy05.MergeTo(bcls_rec1.Tables[0]);
						doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);

						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}
				}

				if (tmmsmgy05["ST_NO"].ToString() == "333333")
				{

					EIClass bcls_ret2;
					EIClass bcls_rec2;
					sqlstr = "select heat_no,sm_plan_nol2 from tmmsmgy05 "
						" where 1=1"
						" and  recv_mat_time<=@end_time and recv_mat_time>=@begin_time  "
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("begin_time", begin_time);
					cmd_inq.Parameters.Set("end_time", end_time);
					cmd_inq.ExecuteQuery(bcls_rec2.Tables[0]);
					cmd_inq.Close();

					if (bcls_rec2.Tables[0].Rows.get_Count() > 0)
					{
						doFlag = f_mmsm_t80r91_snd(&bcls_rec2, &bcls_ret2, conn);

						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
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
