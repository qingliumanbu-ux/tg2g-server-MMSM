
/// <summary>
/// 功能说明:根据时间,将炉次信息插入表中，以及消耗信息一起插入
/// </summary>


#include "stdafx.h"
#include "epex.h" 

BM2_FUNCTION_EXPORT
int f_mmsm_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyft(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq12(conn);
	
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmhl("TMMSMHL");

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();

		sqlstr_where = " and start_time<=@end_time and start_time>=@begin_time";

		// 先删除
		sqlstr = " delete from tmmsmgy05"
			" where 1=1 and LOCK_FLAG !='Y'" + sqlstr_where;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		// 插入炉次信息
		sqlstr = " insert into tmmsmgy05(REC_CREATOR,REC_CREATE_TIME,SM_PLAN_NOL2,HEAT_NO,proc_no,L2_PROC_NO,DEV_CODE,ST_NO,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,LOCK_FLAG,CC_START_TIME,recv_mat_time)"
			" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,proc_no,L2_PROC_NO,DEV_CODE,ST_NO,START_TIME,END_TIME,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,'N'"
			" ,nvl((select max(start_time) CC_START_TIME from tmmsm31 t2 where t2.heat_no=t1.heat_no),' ') CC_START_TIME"
			" ,nvl((select min(recv_mat_time) recv_mat_time from (select recv_mat_time from tmmsm01 t2 where t1.heat_no=t2.heat_no union all select recv_mat_time from hmmsm01 t2 where t1.heat_no=t2.heat_no)),' ') recv_mat_time"
			" from ("
			" select sm_plan_nol2,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3 from tmmsm27 where 1=1" + sqlstr_where +
			" union all"
			" select sm_plan_nol2,heat_no,proc_no,L2_PROC_NO,DEV_CODE,START_TIME,end_time, st_no, HEATNO_PREMELT1, HEATNO_PREMELT2, HEATNO_PREMELT3, ' ' HEATNO_PREMELT4, ' ' HEATNO_PREMELT5, ' ' HEATNO_PREMELT6, WEIGHT_PREMELT1, WEIGHT_PREMELT2, WEIGHT_PREMELT3 from tmmsm21 where 1=1 and st_no != 'DeP'" + sqlstr_where +
		
			") t1"
			" where heat_no not in (select heat_no from tmmsmgy05 where LOCK_FLAG = 'Y' )"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新会计期
		sqlstr = " update tmmsmgy05 t1 set STAT_DATE = substr(recv_mat_time,1,6)"
			" where 1=1"
			" and STAT_DATE = ' '"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新碳钢不锈钢区分
		sqlstr = " update tmmsmgy05 t1 set steel_type = decode(substr(ST_NO,0,1),'1',decode(substr(ST_NO,2,1),'A','镍钢','D','镍钢','F','铬钢','M','铬钢','不锈钢'),'2','碳钢','3','硅钢','4',decode(substr(ST_NO,2,1),'A','镍钢','D','镍钢','F','铬钢','M','铬钢','不锈钢'),'5','碳钢',' ')"
			" where 1=1"
			" and st_no!=' '"
			+ sqlstr_where;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		

		// 先删除
		sqlstr = " delete from tmmsmgy06 t1"
			" where 1=1" 
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.heat_no = t2.heat_no   " + sqlstr_where+")"
			" and AFFIRM_FLAG !='1'"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//// 插入炉次信息
		//sqlstr = " insert into tmmsmgy06(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,DURATION_TIME,MOLTIRON_WT,WEIGHT_PREMELT1,TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO)"
		//	" select @rec_creator,@rec_create_time,sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME,case when DURATION_TIME>9999 then 9999 when  DURATION_TIME<0 then 0 else DURATION_TIME end "
		//	",MOLTIRON_WT,WEIGHT_PREMELT1,TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from ("
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",0 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm19 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm20	t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
		//	", MOLTIRON_WT"
		//	", WEIGHT_PREMELT1+WEIGHT_PREMELT2+WEIGHT_PREMELT3 AS WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm21 t1 "
		//	" where 1=1 "			
		//	" and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"		
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm23 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm24 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME"
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm25 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME	"
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",0 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm26 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
		//	", 0 MOLTIRON_WT"
		//	",WEIGHT_PREMELT1+WEIGHT_PREMELT2+WEIGHT_PREMELT3 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm27 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	" UNION ALL"
		//	" select sm_plan_nol2,HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO,ST_NO,STATION_ID, START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME "
		//	",0 as MOLTIRON_WT"
		//	",0 as WEIGHT_PREMELT1"
		//	",1 as TC_SEND_FLAG,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,PONO"
		//	" from tmmsm31 t1"
		//	" where 1=1 and exists (select 1 from tmmsmgy05 t2 where    1=1 AND LOCK_FLAG LIKE 'N%' and  t1.HEAT_NO = t2.HEAT_NO   " + sqlstr_where + ")"
		//	") "
		//	;
		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("begin_time", begin_time);
		//cmd_inq.Parameters.Set("end_time", end_time);
		//cmd_inq.Parameters.Set("rec_creator", s.userid);
		//cmd_inq.Parameters.Set("rec_create_time", dateNow);
		//cmd_inq.ExecuteNonQuery();
		//cmd_inq.Close(); 	


		////更新时间的合理性
		//sqlstr = " update tmmsmgy06 t1 set DURATION_TIME = (select STD_TIME from tmmsmw2 t3 where 1=1 and t3.DEV_CODE = t1.dev_code)"
		//	" where 1=1"
		//	" and exists (select 1 from tmmsmw2 t3 where 1=1 and t3.HEAT_DURATION<t1.DURATION_TIME  and t3.DEV_CODE = t1.dev_code )"
		//	" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO   and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
		//	" and AFFIRM_FLAG !='1'"
		//	;
		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("begin_time", begin_time);
		//cmd_inq.Parameters.Set("end_time", end_time);
		//cmd_inq.ExecuteNonQuery();
		//cmd_inq.Close();

		//插入消耗值
		sqlstr = " delete from tmmsmgy08 t1"
			" where 1=1"
			" and HANDLE_DIV='I'"
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO   and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		// 插入炉次信息
		sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
			" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,'TS0000',START_TIME,case when DES_TREATMENT_NO=' ' then MOLTIRON_WT*RATIO_B*1000 else MOLTIRON_WT*RATIO_B*RATIO_KR*1000 end,'I'"
			" from tmmsm21 t1 ,tmmsmw3 t3"
			" where 1=1"
			" and MOLTIRON_WT!=0"
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO    and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
			" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,'TS0000',START_TIME,case when HEATNO_PREMELT1 like 'D%' or HEATNO_PREMELT1 like 'H%' then  WEIGHT_PREMELT1*RATIO_A*1000 else 0 end + case when HEATNO_PREMELT2 like 'D%' or HEATNO_PREMELT3 like 'H%' then  WEIGHT_PREMELT2*RATIO_A*1000 else 0 end + case when HEATNO_PREMELT3 like 'D%' or HEATNO_PREMELT3 like 'H%' then  WEIGHT_PREMELT3*RATIO_A*1000 else 0 end,'I'"
			" from tmmsm27 t1 ,tmmsmw3 t3"
			" where 1=1"
			" and WEIGHT_PREMELT1+WEIGHT_PREMELT2+WEIGHT_PREMELT3 !=0"
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO    and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmgy05 t1 set MOLTIRON_WT=(select sum(DEVO_WT) from tmmsmgy08 t2 where 1=1 and t2.mat_code = 'TS0000' and  t1.HEAT_NO = t2.HEAT_NO  )"
			" where 1=1"
			" and exists (select 1 from tmmsmgy08 t2 where 1=1 and t2.mat_code = 'TS0000' and  t1.HEAT_NO = t2.HEAT_NO  )"
			" AND LOCK_FLAG LIKE 'N%'    and start_time <= @end_time  and start_time >= @begin_time"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
			" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,sum(DEVO_WT) DEVO_WT,'I'"
			" from tmmsm2a_yl t1"
			" where 1=1"
			" and mat_code not in (select mat_code from tmmsm50 where SEND_FLAG = '1')"
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO    and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
			" group by  sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmgy08 t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" where 1=1"
			" and exists (select 1 from tmmsmgy05 t2 where LOCK_FLAG LIKE 'N%'  and  t1.HEAT_NO = t2.HEAT_NO    and t2.start_time<=@end_time  and t2.start_time>=@begin_time)"
			" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)" 
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新会计期
		sqlstr = " update tmmsmgy06 t1 set STAT_DATE = (select max(STAT_DATE) from tmmsmgy05 t2 where  t1.HEAT_NO = t2.HEAT_NO )"
			" where 1=1"
			" and exists( select 1 from tmmsmgy05 t2 where  t1.HEAT_NO = t2.HEAT_NO )"
			" and STAT_DATE = ' '"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新会计期
		sqlstr = " update tmmsmgy08 t1 set STAT_DATE = (select max(STAT_DATE) from tmmsmgy05 t2 where  t1.HEAT_NO = t2.HEAT_NO )"
			" where 1=1"
			" and exists( select 1 from tmmsmgy05 t2 where  t1.HEAT_NO = t2.HEAT_NO )"
			" and STAT_DATE = ' '"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		////预熔液分配
		//EIClass bcls_rec_yry;
		//sqlstr = " SELECT SM_PLAN_NOL2,count(1) count_num"
		//	" FROM(	"
		//	" SELECT SM_PLAN_NOL2,HEATNO_PREMELT1 HEATNO_PREMELT1, WEIGHT_PREMELT1"
		//	" FROM TMMSM27	"
		//	" WHERE 1 = 1  AND HEATNO_PREMELT1 <> ' ' " + sqlstr_where +
		//	" UNION ALL"
		//	" SELECT SM_PLAN_NOL2,HEATNO_PREMELT2 HEATNO_PREMELT1, WEIGHT_PREMELT2 as WEIGHT_PREMELT1"
		//	" FROM TMMSM27"
		//	" WHERE 1 = 1 AND HEATNO_PREMELT2 <> ' ' " + sqlstr_where +
		//	" UNION ALL	"
		//	" SELECT SM_PLAN_NOL2,HEATNO_PREMELT3 HEATNO_PREMELT1, WEIGHT_PREMELT3 as WEIGHT_PREMELT1"
		//	" FROM TMMSM27"
		//	" WHERE 1 = 1 AND HEATNO_PREMELT3 <> ' ' " + sqlstr_where +
		//	" UNION ALL	"
		//	" SELECT SM_PLAN_NOL2,HEATNO_PREMELT4 HEATNO_PREMELT1, 0 as WEIGHT_PREMELT1"
		//	" FROM TMMSM27 "
		//	" WHERE 1 = 1 AND HEATNO_PREMELT4 <> ' ' " + sqlstr_where +
		//	/*" UNION ALL	"
		//	" SELECT HEATNO_PREMELT5 HEATNO_PREMELT1, WEIGHT_PREMELT5 as WEIGHT_PREMELT1  "
		//	" FROM TMMSM27 "
		//	" WHERE 1 = 1 AND HEATNO_PREMELT5 <> ' '" + sqlstr_where +
		//	" UNION ALL	 "
		//	" SELECT HEATNO_PREMELT6 HEATNO_PREMELT1, WEIGHT_PREMELT6 as WEIGHT_PREMELT1 "
		//	" FROM TMMSM27 "
		//	" WHERE 1 = 1 AND HEATNO_PREMELT6 <> ' '" + sqlstr_where+	 */
		//	")  GROUP BY SM_PLAN_NOL2 HAVING  COUNT(1)  > 1"
		//	" order by SM_PLAN_NOL2"
		//	;
		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("begin_time", begin_time);
		//cmd_inq.Parameters.Set("end_time", end_time);
		//cmd_inq.ExecuteQuery(bcls_rec_yry.Tables[0]);
		//cmd_inq.Close();
		//if (bcls_rec_yry.Tables[0].Rows.get_Count() > 0)
		//{
		//	doFlag = f_mmsm_yry(&bcls_rec_yry, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}

		//根据钢种分摊
		EIClass bcls_rec_ft;
		bcls_rec_ft.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
		bcls_rec_ft.Tables[0].Rows.Add();
		if (begin_time.SubstringNE(0, 6).Trim() != "")
		{			
			bcls_rec_ft.Tables[0].Rows[0]["STAT_DATE"] = begin_time.SubstringNE(0, 6);
			doFlag = f_mmsm_gyft(&bcls_rec_ft, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (end_time.SubstringNE(0, 6).Trim() != ""&&begin_time.SubstringNE(0, 6) != end_time.SubstringNE(0, 6))
		{
			bcls_rec_ft.Tables[0].Rows[0]["STAT_DATE"] = end_time.SubstringNE(0, 6);
			doFlag = f_mmsm_gyft(&bcls_rec_ft, bcls_ret, conn);
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


