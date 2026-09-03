/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一资源系统-废钢合金消耗实绩发送电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	int row_id = 0;

	/* 业务变量 */
	CString unitCode = " ";
	CString tcNO = " ";
	CString heatNo = " ";
	CString procNo = " ";
	CString lotNo = " ";
	CString stkNo = " ";
	CString matCode = " ";
	CString deal_flag = " ";
	CString seq_no_2a = " ";
	CString weigh_no = " ";
	CString st_no = " ";
	CString quality_batch_no = " ";
	CString recv_mat_time = " ";
	CString flag = "";  // 发送电文排除标记
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm85("TMMSM85");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_85inq(conn);
	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tcNO = "21C005";

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables["MMLCSND"].Rows.get_Count(); i++)
		{
			tmmsm2a_send.Reset();
			tmmsm2a_send.MergeFrom(bcls_rec->Tables["MMLCSND"].Rows[i]);
			deal_flag = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"].ToString(); 			

			//对照关系 小代码 MMDZ: 炼钢工序
			unitCode = " ";
			sqlstr =
				" SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE 1=1"				
				" AND CODE_DESC_5_CONTENT = @STATION_ID"
				" AND CODE_CLASS = 'MMDZ'"
				;
			cmd_85inq.SetCommandText(sqlstr);
			cmd_85inq.Parameters.Set("STATION_ID", tmmsm2a_send["DEV_CODE"].ToString());
			cmd_85inq.ExecuteReader();
			if (cmd_85inq.Read())
			{
				unitCode = cmd_85inq.GetString(1);
			}
			cmd_85inq.Close();

			if (unitCode.Trim() == "")
			{
				sprintf(s.msg, "发送电文失败，原因设备号[" + tmmsm2a_send["DEV_CODE"].ToString() + "]没有对应成本中心" );
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (deal_flag == "D")
			{
				tmmsm2a_send["DEVO_WT"] = 0 - tmmsm2a_send["DEVO_WT"].ToDecimal();
			}
			

			if (
				epex.SetValue("DEAL_FLAG", row_id, "I") < 0 ||
				epex.SetValue("DATA_ID", row_id, tmmsm2a_send["SEQ_NO_2A"].ToString()) < 0 ||
				epex.SetValue("WORK_DATE", row_id, tmmsm2a_send["DEVO_TIME"].ToString().SubstringNE(0, 8)) < 0 ||
				epex.SetValue("FACTORY_CODE", row_id, "6240") < 0 ||
				epex.SetValue("PROD_UNIT_CODE", row_id, unitCode) < 0 ||
				epex.SetValue("HEAT_NO", row_id, tmmsm2a_send["HEAT_NO"].ToString()) < 0 ||
				epex.SetValue("PROCESS_NO", row_id, tmmsm2a_send["SM_PLAN_NOL2"].ToString()) < 0 ||
				epex.SetValue("ST_NO", row_id, tmmsm2a_send["ST_NO"].ToString()) < 0 ||
				epex.SetValue("MAT_CODE", row_id, tmmsm2a_send["MAT_CODE"].ToString()) < 0 ||
				epex.SetValue("WEIGH_NO", row_id, tmmsm2a_send["WEIGH_NO"].ToString()) < 0 ||
				epex.SetValue("MAT_BATCH_NO", row_id, tmmsm2a_send["LOT_NO"].ToString()) < 0 ||
				epex.SetValue("CONSUM_WT", row_id, tmmsm2a_send["DEVO_WT"].ToDecimal() / 1000) < 0 ||
				epex.SetValue("WORK_TIME", row_id, tmmsm2a_send["DEVO_TIME"].ToString()) < 0 ||
				epex.SetValue("BACK3", row_id, tmmsm2a_send["PROD_DATE"].ToString()) < 0
				)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			row_id++;

			if (row_id == 200)
			{
				if (epex.SendTele() < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				// 释放
				epex.Uninitialize();

				if (epex.Initialize(tcNO) < 0)
				{
					sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				row_id = 0;
			} 		
			
		}

		if (row_id != 0)
		{
			if (epex.SendTele() < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			// 释放
			epex.Uninitialize();
		}

		


		/* ********* 程序处理结束 ********** */
		//strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
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

	return doFlag;

}
