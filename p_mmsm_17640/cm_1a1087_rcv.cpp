/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:GZG
Date:2021-08-13
Version:1.0
Description: 接收二级板坯连铸切断实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

#include "tmmsm33.h"
#include "tpssm01b.h"
#include "tpssm03b.h"
#include "tpssm11b.h"
//#include "tmmsm33_fb.h"

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PES板坯连铸炉次实绩
/// <para>
/// 接收PES板坯连铸炉次实绩并处理
/// </para>方坯2#连铸L2系统方坯连铸切断实绩，C2--GE接收方坯连铸切断实绩
/// </summary>
/// <param name="tmmsm33">板坯连铸炉次实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//计算材料重量
int f_mmsmb33_insert(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmb33_delete(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmb33_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//20200810  DHCR自动表判 zqq add

// service入口
BM2F_ENTERACE_TELE(cm_c57z33_rcv)

int f_cm_c57z33_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	CString fin_cut_flag = "";
	CString proc_div = "";
	CDecimal mat_cut_len = 0;

	CTMMSM33 tmmsm33(conn);
	CTPSSM01B tpssm01b(conn);
	CTPSSM03B tpssm03b(conn);
	CTPSSM11B tpssm11b(conn);
	/*CTMMSM33_FB tmmsm33_fb(conn);*/

	EIClass bcls_rec_sm33;
	bcls_rec_sm33.Tables[0].Columns.Add(DT_STRING, "FIN_CUT_FLAG");
	EIClass bcls_rec_sm33_fb;

	EIClass bcls_rec_matwt;
	bcls_rec_matwt.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_matwt.Tables[0].Columns.Add(DT_STRING, "SLAB_LEN");
	bcls_rec_matwt.Tables[0].Columns.Add(DT_STRING, "SLAB_THICK");
	bcls_rec_matwt.Tables[0].Columns.Add(DT_STRING, "SLAB_WIDTH");
	bcls_rec_matwt.Tables[0].Columns.Add(DT_STRING, "CC_MACH_NO");

	EIClass bcls_rec_QM02;//材料表面判定
	bcls_rec_QM02.Tables[0].set_TableName("MM0099");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_MAKER");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_TIME");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MACH_CLEAR_FLAG");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SLAB_PLACE_CODE");
	bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SPARE_ITEM_0");

	try
	{
		proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "PROC_DIV = [{0}]", proc_div);
		mat_cut_len = bcls_rec->Tables[0].Rows[0]["MAT_CUT_LEN"];
		tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		fin_cut_flag = " ";

		Log::Trace("", __FUNCTION__, "PROC_NO = [{0}]", tmmsm33.PROC_NO);
		Log::Trace("", __FUNCTION__, "HEAT_NO = [{0}]", tmmsm33.HEAT_NO);
		Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", tmmsm33.MAT_NO);
		Log::Trace("", __FUNCTION__, "LSLAB_NO = [{0}]", tmmsm33.LSLAB_NO);
		Log::Trace("", __FUNCTION__, "SLAB_CUT_TIME = [{0}]", tmmsm33.SLAB_CUT_TIME);
		Log::Trace("", __FUNCTION__, "fin_cut_flag = [{0}]", fin_cut_flag);
		Log::Trace("", __FUNCTION__, "mat_cut_len = [{0}]", mat_cut_len);
		tpssm11b.HEAT_NO = tmmsm33.HEAT_NO;

		if ("" == tmmsm33.MAT_NO.Trim())
		{
			sprintf(s.msg, "材料号[%s]不能为空", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]不能为空", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "tmmsm33.HEAT_NO = [{0}]", tmmsm33.HEAT_NO);
		if ("" == tmmsm33.HEAT_NO.Trim()
			&& proc_div == "D")
		{
			tmmsm33.HEAT_NO = tmmsm33.MAT_NO.Substring(0, 8);
			Log::Trace("", __FUNCTION__, "D:tmmsm33.HEAT_NO = [{0}]", tmmsm33.HEAT_NO);
		}

		if ("" == tmmsm33.HEAT_NO.Trim())
		{
			sprintf(s.msg, "材料号[%s]的熔炼号不能为空", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]的熔炼号不能为空", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (!tpssm11b.Query("HEAT_NO"))
		{
			sprintf(s.msg, "熔炼号[%s]计划信息查询失败。", (const char*)tpssm11b.HEAT_NO);
			sprintf(s.sysmsg, "熔炼号[%s]计划信息查询失败。", (const char*)tpssm11b.HEAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ("" == tmmsm33.ST_NO.Trim())
		{
			Log::Trace("", __FUNCTION__, "材料号[{0}]的出钢记号取计划出钢记号[{1}]", tmmsm33.MAT_NO, tpssm11b.ST_NO);
			tmmsm33.ST_NO = tpssm11b.ST_NO;
		}

		if ("" == tmmsm33.PONO.Trim())
		{
			Log::Trace("", __FUNCTION__, "材料号[{0}]的制造命令号取计划造命令号[{1}]", tmmsm33.MAT_NO, tpssm11b.PONO);
			tmmsm33.PONO = tpssm11b.PONO;
		}
		else if (tpssm11b.PONO != tmmsm33.PONO.Trim())
		{
			Log::Trace("", __FUNCTION__, "材料号[{0}]的制造命令号与计划造命令号[{1}]不匹配", tmmsm33.MAT_NO, tpssm11b.PONO);
			CFormattable arguments[] = { tmmsm33.MAT_NO, tpssm11b.PONO };// 定义参数列表的数组
			CMessageFormat::Format(s.msg, "材料号[{0}]的制造命令号与计划造命令号[{1}]不匹配", arguments, 2);//格式化字符串
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ("" == tmmsm33.CC_MACH_NO.Trim())
		{
			sprintf(s.msg, "材料号[%s]的连铸机号不能为空", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]的连铸机号不能为空", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm33.CAST_NO.Trim())
		{
			sprintf(s.msg, "材料号[%s]的连铸浇次号不能为空", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]的连铸浇次号不能为空", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*if ("1" != tmmsm33.CAST_NO.Substring(0, 1))*/
		if (tmmsm33.CC_MACH_NO != tmmsm33.CAST_NO.Substring(1, 1))
		{
			sprintf(s.msg, "材料号[%s]的连铸浇次号与连铸机号不符", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]的连铸浇次号与连铸机号不符", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ("1" != tmmsm33.DZ_FLAG.Trim() && "0" != tmmsm33.DZ_FLAG.Trim() && "" != tmmsm33.DZ_FLAG.Trim())
		{
			sprintf(s.msg, "定重标记不为0或1", (const char*)tmmsm33.DZ_FLAG);
			sprintf(s.sysmsg, "定重标记不为0或1", (const char*)tmmsm33.DZ_FLAG);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm33.DZ_FLAG.Trim() == "1")
		{
			if (tmmsm33.SLAB_DZ_WT == 0)
			{
				sprintf(s.msg, "定重钢坯定重为0", tmmsm33.SLAB_DZ_WT);
				sprintf(s.sysmsg, "定重钢坯定重为0", tmmsm33.SLAB_DZ_WT);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if ("" == tmmsm33.SLAB_PLAN_DEST.Trim())
		{
			tpssm01b.PONO = tmmsm33.PONO;
			if (!tpssm01b.Query("PONO"))
			{
				sprintf(s.msg, "制造命令号[%s](tpssm01b)不存在。", (const char*)tpssm11b.PONO);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm33.SLAB_PLAN_DEST = tpssm01b.SLAB_DEST;
		}

		tmmsm33.PRACT_COLL_MODE = "1";
		tmmsm33.FACTORY_DIV = "A2";
		tmmsm33.MAT_TUBE = 1;
		//tmmsm33.SLAB_CUT_SEQ = atoi((const char*)tmmsm33.MAT_NO.Substring(9, 2));
		/*tmmsm33.SLAB_CUT_SEQ = atoi((const char*)tmmsm33.MAT_NO.Substring(8, 3))*/;//方坯会出现100支以上的情况，孙超 2019.3.27
		tmmsm33.PREC_ST_NO = tmmsm33.ST_NO;
		tmmsm33.STATION_NO = tmmsm33.CC_MACH_NO;

		Log::Trace("", __FUNCTION__, "SLAB_CUT_SEQ = [{0}]", tmmsm33.SLAB_CUT_SEQ);

		bcls_rec_matwt.Tables[0].Rows.Add();
		bcls_rec_matwt.Tables[0].Rows[0]["ST_NO"] = tmmsm33.ST_NO;
		bcls_rec_matwt.Tables[0].Rows[0]["SLAB_LEN"] = tmmsm33.SLAB_LEN;
		bcls_rec_matwt.Tables[0].Rows[0]["SLAB_THICK"] = tmmsm33.SLAB_THICK;
		bcls_rec_matwt.Tables[0].Rows[0]["SLAB_WIDTH"] = tmmsm33.SLAB_WIDTH;
		bcls_rec_matwt.Tables[0].Rows[0]["CC_MACH_NO"] = tmmsm33.CC_MACH_NO;

		/*if (tmmsm33.DZ_FLAG.Trim() != "1")
		{
		doFlag = f_mmsm_get_matwt(&bcls_rec_matwt, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm33.SLAB_WT = bcls_ret->Tables[0].Rows[0]["SLAB_WT"];
		}*/


		/*if (tmmsm33.DZ_FLAG.Trim() == "1")
		{
		tmmsm33.SLAB_WT = tmmsm33.SLAB_DZ_WT;
		tmmsm33.SLAB_PLAN_DEST = tmmsm33.SLAB_WT_ID.Substring(0, 2);

		if (12 != tmmsm33.SLAB_WT_ID.GetLength())
		{
		sprintf(s.msg, "定重ID不为12位", (const char*)tmmsm33.SLAB_WT_ID);
		sprintf(s.sysmsg, "定重ID不为12位", (const char*)tmmsm33.SLAB_WT_ID);
		throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm33.SLAB_PLAN_DEST == "B1")
		{
		tmmsm33.SLAB_PLAN_DEST = "L1";
		}
		else if (tmmsm33.SLAB_PLAN_DEST == "B2")
		{
		tmmsm33.SLAB_PLAN_DEST = "L2";
		}
		else if (tmmsm33.SLAB_PLAN_DEST == "B3")
		{
		tmmsm33.SLAB_PLAN_DEST = "L3";
		}
		else if (tmmsm33.SLAB_PLAN_DEST == "D1")
		{
		tmmsm33.SLAB_PLAN_DEST = "W1";
		}
		}*/
		tmmsm33.MAT_THEORY_WT = tmmsm33.SLAB_WT;

		if ("I" == proc_div)
		{
			if (0 < tmmsm33.QueryCount("MAT_NO"))
			{
				//sprintf(s.msg, "材料号[%s]切割实绩已存在，不可重复新增，如需修改请L2先删除后再新增。", (const char*)tmmsm33.MAT_NO);
				//sprintf(s.sysmsg, "材料号[%s]切割实绩已存在，不可重复新增，如需修改请L2先删除后再新增。", (const char*)tmmsm33.MAT_NO);
				//throw CApplicationException(-1, s.msg, log.Location);

				tmmsm33.REC_REVISOR = "C57Z33";
				tmmsm33.REC_REVISE_TIME = datetime;

				tmmsm33.MergeTo(bcls_rec_sm33.Tables[0], false);
				bcls_rec_sm33.Tables[0].Rows[0]["FIN_CUT_FLAG"] = fin_cut_flag;

				doFlag = f_mmsmb33_update(&bcls_rec_sm33, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else
			{
				tmmsm33.REC_CREATOR = "C57Z33";
				tmmsm33.REC_CREATE_TIME = datetime;

				if (tmmsm33.SLAB_LEN == mat_cut_len)
				{
					tmmsm33.INSPECT_PERSON = "1";
				}
				else
				{
					Log::Trace("", __FUNCTION__, "qqqqqqqqq");
					tmmsm33.INSPECT_PERSON = "0";
				}

				tmmsm33.MergeTo(bcls_rec_sm33.Tables[0], false);
				bcls_rec_sm33.Tables[0].Rows[0]["FIN_CUT_FLAG"] = fin_cut_flag;


				if (!bcls_rec_sm33.Tables[0].Columns.Contains("FIX_SLAB_NUM"))
				{
					bcls_rec_sm33.Tables[0].Columns.Add(DT_STRING, "FIX_SLAB_NUM");
				}
				if (!bcls_rec_sm33.Tables[0].Columns.Contains("GETSLAB_MODE"))
				{
					bcls_rec_sm33.Tables[0].Columns.Add(DT_STRING, "GETSLAB_MODE");
				}

				tpssm03b.PONO = tmmsm33.PONO;
				tpssm03b.LSLAB_NO = tmmsm33.LSLAB_NO;

				int fix_slab_num = tpssm03b.QueryCount("PONO, LSLAB_NO");
				Log::Trace("", __FUNCTION__, "FIX_SLAB_NUM = [{0}]", fix_slab_num);

				bcls_rec_sm33.Tables[0].Rows[0]["FIX_SLAB_NUM"] = fix_slab_num;
				bcls_rec_sm33.Tables[0].Rows[0]["GETSLAB_MODE"] = "A";//对应模式  A:自动获取未数据库未核对坯   S：按传入数据库1、2进行核对
				Log::Trace("", __FUNCTION__, "fin_cut_flag = [{0}]", fin_cut_flag);
				doFlag = f_mmsmb33_insert(&bcls_rec_sm33, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			tmmsm33.REC_CREATOR = "C57Z33";
			tmmsm33.REC_CREATE_TIME = datetime;

			
			
			tmmsm33.MergeTo(bcls_rec_sm33_fb.Tables[0], false);
			/*tmmsm33_fb.MergeFrom(bcls_rec_sm33_fb.Tables[0].Rows[0]);
			tmmsm33_fb.Delete();
			tmmsm33_fb.ARCHIVE_STAMP_NO = "新增";
			tmmsm33_fb.Insert();*/

			//针对于DHCR计划 钢坯产出自动表判合格 20200810 zqq add by
			if (tmmsm33.HOT_CHARGE_FLAG == "2")
			{
				bcls_rec_QM02.Tables[0].Rows.Add();
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_ID"] = "QM02";
				bcls_rec_QM02.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM02.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM02.Tables[0].Rows[0]["FUNC_ID"] = "cm_c77z33_rcv";
				bcls_rec_QM02.Tables[0].Rows[0]["MAT_NO"] = tmmsm33.MAT_NO;;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = "1";// 1:合格
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_MAKER"] = s.userid;
				bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_TIME"] = datetime;
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
				bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";
				//bcls_rec_QM02.Tables[0].Rows[0]["SLAB_PLACE_CODE"] = tmmsm01.SLAB_PLACE_CODE;
				bcls_rec_QM02.Tables[0].Rows[0]["SPARE_ITEM_0"] = "DHCR计划，自动表判合格。";
				doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		else if ("D" == proc_div)
		{
			if (1 > tmmsm33.QueryCount("MAT_NO"))
			{
				sprintf(s.msg, "材料号[%s]切割实绩不存在，不可删除。", (const char*)tmmsm33.MAT_NO);
				sprintf(s.sysmsg, "材料号[%s]切割实绩不存在，不可删除。", (const char*)tmmsm33.MAT_NO);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm33.MergeTo(bcls_rec_sm33.Tables[0], false);
			bcls_rec_sm33.Tables[0].Rows[0]["FIN_CUT_FLAG"] = fin_cut_flag;
			Log::Trace("", __FUNCTION__, "fin_cut_flag = [{0}]", fin_cut_flag);

			doFlag = f_mmsmb33_delete(&bcls_rec_sm33, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm33.MergeTo(bcls_rec_sm33_fb.Tables[0], false);
			/*tmmsm33_fb.MergeFrom(bcls_rec_sm33_fb.Tables[0].Rows[0]);
			tmmsm33_fb.Delete();*/

		}
		else
		{
			sprintf(s.msg, "材料号[%s]处理区分[%s]不做处理。", (const char*)tmmsm33.MAT_NO);
			sprintf(s.sysmsg, "材料号[%s]处理区分[%s]不做处理。", (const char*)tmmsm33.MAT_NO);
			throw CApplicationException(-1, s.msg, log.Location);
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

		tmmsm33.MergeTo(bcls_rec_sm33_fb.Tables[0], false);
		//tmmsm33_fb.MergeFrom(bcls_rec_sm33_fb.Tables[0].Rows[0]);
		//if ((tmmsm33_fb.MAT_NO.Trim() != " "))
		//{
		//	tmmsm33_fb.ARCHIVE_STAMP_NO = "新增";
		//	if ("D" == proc_div)
		//	{
		//		tmmsm33_fb.ARCHIVE_STAMP_NO = "删除";
		//		tmmsm33_fb.SLAB_CUT_SEQ = tmmsm33_fb.SLAB_CUT_SEQ + 500;
		//	}
		//	tmmsm33_fb.REC_CREATOR = "C2GE33";
		//	tmmsm33_fb.REC_CREATE_TIME = datetime;
		//	tmmsm33_fb.ARCHIVE_FLAG = "1";//借用ARCHIVE_FLAG来做错误标记
		//	tmmsm33_fb.COMPANY_NAME = s.msg;//借用COMPANY_NAME来做错误信息

		//	tpabort(0);
		//	tpbegin(0, 0);

		//	tmmsm33_fb.Delete();
		//	tmmsm33_fb.Insert();

		//	tpcommit(0);
		//	tpbegin(0, 0);
		//}

	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;

		tmmsm33.MergeTo(bcls_rec_sm33_fb.Tables[0], false);
		//tmmsm33_fb.MergeFrom(bcls_rec_sm33_fb.Tables[0].Rows[0]);
		//if ((tmmsm33_fb.MAT_NO.Trim() != " "))
		//{
		//	tmmsm33_fb.ARCHIVE_STAMP_NO = "新增";
		//	if ("D" == proc_div)
		//	{
		//		tmmsm33_fb.ARCHIVE_STAMP_NO = "删除";
		//		tmmsm33_fb.SLAB_CUT_SEQ = tmmsm33_fb.SLAB_CUT_SEQ + 500;
		//	}
		//	tmmsm33_fb.REC_CREATOR = "C2GE33";
		//	tmmsm33_fb.REC_CREATE_TIME = datetime;
		//	tmmsm33_fb.ARCHIVE_FLAG = "1";//借用ARCHIVE_FLAG来做错误标记
		//	tmmsm33_fb.COMPANY_NAME = s.msg;//借用COMPANY_NAME来做错误信息

		//	tpabort(0);
		//	tpbegin(0, 0);

		//	tmmsm33_fb.Delete();
		//	tmmsm33_fb.Insert();

		//	tpcommit(0);
		//	tpbegin(0, 0);
		//}

	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
