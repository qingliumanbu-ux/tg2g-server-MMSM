/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c2_upd_bat)

int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c2_upd_bat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	CString v_proc_div = "";
	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString v_weigh_no = "";
	CString v_new_lot_no = "";
	CString old_l2_proc_no = "";
	CString heat_no = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm2a_lv("TMMSM2A_LV");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81s("TMMSM81_S");
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

		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			tmmsm2a_yl.Reset();
			tmmsm2a_yl.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			tmmsm2a_yl["LOT_NO"] = bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString();
			tmmsm2a_yl.Print();
			tmmsm2a_yl.TrimOrBlank();

			if (v_proc_div == "M")
			{
				v_weigh_no = bcls_rec->Tables[1].Rows[i]["WEIGH_NO"].ToString();
				v_new_lot_no = bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString();
				if (v_weigh_no.Trim() == "")
				{
					sprintf(s.msg, "计量单号不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
				tmmsm81["WEIGH_NO"] = v_weigh_no;
				if (tmmsm81.QueryCount("WEIGH_NO") == 0)
				{
					tmmsm81s["WEIGH_NO"] = v_weigh_no;
					tmmsm81s["LOT_NO"] = v_new_lot_no;
					tmmsm81s.Update("LOT_NO", "WEIGH_NO");
					Log::Info("", __FUNCTION__, "81s=[{0}]", v_new_lot_no);
				}
				else
				{
					tmmsm81["LOT_NO"] = v_new_lot_no;
					tmmsm81.Update("LOT_NO", "WEIGH_NO");
					Log::Info("", __FUNCTION__, "81=[{0}]", v_new_lot_no);
				}
				Log::Info("", __FUNCTION__, "81=[{0}]", v_weigh_no);
				
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

				Log::Info("", __FUNCTION__, "20250228 =[{0}]", tmmsm2a_yl["L2_PROC_NO"].ToString());
				Log::Info("", __FUNCTION__, "20250228 =[{0}]", tmmsm2a_yl["SEQ_NO_2A"].ToString());
				tmmsm2a_yl["REC_REVISOR"] = s.userid;
				tmmsm2a_yl["REC_REVISE_TIME"] = dateNow;
				tmmsm2a_yl["ADJUST_FLAG"] = "M";
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
					tmmsm2a_lv["EVENT_DESC"] = "批量修改处理号";
					tmmsm2a_lv.TrimOrBlank();
					tmmsm2a_lv.Insert();

					old_l2_proc_no = tmmsm2a_lv["L2_PROC_NO"].ToString();
				}
				cmd_inq.Close();
				tmmsm2a_yl.Update("REC_REVISOR,REC_REVISE_TIME,ADJUST_FLAG,HEAT_NO,LOT_NO", "SEQ_NO_2A");
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