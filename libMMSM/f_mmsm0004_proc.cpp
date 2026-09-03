/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      高海宁
Version:     1.0
Date:        2013-08-13
Description: 无命令板坯号创建目的板坯表和板坯工序表
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 无命令板坯号创建目的板坯表和板坯工序表
/// <para>
/// 1.把传入的参数插入到TMMSM03目的板坯表。
/// </para>
/// <para>数据库表：TMMSM03(目的板坯表)                                        </para>
/// <para>                                                                     </para>
/// </summary>
/// <param name="">                                                           </param>
/// <returns>                                                               </returns>
============================================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

#include "EI_TUXClass.h"

/* ***** 程序表结构引用 ***** */
//#include "tmmsm01.h"
//#include "tmmsm03.h"
//#include "tmmsm04.h"

int f_mmsm0004_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	int i_count = 0;
	CString datetime = "";

	//使用的表结构变量
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	//CTMMSM01 tmmsm01(conn);
	//   CTMMSM03 tmmsm03(conn);
	//CTMMSM04 tmmsm04(conn);
	CString  sqlstr("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		if (bcls_rec->Tables.Contains("MMSM0004") == false)
		{
			strcpy(s.msg, _RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取传入参数
		tmmsm03["MAT_NO"] = bcls_rec->Tables["MMSM0004"].Rows[0]["MAT_NO"].ToString();
		tmmsm03["INFUR_SLAB_WT"] = bcls_rec->Tables["MMSM0004"].Rows[0]["INFUR_SLAB_WT"].ToDecimal();
		tmmsm03["AIM_MAT_NO"] = bcls_rec->Tables["MMSM0004"].Rows[0]["AIM_MAT_NO"].ToString();
		tmmsm03["INFUR_SLAB_LEN"] = bcls_rec->Tables["MMSM0004"].Rows[0]["INFUR_SLAB_LEN"].ToDecimal();
		tmmsm03["INFUR_SLAB_THICK"] = bcls_rec->Tables["MMSM0004"].Rows[0]["INFUR_SLAB_THICK"].ToDecimal();
		tmmsm03["INFUR_SLAB_WID"] = bcls_rec->Tables["MMSM0004"].Rows[0]["INFUR_SLAB_WID"].ToDecimal();
		tmmsm03["ORDER_REMAIN_DIV"] = bcls_rec->Tables["MMSM0004"].Rows[0]["ORDER_REMAIN_DIV"].ToString();
		//tmmsm03.ORDER_NO = bcls_rec->Tables["MMSM0004"].Rows[0]["ORDER_NO"].ToString();
		//tmmsm03.CUST_MAT_NO = bcls_rec->Tables["MMSM0004"].Rows[0]["CUST_MAT_NO"].ToString();
		tmmsm03["PONO_SLAB"] = bcls_rec->Tables["MMSM0004"].Rows[0]["PONO_SLAB"].ToString();

		tmmsm03["ORDER_NO"] = " ";
		/*tmmsm03.PONO_SLAB = " ";*/

		Log::Trace("", __FUNCTION__, "linggu s.userid = [{0}], datetime = [{1}]", s.userid, datetime);
		Log::Trace("", __FUNCTION__, "linggu tmmsm03.INFUR_SLAB_LEN = [{0}]", tmmsm03["INFUR_SLAB_LEN"].ToString());
		Log::Trace("", __FUNCTION__, "linggu tmmsm03.INFUR_SLAB_WT = [{0}]", tmmsm03["INFUR_SLAB_WT"].ToString());

		/*校验传入参数*/
		if (tmmsm03["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, _RES("GCRSS0000035")/*材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm03["AIM_MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, _RES("MMHPS0000074")/*数据校验出错，目的材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm03["PONO_SLAB"].ToString().Trim() == "")
		{
			tmmsm03["PONO_SLAB"] = " ";
		}

		//创建目的板坯表
		tmmsm03["REC_CREATOR"] = s.userid;
		tmmsm03["REC_CREATE_TIME"] = datetime;
		tmmsm03["REC_REVISOR"] = s.userid;
		tmmsm03["REC_REVISE_TIME"] = datetime;
		tmmsm03["ARCHIVE_FLAG"] = " ";
		//tmmsm03.CUST_MAT_NO = tmmsm03.MAT_NO;
		tmmsm03["ORDER_REMAIN_DIV"] = "0";
		tmmsm03["PILE_INDEX"] = " ";
		tmmsm03["HOT_CHARGE_FLAG"] = "0";

		tmmsm03["INFUR_SLAB_MAX_LEN"] = tmmsm03["INFUR_SLAB_LEN"];
		tmmsm03["INFUR_SLAB_MIN_LEN"] = tmmsm03["INFUR_SLAB_LEN"];
		tmmsm03["INFUR_SLAB_MAX_WT"] = tmmsm03["INFUR_SLAB_WT"];
		tmmsm03["INFUR_SLAB_MIN_WT"] = tmmsm03["INFUR_SLAB_WT"];

		//新增目的板坯表
		tmmsm03.Insert();
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