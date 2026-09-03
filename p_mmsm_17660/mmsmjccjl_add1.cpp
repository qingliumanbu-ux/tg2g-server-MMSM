/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 导入报表
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件




BM2F_ENTERACE(mmsmjccjl_add1)

int f_mmsmjccjl_add1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	//CString table_name = " ";
	//CString	datetime("");
	//datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_ccm_ld_1 = "";

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		//table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().TrimOrBlank().ToUpper();
		CModel tmmsmjccjladd1("TMMSMJCCJLADD1");
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmjccjladd1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//tmmsmjccjladd1["REC_CREATOR"] = s.userid;
			//tmmsmjccjladd1["REC_CREATE_TIME"] = datetime;
			if (tmmsmjccjladd1.QueryCount("CCM_LD_1") < 1){
				Log::Trace("", __FUNCTION__, "111CCM_LD_1[{0}]", tmmsmjccjladd1.QueryCount("CCM_LD_1"));

				tmmsmjccjladd1.Insert();
			}
			else
			{
				Log::Trace("", __FUNCTION__, "222CCM_LD_1[{0}]", tmmsmjccjladd1.QueryCount("CCM_LD_1"));

				//v_ccm_ld_1 = bcls_rec->Tables[0].Rows[i]["CCM_LD_1"].ToString;
				v_ccm_ld_1 += bcls_rec->Tables[0].Rows[i]["CCM_LD_1"].ToString() + "," ;
				continue;
			} 
		}
		Log::Trace("", __FUNCTION__, "v_ccm_ld_1[{0}]", v_ccm_ld_1.GetLength());
		if (v_ccm_ld_1.GetLength() > 1){

			Log::Trace("", __FUNCTION__, "444CCM_LD_1[{0}]", tmmsmjccjladd1.QueryCount("CCM_LD_1"));

			Log::Trace("", __FUNCTION__, "重复[{0}]", v_ccm_ld_1);
			CFormattable arguments[] = { (const char*)v_ccm_ld_1 };
			CMessageFormat::Format(s.msg, "卡号[{0}]的信息重复，请检查！", arguments, 1);
			//strcpy(s.sysmsg, s.msg);
			strncpy(s.sysmsg, (const char*)v_ccm_ld_1, sizeof(s.sysmsg) - 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		////Log::Trace("", __FUNCTION__, "tmmsm52--ex.GetCode()=[{0}]", ex.GetCode());
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


