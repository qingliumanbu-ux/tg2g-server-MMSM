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


BM2F_ENTERACE(mmsm2d_pro)

int f_mmsm2d_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm2d("TMMSM2D");

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
		if (bcls_rec->Tables.IndexOf("MMSM_2D_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2D_INS"].Rows.get_Count(); i++)
			{
				tmmsm2d.Reset();
				tmmsm2d.MergeFrom(bcls_rec->Tables["MMSM_2D_INS"].Rows[i]);
				tmmsm2d.TrimOrBlank();

				tmmsm2d.Print();


				/* 新增事件信息 */
				tmmsm2d["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm2d["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm2d.TrimOrBlank();
				
				if (tmmsm2d.QueryCount("HEAT_NO,PROC_NO,PROC_COUNT") == 0){
					tmmsm2d["PROC_COUNT"] = 1;
				}
				
				if (tmmsm2d.QueryCount("HEAT_NO,PROC_NO,PROC_COUNT") > 0)
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
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"] = "TMMSM2D";
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"] = tmmsm2d["HEAT_NO"];
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"] = tmmsm2d["PROC_NO"];


					doFlag = f_mmsm_count(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					tmmsm2d["PROC_COUNT"] = bcls_ret->Tables[0].Rows[0]["PROC_COUNT"];
				}

				//Log::Info("", __FUNCTION__, "tmmsm2d["PROC_COUNT"] =[{0}]", tmmsm2d["PROC_COUNT"].ToDecimal());

				tmmsm2d.Insert();
				
			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM_2D_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2D_UPD"].Rows.get_Count(); i++)
			{
				tmmsm2d.Reset();

				tmmsm2d.MergeFrom(bcls_rec->Tables["MMSM_2D_UPD"].Rows[i]);
				tmmsm2d.TrimOrBlank();


				/* 修改事件信息 */
				tmmsm2d["REC_REVISOR"] = s.userid;
				tmmsm2d["REC_REVISE_TIME"] = datetime;
				tmmsm2d.TrimOrBlank();

				tmmsm2d.Delete();
				tmmsm2d.Insert();

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM_2D_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM_2D_DEL"].Rows.get_Count(); i++)
			{
				tmmsm2d.Reset();
				tmmsm2d.MergeFrom(bcls_rec->Tables["MMSM_2D_DEL"].Rows[i]);
				tmmsm2d.TrimOrBlank();


				/* 删除事件信息 */
				tmmsm2d.Delete("HEAT_NO, PROC_NO,PROC_COUNT");


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


