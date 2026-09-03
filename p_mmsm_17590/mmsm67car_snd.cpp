/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167 -563167
Version:    1.0
Date:       2024-02-20
Description: 废钢装车
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明   
int f_wmsm_21a009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsm_load_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsm_load_d_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm67car_snd)

int f_mmsm67car_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel twmsm61("TWMSM61");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */


		if (bcls_rec->Tables.IndexOf("21A009") >= 0)
		{

			doFlag = f_wmsm_21a009_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_wmsm_21a009_snd失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			CString vdeal_flag = bcls_rec->Tables["21A009"].Rows[0]["DEAL_FLAG"].ToString();
			for (int i = 0; i < bcls_rec->Tables["21A009"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				twmsm61.MergeFrom(bcls_rec->Tables["21A009"].Rows[i]);


				if (vdeal_flag == "I") twmsm61["UNLOAD_STATE"] = "1";
				if (vdeal_flag == "D")
				{
					twmsm61["DEAL_FLAG"] = "D";
					twmsm61["UNLOAD_STATE"] = "2";
				}
				twmsm61.Update("UNLOAD_STATE,DEAL_FLAG", "PRACTICE_NO,MAT_NO");

			}

			Log::Trace("", __FUNCTION__, "1mat_no = [{0}]", twmsm61["MAT_NO"].ToString());
			twmsm61.Query("PRACTICE_NO");
			Log::Trace("", __FUNCTION__, "2mat_no = [{0}]", twmsm61["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "3BACK1 = [{0}]", bcls_rec->Tables["21A009"].Rows[0]["BACK1"].ToString());
			if (vdeal_flag == "I")
			{
				doFlag = f_wmsm_load_proc(bcls_rec, bcls_ret, conn);
			}
			else
			{
			    doFlag = f_wmsm_load_d_proc(bcls_rec, bcls_ret, conn);
			}

			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用 f_wmsm_load_proc失败-------");

				strncpy(s.msg, (const char*)("给大屏传数失败"), sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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


