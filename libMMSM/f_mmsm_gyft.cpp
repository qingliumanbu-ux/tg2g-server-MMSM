
/// <summary>
/// 功能说明:根据时间,分钢种进行分摊
/// </summary>


#include "stdafx.h"
#include "epex.h" 

BM2_FUNCTION_EXPORT
int f_mmsm_gyft(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString vtable = "";
	CDecimal all_wt = 0;
	CString seq_id = "0";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString stat_date = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_2(conn);
	
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm56a("TMMSM56A");
	CModel tmmsm56("TMMSM56");

	try
	{
		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);


		sqlstr = " delete from tmmsm56"
			" where 1=1"
			" and HANDLE_DIV = 'I'"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//分钢种原来的数据进行分摊
		vtable = "("
			" select sm_plan_no,heat_no,st_no,sum(mat_act_wt) mat_act_wt from ("
			" select sm_plan_no,heat_no,st_no,mat_act_wt from tmmsm01 t1 where exists (select 1 from  tmmsmgy05 t2 where t1.sm_plan_no=t2.sm_plan_nol2 and t2.stat_date=@stat_date)"
			" union all "
			" select sm_plan_no,heat_no,st_no,mat_act_wt from hmmsm01 t1 where exists (select 1 from  tmmsmgy05 t2 where t1.sm_plan_no=t2.sm_plan_nol2 and t2.stat_date=@stat_date)"
			" ) group by sm_plan_no,heat_no,st_no"
			" )";
		sqlstr =
			" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
			" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
			" from ("
			" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
			" from " + vtable + " t1"
			//炉总产量
			" left join (select sm_plan_no,sum(mat_act_wt) all_wt from " + vtable + "  group by sm_plan_no) t2 on t1.sm_plan_no=t2.sm_plan_no"
			//机组消耗
			" left join ( select sm_plan_nol2,dev_code,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where stat_date=@stat_date group by sm_plan_nol2,dev_code,WEIGH_NO,DEVO_TIME,QUALITY_BATCH_NO,mat_code ) t3 on t1.sm_plan_no=t3.sm_plan_nol2"
			" where  nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0"
			")"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//尾插处理
		sqlstr = " select sm_plan_nol2,heat_no,dev_code,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(OUT_STOCK_WT) OUT_STOCK_WT"
			" from ("
			"select sm_plan_nol2,heat_no,dev_code,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,0-OUT_STOCK_WT OUT_STOCK_WT"
			" from tmmsm56"
			" where 1=1"
			" and  stat_date=@stat_date"
			" and HANDLE_DIV = 'I'"
			" union all"
			" select sm_plan_nol2,heat_no,dev_code,mat_code,DEVO_TIME as OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(DEVO_WT) OUT_STOCK_WT"
			" from tmmsmgy08"
			" where 1=1"
			" and  stat_date=@stat_date"
			" group by sm_plan_nol2,heat_no,dev_code,mat_code,DEVO_TIME ,WEIGH_NO,QUALITY_BATCH_NO"
			")"
			" group by sm_plan_nol2,heat_no,dev_code,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO"
			" having sum(OUT_STOCK_WT)!=0"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm56);
			sqlstr = " update tmmsm56 set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt,DEVO_WT = DEVO_WT+@out_stock_wt"
				" where 1=1"
				" and OUT_STOCK_WT in (select max(OUT_STOCK_WT) from tmmsm56 where  dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no=@quality_batch_no and HANDLE_DIV = 'I' and  sm_plan_nol2=@sm_plan_nol2 and stat_date=@stat_date)"
				" and dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no=@quality_batch_no  and  sm_plan_nol2=@sm_plan_nol2 "
				" and HANDLE_DIV = 'I'"
				" and  stat_date=@stat_date"
				" and rownum=1"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("out_stock_wt", tmmsm56["OUT_STOCK_WT"].ToDecimal());
			cmd_inq_1.Parameters.Set("dev_code", tmmsm56["DEV_CODE"].ToString());
			cmd_inq_1.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
			cmd_inq_1.Parameters.Set("out_stock_time", tmmsm56["OUT_STOCK_TIME"].ToString());
			cmd_inq_1.Parameters.Set("weigh_no", tmmsm56["WEIGH_NO"].ToString());
			cmd_inq_1.Parameters.Set("quality_batch_no", tmmsm56["QUALITY_BATCH_NO"].ToString());
			cmd_inq_1.Parameters.Set("sm_plan_nol2", tmmsm56["SM_PLAN_NOL2"].ToString());
			cmd_inq_1.Parameters.Set("stat_date", stat_date);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();
		}
		cmd_inq.Close();

		//更新钢种大类 ,更新记账日期
		sqlstr = " update tmmsm56 t1 set (steel_type,RECV_MAT_TIME)=(select max(steel_type),max(RECV_MAT_TIME) from tmmsmgy05 t2 where t1.SM_PLAN_NOL2=t2.SM_PLAN_NOL2 and t2.stat_date=@stat_date )"
			" where 1=1"
			" and exists(select 1 from tmmsmgy05 t2 where t1.SM_PLAN_NOL2=t2.SM_PLAN_NOL2 and t2.stat_date=@stat_date )"
			" and HANDLE_DIV = 'I'"
			" and  stat_date=@stat_date"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "update tmmsm56 t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" where 1=1"
			" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" and stat_date =@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//分摊完成将结果插入到表
		sqlstr = " delete from tmmsm2a_send"
			" where 1=1"
			" and send_flag in (' ','0')"
			" and stat_date = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " select max(SEQ_NO_2A) "
			" from tmmsm2a_send"
			" where stat_date =@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			seq_id = cmd_inq.GetString(1).SubstringNE(7, 8);
		}
		cmd_inq.Close();

		if (seq_id.Trim() == "")
		{
			seq_id = "0";
		}

		Log::Info("", __FUNCTION__, "seq_id =[{0}]", seq_id);

		sqlstr = " insert into tmmsm2a_send(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,prod_date,devo_wt,SEQ_NO_2A)"
			" select @rec_creator,@rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,prod_date,devo_wt ,@stat_date||trim(to_char(rownum+@seq_id, '00000000')) "
			" from ("
			" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,prod_date,sum(devo_wt) devo_wt "
			" from ("
			" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_TIME as DEVO_TIME,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
			" from tmmsm56"
			" where stat_date =@stat_date"
			" union all"
			" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,prod_date,0-devo_wt as devo_wt"
			" from tmmsm2a_send"
			" where 1=1"
			" and send_flag = '1'"
			" and stat_date =@stat_date"
			") group by stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,prod_date"
			" having sum(devo_wt)!=0"
			")"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.Parameters.Set("seq_id", atol(seq_id));
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		


		sqlstr = "update tmmsm2a_send t1 set (mat_name,SYSTEM_ID_MAT) = (select mat_name,SYSTEM_ID_MAT from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" where 1=1"
			" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" and send_flag != '1'"
			" and stat_date =@stat_date"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();



		
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


