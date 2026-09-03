/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 接收二级转炉实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

#include "tmmsm21.h"


/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PES转炉实绩
/// <para>
/// 接收PES转炉实绩并处理
/// </para>
/// </summary>
/// <param name="tmmsm21">转炉实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

int f_mmsmb_area3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmb_area3_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_gegsb2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE_TELE(cm_1a1021_rcv)

int f_cm_1a1021_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString l_HM_ID;	   // 铁水处理号
    CString l_PLAN_PROD_ORDER_ID;//计划生产指令号
	int l_TREATMENT_COUNT;	// 处理次数
	CString l_STATION_ID;  //    设备号      
	CString l_CAR_NO;	//    工位号     
	CString l_HM_LADLE_ID;	 //  铁水包号      
	CString l_DATI_ARRIVAL;     //  开始(进站)时间         
	CString l_EQU_NO;//设备区域号
              

	Decimal l_HM_WEIGHT  ;   //铁水重量
	Decimal l_HM_TEMP;   //铁水温度
	CString l_SHIFTNO;     // 班别组         
	CString l_GROUPID; //班次

	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		//数据项赋值
		l_HM_ID = bcls_rec->Tables[0].Rows[0]["l_HM_ID"].ToString();
	    l_PLAN_PROD_ORDER_ID= bcls_rec->Tables[0].Rows[0]["l_PLAN_PROD_ORDER_ID"].ToString();
		l_TREATMENT_COUNT = bcls_rec->Tables[0].Rows[0]["l_TREATMENT_COUNT"].ToString();
		l_STATION_ID = bcls_rec->Tables[0].Rows[0]["l_STATION_ID"].ToString();
		l_CAR_NO = bcls_rec->Tables[0].Rows[0]["l_CAR_NO"].ToString();
		l_HM_LADLE_ID = bcls_rec->Tables[0].Rows[0]["l_HM_LADLE_ID"].ToString();
		l_DATI_ARRIVAL = bcls_rec->Tables[0].Rows[0]["l_DATI_ARRIVAL"].ToString();

		if (l_HM_ID.Trim() == "" || l_PLAN_PROD_ORDER_ID.Trim() == "")
		{
			strcpy(s.msg, "l_HM_ID或l_PLAN_PROD_ORDER_ID为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Log::Trace("", __FUNCTION__, "heat_no = [{0}]", (const char*)tmmsm21.HEAT_NO);

		if (l_DATI_ARRIVAL.Trim() == "")
		{
			l_DATI_ARRIVAL = datetime;
		}
		

		sqlstr =
			" SELECT  STEEL_GRADE, DS_ET, PONO, MOLTIRON_TANK_NO "
			" FROM  LO_TAP_PLAN "
			" WHERE  HM_ID = @t_hm_id ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("t_hm_id", l_HM_ID);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_STEEL_GRADE = cmd_inq.GetString(1);
			l_DS_ET = cmd_inq.GetString(2);
			l_PLAN_PROD_ORDER_ID = cmd_inq.GetString(3);
			l_HM_LADLE_ID = cmd_inq.GetString(4);
		}
		else
		{
			sprintf(s.msg, "铁水处理号[%s]不存在，检查计划!", (const char*)l_HM_ID);
			sprintf(s.sysmsg, "铁水处理号[%s]不存在，检查计划!", (const char*)l_HM_ID);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

		sqlstr =
			" SELECT  HM_WEIGHT,HM_TEMP"
			" FROM  LM_MI_OUT"
			" WHERE  HM_ID = @t_hm_id ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("t_hm_id", l_HM_ID);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_HM_WEIGHT = cmd_inq.GetString(1);
			l_HM_TEMP = cmd_inq.GetString(2);
		}
		cmd_inq.Close();
		

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
