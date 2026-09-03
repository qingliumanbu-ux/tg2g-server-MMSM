
/// <summary>
/// 功能说明:根据时间,将炉次信息插入表中，以及消耗信息一起插入
/// </summary>


#include "stdafx.h"
#include "epex.h" 

BM2_FUNCTION_EXPORT
int f_mmsm_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString sqlstr = "";
	CString heat_no = "";
	CString lock_flag = "";
	CString send_xh = "";
	CString stat_date = "";
	CString vtable = "";
	CDecimal all_wt = 0;
	CString seq_id = "0";
	CDecimal seq_no = 0;
	CDecimal count_num = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString heatno_premelt = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_2(conn);
	
	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmhl("TMMSMHL");
	CModel tmmsm56a("TMMSM56A");
	CModel tmmsm56("TMMSM56");

	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].set_TableName("xh");
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();

		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString(); 
		tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr = " select LOCK_FLAG from tmmsmgy05"
			" where heat_no = @heat_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			lock_flag = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "heat_no =[{0}]", heat_no);

		if (heat_no.Trim()!="")
		{
			Log::Info("", __FUNCTION__, "heat_no =[{0}]", heat_no);	

			//判断是否进行了炉次更改
			sqlstr = " select count(1) from tmmsmgy05 "
				" where   1=1"				
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				count_num = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();
			
			if (count_num != 0)
			{ 				
					sqlstr = " update tmmsmgy05 t1 set (sm_plan_nol2,ST_NO,pono,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,CC_START_TIME,cast_div_no,td_no_1,recv_mat_time)="
						" (select sm_plan_nol2,ST_NO,pono,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3"
						" ,nvl((select max(LADLE_ARRIVE_TIME) CC_START_TIME from tmmsm31 t2 where t2.heat_no=t1.heat_no),' ') CC_START_TIME"
						" ,nvl((select max(cast_div_no) cast_div_no from tmmsm31 t2 where t2.heat_no=t1.heat_no),0) cast_div_no"
						" ,nvl((select max(td_no_1) td_no_1 from tmmsm31 t2 where t2.heat_no=t1.heat_no),' ') td_no_1"
						" ,nvl((select max(recv_mat_time) recv_mat_time from (select recv_mat_time from tmmsm01 t2 where t1.heat_no=t2.heat_no union all select recv_mat_time from hmmsm01 t2 where t1.heat_no=t2.heat_no)),' ') recv_mat_time"
						" from ("
						" select sm_plan_nol2,pono,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3 from tmmsm27 where 1=1  and heat_no=@heat_no"
						" union all"
						" select sm_plan_nol2,pono,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3, ' ' HEATNO_PREMELT4, ' ' HEATNO_PREMELT5, ' ' HEATNO_PREMELT6, WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3 from tmmsm21 where 1=1 and st_no != 'DeP' and st_no not like '1%'  and heat_no=@heat_no"
						") t2 where t1.heat_no = t2.heat_no and rownum=1)"
						" where 1=1"
						" and exists( select sm_plan_nol2, pono, heat_no, proc_no, L2_PROC_NO, DEV_CODE, START_TIME, end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6, WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3 from tmmsm27 where 1 = 1  and heat_no = @heat_no"
						" union all"
						" select sm_plan_nol2,pono,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3, ' ' HEATNO_PREMELT4, ' ' HEATNO_PREMELT5, ' ' HEATNO_PREMELT6, WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3 from tmmsm21 where 1=1 and st_no != 'DeP' and st_no not like '1%'  and heat_no=@heat_no"
						")"
						" and heat_no = @heat_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.Parameters.Set("rec_creator", s.userid);
					cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

					//获取浇次号信息
					sqlstr = " select nvl(CAST_NUMBER, '0') from ( select t.*,"
						" LAST_VALUE(t.cast_temp IGNORE NULLS) OVER(PARTITION BY t.aggregatecode ORDER BY t.ladleopentime rows BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS CAST_NUMBER	"
						" from(	"
						" select t.heatnumber, t.heatincast, t.castcounter, t.tundishnumber1, t.id, t.ladleopentime, t.aggregatecode, t.timestamp, "
						" case when t.tundishnumber1 = lead(t.tundishnumber1)over(partition by t.aggregatecode order by t.ladleopentime desc) then null else t.id end as cast_temp "
						" from DA_ccm_PRO_SUMMARY t  "
						" ) t where t.timestamp>sysdate - 3 order by t.aggregatecode, t.timestamp) t2 "
						" where HEATNUMBER = @heat_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						sqlstr = " update tmmsmgy05 t1 set CAST_DIV_NO_1 = @cast_div_no_1 "
							" where 1=1"
							" and heat_no = @heat_no"
							;
						cmd_inq_1.SetCommandText(sqlstr);
						cmd_inq_1.Parameters.Set("heat_no", heat_no);
						cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(1));
						cmd_inq_1.ExecuteNonQuery();
						cmd_inq_1.Close(); 
					}
					cmd_inq.Close(); 				
			}
			else
			{  				

			// 插入炉次信息
			sqlstr = " insert into tmmsmgy05(REC_CREATOR,REC_CREATE_TIME,SM_PLAN_NOL2,HEAT_NO,pono,proc_no,L2_PROC_NO,DEV_CODE,ST_NO,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,MOLTIRON_WT,LOCK_FLAG,CC_START_TIME,cast_div_no,td_no_1,recv_mat_time)"
				" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,pono,proc_no,L2_PROC_NO,DEV_CODE,ST_NO,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,MOLTIRON_WT,'N'"
				" ,nvl((select max(LADLE_ARRIVE_TIME) CC_START_TIME from tmmsm31 t2 where t2.heat_no=t1.heat_no),' ') CC_START_TIME"
				" ,nvl((select max(cast_div_no) cast_div_no from tmmsm31 t2 where t2.heat_no=t1.heat_no),0) cast_div_no"
				" ,nvl((select max(td_no_1) td_no_1 from tmmsm31 t2 where t2.heat_no=t1.heat_no),' ') td_no_1"
				" ,nvl((select max(recv_mat_time) recv_mat_time from (select recv_mat_time from tmmsm01 t2 where t1.heat_no=t2.heat_no union all select recv_mat_time from hmmsm01 t2 where t1.heat_no=t2.heat_no)),' ') recv_mat_time"
				" from ("
				" select sm_plan_nol2,heat_no,pono,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,0 MOLTIRON_WT from tmmsm27 where 1=1  and heat_no=@heat_no"
				" union all"
				" select sm_plan_nol2,heat_no,pono,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3, ' ' HEATNO_PREMELT4, ' ' HEATNO_PREMELT5, ' ' HEATNO_PREMELT6, WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3,MOLTIRON_WT from tmmsm21 where 1=1 and st_no != 'DeP' and st_no not like '1%'  and heat_no=@heat_no"
				") t1"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//获取浇次号信息
			sqlstr = " select nvl(CAST_NUMBER, '0') from ( select t.*,"
				" LAST_VALUE(t.cast_temp IGNORE NULLS) OVER(PARTITION BY t.aggregatecode ORDER BY t.ladleopentime rows BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS CAST_NUMBER	"
				" from(	"
				" select t.heatnumber, t.heatincast, t.castcounter, t.tundishnumber1, t.id, t.ladleopentime, t.aggregatecode, t.timestamp, "
				" case when t.tundishnumber1 = lead(t.tundishnumber1)over(partition by t.aggregatecode order by t.ladleopentime desc) then null else t.id end as cast_temp "
				" from DA_ccm_PRO_SUMMARY t  "
				" ) t where t.timestamp>sysdate - 3 order by t.aggregatecode, t.timestamp) t2 "
				" where HEATNUMBER = @heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				sqlstr = " update tmmsmgy05 t1 set CAST_DIV_NO_1 = @cast_div_no_1 "
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("heat_no", heat_no);
				cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(1));
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();
			}
			cmd_inq.Close();

			}

			// 先删除
			sqlstr = " delete from tmmsmgy06 t1"
				" where 1=1"
				" and HANDLE_DIV !='U'"	 //手动插入的数据不删除
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			// 插入精炼连铸炉次信息
			sqlstr = " insert into tmmsmgy06(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,DURATION_TIME,MOLTIRON_WT,WEIGHT_PREMELT1,TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO)"
				" select @rec_creator,@rec_create_time,sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,case when DURATION_TIME>9999 then 9999 when  DURATION_TIME<0 then 0 else DURATION_TIME end "
				",MOLTIRON_WT,WEIGHT_PREMELT1,TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from ("
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
				", MOLTIRON_WT"
				", 0 AS WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm21 t1 "
				" where 1=1 "
				" and st_no != 'DeP'  and st_no not like '1%'"
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
				",0 as MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm23 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
				",0 as MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm24 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
				",0 as MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm25 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME	"
				",0 as MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",0 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm26 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
				", 0 MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm27 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				" UNION ALL"
				" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
				",0 as MOLTIRON_WT"
				",0 as WEIGHT_PREMELT1"
				",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
				" from tmmsm31 t1"
				" where 1=1 "
				" and heat_no=@heat_no"
				") t1"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//插入预熔液信息
			sqlstr = " insert into tmmsmgy06(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,HEAT_NO,L2_PROC_NO,DEV_CODE,WEIGHT_PREMELT1,HANDLE_DIV,HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,@sm_plan_nol2,@heat_no,SUBSTR(HEATNO_PREMELT,1,8) ,SUBSTR(HEATNO_PREMELT,1,2),WEIGHT_PREMELT,'Y',1"
				" from ("
				" select HEATNO_PREMELT,sum(WEIGHT_PREMELT) WEIGHT_PREMELT from ("
				" SELECT  HEATNO_PREMELT1 HEATNO_PREMELT,WEIGHT_PREMELT1 as WEIGHT_PREMELT"
				" FROM TMMSM21	"
				" WHERE 1 = 1 and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT1 !=' '"
				" and heat_no = @heat_no"
				" UNION ALL"
				" SELECT  HEATNO_PREMELT2 HEATNO_PREMELT,WEIGHT_PREMELT2 as WEIGHT_PREMELT"
				" FROM TMMSM21	"
				" WHERE 1 = 1  and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT2 !=' '"
				" and heat_no = @heat_no"
				" UNION ALL	"
				" SELECT  HEATNO_PREMELT3 HEATNO_PREMELT,WEIGHT_PREMELT3 as WEIGHT_PREMELT"
				" FROM TMMSM21	"
				" WHERE 1 = 1  and st_no != 'DeP'  and st_no not like '1%' AND HEATNO_PREMELT3 !=' '"
				" and heat_no = @heat_no"
				" union all"
				" SELECT  HEATNO_PREMELT1 HEATNO_PREMELT,WEIGHT_PREMELT1 as WEIGHT_PREMELT"
				" FROM TMMSM27	"
				" WHERE 1 = 1  AND HEATNO_PREMELT1 !=' '"
				" and heat_no = @heat_no"
				" UNION ALL"
				" SELECT  HEATNO_PREMELT2 HEATNO_PREMELT,WEIGHT_PREMELT2 as WEIGHT_PREMELT"
				" FROM TMMSM27	"
				" WHERE 1 = 1  AND HEATNO_PREMELT2 !=' '"
				" and heat_no = @heat_no"
				" UNION ALL	"
				" SELECT  HEATNO_PREMELT3 HEATNO_PREMELT,WEIGHT_PREMELT3 as WEIGHT_PREMELT"
				" FROM TMMSM27	"
				" WHERE 1 = 1  AND HEATNO_PREMELT3 !=' '"
				" and heat_no = @heat_no"
				" UNION ALL	"
				" SELECT  HEATNO_PREMELT4 HEATNO_PREMELT,0 as WEIGHT_PREMELT"
				" FROM TMMSM27	"
				" WHERE 1 = 1  AND HEATNO_PREMELT4 !=' '"
				" and heat_no = @heat_no"
				" ) group by HEATNO_PREMELT"
				")"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.Parameters.Set("sm_plan_nol2", tmmsmgy05["SM_PLAN_NOL2"].ToString());
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			//更新电炉的信息
			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm21 t2"
				" where t2.L2_PROC_NO = t1.L2_PROC_NO"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm21 t2 where t2.L2_PROC_NO = t1.L2_PROC_NO)"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();
			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm27 t2"
				" where t2.L2_PROC_NO = t1.L2_PROC_NO"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm27 t2 where t2.L2_PROC_NO = t1.L2_PROC_NO)"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();
			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm20 t2"
				" where t2.L2_PROC_NO = t1.L2_PROC_NO"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm20 t2 where t2.L2_PROC_NO = t1.L2_PROC_NO)"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'0'"
				" from tmmsm26 t2"
				" where t2.L2_PROC_NO = t1.L2_PROC_NO"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm26 t2 where t2.L2_PROC_NO = t1.L2_PROC_NO)"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'0'"
				" from tmmsm19 t2"
				" where t2.L2_PROC_NO = t1.L2_PROC_NO"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm19 t2 where t2.L2_PROC_NO = t1.L2_PROC_NO)"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			//过精炼的预熔液，l2_proc_no 与电炉号是一致的
			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', 1"
				" from tmmsm24 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no"
				" where  t1.L2_PROC_NO in(select L2_PROC_NO from tmmsmgy06 where HANDLE_DIV = 'Y' and heat_no=@heat_no) "
				" and t1.L2_PROC_NO like 'E%'"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', 1"
				" from tmmsm23 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no"
				" where  t1.L2_PROC_NO in(select L2_PROC_NO from tmmsmgy06 where HANDLE_DIV = 'Y' and heat_no=@heat_no) "
				" and t1.L2_PROC_NO like 'E%'"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', 1"
				" from tmmsm25 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no"
				" where  t1.L2_PROC_NO in(select L2_PROC_NO from tmmsmgy06 where HANDLE_DIV = 'Y' and heat_no=@heat_no) "
				" and t1.L2_PROC_NO like 'E%'"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', 1"
				" from tmmsm26 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no"
				" where  t1.L2_PROC_NO in(select L2_PROC_NO from tmmsmgy06 where HANDLE_DIV = 'Y' and heat_no=@heat_no) "
				" and t1.L2_PROC_NO like 'E%'"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close(); 


			//更新时间的合理性
			sqlstr = " update tmmsmgy06 t1 set DURATION_TIME = (select STD_TIME from tmmsmw2 t3 where 1=1 and t3.DEV_CODE = t1.dev_code)"
				" where 1=1"
				" and exists (select 1 from tmmsmw2 t3 where 1=1 and t3.HEAT_DURATION<t1.DURATION_TIME  and t3.DEV_CODE = t1.dev_code )"
				" and heat_no=@heat_no"
				" and AFFIRM_FLAG !='1'"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();


			//插入预熔液的工艺,预熔液分配 
			sqlstr = " SELECT HEATNO_PREMELT1,count(1) count_num"
				" FROM(	"
				" SELECT HEATNO_PREMELT1 HEATNO_PREMELT1"
				" FROM TMMSM27	"
				" WHERE 1 = 1  AND HEATNO_PREMELT1 <> ' ' "
				" and heat_no = @heat_no"
				" UNION ALL"
				" SELECT HEATNO_PREMELT2 HEATNO_PREMELT1"
				" FROM TMMSM27"
				" WHERE 1 = 1 AND HEATNO_PREMELT2 <> ' ' "
				" and heat_no = @heat_no"
				" UNION ALL	"
				" SELECT HEATNO_PREMELT3 HEATNO_PREMELT1"
				" FROM TMMSM27"
				" WHERE 1 = 1 AND HEATNO_PREMELT3 <> ' ' "
				" and heat_no = @heat_no"
				" UNION ALL	"
				" SELECT HEATNO_PREMELT4 HEATNO_PREMELT1"
				" FROM TMMSM27 "
				" WHERE 1 = 1 AND HEATNO_PREMELT4 <> ' ' "
				" and heat_no = @heat_no"
				")  GROUP BY HEATNO_PREMELT1 "
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			EIClass bcls_rec_yry;
			EIClass bcls_ret_yry;
			bcls_rec_yry.Tables[0].Columns.Add(DT_STRING, "HEATNO_PREMELT");
			bcls_rec_yry.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");  			
			while (cmd_inq.Read())
			{
				heatno_premelt = cmd_inq.GetString(1);
				//插入预熔液的工艺
				sqlstr = " SELECT  sum(WEIGHT_PREMELT) as WEIGHT_PREMELT, count(1) count_num"
					" from "
					"( SELECT sm_plan_nol2,heat_no,sum(WEIGHT_PREMELT) WEIGHT_PREMELT,count(1) count_num "
					" FROM(	"
					" select distinct sm_plan_nol2 ,heat_no,sum(WEIGHT_PREMELT) WEIGHT_PREMELT from ("
					" SELECT sm_plan_nol2 ,heat_no, WEIGHT_PREMELT1 as WEIGHT_PREMELT"
					" FROM TMMSM27	"
					" WHERE 1 = 1  AND HEATNO_PREMELT1 = @heatno_premelt"
					" UNION ALL"
					" SELECT sm_plan_nol2,heat_no, WEIGHT_PREMELT2 as WEIGHT_PREMELT"
					" FROM TMMSM27"
					" WHERE 1 = 1  AND HEATNO_PREMELT2 = @heatno_premelt"
					" UNION ALL	"
					" SELECT sm_plan_nol2,heat_no, WEIGHT_PREMELT3  as WEIGHT_PREMELT"
					" FROM TMMSM27"
					" WHERE 1 = 1  AND HEATNO_PREMELT3 = @heatno_premelt"
					" UNION ALL	"
					" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
					" FROM TMMSM27"
					" WHERE 1 = 1  AND HEATNO_PREMELT4 = @heatno_premelt"
					" )"
					" group by sm_plan_nol2,heat_no)"
					" group by sm_plan_nol2,heat_no)"
					;
				//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq_s.SetCommandText(sqlstr);
				cmd_inq_s.Parameters.Set("heatno_premelt", heatno_premelt);
				cmd_inq_s.ExecuteReader();
				all_wt = 1;
				if (cmd_inq_s.Read())
				{
					all_wt = cmd_inq_s.GetDecimal(2);
				}
				cmd_inq_s.Close(); 				
				if (all_wt > 1)
				{ 
					sqlstr = " update tmmsmgy06 set HEAT_COUNT = @all_wt"
						" where 1=1"
						" and HANDLE_DIV = 'Y' "
						" and l2_proc_no = @l2_proc_no "
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("all_wt", all_wt);
					cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

					sqlstr = " select heat_no from tmmsmgy06"
						" where 1=1"
						" and HANDLE_DIV = 'Y'"
						" and heat_no !=@heat_no"
						" and l2_proc_no = @heatno_premelt"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("heatno_premelt", heatno_premelt);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.ExecuteReader();
					while (cmd_inq_1.Read())
					{
						bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq_1.GetString(1);
						doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					cmd_inq.Close();					
				}
			}
			cmd_inq.Close();  			
				

			//插入值
			sqlstr = "select count(1) from tmmsmgy05 where heat_no=@heat_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			count_num = 0;
			if (cmd_inq.Read())
			{
				count_num = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();

			if (count_num != 0)
			{  
				sqlstr = " select count(1) from DA_HEAT_RELATION_syn"
					" where HEAT_NUMBER = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				if (cmd_inq.ExecuteScalar().ToInt32() > 0)
				{
					sqlstr = " update DA_HEAT_RELATION_syn t1 set (AGGREGATE_NAME,GRADEACT,CONFIRM_READ,COMFIRM_TIME,START_TIME,END_TIME)=(select dev_code,st_no,'Y',to_date(@dateNow,'yyyy-mm-dd hh24-mi-ss'),to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss'),to_date(END_TIME,'yyyy-mm-dd hh24-mi-ss') from tmmsmgy05 t2 where t1.HEAT_NUMBER = t2.heat_no and t2.heat_no=@heat_no and rownum=1)"
						" where exists(select 1 from  tmmsmgy05 t2 where t1.HEAT_NUMBER = t2.heat_no)"
						" and HEAT_NUMBER=@heat_no"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.Parameters.Set("dateNow", dateNow);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();
				}
				else
				{
					sqlstr = " select max(ID) from DA_HEAT_RELATION_syn";
					seq_no = 1;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.ExecuteReader();
					if (cmd_inq_1.Read())
					{
						seq_no = cmd_inq_1.GetDecimal(1) + 1;
					}
					cmd_inq_1.Close();

					sqlstr = " insert into DA_HEAT_RELATION_syn(ID,AGGREGATE_NAME,HEAT_NUMBER,TIME_STAMP,CONFIRM_READ,COMFIRM_TIME,GRADEACT,START_TIME,END_TIME)"
						" select @seq_no,dev_code,heat_no,to_date(@dateNow,'yyyy-mm-dd hh24-mi-ss'),'Y',to_date(@dateNow,'yyyy-mm-dd hh24-mi-ss'),st_no,to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss'),to_date(END_TIME,'yyyy-mm-dd hh24-mi-ss')"
						" from tmmsmgy05"
						" where heat_no=@heat_no"
						" and rownum=1"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("seq_no", seq_no);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.Parameters.Set("dateNow", dateNow);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();
				}
				cmd_inq.Close();
			}
			else
			{
				sqlstr = " delete from DA_HEAT_RELATION_syn"
					" where HEAT_NUMBER=@heat_no"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close(); 
			}  		

			sqlstr = " select max(ID) from DA_HEAT_RELATION" ;
			seq_no = 1;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				seq_no = cmd_inq_1.GetDecimal(1) + 1;
			}
			cmd_inq_1.Close();
			sqlstr = " insert into DA_HEAT_RELATION(ID,AGGREGATE_NAME,HEAT_NUMBER,PROC_NUMBER,TIME_STAMP,CONFIRM_READ,COMFIRM_TIME,GRADEACT,HMWEIGHT,WEIGHT_PREMELT)"
				" select rownum+@seq_no,dev_code,heat_no,l2_proc_no,to_date(@dateNow,'yyyy-mm-dd hh24-mi-ss'),'N','',st_no,moltiron_wt,weight_premelt1"
				" from tmmsmgy06 t1"
				" where 1=1"
				" and not exists ( select 1 from DA_HEAT_RELATION t2 where t1.dev_code = t2.AGGREGATE_NAME and t1.l2_proc_no = t2.PROC_NUMBER and t2.HEAT_NUMBER=@heat_no)"
				" and heat_no=@heat_no"
				" order by START_TIME "
				;
			//Log::Info("", __FUNCTION__, "seq_no=[{1}],sqlstr =[{0}]", sqlstr, seq_no);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("seq_no", seq_no);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.Parameters.Set("dateNow", dateNow);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();	

			//删除多余的
			sqlstr = " delete from DA_HEAT_RELATION t2"
				" where 1=1"
				" and not exists ( select 1 from tmmsmgy06 t1 where t1.dev_code = t2.AGGREGATE_NAME and t1.l2_proc_no = t2.PROC_NUMBER and t1.heat_no=@heat_no)"
				" and HEAT_NUMBER=@heat_no"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
			doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}
		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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


