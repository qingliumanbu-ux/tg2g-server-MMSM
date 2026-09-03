
/// <summary>
/// 功能说明:根据预熔液的数据修改，将过钢量和消耗进行分配
/// </summary>	

#include "stdafx.h"

BM2_FUNCTION_EXPORT
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 		
	CString sqlstr = " ";
	CString vtable = " ";
	CString heat_no = " ";
	CString heatno_premelt = "";
	CDecimal all_wt = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss"); 
	CDecimal seq_no = 0;
	
	CModel tpssm35("TPSSM35");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);
	
	try
	{
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			heatno_premelt = bcls_rec->Tables[0].Rows[i]["HEATNO_PREMELT"].ToString();
			heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();

			Log::Info("", __FUNCTION__, "heatno_premelt =[{0}],heat_no =[{1}]", heatno_premelt, heat_no);

			vtable = "( SELECT sm_plan_nol2,heat_no,sum(WEIGHT_PREMELT) WEIGHT_PREMELT,count(1) count_num "
				" FROM(	"
				" select  sm_plan_nol2 ,heat_no,sum(WEIGHT_PREMELT) WEIGHT_PREMELT from ("
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
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
				" FROM TMMSM27"
				" WHERE 1 = 1  AND HEATNO_PREMELT5 = @heatno_premelt"
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
				" FROM TMMSM27"
				" WHERE 1 = 1  AND HEATNO_PREMELT6 = @heatno_premelt"
				" union "
				" SELECT sm_plan_nol2 ,heat_no, WEIGHT_PREMELT1 as WEIGHT_PREMELT"
				" FROM TMMSM21	"
				" WHERE 1 = 1  AND HEATNO_PREMELT1 = @heatno_premelt"
				" UNION ALL"
				" SELECT sm_plan_nol2,heat_no, WEIGHT_PREMELT2 as WEIGHT_PREMELT"
				" FROM TMMSM21"
				" WHERE 1 = 1  AND HEATNO_PREMELT2 = @heatno_premelt"
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, WEIGHT_PREMELT3  as WEIGHT_PREMELT"
				" FROM TMMSM21"
				" WHERE 1 = 1  AND HEATNO_PREMELT3 = @heatno_premelt"
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
				" FROM TMMSM21"
				" WHERE 1 = 1  AND HEATNO_PREMELT4 = @heatno_premelt"
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
				" FROM TMMSM21"
				" WHERE 1 = 1  AND HEATNO_PREMELT5 = @heatno_premelt"
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no, 0  as WEIGHT_PREMELT"
				" FROM TMMSM21"
				" WHERE 1 = 1  AND HEATNO_PREMELT6 = @heatno_premelt"
				" )"
				" group by sm_plan_nol2,heat_no)"
				" group by sm_plan_nol2,heat_no)"
				;
			//查预熔液有哪些计划号
			sqlstr = " SELECT  sum(WEIGHT_PREMELT) as WEIGHT_PREMELT, count(1) count_num"
				" from "
				+ vtable 				
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

			//更新炉号信息
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

			sqlstr = " delete from tmmsmgy06 t1 where 1=1"
				" and HANDLE_DIV = 'Y' " 				
				" and l2_proc_no = @l2_proc_no "
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();				

			//插入工艺卡
			sqlstr = "  insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, DEV_CODE, WEIGHT_PREMELT1, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,@l2_proc_no ,SUBSTR(@l2_proc_no,1,2),WEIGHT_PREMELT,'Y',@all_wt"
				" from" + vtable + " t2"
				" where 1=1"
				" and heat_no = @heat_no"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("all_wt", all_wt);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heatno_premelt", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm21 t2"
				" where t2.L2_PROC_NO = @l2_proc_no"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm21 t2 where t2.L2_PROC_NO = @l2_proc_no)"
				" and l2_proc_no=@l2_proc_no"
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm27 t2"
				" where t2.L2_PROC_NO = @l2_proc_no"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm27 t2 where t2.L2_PROC_NO = @l2_proc_no)"
				" and l2_proc_no=@l2_proc_no"
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();



			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'1'"
				" from tmmsm20 t2"
				" where t2.L2_PROC_NO = @l2_proc_no"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm20 t2 where t2.L2_PROC_NO = @l2_proc_no)"
				" and l2_proc_no=@l2_proc_no"
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'0'"
				" from tmmsm26 t2"
				" where t2.L2_PROC_NO = @l2_proc_no"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm26 t2 where t2.L2_PROC_NO = @l2_proc_no)"
				" and l2_proc_no=@l2_proc_no"
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set  (DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, DURATION_TIME,TC_SEND_FLAG)="
				" (select  DEV_CODE, PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,'0'"
				" from tmmsm19 t2"
				" where t2.L2_PROC_NO = @l2_proc_no"
				" and rownum=1)"
				" where HANDLE_DIV = 'Y'"
				" and exists(select 1 from tmmsm19 t2 where t2.L2_PROC_NO = @l2_proc_no)"
				" and L2_PROC_NO=@l2_proc_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			//过精炼的预熔液，l2_proc_no 与电炉号是一致的
			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', @all_wt"
				" from tmmsm24 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no and t2.L2_PROC_NO = @l2_proc_no"
				" where  t1.L2_PROC_NO = @l2_proc_no "
				" and nvl(t2.HEAT_NO,' ') !=' '"
				" and t1.L2_PROC_NO like 'E%'"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("all_wt", all_wt);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', @all_wt"
				" from tmmsm23 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no and t2.L2_PROC_NO = @l2_proc_no"
				" where  t1.L2_PROC_NO = @l2_proc_no "
				" and nvl(t2.HEAT_NO,' ') !=' '"
				" and t1.L2_PROC_NO like 'E%'"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("all_wt", all_wt);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', @all_wt"
				" from tmmsm25 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no and t2.L2_PROC_NO = @l2_proc_no"
				" where  t1.L2_PROC_NO = @l2_proc_no "
				" and nvl(t2.HEAT_NO,' ') !=' '"
				" and t1.L2_PROC_NO like 'E%'"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("all_wt", all_wt);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy06(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, HEAT_NO, L2_PROC_NO, WEIGHT_PREMELT1, DEV_CODE, PROC_NO, ST_NO, STATION_ID, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, DURATION_TIME, HANDLE_DIV, HEAT_COUNT)"
				" select @rec_creator,@rec_create_time,t2.sm_plan_nol2, t2.HEAT_NO, t2.L2_PROC_NO, t2.WEIGHT_PREMELT1, t1.DEV_CODE, t1.PROC_NO, t1.ST_NO, t1.STATION_ID, t1.START_TIME, t1.END_TIME, t1.PROD_DATE, t1.PROD_SHIFT_NO, t1.PROD_SHIFT_GROUP"
				", case when t1.END_TIME != ' ' and t1.start_time != ' ' then ROUND((to_date(t1.END_TIME, 'yyyy-mm-dd hh24-mi-ss') - to_date(t1.START_TIME, 'yyyy-mm-dd hh24-mi-ss')) * 24 * 60, 0) else 0 end DURATION_TIME, 'Y', @all_wt"
				" from tmmsm26 t1"
				" left join tmmsmgy06 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO  and HANDLE_DIV = 'Y'	and t2.heat_no=@heat_no  and t2.L2_PROC_NO = @l2_proc_no"
				" where  t1.L2_PROC_NO = @l2_proc_no "
				" and nvl(t2.HEAT_NO,' ') !=' '"
				" and t1.L2_PROC_NO like 'E%'"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("all_wt", all_wt);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set DURATION_TIME = (select STD_TIME from tmmsmw2 t3 where 1=1 and t3.DEV_CODE = t1.dev_code)"
				" where 1=1"
				" and HANDLE_DIV = 'Y' "
				" and exists (select 1 from tmmsmw2 t3 where 1=1 and t3.HEAT_DURATION<t1.DURATION_TIME  and t3.DEV_CODE = t1.dev_code )"
				" and L2_PROC_NO=@l2_proc_no"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " update tmmsmgy06 t1 set DURATION_TIME = round(DURATION_TIME/ HEAT_COUNT ,0)"
				" where 1=1"
				" and HEAT_COUNT!=0"
				" and HANDLE_DIV = 'Y' "
				" and L2_PROC_NO=@l2_proc_no"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " delete from DA_HEAT_RELATION t"
				" where 1=1"
				" and not exists ( select 1 from tmmsmgy06 t2 where  t2.dev_code=t.AGGREGATE_NAME and t2.L2_PROC_NO = @l2_proc_no and t2.heat_no=t.HEAT_NUMBER)"
				" and heat_number !=PROC_NUMBER"
				" and PROC_NUMBER = @l2_proc_no"
				" and heat_number =@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			//更新另一个关系表
			sqlstr = " select max(ID) from DA_HEAT_RELATION";
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
				" from tmmsmgy06 t2"
				" where 1=1"
				" and not exists ( select 1 from DA_HEAT_RELATION t where  t2.dev_code=t.AGGREGATE_NAME and t.PROC_NUMBER = @l2_proc_no and t2.heat_no=t.HEAT_NUMBER)"
				" and HANDLE_DIV = 'Y' "
				" and l2_proc_no=@l2_proc_no"
				" and heat_no =@heat_no"
				;
			//Log::Info("", __FUNCTION__, "seq_no=[{1}],sqlstr =[{0}]", sqlstr, seq_no);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("seq_no", seq_no);
			cmd_inq_1.Parameters.Set("l2_proc_no", heatno_premelt);
			cmd_inq_1.Parameters.Set("dateNow", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close(); 				
		}

		//if (bcls_rec->Tables[0].Rows.get_Count() > 0)
		//{
		//	heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		//	//转炉的铁水修改
		//	sqlstr = " delete from tmmsmgy08 t1"
		//		" where 1=1"
		//		" and mat_code = 'TS0000'"
		//		" and heat_no=@heat_no"
		//		;
		//	cmd_inq_1.SetCommandText(sqlstr);
		//	cmd_inq_1.Parameters.Set("heat_no", heat_no);
		//	cmd_inq_1.ExecuteNonQuery();
		//	cmd_inq_1.Close();

		//	//更新铁水
		//	// 插入炉次信息
		//	sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
		//		" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME,case when  nvl((select DES_TREATMENT_NO from tmmsm21 t2 where t2.heat_no= t1.l2_proc_no and rownum=1),' ')=' ' then round(MOLTIRON_WT*RATIO_B/HEAT_COUNT*1000,0) else round(MOLTIRON_WT*RATIO_B*RATIO_KR/HEAT_COUNT*1000,0) end,HANDLE_DIV"
		//		" from ("
		//		" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,decode(HEAT_COUNT,0,1,HEAT_COUNT) HEAT_COUNT,HANDLE_DIV,START_TIME,RATIO_B,RATIO_KR"
		//		" from tmmsmgy06 t1 ,tmmsmw3 t3"
		//		" where 1=1"
		//		" and dev_code like 'B%'"
		//		" and heat_no=@heat_no"
		//		" ) t1 left join tmmsmgy05 t2 on t1.l2_proc_no=t2.heat_no"
		//		" where  nvl(MOLTIRON_WT,0)!=0"
		//		;
		//	//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("heat_no", heat_no);
		//	cmd_inq.Parameters.Set("rec_creator", s.userid);
		//	cmd_inq.Parameters.Set("rec_create_time", dateNow);
		//	cmd_inq.ExecuteNonQuery();
		//	cmd_inq.Close();

		//	sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
		//		" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME,case when  nvl((select DES_TREATMENT_NO from tmmsm21 t2 where t2.heat_no= t1.l2_proc_no and rownum=1),' ')=' ' then round(MOLTIRON_WT*RATIO_B/HEAT_COUNT*1000,0) else round(MOLTIRON_WT*RATIO_B*RATIO_KR/HEAT_COUNT*1000,0) end,HANDLE_DIV"
		//		" from ("
		//		" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,decode(HEAT_COUNT,0,1,HEAT_COUNT) HEAT_COUNT,HANDLE_DIV,START_TIME,RATIO_B,RATIO_KR"
		//		" from tmmsmgy06 t1 ,tmmsmw3 t3"
		//		" where 1=1"
		//		" and not exists(select 1 from tmmsmgy05 t4 WHERE t1.l2_proc_no = t4.heat_no)"
		//		" and dev_code like 'B%'"
		//		" and heat_no=@heat_no"
		//		" ) t1 left join tmmsm21 t2 on t1.l2_proc_no=t2.l2_proc_no"
		//		" where  nvl(MOLTIRON_WT,0)!=0"
		//		;
		//	//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("heat_no", heat_no);
		//	cmd_inq.Parameters.Set("rec_creator", s.userid);
		//	cmd_inq.Parameters.Set("rec_create_time", dateNow);
		//	cmd_inq.ExecuteNonQuery();
		//	cmd_inq.Close();

		//	sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
		//		" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME,case when t2.HEATNO_PREMELT1 like 'D%' or t2.HEATNO_PREMELT1 like 'H%' then  round(t2.WEIGHT_PREMELT1*RATIO_A*1000,0) else 0 end + case when t2.HEATNO_PREMELT2 like 'D%' or t2.HEATNO_PREMELT3 like 'H%' then  round(t2.WEIGHT_PREMELT2*RATIO_A*1000,0) else 0 end + case when t2.HEATNO_PREMELT3 like 'D%' or t2.HEATNO_PREMELT3 like 'H%' then  round(t2.WEIGHT_PREMELT3*RATIO_A*1000,0) else 0 end,HANDLE_DIV"
		//		" from ("
		//		" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,START_TIME,HANDLE_DIV,RATIO_A"
		//		" from tmmsmgy06 t1 ,tmmsmw3 t3"
		//		" where 1=1"
		//		" and dev_code like 'A%'"
		//		" and heat_no=@heat_no"
		//		") t1 left join tmmsmgy05 t2 on t1.l2_proc_no=t2.heat_no"
		//		" where 1=1"
		//		" and case when t2.HEATNO_PREMELT1 like 'D%' or t2.HEATNO_PREMELT1 like 'H%' then  t2.WEIGHT_PREMELT1*RATIO_A*1000 else 0 end + case when t2.HEATNO_PREMELT2 like 'D%' or t2.HEATNO_PREMELT3 like 'H%' then  t2.WEIGHT_PREMELT2*RATIO_A*1000 else 0 end + case when t2.HEATNO_PREMELT3 like 'D%' or HEATNO_PREMELT3 like 'H%' then  t2.WEIGHT_PREMELT3*RATIO_A*1000 else 0 end!=0"
		//		;
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("heat_no", heat_no);
		//	cmd_inq.Parameters.Set("rec_creator", s.userid);
		//	cmd_inq.Parameters.Set("rec_create_time", dateNow);
		//	cmd_inq.ExecuteNonQuery();
		//	cmd_inq.Close();

		//}		
		
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


