/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-10-30
Version:1.0
Description: 计算理论重量
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

BM2_FUNCTION_EXPORT

int f_mmsm_get_theorywt( CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	const CDecimal density = 7.85;
	CString sqlstr = "";
	CDecimal mat_radius;


	try
	{
		////Log::Info("", __FUNCTION__, "density=[{0}]", density);

		////Log::Info("", __FUNCTION__, "MAT_ACT_THICK=[{0}]", MAT_ACT_THICK);
		////Log::Info("", __FUNCTION__, "MAT_ACT_WIDTH=[{0}]", MAT_ACT_WIDTH);
		////Log::Info("", __FUNCTION__, "MAT_ACT_LEN=[{0}]", MAT_ACT_LEN);
		////Log::Info("", __FUNCTION__, "MAT_NUM=[{0}]", MAT_NUM);

		

		if (MAT_ACT_WIDTH == 0) //圆坯
		{
			mat_radius = MAT_ACT_THICK / 2000;
			MAT_THEORY_WT = 3.14* mat_radius * mat_radius *  (MAT_ACT_LEN / 1000) * density * MAT_NUM;
		}
		else
		{
			MAT_THEORY_WT = (MAT_ACT_THICK / 1000) * (MAT_ACT_WIDTH / 1000) * (MAT_ACT_LEN / 1000) * density * MAT_NUM;
		}
	

		MAT_THEORY_WT = MAT_THEORY_WT.Round(3);
		////Log::Info("", __FUNCTION__, "MAT_THEORY_WT=[{0}]", MAT_THEORY_WT);
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

	return doFlag;

}
