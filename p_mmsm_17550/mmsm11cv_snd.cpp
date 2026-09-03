/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:    563167
Version:    1.0
Date:       2023-10-16
Description: 铁水实绩电文发送
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 


BM2F_ENTERACE(mmsm11cv_snd)

int f_mmsm11cv_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int j = 0;

	/* 业务变量 */
	CString	datetime("");
	int		fetchRowCount = 0;
	CString v_table_type = "";
	CString v_proc_div = "";
	CString v_Ticode("");

	/* 创建电文处理对象 */
	EPEX epex(&s, conn);

	/* 实体类定义 */
	//CModel tmm009a("TMM009A");
	CModel tmmsm11("TMMSM11");
	

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMdd");

		/*blkNum = bcls_rec->Tables.IndexOf("MMSM11");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSMSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
	

	//	Log::Trace("", __FUNCTION__, "传入参数,v_tc_backlog				= [{0}]", v_tc_backlog);
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_FLAG"))
			v_proc_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		if (v_proc_div == "") v_proc_div = "3";  // 缺省为 3
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("TICODE"))
				v_Ticode = bcls_rec->Tables[0].Rows[i]["TICODE"].ToString().Trim();
			 //电文发送内容： 1,2,3,5  四个电文状态。
			if (v_Ticode.Trim() == "")
			{
				strcpy(s.msg, "铁水调拨单号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "传入参数1,= [{0}]", v_Ticode);
			Log::Trace("", __FUNCTION__, "传入参数2,= [{0}]", v_proc_div);
			tmmsm11.Reset();
			tmmsm11["TICODE"] = v_Ticode;
			tmmsm11.Query("TICODE");
			tmmsm11.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "设置电文数据00");

			if (epex.Initialize("T8B003") < 0)
			{
				CString ls = epex.GetMsg();
				strcpy(s.msg, ls + "123");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Trace("", __FUNCTION__, "设置电文数据11");

			if (epex.SetValue(0, tmmsm11) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());

				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", __FUNCTION__, "设置电文数据22");
			if (epex.SetValue("DEAL_FLAG", 0, v_proc_div) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("WORK_DATE", 0, datetime) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("TCP_NO", 0, tmmsm11["TAPNO"].ToString().Trim()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (epex.SetValue("TPC_SEQ", 0, tmmsm11["TPC_ID"].ToString().Trim()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("TPC_NO", 0, tmmsm11["POTID"].ToString().Trim()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "设置电文数据33");

			if (epex.SetValue("TARE_WT", 0, 0) < 0)
			{
				sprintf(s.msg, epex.GetMsg());

				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "设置电文数据44");
			if (epex.SetValue("GROSS_WT", 0, 0) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (v_proc_div == "1" || v_proc_div == "2" || v_proc_div == "5")
			{

				if (epex.SetValue("NET_WT", 0, 0) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("SETTLEMENT_WT", 0, 0) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
			else  //  3 的时候，设置重量
			{
				Log::Trace("", __FUNCTION__, "设置电文数据55");
				if (epex.SetValue("GROSS_WT", 0, tmmsm11["ADDSCRAP_WT"].ToDecimal().Round(3)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "设置电文数据66");

				if (epex.SetValue("NET_WT", 0, tmmsm11["NWEIGHT"].ToDecimal().Round(3)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);

				}
				Log::Trace("", __FUNCTION__, "设置电文数据77");
				if (epex.SetValue("SETTLEMENT_WT", 0, (tmmsm11["NWEIGHT"].ToDecimal()*0.993).Round(3)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "设置电文数据88{0}", tmmsm11["TICODE"].ToString());

				if (epex.SetValue("BACK1", 0, tmmsm11["TICODE"].ToString().Substring(4, 12)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "设置电文数据99");

			}
			//WEIGH_TIME

			if (v_proc_div == "1")
			{

				if (epex.SetValue("WEIGH_TIME", 0, tmmsm11["EMPTY_TIME"].ToString().Trim()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
			Log::Trace("", __FUNCTION__, "设置电文数据aa");

			if (v_proc_div == "2")
			{

				if (epex.SetValue("WEIGH_TIME", 0, tmmsm11["EMPTY_TIME_ACT"].ToString().Trim()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//EMPTY_SIGN
				if (epex.SetValue("EMPTY_SIGN", 0, tmmsm11["EMPTY_FLAG"].ToString().Trim()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}

			if (v_proc_div == "5")
			{

				if (epex.SetValue("BACK1", 0, tmmsm11["IRON_TEMP"].ToString().Trim()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}


				if (epex.SetValue("BACK5", 0, tmmsm11["IRON_TEMP_TIME"].ToString().Trim()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}
			Log::Trace("", __FUNCTION__, "设置电文数据X,= [{0}]", epex.GetMsg());


			//Log::Trace("", __FUNCTION__, "调用框架发送电文开始");
			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//Log::Trace("", __FUNCTION__, "调用框架发送电文结束");
			/* 释放 */
			epex.Uninitialize();

			//Log::Trace("", __FUNCTION__, "调用框架释放电文结束");	
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




