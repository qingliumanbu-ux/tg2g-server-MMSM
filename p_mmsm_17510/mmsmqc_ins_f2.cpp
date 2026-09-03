/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     周平
Version:    1.0
Date:       2018-11-08
Description: 炼钢板坯期初数据导入
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 
#include "math.h" 

//业务头




#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯期初数据导入
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsmqc_ins_f2)

int f_mmsmqc_ins_f2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	int trace_line = 0;
	int message_line = 0;
	int l_ok_flag = 1;
	int i_number = 0;
	int v_count = 0;
	int v_count_29 = 0;
	int v_count_04 = 0;
	int v_count_0x = 0;

	CString	v_date = "";
	CString	datetime = "";
	CString	datetime_18 = "";
	CString l_reason = " ";
	CString l_scrap_remark = " ";
	CString l_heat_no = " ";

	/* 实体类定义 */
	CModel tmmsm01_rec("TMMSM01");

	CModel tmmsm01qc("TMMSM01QC");
	CModel tmmsmqc_fp_rec("TMMSMQC_FP");
	CModel tmmsmqc_fp_ret("TMMSMQC_FP");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_tmmsmbp_inq(conn);
	CDbCommand cmd_tmmsm01qc_inq(conn);
	CDbCommand cmd_tqmts0z_inq(conn);
	CDbCommand cmd_tymsm04_inq(conn);
	CDbCommand cmd_ltmmsm01_test_inq(conn);

	try
	{
		EIClass inBlk_pes;
		inBlk_pes.Tables[0].Rows.Clear();

		datetime = CDateTime::Now().ToString("yyyymmddhhmiss");
		datetime_18 = CDateTime::Now().ToString("yyyymmddhhmissff");

		v_date = CDateTime::Now().ToString("yyyyMMdd");

		trace_line = trace_line + 1;
		l_ok_flag = 1;
		
		l_scrap_remark = ' ';
		l_heat_no = ' ';
		i_number = 0;
		v_count = -1;

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT * "
				" FROM TMMSM01QC "
				" WHERE ARCHIVE_FLAG != '1' and MAT_SHAPE_FLAG ='A' AND mat_line_type ='SM' AND REC_CREATOR ='QC'   "
				" ORDER BY MAT_NO ";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}];", sqlstr);
		cmd_tmmsmbp_inq.SetCommandText(sqlstr);
		cmd_tmmsmbp_inq.ExecuteReader();

		while (cmd_tmmsmbp_inq.Read())
		{
			cmd_tmmsmbp_inq.Fetch(tmmsm01qc);
			tmmsm01_rec.CopyFrom(tmmsm01qc);
			tmmsm01_rec["MAT_WT"] = tmmsm01_rec["MAT_ACT_WT"];
			tmmsm01_rec["FACTORY_PROD"] = tmmsm01_rec["FACTORY_DIV"];
			tmmsm01_rec.Insert();
			tmmsm01qc["ARCHIVE_FLAG"] = "1";
			if (tmmsm01qc["STOCK_NO"].ToString().Substring(0, 1) == "A" || tmmsm01qc["STOCK_NO"].ToString().Substring(0, 1) == "B")
			{
				tmmsm01_rec.MergeTo(inBlk_pes.Tables[0], false);
			}
		  
			tmmsm01qc.Update("ARCHIVE_FLAG", "MAT_NO");

			/*i_number = i_number + 1 ;

			if (i_number == 5000)
			{
				break;
			}*/
		}
		cmd_tmmsmbp_inq.Close();

	

		int inblk_pes_count = inBlk_pes.Tables[0].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "====inBlk_pes.Tables[0].Rows.get_Count [{0}]====", inblk_pes_count);
		if (inBlk_pes.Tables[0].Rows.get_Count() > 0)
		{
			//直接调用这个即可，EGGGP为系统名 100不知道啥意思..完整调用如下:
			//serivce 调用serivce
			Log::Trace("", __FUNCTION__, "====111====");
			try
			{
				f_epex_call_cgi_svc(conn, "CG7ZZ", "mmsmqc_ins_f", &inBlk_pes, bcls_ret, 100);
			}
			catch (CException& ex)
			{
				strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
				s.flag = ex.GetCode();
				doFlag = -1;
			}
			Log::Trace("", __FUNCTION__, "====222====");
			struct  ei_sys s_tmp;
			Log::Trace("", __FUNCTION__, "====333====");
			bcls_ret->GetSYS(&s_tmp);
			Log::Trace("", __FUNCTION__, "====444====");
			if (s_tmp.flag < 0)
			{
				Log::Trace("", __FUNCTION__, "mmsmqc_ins_f调用失败！s.flag = [{0}] s.msg = [{1}] s.sysmsg = [{2}]", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", __FUNCTION__, "====555====");
		}
		return doFlag = 0;
		Log::Trace("", __FUNCTION__, "导入成功！");
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
