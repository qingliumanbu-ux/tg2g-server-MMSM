/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
///
///废钢需求
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm_210045_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//二钢北区实际规格发送
BM2F_ENTERACE_TELE(cm_ect801_rcv)

int f_cm_ect801_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm3g("TMMSM3G");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		tmmsm3g["ID"] = bcls_rec->Tables[0].Rows[0]["id"].ToDecimal();
		tmmsm3g["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["mat_no"].ToString().Trim();
		tmmsm3g["MAT_ACT_THICK"] = bcls_rec->Tables[0].Rows[0]["mat_act_thick"].ToDecimal();
		tmmsm3g["MAT_ACT_WIDTH"] = bcls_rec->Tables[0].Rows[0]["mat_act_width"].ToDecimal();
		tmmsm3g["MAT_ACT_LEN"] = bcls_rec->Tables[0].Rows[0]["mat_act_len"].ToDecimal();
		tmmsm3g["SLAB_HEAD_WIDTH"] = bcls_rec->Tables[0].Rows[0]["mat_act_width_h"].ToDecimal();
		tmmsm3g["SLAB_MID_WIDTH"] = bcls_rec->Tables[0].Rows[0]["mat_act_width_m"].ToDecimal();
		tmmsm3g["SLAB_TAIL_WIDTH"] = bcls_rec->Tables[0].Rows[0]["mat_act_width_t"].ToDecimal();
		tmmsm3g["SLAB_MEASURE_CONCAVE"] = bcls_rec->Tables[0].Rows[0]["mat_cwz"].ToDecimal();
		tmmsm3g["SLAB_MEA_CON_FLAG"]  = bcls_rec->Tables[0].Rows[0]["mat_cwz_flag"].ToString().Trim();
		tmmsm3g["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[0]["timestamp"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("temp_av"))
		{
			tmmsm3g["CC_CUT_TEMP"] = bcls_rec->Tables[0].Rows[0]["temp_av"].ToDecimal();
		}
		if (tmmsm3g.QueryCount("ID,MAT_NO"))
		{
			tmmsm3g.Update("MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN,SLAB_HEAD_WIDTH,SLAB_MID_WIDTH,SLAB_TAIL_WIDTH,SLAB_MEASURE_CONCAVE,SLAB_MEA_CON_FLAG,TIME_STAMPS,CC_CUT_TEMP", "ID,MAT_NO");
		}
		else
		{
			tmmsm3g.Insert();
		}

		EIClass snd_210035;
		snd_210035.Tables[0].set_TableName("210045");
		snd_210035.Tables[0].Columns.Add(tmmsm3g);
		tmmsm3g.MergeTo(snd_210035.Tables[0]);

		doFlag = f_mmsm_210045_snd(&snd_210035, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


