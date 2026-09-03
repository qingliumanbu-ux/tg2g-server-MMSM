/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2024-01-07
Description: MMSM67
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明
int f_mmsm_21c002_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm67_snd)

int f_mmsm67_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm67("TMMSM67");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */

		 
		if (bcls_rec->Tables.IndexOf("MMSM67_SND") >= 0)
		{
			CString vdeal_flag = bcls_rec->Tables["MMSM67_SND"].Rows[0]["DEAL_FLAG"].ToString();
			for (int i = 0; i < bcls_rec->Tables["MMSM67_SND"].Rows.get_Count(); i++)
			{
				tmmsm67.Reset();
				tmmsm67.MergeFrom(bcls_rec->Tables["MMSM67_SND"].Rows[i]);


				//tmmsm65.Update("*", "PURCHASEDOCID");


				EIClass inBlock_1000;

				inBlock_1000.Tables[0].Clear();
				tmmsm67.MergeTo(inBlock_1000.Tables[0]);
				inBlock_1000.Tables.Add("MMSM");
				tmmsm67.MergeTo(inBlock_1000.Tables["MMSM"]);


				if (!inBlock_1000.Tables[0].Columns.Contains("DEAL_FLAG"))
				{
					inBlock_1000.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					inBlock_1000.Tables[0].Rows[0]["DEAL_FLAG"] = vdeal_flag;
				}

				/*
				CDataRow &data_row = inBlock_1000.Tables[0].Rows.Add();
				data_row["HEAT_NO"] = vdeal_flag;*/

				doFlag = f_mmsm_21c002_snd(&inBlock_1000, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c002_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (vdeal_flag == "I") tmmsm67["STATUS"] = "1";
				if (vdeal_flag == "C") tmmsm67["STATUS"] = "2";
				tmmsm67.Update("STATUS", "PURCHASEDOCID");

			}


		}
		/*doFlag = f_mmsm_21c001_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}*/




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


