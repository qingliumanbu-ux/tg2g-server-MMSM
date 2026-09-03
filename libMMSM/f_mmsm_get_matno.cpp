/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 获取材料号
remark:本函数目前暂不考虑按批管理
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/ 
//#include "ted54.h"

//#include "hmmsm01.h"
//外部函数声明
BM2_FUNCTION_EXPORT
 int f_mmsm_get_matno(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
 
	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	CString heatNo = "";
	CString newMatNo = "";
	CString newMatNoSeq = "";

	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString v_func_id = "";
	CString v_mat_no = "";
	CString c_mat_no = "";
	CString c_strand_no = "";
	CString cs_mat_no_max = "";
	CDecimal n_count = 0;             //校验是否已存在该记录
	CDecimal v_mat_id_max = 0;
	CDecimal c_mat_id_max = 0;

	int fetchRowCount = 0;
	CString sqlstr = "";
	EIClass inBlock;          //材料主档信息处理用
	//EIClass outBlock;


	CDbCommand cmd_inq(conn);
	CModel tmmsm01("TMMSM01");

	try
	{
		if (bcls_rec->Tables.Contains("TMMSM33"))
		{
			heatNo = bcls_rec->Tables["TMMSM33"].Rows[0]["HEAT_NO"].ToString().Trim();
			c_strand_no = bcls_rec->Tables["TMMSM33"].Rows[0]["STRAND_NO"].ToString().Trim();
			//Log::Trace("", __FUNCTION__, "c_strand_no	= [{0}]", c_strand_no);
			//Log::Trace("", __FUNCTION__, "heatNo	= [{0}]", heatNo);
			if (bcls_rec->Tables["TMMSM33"].Columns.Contains("SLAB_TYPE") && bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_TYPE"].ToString().Trim() != "3"){
				cmd_inq.SetCommandText("SELECT MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 )) FROM TMMSM33 WHERE HEAT_NO = @heatNo AND STRAND_NO = @strand_no ");

				cmd_inq.Parameters.Set("heatNo", heatNo);
				cmd_inq.Parameters.Set("strand_no", c_strand_no);

				cmd_inq.ExecuteReader();

				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1).Trim() != "")
					{
						newMatNoSeq = (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 1).ToString().Trim();//(CDecimal::Parse(cmd_inq.GetString(1).Trim().Substring(cmd_inq.GetString(1).Trim().GetLength() - 2, 2)) + 1).ToString().Trim();
						if (newMatNoSeq.GetLength() == 1) newMatNoSeq = "0" + newMatNoSeq;
						newMatNo = heatNo + c_strand_no.Trim() + newMatNoSeq;
					}
					else
					{
						newMatNo = heatNo + c_strand_no.Trim() + "01";
					}
				}
				else
				{
					newMatNo = heatNo + c_strand_no.Trim() + "01";
				}
				cmd_inq.Close();

				bcls_rec->Tables["TMMSM33"].Rows[0]["MAT_NO"] = newMatNo;
			}
			else{
				cmd_inq.SetCommandText("SELECT  MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 2 )) FROM TMMSM33 WHERE HEAT_NO = @heatNo");

				cmd_inq.Parameters.Set("heatNo", heatNo);

				cmd_inq.ExecuteReader();

				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1).Trim() != "")
					{
						newMatNoSeq = (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 1).ToString().Trim();//(CDecimal::Parse(cmd_inq.GetString(1).Trim().Substring(cmd_inq.GetString(1).Trim().GetLength()-3, 3)) + 1).ToString().Trim();
						if (newMatNoSeq.GetLength() == 1) newMatNoSeq = "00" + newMatNoSeq;
						if (newMatNoSeq.GetLength() == 2) newMatNoSeq = "0" + newMatNoSeq;
						newMatNo = heatNo + newMatNoSeq;
					}
					else
					{
						newMatNo = heatNo + "001";
					}
				}
				else
				{
					newMatNo = heatNo + "001";
				}
				cmd_inq.Close();

				bcls_rec->Tables["TMMSM33"].Rows[0]["MAT_NO"] = newMatNo;
			}
		}
		else if (bcls_rec->Tables.Contains("GENMATNO"))
		{
			//c_strand_no = bcls_rec->Tables["GENMATNO"].Rows[0]["STRAND_NO"].ToString().Trim();
			//Log::Trace("", __FUNCTION__, "c_strand_no	= [{0}]", c_strand_no);
			if (bcls_rec->Tables["GENMATNO"].Columns.Contains("MAT_SHAPE_FLAG") && bcls_rec->Tables["GENMATNO"].Rows[0]["MAT_SHAPE_FLAG"].ToString().Trim() != "A"){
				//Log::Trace("", __FUNCTION__, "TMMSM01.STOCK_NO	= [{0}]", tmmsm01["STOCK_NO"].ToString());

				tmmsm01["MAT_NO"] = bcls_rec->Tables["GENMATNO"].Rows[0]["MAT_NO"].ToString().Trim();
				tmmsm01.Query();

				c_strand_no = tmmsm01["STRAND_NO"];
				//Log::Trace("", __FUNCTION__, "c_strand_no11	= [{0}]", c_strand_no);
				//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO	= [{0}]", tmmsm01["MAT_NO"].ToString());
				if (tmmsm01["OLD_HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "传入的材料原始熔炼号为空");
					strcpy(s.sysmsg, "传入的材料原始熔炼号为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				sqlstr = "SELECT MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 )) FROM            "
					"(                                  "
					"  SELECT MAT_NO FROM TMMSM01       "
					"  WHERE LENGTH(MAT_NO) = @matNoLen "
					"  AND MAT_NO LIKE @startMatNo      "
					"  AND OLD_HEAT_NO = @oldHeatNo       "
					"  AND STRAND_NO = @strand_no       "
					"  UNION ALL                        "
					"  SELECT MAT_NO FROM HMMSM01       "
					"  WHERE LENGTH(MAT_NO) = @matNoLen "
					"  AND MAT_NO LIKE @startMatNo      "
					"  AND OLD_HEAT_NO = @oldHeatNo       "
					")                                  ";


				CString sysCode = "";

				/*	if (tmmsm01.STORE_FACTORY.Trim() == "J")
				{
				sysCode = "";
				}
				else if (tmmsm01.STORE_FACTORY.Trim() == "1" || tmmsm01.STORE_FACTORY.Trim() == "2" || tmmsm01.STORE_FACTORY.Trim() == "3" || tmmsm01.STORE_FACTORY.Trim() == "A")
				{
				sysCode = "1";
				}
				else
				{
				sysCode = "2";
				}*/

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();

				int matNoLen = tmmsm01["OLD_HEAT_NO"].ToString().Trim().GetLength() + 2;
				if (sysCode.Trim() != "")
				{
					matNoLen = matNoLen + 1;
				}

				cmd_inq.Parameters.Set("matNoLen", matNoLen);
				CString startMatNo = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
				startMatNo = startMatNo.Trim() + "%";
				cmd_inq.Parameters.Set("startMatNo", startMatNo.Trim());
				cmd_inq.Parameters.Set("oldHeatNo", tmmsm01["OLD_HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("strand_no", c_strand_no);

				//Log::Trace("", "", "cmd.ExecuteReader = [{0}]", cmd_inq.ExecuteReader());

				//Log::Trace("", "", "sqlstr = {0}", sqlstr);
				//Log::Trace("", "", "startMatNo = {0}", startMatNo);
				//Log::Trace("", "", "oldHeatNo = {0}", tmmsm01["OLD_HEAT_NO"].ToString());
				//Log::Trace("", "", "strand_no = {0}", c_strand_no);

				if (cmd_inq.Read())
				{
					cs_mat_no_max = cmd_inq.GetString(1).Trim();
					//Log::Trace("", "", cs_mat_no_max);
				}
				cmd_inq.Close();

				/*if (cs_mat_no_max.Trim() == "")
				{
					cs_mat_no_max = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
					cs_mat_no_max = cs_mat_no_max.Trim() + "00";
				}
				cs_mat_no_max = cs_mat_no_max.Trim();

				int cs_mat_no_max_len = cs_mat_no_max.GetLength();

				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:cs_mat_no_max = [%s]", cs_mat_no_max);
				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:cs_mat_no_max_len = [%d] ", cs_mat_no_max_len);*/

				CString mat_no_seq = (CDecimal::Parse(cs_mat_no_max.Trim()) + 1).ToString().Trim();//(CDecimal::Parse(cs_mat_no_max.Trim().Substring(cs_mat_no_max.Trim().GetLength() - 2, 2)) + 1).ToString().Trim();

				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:mat_no_seq = [%s] ", mat_no_seq);

				if (mat_no_seq.GetLength() == 1)
				{
					mat_no_seq = "0" + mat_no_seq;
				}

				newMatNo = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
				newMatNo = newMatNo.Trim() + c_strand_no.Trim() + mat_no_seq;
				//Log::Trace("", __FUNCTION__, "newMatNo = { 0 }", newMatNo);

				if (bcls_rec->Tables["GENMATNO"].Columns.Contains("NEW_MAT_NO") == false)
				{
					bcls_rec->Tables["GENMATNO"].Columns.Add(DT_STRING, "NEW_MAT_NO");
				}
				bcls_rec->Tables["GENMATNO"].Rows[0]["NEW_MAT_NO"] = newMatNo;
			}
			else{
				//Log::Trace("", __FUNCTION__, "TMMSM01.STOCK_NO	= [{0}]", tmmsm01["STOCK_NO"].ToString());

				tmmsm01["MAT_NO"] = bcls_rec->Tables["GENMATNO"].Rows[0]["MAT_NO"].ToString().Trim();
				tmmsm01.Query();
				if (tmmsm01["OLD_HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "传入的材料原始熔炼号为空");
					strcpy(s.sysmsg, "传入的材料原始熔炼号为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				sqlstr = "SELECT MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 2 )) FROM            "
					"(                                  "
					"  SELECT MAT_NO FROM TMMSM01       "
					"  WHERE LENGTH(MAT_NO) = @matNoLen "
					"  AND MAT_NO LIKE @startMatNo      "
					"  AND OLD_HEAT_NO = @oldHeatNo       "
					"  UNION ALL                        "
					"  SELECT MAT_NO FROM HMMSM01       "
					"  WHERE LENGTH(MAT_NO) = @matNoLen "
					"  AND MAT_NO LIKE @startMatNo      "
					"  AND OLD_HEAT_NO = @oldHeatNo       "
					")                                  ";


				CString sysCode = "";

				/*	if (tmmsm01.STORE_FACTORY.Trim() == "J")
				{
				sysCode = "";
				}
				else if (tmmsm01.STORE_FACTORY.Trim() == "1" || tmmsm01.STORE_FACTORY.Trim() == "2" || tmmsm01.STORE_FACTORY.Trim() == "3" || tmmsm01.STORE_FACTORY.Trim() == "A")
				{
				sysCode = "1";
				}
				else
				{
				sysCode = "2";
				}*/

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();

				int matNoLen = tmmsm01["OLD_HEAT_NO"].ToString().Trim().GetLength() + 2;
				if (sysCode.Trim() != "")
				{
					matNoLen = matNoLen + 1;
				}

				cmd_inq.Parameters.Set("matNoLen", matNoLen);
				CString startMatNo = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
				startMatNo = startMatNo.Trim() + "%";
				cmd_inq.Parameters.Set("startMatNo", startMatNo.Trim());
				cmd_inq.Parameters.Set("oldHeatNo", tmmsm01["OLD_HEAT_NO"].ToString());

				//Log::Trace("", "", "cmd.ExecuteReader = [{0}]", cmd_inq.ExecuteReader());

				//Log::Trace("", "", "sqlstr = {0}", sqlstr);

				if (cmd_inq.Read())
				{
					cs_mat_no_max = cmd_inq.GetString(1).Trim();
					//Log::Trace("", "", cs_mat_no_max);
				}
				cmd_inq.Close();

				/*if (cs_mat_no_max.Trim() == "")
				{
					cs_mat_no_max = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
					cs_mat_no_max = cs_mat_no_max.Trim() + "00";
				}
				cs_mat_no_max = cs_mat_no_max.Trim();

				int cs_mat_no_max_len = cs_mat_no_max.GetLength();

				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:cs_mat_no_max = [%s]", cs_mat_no_max);
				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:cs_mat_no_max_len = [%d] ", cs_mat_no_max_len);*/

				CString mat_no_seq = (CDecimal::Parse(cs_mat_no_max.Trim()) + 1).ToString().Trim();//(CDecimal::Parse(cs_mat_no_max.Trim().Substring(cs_mat_no_max.Trim().GetLength() - 3, 3)) + 1).ToString().Trim();

				//Log::Trace("", __FUNCTION__, "当前表的最大材料号:mat_no_seq = [{0}] ", mat_no_seq);

				if (mat_no_seq.GetLength() == 1)
				{
					mat_no_seq = "00" + mat_no_seq;
				}
				if (mat_no_seq.GetLength() == 2)
				{
					mat_no_seq = "0" + mat_no_seq;
				}

				newMatNo = tmmsm01["OLD_HEAT_NO"].ToString().Trim() + sysCode;
				newMatNo = newMatNo.Trim() + mat_no_seq;
				//Log::Trace("", __FUNCTION__, "newMatNo = { 0 }", newMatNo);

				if (bcls_rec->Tables["GENMATNO"].Columns.Contains("NEW_MAT_NO") == false)
				{
					bcls_rec->Tables["GENMATNO"].Columns.Add(DT_STRING, "NEW_MAT_NO");
				}
				bcls_rec->Tables["GENMATNO"].Rows[0]["NEW_MAT_NO"] = newMatNo;
			}
			
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
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm33.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
