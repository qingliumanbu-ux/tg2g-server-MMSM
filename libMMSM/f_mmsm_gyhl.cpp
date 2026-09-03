
/// <summary>
/// 功能说明:根据回炉信息，将实绩的重量及过钢量插入到表里，原炉号扣除，新炉号新增
/// </summary> 

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyhl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString heat_no = " "; 	
	CString heat_no_e = " ";
	CString ret_heat_no = " ";
	CString sqlstr = " ";
	CDecimal all_wt = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);

	CModel tpssm35("TPSSM35");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmhl("TMMSMHL");
	
	try
	{
		//
		//判断是回炉调还是工艺路线确认调,1表示回炉，2表示工艺路线
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
			if (heat_no.Trim() != "")
			{

				//先删除所有的回炉信息
				sqlstr = " delete from tmmsmgy07"
					" where 1=1"
					" and HANDLE_DIV = 'H'"
					" and HEAT_NO_OLD = @heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("heat_no", heat_no);
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();

				sqlstr = " delete from tmmsmgy07a"
					" where 1=1"
					" and HANDLE_DIV = 'H'"
					" and HEAT_NO_OLD = @heat_no"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("heat_no", heat_no);
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();

				//取得heat_no,然后针对所有的回炉信息进行重新核算
				sqlstr = " select ret_heat_no,heat_no,rate,sum(rate) over() all_rate"
					" from tpssm35"
					" where heat_no=@heat_no"
					" order by rate ,ret_heat_no desc"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				heat_no_e = "";
				while (cmd_inq.Read())
				{
					if (cmd_inq.GetDecimal(4) == 1 && cmd_inq.GetDecimal(3) != 1)
					{
						heat_no_e = cmd_inq.GetString(1);
					}

					sqlstr = " insert into tmmsmgy07(REC_CREATOR, REC_CREATE_TIME, heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,DURATION_TIME ,RETURN_MLSL,HEAT_NO_OLD,RET_HEAT_NO,HANDLE_DIV)"
						" select @rec_creator,@rec_create_time,@ret_heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,round(DURATION_TIME*@rate,0) ,@rate,HEAT_NO,@ret_heat_no,'H'"
						" from tmmsmgy06 t1"
						" where  1=1"
						" and dev_code not like 'C%'"
						" and heat_no = @heat_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("rec_creator", s.userid);
					cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
					cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
					cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
					cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

					sqlstr = " insert into tmmsmgy07(REC_CREATOR, REC_CREATE_TIME, heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,DURATION_TIME ,RETURN_MLSL,HEAT_NO_OLD,RET_HEAT_NO,HANDLE_DIV)"
						" select @rec_creator,@rec_create_time,heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,0-round(DURATION_TIME*@rate,0) ,@rate,HEAT_NO,@ret_heat_no,'H'"
						" from tmmsmgy06 t1"
						" where  1=1"
						" and dev_code not like 'C%'"
						" and heat_no = @heat_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("rec_creator", s.userid);
					cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
					cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
					cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
					cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

					//插入消耗数据
					//20241119 回炉的铁水使用浇次合并前的铁水量
					sqlstr = " insert into tmmsmgy07a(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code,mat_name, ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
						" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,@ret_heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t2.mat_name,t1.ID_2A,t1.PROC_COUNT,t1.WEIGH_NO,t1.QUALITY_BATCH_NO,t1.LOT_NO,t1.DEVO_TIME,round(DEVO_WT*@rate,DECIMAL_PLACE) ,'H',t1.STK_NO,t1.CHARGE_TYPE,@heat_no,@ret_heat_no"
						" from tmmsmgy08 t1"
						" left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
						" where  1=1"
						" and t1.mat_code !='TS0000'"
						" and HANDLE_DIV != 'H'"
						" and HEAT_NO_OLD = ' '"
						" and heat_no = @heat_no"
						;
					Log::Trace("", "", "sqlstr={0}", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("rec_creator", s.userid);
					cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
					cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
					cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
					cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

					sqlstr = " insert into tmmsmgy07a(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code,mat_name, ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
						" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,@ret_heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t2.mat_name,' ',0,' ',' ',' ',t1.DEVO_TIME,round(DEVO_WT*@rate,DECIMAL_PLACE) ,'H',' ',' ',HEAT_NO,@ret_heat_no"
						" from tmmsm2a_ts t1"
						" left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
						" where  1=1"
						" and t1.mat_code ='TS0000'"
						" and HANDLE_DIV != 'H'"
						" and heat_no = @heat_no"
						;
					Log::Trace("", "", "sqlstr={0}", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("rec_creator", s.userid);
					cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
					cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
					cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
					cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();
				}
				cmd_inq.Close();

				//进行尾差处理
				if (heat_no_e.Trim() != "")
				{
					sqlstr = " select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,sum(devo_wt) "
						" from ("
						" select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,devo_wt"
						" from tmmsmgy08 "
						" where 1=1"
						" and HANDLE_DIV != 'H' "
						" and heat_no = @heat_no"
						" union all"
						" select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,0-devo_wt devo_wt"
						" from tmmsmgy07a "
						" where 1=1"
						" and heat_no !=@heat_no"
						" and HANDLE_DIV = 'H' "
						" and HEAT_NO_OLD = @heat_no"
						" )"
						" group by l2_proc_no,proc_no,dev_code,mat_code,id_2a,proc_count,weigh_no,quality_batch_no,lot_no,devo_time,stk_no,charge_type "
						" having sum(devo_wt)!=0"
						;
					//Log::Trace("", "", "sqlstr={0}", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						sqlstr = " update tmmsmgy07a set devo_wt = devo_wt+@dif_wt"
							" where 1=1"
							" and l2_proc_no = @l2_proc_no"
							" and proc_no = @proc_no"
							" and dev_code = @dev_code"
							" and mat_code = @mat_code"
							" and id_2a = @id_2a"
							" and proc_count = @proc_count"
							" and weigh_no = @weigh_no"
							" and quality_batch_no = @quality_batch_no"
							" and lot_no = @lot_no"
							" and devo_time = @devo_time"
							" and stk_no = @stk_no"
							" and charge_type = @charge_type"
							" and HANDLE_DIV = 'H'"
							" and heat_no = @heat_no_e and HEAT_NO_OLD = @heat_no"
							;
						cmd_inq_s.SetCommandText(sqlstr);
						cmd_inq_s.Parameters.Set("heat_no_e", heat_no_e);
						cmd_inq_s.Parameters.Set("heat_no", heat_no);
						cmd_inq_s.Parameters.Set("l2_proc_no", cmd_inq.GetString(1));
						cmd_inq_s.Parameters.Set("proc_no", cmd_inq.GetString(2));
						cmd_inq_s.Parameters.Set("dev_code", cmd_inq.GetString(3));
						cmd_inq_s.Parameters.Set("mat_code", cmd_inq.GetString(4));
						cmd_inq_s.Parameters.Set("id_2a", cmd_inq.GetString(5));
						cmd_inq_s.Parameters.Set("proc_count", cmd_inq.GetString(6));
						cmd_inq_s.Parameters.Set("weigh_no", cmd_inq.GetString(7));
						cmd_inq_s.Parameters.Set("quality_batch_no", cmd_inq.GetString(8));
						cmd_inq_s.Parameters.Set("lot_no", cmd_inq.GetString(9));
						cmd_inq_s.Parameters.Set("devo_time", cmd_inq.GetString(10));
						cmd_inq_s.Parameters.Set("stk_no", cmd_inq.GetString(11));
						cmd_inq_s.Parameters.Set("charge_type", cmd_inq.GetString(12));
						cmd_inq_s.Parameters.Set("dif_wt", cmd_inq.GetDecimal(13));
						cmd_inq_s.ExecuteNonQuery();
						cmd_inq_s.Close();
					}
					cmd_inq.Close();
				}

				//插入负数
				sqlstr = " insert into tmmsmgy07a(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code, mat_name,ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
					" select @rec_creator,@rec_create_time,nvl(t2.sm_plan_nol2,' '),@heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t1.mat_name,t1.ID_2A,t1.PROC_COUNT,t1.WEIGH_NO,t1.QUALITY_BATCH_NO,t1.LOT_NO,DEVO_TIME,0-DEVO_WT ,'H',t1.STK_NO,t1.CHARGE_TYPE,@heat_no,ret_heat_no"
					" from tmmsmgy07a t1"
					" left join tmmsmgy05 t2 on t2.heat_no = @heat_no"
					" where  1=1"
					" and HANDLE_DIV = 'H'"
					" and t1.HEAT_NO_OLD = @heat_no"
					;
				Log::Trace("", "", "sqlstr={0}", sqlstr);
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("rec_creator", s.userid);
				cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
				cmd_inq_1.Parameters.Set("heat_no", heat_no);
				cmd_inq_1.ExecuteNonQuery();
				cmd_inq_1.Close();

				//判断是否有回炉信息，如果有则进行重新核算
				sqlstr = " select ret_heat_no"
					" from tpssm35"
					" where heat_no=@heat_no"
					" order by rate desc"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				EIClass bcls_ret1;
				EIClass bcls_rec1;
				bcls_rec1.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
				bcls_rec1.Tables[0].Rows.Clear();
				bcls_rec1.Tables[0].Rows.Add();
				while (cmd_inq.Read())
				{
					bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
					doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);

					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				cmd_inq.Close();

				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
				doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);

				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
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


