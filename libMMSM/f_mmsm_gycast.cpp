
/// <summary>
/// 功能说明:根据回炉信息，将实绩的重量及过钢量插入到表里，原炉号扣除，新炉号新增
/// </summary> 

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT	
int f_mmsm_gyhl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString heat_no = " "; 	
	CString ret_heat_no = " ";
	CString sqlstr = " ";
	CDecimal all_wt = 0;
	CString vtable = " ";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	CModel tpssm35("TPSSM35");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmhl("TMMSMHL");
	
	try
	{
		//
		//判断是回炉调还是工艺路线确认调,1表示回炉，2表示工艺路线
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tpssm35.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tpssm35["RET_HEAT_NO"].ToString() != ""&&tpssm35["HEAT_NO"].ToString() != "")
			{ 
				//将工艺路径的的信息插入
				sqlstr = " delete tmmsmgy07"
					" where 1=1"
					" and heat_no_old = @heat_no"
					" and heat_no=@ret_heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();	


				//分钢种原来的数据进行分摊
				vtable = "("
					" select sm_plan_no,heat_no,st_no,sum(mat_act_wt) mat_act_wt from ("
					" select sm_plan_no,heat_no,st_no,mat_act_wt from tmmsm01 t1 where heat_no=@ret_heat_no"
					" union all "
					" select sm_plan_no,heat_no,st_no,mat_act_wt from hmmsm01 t1 where mat_no not in (select IN_MAT_NO from tmmsm35 ) and heat_no=@ret_heat_no"
					" ) group by sm_plan_no,heat_no,st_no"
					" )";
				sqlstr =
					" insert into tmmsmgy07(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,st_no,heat_no_old,l2_proc_no,dev_code,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,DURATION_TIME,HANDLE_DIV,RETURN_MLSL)"
					" select @rec_creator,@rec_create_time,t1.sm_plan_no,t1.heat_no,t1.st_no,@heat_no,t2.l2_proc_no,t2.dev_code,t2.START_TIME,t2.END_TIME,t2.PROD_DATE,t2.PROD_SHIFT_NO,t2.PROD_SHIFT_GROUP,round(@rate*t2.DURATION_TIME,0),'H',@rate"
					" from " + vtable + " t1"
					//炉总产量
					" left join (select heat_no,sum(mat_act_wt) all_wt from " + vtable + "  group by heat_no) t3 on t1.heat_no=t3.heat_no"
					//机组消耗
					" left join tmmsmgy06 t2 on t2.heat_no=@heat_no"
					" where  nvl(all_wt,0)!=0 and nvl(l2_proc_no,' ')!=' '"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("rec_creator", s.userid);
				cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
				cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("rate", tpssm35["RATE"].ToDecimal());
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close(); 	
				

				//插入消耗
				sqlstr = " delete  from tmmsmgy07A"
					" where  1=1"
					" and heat_no_old = @heat_no"
					" and HANDLE_DIV = 'H'"
					" and heat_no=@ret_heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();

				//分钢种原来的数据进行分摊
				vtable = "("
					" select sm_plan_no,heat_no,st_no,sum(mat_act_wt) mat_act_wt from ("
					" select sm_plan_no,heat_no,st_no,mat_act_wt from tmmsm01 t1 where heat_no=@ret_heat_no"
					" union all "
					" select sm_plan_no,heat_no,st_no,mat_act_wt from hmmsm01 t1 where mat_no not in (select IN_MAT_NO from tmmsm35 ) and heat_no=@ret_heat_no"
					" ) group by sm_plan_no,heat_no,st_no"
					" )";
				sqlstr =
					" insert into tmmsmgy07A(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no_old,heat_no,st_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,DEVO_WT,HANDLE_DIV,STK_NO,CHARGE_TYPE)"
					" select @rec_creator,@rec_create_time,t1.sm_plan_no,@heat_no,@ret_heat_no,t1.st_no,t3.l2_proc_no,t3.PROC_NO,t3.dev_code,t3.mat_code,t3.ID_2A,t3.PROC_COUNT,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,t3.DEVO_TIME,round(t3.DEVO_WT*@rate/t2.all_wt*t1.mat_act_wt ,0) DEVO_WT,'H',t3.STK_NO,t3.CHARGE_TYPE"
					" from " + vtable + " t1"
					//炉总产量
					" left join (select heat_no,sum(mat_act_wt) all_wt from " + vtable + "  group by heat_no) t2 on t1.heat_no=t2.heat_no"
					//机组消耗
					" left join tmmsmgy08 t3 on t3.heat_no=@heat_no"
					" where  nvl(all_wt,0)!=0 and nvl(DEVO_WT,0)!=0"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("rec_creator", s.userid);
				cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
				cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("rate", tpssm35["RATE"].ToDecimal());
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();

				sqlstr = " update tmmsmgy07a t1 set LOT_NO = (select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					" where exists(select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					//" and heat_no_old = @heat_no_old"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("heat_no_old", tpssm35["HEAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " update tmmsmgy07a t1 set RECV_MAT_TIME = (select RECV_MAT_TIME from tmmsmgy05 t2 where t1.heat_no=t2.heat_no)"
					" where exists(select 1 from tmmsmgy05 t2 where t1.heat_no=t2.heat_no)"
					//" and heat_no_old = @heat_no_old"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("heat_no_old", tpssm35["HEAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " update tmmsmgy07a t1 set MAT_NAME = (select MAT_NAME from TMMSM50 t2 where t1.MAT_CODE=t2.MAT_CODE)"
					" where exists(select MAT_NAME from TMMSM50 t2 where t1.MAT_CODE=t2.MAT_CODE)"
					//" and heat_no_old = @heat_no_old"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("heat_no_old", tpssm35["HEAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();



			/*	sqlstr = " insert into tmmsmgy07A(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no_old,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,DEVO_WT,HANDLE_DIV,STK_NO,CHARGE_TYPE)"
					" select @rec_creator,@rec_create_time,sm_plan_nol2,@heat_no,@ret_heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,round(DEVO_WT*@rate,0) DEVO_WT,'H',STK_NO,CHARGE_TYPE"
					" from  tmmsmgy08 t2 "
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("rec_creator", s.userid);
				cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
				cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
				cmd_inq_1.Parameters.Set("rate", tpssm35["RATE"].ToDecimal());
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();	*/
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


