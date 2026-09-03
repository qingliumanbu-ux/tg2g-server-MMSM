/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: 工序测温生产实绩增删改
**************************************************/
//框架头文件
#include "stdafx.h" 
 



//业务头文件


//外部函数声明
int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm2b_pro)

int f_mmsm2b_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm2b("TMMSM2B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		/* 维护事件表 */
		// 新增事件
		if (bcls_rec->Tables.IndexOf("MMSM_2B_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2B_INS"].Rows.get_Count(); i++)
			{
				tmmsm2b.Reset();
				tmmsm2b.MergeFrom(bcls_rec->Tables["MMSM_2B_INS"].Rows[i]);
				tmmsm2b.TrimOrBlank();

				tmmsm2b.Print();


				/* 新增事件信息 */
				tmmsm2b["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm2b["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm2b.TrimOrBlank();
				

				Log::Trace("", __FUNCTION__, "HEAT_NO =[{0}]", tmmsm2b["HEAT_NO"].ToString());
				Log::Trace("", __FUNCTION__, "PROC_NO =[{0}]", tmmsm2b["PROC_NO"].ToString());
				
				if (tmmsm2b.QueryCount("HEAT_NO,PROC_NO,PROC_COUNT") == 0){
					tmmsm2b["PROC_COUNT"] = 1;
					

				}
				Log::Trace("", __FUNCTION__, "PROC_COUNT =[{0}]", tmmsm2b["PROC_COUNT"].ToString());

				if (tmmsm2b.QueryCount("HEAT_NO,PROC_NO,PROC_COUNT") > 0)
				{
					blkNum = bcls_rec->Tables.IndexOf("PROCCOUNT");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("PROCCOUNT");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("TABLE_TYPE"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "TABLE_TYPE");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "PROC_NO");
					}

					bcls_rec->Tables["PROCCOUNT"].Rows.Add();
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"] = "TMMSM2B";
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"] = tmmsm2b["HEAT_NO"];
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"] = tmmsm2b["PROC_NO"];


					doFlag = f_mmsm_count(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					tmmsm2b["PROC_COUNT"] = bcls_ret->Tables[0].Rows[0]["PROC_COUNT"];
				}

				tmmsm2b.Insert();
				
			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM_2B_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2B_UPD"].Rows.get_Count(); i++)
			{
				tmmsm2b.Reset();

				tmmsm2b.MergeFrom(bcls_rec->Tables["MMSM_2B_UPD"].Rows[i]);
				tmmsm2b.TrimOrBlank();


				/* 修改事件信息 */
				tmmsm2b["REC_REVISOR"] = s.userid;
				tmmsm2b["REC_REVISE_TIME"] = datetime;
				tmmsm2b.TrimOrBlank();

				tmmsm2b.Delete();
				tmmsm2b.Insert();

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM_2B_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2B_DEL"].Rows.get_Count(); i++)
			{
				tmmsm2b.Reset();
				tmmsm2b.MergeFrom(bcls_rec->Tables["MMSM_2B_DEL"].Rows[i]);
				tmmsm2b.TrimOrBlank();


				/* 删除事件信息 */
				tmmsm2b.Delete("HEAT_NO, PROC_NO,PROC_COUNT");


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


