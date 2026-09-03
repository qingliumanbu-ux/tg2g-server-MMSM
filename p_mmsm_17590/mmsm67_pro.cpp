/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2024-01-08
Description: MMSM67增删改
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明

BM2F_ENTERACE(mmsm67_pro)

int f_mmsm67_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");
	CString	c_datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm67("TMMSM67");

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

		if (bcls_rec->Tables.IndexOf("MMSM67_ADD") >= 0)
		{



			for (int i = 0; i < bcls_rec->Tables["MMSM67_ADD"].Rows.get_Count(); i++)
			{
				tmmsm67.Reset();
				//Log::Trace("", __FUNCTION__, "datetime1				= [{0}]", (const char*)datetime);

				tmmsm67.MergeFrom(bcls_rec->Tables["MMSM67_ADD"].Rows[i]);
				//Log::Trace("", __FUNCTION__, "datetime2				= [{0}]", (const char*)datetime);

				tmmsm67.TrimOrBlank();

				/*	CException ex;
				ex.SetMsg("123");
				throw ex;*/

				CString  dh = "21JF6240" + CDateTime::Today().ToString("yyyyMMdd").Substring(2,6) + EPGetNextSeq("SQ_JLYLID", conn);
				//Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);
				///* 新增事件信息 */
				tmmsm67["PURCHASEDOCID"] = dh;
				tmmsm67["APTIME"] = datetime; 
				/*sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM67     WHERE 1=1   AND MAT_CODE= @MAT_CODE and PURCHASEDOCID = ' ' ";
				cmd_inq.Parameters.Set("MAT_CODE", tmmsm67["MAT_CODE"].ToString());
				Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", tmmsm67["MAT_CODE"].ToString());*/
				//分页获取
				/*cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cd_seq_no = cmd_inq.GetDecimal(1) + 1;
				}
				cmd_inq.Close();*/
			
				tmmsm67["APPLY_BY"] = s.username;
				tmmsm67["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm67["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm67["STATUS"] = "0";
				tmmsm67["DG_UNIT_CODE"] = "6240";
				tmmsm67["RECV_DEPT_CODE"] = "6460";
				tmmsm67["CAR_USE_UNIT_CODE"] = tmmsm67["RECV_DEPT_CODE"];
				tmmsm67.TrimOrBlank();
				tmmsm67.Insert();




			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM67_MODIFY") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm67.Reset();

				tmmsm67.MergeFrom(bcls_rec->Tables["MMSM67_MODIFY"].Rows[i]);
				tmmsm67.TrimOrBlank();

				/* 修改事件信息 */
				tmmsm67["REC_REVISOR"] = s.userid;
				tmmsm67["REC_REVISE_TIME"] = datetime;

				//tmmsm67["CAR_USE_UNIT_CODE"] = tmmsm67["DG_UNIT_CODE"];
				tmmsm67.TrimOrBlank();
				tmmsm67["APTIME"] = datetime;
				/*	tmmsm67.Update("ELEM_SI, ELEM_MN, ELEM_P, ELEM_S, ELEM_TI, NET_WT_COMPUT, IRON_TEMP_COM, TPC_ST_END_TIME, IRON_TEMP, IRON_TEMP_TIME, ADDSCRAP_WT, "
				"START_TIME, END_TIME, EMPTY_FLAG, EMPTY_TIME, EMPTY_TIME_ACT, PRE_RAILNO, TPC_SOURCE, SAP_WT, SAP_TIME, SAP_FLAG, REC_REVISOR, REC_REVISE_TIME,"
				"PRACT_COLL_MODE, FACTORY_DIV, TIDCODE, TICODE, TAPNO, TPC_ID, POTID, BF_ID, GWEIGHT, GWTIME, TWEIGHT, TWTIME, NWEIGHT, ELEM_C",
				"TICODE");*/
				Log::Trace("", __FUNCTION__, "applytime			= [{0}]", tmmsm67["APTIME"].ToString());
				if (tmmsm67["PURCHASEDOCID"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tmmsm67.Update("*", "PURCHASEDOCID");
				}
				


			}


		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM67_DELETE") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm67.Reset();
				tmmsm67.MergeFrom(bcls_rec->Tables["MMSM67_DELETE"].Rows[i]);
				tmmsm67.TrimOrBlank();
				if (tmmsm67["PURCHASEDOCID"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tmmsm67.Delete("PURCHASEDOCID");
				}

			}
		}

		if (bcls_rec->Tables.IndexOf("MMSM67_SQH") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM67_SQH"].Rows.get_Count(); i++)
			{
				tmmsm67.Reset();

				tmmsm67.MergeFrom(bcls_rec->Tables["MMSM67_SQH"].Rows[i]);
				tmmsm67.TrimOrBlank();
				CString  dh = "21PS6240" + CDateTime::Today().ToString("yyyyMMdd") + EPGetNextSeq("SQ_JLYLID", conn);
				Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);

				/*if (tmmsm67["APTIME"].ToString().Trim() == "")
				{

				}*/
				if (tmmsm67["APTIME"].ToString().Trim() != "")
				{
					if (tmmsm67["APTIME"].ToString().Trim() <c_datetime)
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

				if (tmmsm67["PURCHASEDOCID"].ToString().Trim() == "")
				{
					/* 新增事件信息 */
					tmmsm67["PURCHASEDOCID"] = dh;
					//tmmsm67["APTIME"] = datetime;
					tmmsm67["APPLY_BY"] = s.username;
					tmmsm67["REC_CREATOR"] = s.userid;   //记录创建责任者
					tmmsm67["REC_CREATE_TIME"] = datetime;   //记录创建时刻
					tmmsm67.Update("PURCHASEDOCID,APTIME", "MAT_CODE,SEQ_NO");
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


