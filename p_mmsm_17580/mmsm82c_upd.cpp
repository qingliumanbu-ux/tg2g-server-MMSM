
/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>


#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c_upd)
int f_mmsm82c_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm56("TMMSM56");
	try
	{
		
		if (bcls_rec->Tables.Contains("gy"))   //工艺路径新增
		{
			for (int i = 0; i < bcls_rec->Tables["gy"].Rows.get_Count(); i++)
			{
				tmmsmgy06.MergeFrom(bcls_rec->Tables["gy"].Rows[i]);
				tmmsmgy06["REC_CREATOR"] = s.userid;
				tmmsmgy06["REC_CREATE_TIME"] = dateNow;
				tmmsmgy06.Delete("HEAT_NO,L2_PROC_NO,PROC_NO,DEV_CODE"); 
				tmmsmgy06.TrimOrBlank();
				tmmsmgy06.Insert();
			}			
		}
		if (bcls_rec->Tables.Contains("xh"))   //消耗维护
		{
			for (int i = 0; i < bcls_rec->Tables["xh"].Rows.get_Count(); i++)
			{
				tmmsm56.MergeFrom(bcls_rec->Tables["xh"].Rows[i]);
				tmmsm56["REC_CREATOR"] = s.userid;
				tmmsm56["REC_CREATE_TIME"] = dateNow;
				tmmsm56["OUT_STOCK_TIME"] = bcls_rec->Tables["xh"].Rows[i]["DEVO_TIME"].ToString();
				tmmsm56["OUT_STOCK_WT"] = bcls_rec->Tables["xh"].Rows[i]["DEVO_WT"].ToDecimal();
				tmmsm56["SRC_STOCK_CODE"] = bcls_rec->Tables["xh"].Rows[i]["STK_NO"].ToString();
				tmmsm56["HANDLE_DIV"] = "I";
				if (tmmsm56["OUT_STOCK_TIME"].ToString().Trim() == "")
				{
					tmmsm56["OUT_STOCK_TIME"] = dateNow;
				}				
				tmmsm56.Delete("HEAT_NO,L2_PROC_NO,ID_2A,DEV_CODE,MAT_CODE,HANDLE_DIV");
				tmmsm56.TrimOrBlank();
				tmmsm56.Insert();
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


