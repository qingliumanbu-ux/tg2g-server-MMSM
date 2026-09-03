/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2023-09-28
Description: MMSM增删
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明

BM2F_ENTERACE(mmsm11cv_iud)

int f_mmsm11cv_iud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm11("TMMSM11");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		
		if (bcls_rec->Tables.IndexOf("MMSM11CV_ADD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM11CV_ADD"].Rows.get_Count(); i++)
			{
				tmmsm11.Reset();
				tmmsm11.MergeFrom(bcls_rec->Tables["MMSM11CV_ADD"].Rows[i]);
				tmmsm11.TrimOrBlank();

			/*	CException ex;
				ex.SetMsg("123");
				throw ex;*/
				
				tmmsm11.Print();
				CDecimal  csl = 123;
				CString  dh = "6120" + CDateTime::Today().ToString("yyyyMMdd") +EPGetNextSeq("SQ_IRONID", conn);
				Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);
				/* 新增事件信息 */
				tmmsm11["TICODE"] = dh;
				tmmsm11["START_TIME"] = CDateTime::Now().AddMinutes(-30).ToString("yyyyMMddHHmmss");
				Log::Trace("", __FUNCTION__, "start_time				= [{0}]", (const char*)tmmsm11["START_TIME"]);
				tmmsm11["END_TIME"] = CDateTime::Now().AddHours(1.5).ToString("yyyyMMddHHmmss");
				Log::Trace("", __FUNCTION__, "end_time				= [{0}]", (const char*)tmmsm11["END_TIME"]);
				tmmsm11["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm11["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm11.TrimOrBlank();
				tmmsm11.Insert();
				


			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM11CV_MODIFY") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM11CV_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm11.Reset();

				tmmsm11.MergeFrom(bcls_rec->Tables["MMSM11CV_MODIFY"].Rows[i]);
				tmmsm11.TrimOrBlank();

				/* 修改事件信息 */
				tmmsm11["REC_REVISOR"] = s.userid;
				tmmsm11["REC_REVISE_TIME"] = datetime;
				tmmsm11.TrimOrBlank();
			/*	tmmsm11.Update("ELEM_SI, ELEM_MN, ELEM_P, ELEM_S, ELEM_TI, NET_WT_COMPUT, IRON_TEMP_COM, TPC_ST_END_TIME, IRON_TEMP, IRON_TEMP_TIME, ADDSCRAP_WT, " 
					"START_TIME, END_TIME, EMPTY_FLAG, EMPTY_TIME, EMPTY_TIME_ACT, PRE_RAILNO, TPC_SOURCE, SAP_WT, SAP_TIME, SAP_FLAG, REC_REVISOR, REC_REVISE_TIME,"
					"PRACT_COLL_MODE, FACTORY_DIV, TIDCODE, TICODE, TAPNO, TPC_ID, POTID, BF_ID, GWEIGHT, GWTIME, TWEIGHT, TWTIME, NWEIGHT, ELEM_C",
					"TICODE");*/
				tmmsm11.Update("*","TICODE");
			
			}
			CDecimal w = bcls_rec->Tables["MMSM11CV_MODIFY"].Rows.get_Count();
			CFormattable arguments[] = { w,888,"123" };
			//w = (CDateTime::Now() - CDateTime::Now().AddDays(-1)).Milliseconds;
			//CMessageFormat::Format(s.msg,w.ToString()+"条数据被{0}修{1}改" , arguments, 2);
			CString str = w.ToString() + "rows are modified";
			//strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
			//strncpy(s.msg, (const char*)str, sizeof(s.msg) - 1);
		
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM11CV_DELETE") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM11CV_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm11.Reset();
				tmmsm11.MergeFrom(bcls_rec->Tables["MMSM11CV_DELETE"].Rows[i]);
				tmmsm11.TrimOrBlank();


				/* 删除事件信息 */
				tmmsm11.Delete("TICODE");
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


