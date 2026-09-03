
/// <summary>
/// 功能说明:消耗物料导入
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsmq1_save)
int f_mmsmq1_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsmq1("TMMSMQ1");

	CDbCommand cmd_inq(conn);
	if (bcls_rec->Tables.Contains("ADD"))
	{
		if (bcls_rec->Tables["ADD"].Rows.get_Count()>0)
		{
			sqlstr = " delete from  tmmsmq1 "
				" where 1=1 "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}

	}
	try
	{
		

		for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
		{
			tmmsmq1.Reset();
			tmmsmq1.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);

			tmmsmq1["REC_CREATOR"] = s.userid;
			tmmsmq1["REC_CREATE_TIME"] = dateNow;
			tmmsmq1.TrimOrBlank();

			if (tmmsmq1["HEAT_NO"].ToString().Trim()!="")
			{
				tmmsmq1.Insert();
			}
			

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


