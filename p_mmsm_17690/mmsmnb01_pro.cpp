/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2023-12-04
Description: MMSM65增删改
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明

BM2F_ENTERACE(mmsmnb01_pro)

int f_mmsmnb01_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	Log::Trace("", __FUNCTION__, "进来了");


	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");
	CString	c_datetime("");
	CString aptime("");
	CString datetime1("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm65("TMMSMNB01"); //南北区互调

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_seq_no = 0;
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "TICODE");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_datetime = datetime.Substring(0, 8) + "000000";
		datetime1 = CDateTime::Today().ToString("yyyyMMdd");
		//datetime1 = datetime1.Substring(2, 6);
		/* 获得传入参数 */
		Log::Trace("", __FUNCTION__, "TABLE= [{0}]", datetime1);

		Log::Trace("", __FUNCTION__, "TABLE= [{0}]", bcls_rec->Tables.get_Count());
		Log::Trace("", __FUNCTION__, "get_TableName= [{0}]", bcls_rec->Tables[0].get_TableName());

		if (bcls_rec->Tables.IndexOf("MMSM65_ADD") >= 0)
		{



			for (int i = 0; i < bcls_rec->Tables["MMSM65_ADD"].Rows.get_Count(); i++)
			{
				tmmsm65.Reset();
				//Log::Trace("", __FUNCTION__, "datetime1				= [{0}]", (const char*)datetime);

				tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_ADD"].Rows[i]);
				//Log::Trace("", __FUNCTION__, "datetime2				= [{0}]", (const char*)datetime);

				tmmsm65.TrimOrBlank();

				/*	CException ex;
				ex.SetMsg("123");
				throw ex;*/

				CString  dh = "6240" + CDateTime::Today().ToString("yyyyMMdd") + EPGetNextSeq("SQ_NBHD_ID", conn);
				Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);
				/* 新增事件信息 */
				tmmsm65["PURCHASEDOCID"] = dh;
				tmmsm65["APTIME"] = datetime;
				sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSMNB01     WHERE 1=1   AND MAT_CODE= @MAT_CODE  ";
				cmd_inq.Parameters.Set("MAT_CODE", tmmsm65["MAT_CODE"].ToString());
				Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", tmmsm65["MAT_CODE"].ToString());
				//分页获取
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cd_seq_no = cmd_inq.GetDecimal(1) + 1;
				}
				cmd_inq.Close();
				tmmsm65["SEQ_NO"] = cd_seq_no;
				if ("" == tmmsm65["APPLY_BY"].ToString().Trim())
				{
					tmmsm65["APPLY_BY"] = s.username;
				}
				tmmsm65["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm65["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm65["STATUS"] = "0";
				tmmsm65["STATUS1"] = "0";					// 数据来源 1电文 2 手动新增
				tmmsm65["USE_LOGO"] = "0";
				//tmmsm65["CAR_USE_UNIT_CODE"] = tmmsm65["DG_UNIT_CODE"];
				tmmsm65.TrimOrBlank();
				tmmsm65.Insert();




			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM65_MODIFY") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM65_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm65.Reset();

				tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_MODIFY"].Rows[i]);
				tmmsm65.TrimOrBlank();

				/* 修改事件信息 */
				tmmsm65["REC_REVISOR"] = s.userid;
				tmmsm65["REC_REVISE_TIME"] = datetime;
				//tmmsm65["CAR_USE_UNIT_CODE"] = tmmsm65["DG_UNIT_CODE"];
				tmmsm65.TrimOrBlank();
				/*	tmmsm65.Update("ELEM_SI, ELEM_MN, ELEM_P, ELEM_S, ELEM_TI, NET_WT_COMPUT, IRON_TEMP_COM, TPC_ST_END_TIME, IRON_TEMP, IRON_TEMP_TIME, ADDSCRAP_WT, "
				"START_TIME, END_TIME, EMPTY_FLAG, EMPTY_TIME, EMPTY_TIME_ACT, PRE_RAILNO, TPC_SOURCE, SAP_WT, SAP_TIME, SAP_FLAG, REC_REVISOR, REC_REVISE_TIME,"
				"PRACT_COLL_MODE, FACTORY_DIV, TIDCODE, TICODE, TAPNO, TPC_ID, POTID, BF_ID, GWEIGHT, GWTIME, TWEIGHT, TWTIME, NWEIGHT, ELEM_C",
				"TICODE");*/
				if (tmmsm65["TICODE"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tmmsm65.Update("*", "TICODE");
				}
				else
				{
					/* 删除事件信息 */
					tmmsm65.Update("MAT_CODE,MAT_CODE_NAME,COST_CENTER,LOAD_CODE,UNLOAD_POINT_CODE,MEASURE_MODE,TRUCK_MODEL,CAR_NUM,PLAN_WT,S_DATETIME,E_DATETIME,RECV_DEPT_CODE", "TICODE,MAT_CODE,SEQ_NO");
					//tmmsm65.Delete("SEQ_NO");
				}


			}


		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM65_DELETE") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM65_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm65.Reset();
				tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_DELETE"].Rows[i]);
				tmmsm65.TrimOrBlank();
				if (tmmsm65["TICODE"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tmmsm65.Delete("TICODE");
				}
				else
				{
					/* 删除事件信息 */
					tmmsm65.Delete("MAT_CODE,SEQ_NO");
				}




			}
		}

		if (bcls_rec->Tables.IndexOf("MMSM65_SQH") >= 0)
		{
			//for (int i = 0; i < bcls_rec->Tables["MMSM65_SQH"].Rows.get_Count(); i++)
			//{
			//	tmmsm65.Reset();

			//	tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_SQH"].Rows[i]);
			//	tmmsm65.TrimOrBlank();
			//	CString  dh = "6240" + datetime1 + EPGetNextSeq("SQ_NBHD_ID", conn);
			//	Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);

			//	/*if (tmmsm65["APTIME"].ToString().Trim() == "")
			//	{

			//	}*/
			//	aptime = bcls_rec->Tables[1].Rows[0]["APTIME"].ToString().Trim();
			//	tmmsm65["APTIME"] = aptime;
			//	

			//	if (tmmsm65["TICODE"].ToString().Trim() == "")
			//	{
			//		/* 新增事件信息 */
			//		tmmsm65["TICODE"] = dh;
			//		//tmmsm65["APTIME"] = datetime;
			//		tmmsm65["APPLY_BY"] = s.username;
			//		tmmsm65["REC_CREATOR"] = s.userid;   //记录创建责任者
			//		tmmsm65["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			//		tmmsm65.Update("TICODE,APTIME", "MAT_CODE,SEQ_NO");

			//		bcls_ret->Tables[0].Rows.Add();
			//		bcls_ret->Tables[0].Rows[i]["TICODE"] = dh;

			//	}
			//	else
			//	{
			//		sprintf(s.msg, "已经生成调拨单号的物料不能再次生成要料调拨单号！！！");
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}
			//}
		}


	}


	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
	return doFlag;
}


