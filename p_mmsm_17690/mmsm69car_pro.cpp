/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2024-02-02
Description: 废钢装车的实绩生成方法-
**************************************************/
#include "stdafx.h"
//using namespace BM2;
//using namespace BM2::Data;
//using namespace BM2::Data::DbClient;


// service入口
int f_mmsm_21a009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm69car_pro)

int f_mmsm69car_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");
	CString	c_datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel    twmsm61("TWMSM61");
	CModel    tmmsm69("TMMSM69");

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

		if (bcls_rec->Tables.IndexOf("MMSM69_CAR") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_CAR"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				twmsm61["TRUCK_NO"] = bcls_rec->Tables["MMSM69_CAR"].Rows[i]["TRUCK_NO"];
				twmsm61["TRUCK_MODEL"] = bcls_rec->Tables["MMSM69_CAR"].Rows[i]["TRUCK_MODEL"];
				twmsm61["PLAN_NO"] = bcls_rec->Tables["MMSM69_CAR"].Rows[i]["PLAN_NO"];

				cmd_inq.SetCommandText("select count(*)  from twmsm61 t where t.truck_no = '" + twmsm61["TRUCK_NO"].ToString() + "' and t.mat_wt =  0  and t.back11 = ' ' and unload_state = '1'");
				if (cmd_inq.ExecuteScalar().ToInt8() > 0)
				{
					cmd_inq.Close();
					CException ex;
					ex.SetMsg(twmsm61["TRUCK_NO"].ToString() + "有未计量完成的装车实绩,不能再装了");
					throw ex;
				}
				if (twmsm61.QueryCount("PLAN_NO") > 0)

				{
					CException ex;
					ex.SetMsg(twmsm61["PLAN_NO"].ToString() + "交料申请只能装一车！");
					throw ex;

				}
				cmd_inq.Close();
				CString sj = "xxxxxx" + datetime;
				//Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);
				///* 新增事件信息 */
				CString  dh = "21XHS" + datetime.Substring(0, 10) + EPGetNextSeq("SQ_XHYLID", conn);
				twmsm61["PRACTICE_NO"] = dh;
				tmmsm69.Reset();
				tmmsm69["PLAN_NO"] = twmsm61["PLAN_NO"];
				tmmsm69.Query();
				twmsm61["TRANS_TYPE"] = "3";
				twmsm61["LOAD_CODE"] = tmmsm69["LOAD_CODE"];
				twmsm61["LOAD_CODE_FACTORY"] = tmmsm69["LOAD_CODE"].ToString().Substring(0, 4);
				twmsm61["LOAD_CODE_AREA"] = tmmsm69["LOAD_CODE"].ToString().Substring(0, 6);
				twmsm61["UNLOAD_CODE"] = tmmsm69["UNLOAD_POINT_CODE"];
				twmsm61["UNLOAD_CODE_FACTORY"] = tmmsm69["UNLOAD_POINT_CODE"].ToString().Substring(0, 4);
				twmsm61["UNLOAD_CODE_AREA"] = tmmsm69["UNLOAD_POINT_CODE"].ToString().Substring(0, 6);
				twmsm61["DEAL_FLAG"] = "I";
				twmsm61["DG_UNIT_CODE"] = tmmsm69["DG_UNIT_CODE"];
				twmsm61["RECEIVE_UNIT_CODE"] = tmmsm69["RECV_DEPT_CODE"];
				twmsm61["LOAD_END_TIME"] = datetime;
				twmsm61["MATERIAL_CODE"] = tmmsm69["MAT_CODE"];
				//区分标记 1-交废钢 2-自循环
				twmsm61["ARCHIVE_FLAG"] = "2";
				twmsm61["UNLOAD_STATE"] = "0";
				twmsm61["REC_CREATOR"] = s.userid;   //记录创建责任者
				twmsm61["REC_CREATE_TIME"] = datetime;   //记录创建时刻

				twmsm61.TrimOrBlank();
				twmsm61.Insert();
			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("21A009") >= 0)
		{

			doFlag = f_mmsm_21a009_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a009_snd失败-------");
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
		}


		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM69_UNLOADCAR") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_UNLOADCAR"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				twmsm61.MergeFrom(bcls_rec->Tables["MMSM69_UNLOADCAR"].Rows[i]);
				twmsm61.TrimOrBlank();
				if (twmsm61["PRACTICE_NO"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					twmsm61.Delete();
				}

			}
		}

		if (bcls_rec->Tables.IndexOf("MMSM69_SQH") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM69_SQH"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();

				twmsm61.MergeFrom(bcls_rec->Tables["MMSM69_SQH"].Rows[i]);
				twmsm61.TrimOrBlank();
				CString  dh = "21PS6240" + CDateTime::Today().ToString("yyyyMMdd") + EPGetNextSeq("SQ_JLYLID", conn);
				Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);

				/*if (twmsm61["APTIME"].ToString().Trim() == "")
				{

				}*/
				if (twmsm61["APTIME"].ToString().Trim() != "")
				{
					if (twmsm61["APTIME"].ToString().Trim() <c_datetime)
					{
						sprintf(s.msg, "生成要料申请号要料申请时间不能小于今天！！！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					sprintf(s.msg, "生成要料申请号必须选择要料申请时间！！！");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (twmsm61["PURCHASEDOCID"].ToString().Trim() == "")
				{
					/* 新增事件信息 */
					twmsm61["PURCHASEDOCID"] = dh;
					//twmsm61["APTIME"] = datetime;
					twmsm61["APPLY_BY"] = s.username;
					twmsm61["REC_CREATOR"] = s.userid;   //记录创建责任者
					twmsm61["REC_CREATE_TIME"] = datetime;   //记录创建时刻
					twmsm61.Update("PURCHASEDOCID,APTIME", "MAT_CODE,SEQ_NO");
				}
				else
				{
					sprintf(s.msg, "已经生成要料申请号的物料不能再次生成要料申请号！！！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
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
