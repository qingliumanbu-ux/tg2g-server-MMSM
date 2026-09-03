/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		lidq
Version:    1.0
Date:		2024-04-22
Description:修磨难度系数实绩---发智慧质量
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t823sb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString dealFlag = " ";
	CString prodDate = " ";
	CString costCenter = " ";
	CString heatNo = " ";
	CString stNo = " ";
	CDecimal cs06 = 0;
	CDecimal jq10 = 0;
	CDecimal rg10 = 0;

	CString cs_tc_no = "";//电文号
	EPEX epex;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 实体类定义 */
	CModel tmmsm34("TMMSM34");

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("t823sb");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 t823sb 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		for (int i = 0; i < bcls_rec->Tables["t823sb"].Rows.get_Count(); i++)
		{
			cs_tc_no = "T823SB";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			tmmsm34.Reset();//将数据清空
			tmmsm34.MergeFrom(bcls_rec->Tables["t823sb"].Rows[i]);//获取数据
			tmmsm34.Query();
			if (epex.SetValue(0, tmmsm34) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("DEAL_FLAG", 0, "1") < 0 //操作标记
				|| epex.SetValue("PROD_DATE", 0, tmmsm34["START_TIME"].ToString()) < 0  //生产日期
				|| epex.SetValue("COST_CENTER", 0, " ") < 0     //成本中心
				|| epex.SetValue("HEAT_NO", 0, tmmsm34["HEAT_NO"].ToString()) < 0  //炉号
				|| epex.SetValue("ST_NO", 0, tmmsm34["SG_SIGN"].ToString()) < 0	//出钢记号----钢种描述
				|| epex.SetValue("CS06", 0, 0.0) < 0     //损失率
				|| epex.SetValue("JQ10", 0, 0.0) < 0	 //机器
				|| epex.SetValue("RG10", 0, 0.0) < 0	 //人工
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}
			epex.Uninitialize();
		}

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
