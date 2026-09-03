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
BM2F_ENTERACE(mmsm67loadcar_pro)

int f_mmsm67loadcar_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel    tmmsm67("TMMSM67");

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

		if (bcls_rec->Tables.IndexOf("MMSM67_CAR") >= 0)
		{



			for (int i = 0; i < bcls_rec->Tables["MMSM67_CAR"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				//Log::Trace("", __FUNCTION__, "datetime1				= [{0}]", (const char*)datetime);

				//twmsm61.MergeFrom(bcls_rec->Tables["MMSM67_ADD"].Rows[i]);
				//Log::Trace("", __FUNCTION__, "datetime2				= [{0}]", (const char*)datetime);

				twmsm61.TrimOrBlank();
				twmsm61["TRUCK_NO"] = bcls_rec->Tables["MMSM67_CAR"].Rows[i]["TRUCK_NO"];
				twmsm61["TRUCK_MODEL"] = bcls_rec->Tables["MMSM67_CAR"].Rows[i]["TRUCK_MODEL"];
				twmsm61["PLAN_NO"] = bcls_rec->Tables["MMSM67_CAR"].Rows[i]["PLAN_NO"];
				
				cmd_inq.SetCommandText("select count(*)  from twmsm61 t where t.truck_no = '" + twmsm61["TRUCK_NO"].ToString() + "' and t.mat_wt =  0  and t.back11 = ' ' and unload_state = '1'");
				if (cmd_inq.ExecuteScalar().ToInt8() > 0)
				{
					cmd_inq.Close();
				    CException ex;
					ex.SetMsg(twmsm61["TRUCK_NO"].ToString()+"有未计量完成的装车实绩,不能再装了");
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
				twmsm61["PRACTICE_NO"] = sj;
				tmmsm67.Reset();
				tmmsm67["PURCHASEDOCID"] = twmsm61["PLAN_NO"];
				tmmsm67.Query();
				twmsm61["TRANS_TYPE"] = "3";
				twmsm61["LOAD_CODE"] = tmmsm67["LOAD_CODE"];
				twmsm61["LOAD_CODE_FACTORY"] = tmmsm67["LOAD_CODE"].ToString().Substring(0, 4);
				twmsm61["LOAD_CODE_AREA"] = tmmsm67["LOAD_CODE"].ToString().Substring(0,6 );
				twmsm61["UNLOAD_CODE"] = tmmsm67["UNLOAD_POINT_CODE"];
				twmsm61["UNLOAD_CODE_FACTORY"] = tmmsm67["UNLOAD_POINT_CODE"].ToString().Substring(0, 4);
				twmsm61["UNLOAD_CODE_AREA"] = tmmsm67["UNLOAD_POINT_CODE"].ToString().Substring(0, 6);
				twmsm61["DEAL_FLAG"] = "I";
				twmsm61["DG_UNIT_CODE"] = tmmsm67["DG_UNIT_CODE"];
				twmsm61["RECEIVE_UNIT_CODE"] = tmmsm67["RECV_DEPT_CODE"]; 
				twmsm61["LOAD_END_TIME"] = datetime;  
				twmsm61["MATERIAL_CODE"] = tmmsm67["MAT_CODE"];
				twmsm61["ARCHIVE_FLAG"] = "1";
				twmsm61["UNLOAD_STATE"] = "0";

			   
			

				twmsm61["REC_CREATOR"] = s.userid;   //记录创建责任者
				twmsm61["REC_CREATE_TIME"] = datetime;   //记录创建时刻
		
				twmsm61.TrimOrBlank();
				twmsm61.Insert();




			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM67_MODIFY") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_MODIFY"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();

				twmsm61.MergeFrom(bcls_rec->Tables["MMSM67_MODIFY"].Rows[i]);
				twmsm61.TrimOrBlank();

				/* 修改事件信息 */
				twmsm61["REC_REVISOR"] = s.userid;
				twmsm61["REC_REVISE_TIME"] = datetime;
				//twmsm61["CAR_USE_UNIT_CODE"] = twmsm61["DG_UNIT_CODE"];
				twmsm61.TrimOrBlank();
				/*	twmsm61.Update("ELEM_SI, ELEM_MN, ELEM_P, ELEM_S, ELEM_TI, NET_WT_COMPUT, IRON_TEMP_COM, TPC_ST_END_TIME, IRON_TEMP, IRON_TEMP_TIME, ADDSCRAP_WT, "
				"START_TIME, END_TIME, EMPTY_FLAG, EMPTY_TIME, EMPTY_TIME_ACT, PRE_RAILNO, TPC_SOURCE, SAP_WT, SAP_TIME, SAP_FLAG, REC_REVISOR, REC_REVISE_TIME,"
				"PRACT_COLL_MODE, FACTORY_DIV, TIDCODE, TICODE, TAPNO, TPC_ID, POTID, BF_ID, GWEIGHT, GWTIME, TWEIGHT, TWTIME, NWEIGHT, ELEM_C",
				"TICODE");*/
				if (twmsm61["PRACTICE_NO"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					twmsm61.Update("*", "PURCHASEDOCID");
				}



			}


		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM67_UNLOADCAR") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_UNLOADCAR"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				twmsm61.MergeFrom(bcls_rec->Tables["MMSM67_UNLOADCAR"].Rows[i]);
				twmsm61.TrimOrBlank();
				if (twmsm61["PRACTICE_NO"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					twmsm61.Delete();
				}

			}
		}

		if (bcls_rec->Tables.IndexOf("MMSM67_SQH") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_SQH"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();

				twmsm61.MergeFrom(bcls_rec->Tables["MMSM67_SQH"].Rows[i]);
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
