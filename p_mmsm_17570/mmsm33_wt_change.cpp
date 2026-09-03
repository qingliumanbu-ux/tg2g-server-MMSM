/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-05-25
Description: 板坯切断炉次查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/ 

/* ***** 静态函数申明 ***** */
//获取重量
int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC实绩查询
/// <para>
/// 1.根据时间范围,炉号等条件进行CC实绩查询。
/// 
/// </para>
/// <para>数据库表：TMMSM31(CC炉次实绩表)          </para>
/// <para>主调用函数：前台MMSM31画面F2(查询)按钮         </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33_wt_change)

int f_mmsm33_wt_change(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString v_lslab_no = "";
	//系统的分页类信息。
	CPageInfo pageInfo; 
	CModel tpssm11("TPSSM11");
	CDbCommand cmd_inq(conn);

	try
	{
		
		if (!bcls_ret->Tables[0].Columns.Contains("SLAB_WT"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SLAB_WT");
		}
		bcls_ret->Tables[0].Rows.Add();
		doFlag = f_mmsm_get_matwt(bcls_rec, bcls_ret, conn);
		bcls_ret->Tables[0].Rows[0]["SLAB_WT"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_WT"];
		

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
