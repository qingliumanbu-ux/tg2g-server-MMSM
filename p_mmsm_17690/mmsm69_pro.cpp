/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2024-01-08
Description:自循环废钢转运车计划
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明
int f_mmsm_21a008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm69_pro)

int f_mmsm69_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");
	CString	c_datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm69("TMMSM69");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_seq_no = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_datetime = datetime.Substring(0, 8) + "000000";
		/* 获得传入参数 */

		if (bcls_rec->Tables.IndexOf("MMSM69_ADD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_ADD"].Rows.get_Count(); i++)
			{
				tmmsm69.Reset();
                tmmsm69.MergeFrom(bcls_rec->Tables["MMSM69_ADD"].Rows[i]);
				tmmsm69.TrimOrBlank();
				CString  dh = "21XH" + datetime.Substring(0, 10) + EPGetNextSeq("SQ_XHYLID", conn);
				tmmsm69["PLAN_NO"] = dh;
				tmmsm69["APTIME"] = datetime;
				tmmsm69["APPLY_BY"] = s.username;
				tmmsm69["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm69["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm69["STATUS"] = "0";
				//2024.08.05 王雅婷 自循环废钢6240-6240 
				tmmsm69["DG_UNIT_CODE"] = "6240";
				tmmsm69["RECV_DEPT_CODE"] = "6240";
				tmmsm69["CAR_USE_UNIT_CODE"] = tmmsm69["RECV_DEPT_CODE"];
				tmmsm69["LOAD_CODE_AREA"] = tmmsm69["LOAD_CODE"]	.ToString().SubstringNE(0,6);
				tmmsm69["LOAD_CODE_FACTORY"] = tmmsm69["LOAD_CODE"].ToString().SubstringNE(0, 4);
				tmmsm69["UNLOAD_CODE_AREA"] = tmmsm69["UNLOAD_POINT_CODE"].ToString().SubstringNE(0, 6);
				tmmsm69["UNLOAD_CODE_FACTORY"] = tmmsm69["LOAD_CODE"].ToString().SubstringNE(0, 4);

				tmmsm69.TrimOrBlank();
				tmmsm69.Print();
				tmmsm69.Insert();
			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM69_MODIFY") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm69.Reset();
				tmmsm69.MergeFrom(bcls_rec->Tables["MMSM69_MODIFY"].Rows[i]);
				tmmsm69.TrimOrBlank();
				tmmsm69["REC_REVISOR"] = s.userid;
				tmmsm69["REC_REVISE_TIME"] = datetime;
				tmmsm69["LOAD_CODE_AREA"] = tmmsm69["LOAD_CODE"].ToString().SubstringNE(0, 6);
				tmmsm69["LOAD_CODE_FACTORY"] = tmmsm69["LOAD_CODE"].ToString().SubstringNE(0, 4);
				tmmsm69["UNLOAD_CODE_AREA"] = tmmsm69["UNLOAD_POINT_CODE"].ToString().SubstringNE(0, 6);
				tmmsm69["UNLOAD_CODE_FACTORY"] = tmmsm69["LOAD_CODE"].ToString().SubstringNE(0, 4);
				tmmsm69.TrimOrBlank();
				tmmsm69["APTIME"] = datetime;
				Log::Trace("", __FUNCTION__, "applytime			= [{0}]", tmmsm69["APTIME"].ToString());
				if (tmmsm69["PLAN_NO"].ToString().Trim() != "")
				{
					tmmsm69.Update("*", "PLAN_NO");
				}
			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM69_DELETE") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm69.Reset();
				tmmsm69.MergeFrom(bcls_rec->Tables["MMSM69_DELETE"].Rows[i]);
				tmmsm69.TrimOrBlank();
				if (tmmsm69["PLAN_NO"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tmmsm69.Delete("PLAN_NO");
				}

			}
		}

		//发送物流系统-转运车计划
		if (bcls_rec->Tables.IndexOf("MMSM69_SND") >= 0)
		{
			CString vdeal_flag = bcls_rec->Tables["MMSM69_SND"].Rows[0]["DEAL_FLAG"].ToString();
			for (int i = 0; i < bcls_rec->Tables["MMSM69_SND"].Rows.get_Count(); i++)
			{
				tmmsm69.Reset();
				tmmsm69.MergeFrom(bcls_rec->Tables["MMSM69_SND"].Rows[i]);

				EIClass inBlock_1000;

				inBlock_1000.Tables[0].Clear();
				tmmsm69.MergeTo(inBlock_1000.Tables[0]);
				inBlock_1000.Tables.Add("MMSM");
				tmmsm69.MergeTo(inBlock_1000.Tables["MMSM"]);


				if (!inBlock_1000.Tables[0].Columns.Contains("DEAL_FLAG"))
				{
					inBlock_1000.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					inBlock_1000.Tables[0].Rows[0]["DEAL_FLAG"] = vdeal_flag;
				}

				doFlag = f_mmsm_21a008_snd(&inBlock_1000, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c002_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (vdeal_flag == "I") tmmsm69["STATUS"] = "1";
				if (vdeal_flag == "C") tmmsm69["STATUS"] = "2";
				tmmsm69.Update("STATUS", "PLAN_NO");

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


