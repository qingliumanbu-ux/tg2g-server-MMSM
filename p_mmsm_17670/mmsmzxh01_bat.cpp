/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 总消耗查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
BM2_FUNCTION_IMPORT
int f_mmsm_updcf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh01(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmzxh01_bat)

int f_mmsmzxh01_bat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMdd");
	CString stat_date = CDateTime::Now().ToString("yyyyMM");
	

	try
	{
		EIClass bcls_rec_xh;
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
		bcls_rec_xh.Tables[0].Rows.Add();

		//Log::Trace("", __FUNCTION__, "dateNow.SubstringNE(6, 2)  = [{0}]", dateNow.SubstringNE(6, 2));

		if (bcls_rec->Tables.Contains("PARA"))
		{		
		if (bcls_rec->Tables["PARA"].Columns.Contains("END_TIME"))
		{
			stat_date = bcls_rec->Tables["PARA"].Rows[0]["END_TIME"].ToString().SubstringNE(0, 6);
		}
		}
		else
		{
			//Log::Trace("", __FUNCTION__, "STAT_DATE  = [{0}]", dateNow.SubstringNE(6, 2));

			//如果是1号或是2号，需要将前一个月的
			if (dateNow.SubstringNE(6, 2) == "01" || dateNow.SubstringNE(6, 2) == "02")
			{
				//Log::Trace("", __FUNCTION__, "STAT_DATE111111  = [{0}]", dateNow.SubstringNE(6, 2));
				bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = CDateTime::Now().AddMonths(-1).ToString("yyyyMM");
				//Log::Trace("", __FUNCTION__, "STAT_DATE  = [{0}]", bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"].ToString());

				doFlag = f_mmsm_zxh01(&bcls_rec_xh, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
		}

		
		bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = stat_date;
		
		doFlag = f_mmsm_zxh01(&bcls_rec_xh, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = " ";
		doFlag = f_mmsm_updcf(&bcls_rec_xh, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
*/


		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
