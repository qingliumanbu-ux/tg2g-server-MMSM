/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c1_pro)

int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c1_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	CString v_proc_div = "";
	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString old_l2_proc_no = "";
	CString heat_no = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm2a_lv("TMMSM2A_LV");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].set_TableName("xh");
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();

		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm2a_yl.Reset();
			tmmsm2a_yl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//tmmsm2a_yl.Print();
			tmmsm2a_yl.TrimOrBlank();

			if (v_proc_div == "I")
			{
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				tmmsm2a_yl["REC_CREATOR"] = s.userid;
				tmmsm2a_yl["REC_CREATE_TIME"] = dateNow;
				tmmsm2a_yl["ID_2A"] = " ";
				tmmsm2a_yl["PROC_COUNT"] = 1;

				sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm2a_yl["SEQ_NO_2A"] = dateNow + cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();

				tmmsm2a_yl["ADJUST_FLAG"] = "U";
				tmmsm2a_yl["REMARK_2"] = " ";
				tmmsm2a_yl["DEVO_TIME"] = dateNow;
				sqlstr = " select mat_name from tmmsm50 where mat_code = @mat_code";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code", tmmsm2a_yl["MAT_CODE"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_yl["MAT_NAME"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				//20241226

				sqlstr = "select MAX(T.HEAT_NO) from tmmsmgy06 t where T.L2_PROC_NO= @l2_proc_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("l2_proc_no", tmmsm2a_yl["L2_PROC_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm2a_yl["HEAT_NO"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();


				if (tmmsm2a_yl["SM_PLAN_NOL2"].ToString().Trim() == "")
				{
					//tmmsm2a_yl["SM_PLAN_NOL2"] = bcls_rec->Tables["xh"].Rows[0]["SM_PLAN_NOL2"].ToString();
				}
				tmmsm2a_yl.TrimOrBlank();
				tmmsm2a_yl.Insert();

				tmmsm2a_lv.CopyFrom(tmmsm2a_yl);
				tmmsm2a_lv["EVENT_DESC"] = "新增";
				tmmsm2a_lv.TrimOrBlank();
				tmmsm2a_lv.Insert();
			}
			else if (v_proc_div == "U")
			{
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				//20241226

				sqlstr = "select MAX(T.HEAT_NO) from tmmsmgy06 t where T.L2_PROC_NO= @l2_proc_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("l2_proc_no", tmmsm2a_yl["L2_PROC_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm2a_yl["HEAT_NO"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();


				tmmsm2a_yl["REC_REVISOR"] = s.userid;
				tmmsm2a_yl["REC_REVISE_TIME"] = dateNow;
				tmmsm2a_yl["ADJUST_FLAG"] = "U";
				sqlstr = " select * from tmmsm2a_yl"
					" where 1=1"
					" and SEQ_NO_2A=@seq_no_2a"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("seq_no_2a", tmmsm2a_yl["SEQ_NO_2A"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_lv.Reset();
					cmd_inq.Fetch(tmmsm2a_lv);
					tmmsm2a_lv["REC_CREATOR"] = s.userid;
					tmmsm2a_lv["REC_CREATE_TIME"] = dateNow;
					tmmsm2a_lv["EVENT_DESC"] = "修改";
					tmmsm2a_lv.TrimOrBlank();
					tmmsm2a_lv.Insert();

					old_l2_proc_no = tmmsm2a_lv["L2_PROC_NO"].ToString();
				}
				cmd_inq.Close();
				tmmsm2a_yl.Update("REC_REVISOR,REC_REVISE_TIME,DEVO_WT,ADJUST_FLAG,WEIGH_NO,QUALITY_BATCH_NO,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO", "SEQ_NO_2A");

				//更改处理号 重新计算之前炉号
				if (old_l2_proc_no != tmmsm2a_yl["L2_PROC_NO"].ToString())
				{
					sqlstr = " select distinct  heat_no from tmmsmgy06"
						" where l2_proc_no = @l2_proc_no"
						;
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Set("l2_proc_no", old_l2_proc_no);
					cmd_sql.ExecuteReader();
					while (cmd_sql.Read())
					{
						bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_sql.GetString(1);
						doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					cmd_sql.Close();
				}
			}
			
			else if (v_proc_div == "D")
			{
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				sqlstr = " select * from tmmsm2a_yl"
					" where 1=1"
					" and SEQ_NO_2A=@seq_no_2a"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("seq_no_2a", tmmsm2a_yl["SEQ_NO_2A"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_lv.Reset();
					cmd_inq.Fetch(tmmsm2a_lv);
					tmmsm2a_lv["REC_CREATOR"] = s.userid;
					tmmsm2a_lv["REC_CREATE_TIME"] = dateNow;
					tmmsm2a_lv["EVENT_DESC"] = "删除";
					tmmsm2a_lv.TrimOrBlank();
					tmmsm2a_lv.Insert();
				}
				cmd_inq.Close();

				tmmsm2a_yl.Delete("SEQ_NO_2A");
			}

			//查找对应的处理号
			sqlstr = " select distinct  heat_no from tmmsmgy06"
				" where l2_proc_no = @l2_proc_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("l2_proc_no", tmmsm2a_yl["L2_PROC_NO"].ToString());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
