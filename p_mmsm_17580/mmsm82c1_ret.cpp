
/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c1_ret)
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c1_ret(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	CString v_proc_div = "";
	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";

	CString heat_no = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm2a_lv("TMMSM2A_LV");

	CDbCommand cmd_inq(conn);
	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].set_TableName("xh");
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm2a_yl.Reset();
			tmmsm2a_yl.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tmmsm2a_lv.CopyFrom(tmmsm2a_yl);
			tmmsm2a_lv.Delete("SEQ_NO_2A");

			tmmsm2a_yl.Delete("SEQ_NO_2A");
			tmmsm2a_yl.TrimOrBlank();
			tmmsm2a_yl["REC_CREATOR"] = s.userid;
			tmmsm2a_yl["REC_CREATE_TIME"] = dateNow;
			tmmsm2a_yl.Insert();

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


