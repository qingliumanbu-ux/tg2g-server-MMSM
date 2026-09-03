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
//获取材料号
int f_mmsm_get_matno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

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
BM2F_ENTERACE(mmsm33matno_inq)

int f_mmsm33matno_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString v_lslab_no = "";
	//系统的分页类信息。
	CPageInfo pageInfo; 
	CModel tpssm03("TPSSM03");
	CModel tpssm11("TPSSM11");
	CDbCommand cmd_inq(conn);

	try
	{
		
		/*获取板坯号*/
		if (!bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		}
		bcls_rec->Tables[0].Rows[0]["FUNC_ID"] = "MMSM01_MATNO";
		doFlag = f_mmsm_get_matno(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		

		if (!bcls_ret->Tables[0].Columns.Contains("FIX_SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "FIX_SLAB_NUM");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("SLAB_LEN"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SLAB_LEN");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SLAB_NUM");
		}
		if (bcls_rec->Tables[0].Columns.Contains("LSLAB_NO") && bcls_rec->Tables[0].Rows[0]["LSLAB_NO"].ToString()>" ")
		{
			v_lslab_no = bcls_rec->Tables[0].Rows[0]["LSLAB_NO"];
			tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
			//Log::Trace("", __FUNCTION__, "v_lslab_no=[{0}] PONO=[{1}]", v_lslab_no, tpssm11["PONO"].ToString());
			if (tpssm11.Query("HEAT_NO") == true)
			{
				sqlstr = CString(
					" SELECT LSLAB_NO,nvl(sum(slab_len),0),nvl(count(1),0),nvl(SLAB_NUM,0) FROM TPSSM03 "
					" WHERE LSLAB_NO = @lslab_no "
					" AND PONO = @pono "
					" AND SLAB_PROD_FLAG <> '1'"
					" group by LSLAB_NO,SLAB_NUM order by LSLAB_NO "
					);
				//Log::Trace("", __FUNCTION__, "sqlstr      =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("lslab_no", v_lslab_no);
				cmd_inq.Parameters.Set("pono", tpssm11["PONO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_ret->Tables[0].Rows[0]["FIX_SLAB_NUM"] = cmd_inq.GetDecimal(3);
					bcls_ret->Tables[0].Rows[0]["SLAB_LEN"] = cmd_inq.GetDecimal(2);
					bcls_ret->Tables[0].Rows[0]["SLAB_NUM"] = cmd_inq.GetDecimal(4);
				}
				cmd_inq.Close();
			}
			else
			{
				bcls_ret->Tables[0].Rows[0]["FIX_SLAB_NUM"] = 0;
				bcls_ret->Tables[0].Rows[0]["SLAB_LEN"] = 0;
				bcls_ret->Tables[0].Rows[0]["SLAB_NUM"] = 1;
			}
		}
		else if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
			//Log::Trace("", __FUNCTION__, "HEAT_NO      =[{0}]", tpssm11["HEAT_NO"].ToString());
			
			if (tpssm11.Query("HEAT_NO") == true)
			{
				sqlstr = CString(
					" SELECT LSLAB_NO,nvl(sum(slab_len),0),nvl(count(1),0),nvl(SLAB_NUM,0) FROM TPSSM03 "
					" WHERE SLAB_PROD_FLAG <> '1' "
					"   AND PONO = @pono "
					" group by LSLAB_NO,slab_num order by LSLAB_NO "
					);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("lslab_no", v_lslab_no);
				cmd_inq.Parameters.Set("pono", tpssm11["PONO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_ret->Tables[0].Rows[0]["FIX_SLAB_NUM"] = cmd_inq.GetDecimal(3);
					bcls_ret->Tables[0].Rows[0]["SLAB_LEN"] = cmd_inq.GetDecimal(2);
					bcls_ret->Tables[0].Rows[0]["SLAB_NUM"] = cmd_inq.GetDecimal(4);
				}
				cmd_inq.Close();
			}
			else
			{
				bcls_ret->Tables[0].Rows[0]["FIX_SLAB_NUM"] = 0;
				bcls_ret->Tables[0].Rows[0]["SLAB_LEN"] = 0;
				bcls_ret->Tables[0].Rows[0]["SLAB_NUM"] = 1;
			}
			
		}
		else
		{
			bcls_ret->Tables[0].Rows[0]["FIX_SLAB_NUM"] = 0;
			bcls_ret->Tables[0].Rows[0]["SLAB_LEN"] = 0;
			bcls_ret->Tables[0].Rows[0]["SLAB_NUM"] = 1;
		}


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
