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

BM2F_ENTERACE(mmsmqc_ins_f)

int f_mmsmqc_ins_f(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int i = 0;

	/* 业务变量 */
	int trace_line = 0;
	int message_line = 0;
	int l_ok_flag = 1;
	int i_number = 0;
	int v_count = 0;
	int v_count_29 = 0;
	int v_count_04 = 0;
	int v_count_0x = 0;
	int v_max_seq = 0;

	CString	datetime = "";
	CString	datetime_18 = "";
	CString l_reason = " ";
	CString l_scrap_remark = " ";
	CString l_heat_no = " ";
	CDecimal v_mat_theory_wt = 0;
	CDecimal v_mat_act_wt = 0;
	CDecimal flag_num = 0;
	CDecimal sum_num = 0;
	CDecimal sum_wt = 0;
	CString v_process_desc = "";
	CString v_cc_mach_no = "";

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
	CDbCommand cmd_twm04_inq(conn);

	try
	{
		EIClass inBlk_pes;

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		datetime_18 = CDateTime::Now().ToString("yyyymmddhhmissff");

		trace_line = trace_line + 1;
		l_ok_flag = 1;
		tmmsmqc_fp_rec["PROCESS_DESC"] = ' ';
		l_scrap_remark = ' ';
		l_heat_no = ' ';
		i_number = 0;
		v_count = -1;

		//Log::Trace("", __FUNCTION__, "BOF = [{0}];", day_yield);
		//for (int i_row = 0; i_row < bcls_rec->Tables[0].Columns.get_Count(); i_row++)
		//{
		//	tmmsmqc_fp_rec.MergeFrom(bcls_rec->Tables[0].Rows[i_row]);

		//	tmmsmqc_fp_rec.Query("HEAT_NO, OLD_ST_NO, STOCK_PLACE_NO, MAT_NUM, MAT_LEN");

		//	if (tmmsmqc_fp_rec["PROCESS_FLAG"].ToString() == "1")
		//	{
		//		continue;
		//	}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT * "
				" FROM TMMSMQC_FP "
				" WHERE PROCESS_FLAG <> '1' "
				" ORDER BY HEAT_NO ";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}];", sqlstr);
		cmd_tmmsmbp_inq.SetCommandText(sqlstr);
		cmd_tmmsmbp_inq.ExecuteReader();

		while (cmd_tmmsmbp_inq.Read())
		{
			cmd_tmmsmbp_inq.Fetch(tmmsmqc_fp_rec);

			l_scrap_remark = "";
			l_ok_flag = 1;
			/***************************数据校验  begin ***************************/
			Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec.OLD_ORDER_NO.ToString().GetLength() =[{0}]", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString().GetLength());
			if (tmmsmqc_fp_rec["HEAT_NO"].ToString().GetLength() < 6)
			{
				l_ok_flag = 2;
				l_reason = "炉号:" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "长度有误";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString().GetLength() < 10)
			{
				l_ok_flag = 2;
				l_reason = "炉坯号:" + tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString() + "长度有误";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_THEORY_WT"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "理论重量小于等于0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_NUM"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "支数不能为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_NUM"].ToDecimal() >= 100)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "支数不能超过100";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_THICK"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "厚度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_WIDTH"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "宽度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["MAT_LEN"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "长度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			

			if (tmmsmqc_fp_rec["FACTORY_DIV"].ToString() != "A1" && tmmsmqc_fp_rec["FACTORY_DIV"].ToString() != "A2" )
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "厂别区分不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "机组代码为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}
			else
			{
				if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "5")
				{
					tmmsmqc_fp_rec["UNIT_CODE"] = "A212";
				}
				else if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "6")
				{
					tmmsmqc_fp_rec["UNIT_CODE"] = "A213";
				}
				else if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "7")
				{
					tmmsmqc_fp_rec["UNIT_CODE"] = "A214";
				}
				else
				{
					l_ok_flag = 2;
					l_reason = "材料号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "机组代码有误";
					l_scrap_remark = l_scrap_remark + "-" + l_reason;
				}
			}

			

			tmmsmqc_fp_rec["WHOLE_BACKLOG_CODE"] = "A1";

			if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "出钢记号为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["HEAT_NO"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "熔炼号为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库

			//default: // 所有数据库适用，通用SQL语句
			//	sqlstr = " SELECT COUNT(1) "
			//		"  FROM TQMTQQ0 "
			//		"  WHERE HEAT_NO = @tmmsmqc_fp_rec.HEAT_NO ";
			//	break;
			//}
			//cmd_tqmts0z_inq.SetCommandText(sqlstr);
			//cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_fp_rec.HEAT_NO", tmmsmqc_fp_rec["HEAT_NO"].ToString());
			//v_count_29 = cmd_tqmts0z_inq.ExecuteScalar().ToInt32();

			//if (v_count_29 == 0)
			//{
			//	l_ok_flag = 2;
			//	l_reason = "炉号[" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "]的成分不存在";
			//	l_scrap_remark = l_scrap_remark + "-" + l_reason;
			//}
			tmmsmqc_fp_rec["HOLD_FLAG"] = "0";
			tmmsmqc_fp_rec["SURFACE_DECIDE_CODE"] = "1";

			if (tmmsmqc_fp_rec["PRODUCT_FLAG"].ToString() != "0" && tmmsmqc_fp_rec["PRODUCT_FLAG"].ToString() != "1")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "成品标记不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["HOLD_FLAG"].ToString() != "0" && tmmsmqc_fp_rec["HOLD_FLAG"].ToString() != "2")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "封锁标记不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["SURFACE_DECIDE_CODE"].ToString() != "0" && tmmsmqc_fp_rec["SURFACE_DECIDE_CODE"].ToString() != "1")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "表判代码不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_fp_rec["PRODUCT_CODE"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "产副品码为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}
			

			//tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_fp_rec["STOCK_PLACE_NO"].ToString().Trim(); //材料库位号
			tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_NO"].ToString() + tmmsmqc_fp_rec["HALL_NO"].ToString();

			if (tmmsmqc_fp_rec["ROWNO"].ToString().GetLength() > 1)
			{
				tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_fp_rec["ROWNO"].ToString();
			}
			else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_fp_rec["ROWNO"].ToString();

			if (tmmsmqc_fp_rec["COLUMN_NO"].ToString().GetLength() > 1)
			{
				tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_fp_rec["COLUMN_NO"].ToString();
			}
			else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_fp_rec["COLUMN_NO"].ToString();

			

			v_count_04 = 0;
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库
			//default: // 所有数据库适用，通用SQL语句
			//	sqlstr = "		 SELECT  COUNT(1)   "
			//		"		   FROM TWM04 A     "
			//		"		  WHERE A.STOCK_NO =  @tmmsmqc_fp_rec.STOCK_NO    "
			//		"		    AND A.HALL_NO =  @tmmsmqc_fp_rec.HALL_NO    ";
			//	break;
			//}

			//cmd_tymsm04_inq.SetCommandText(sqlstr);
			//cmd_tymsm04_inq.Parameters.Set("tmmsmqc_fp_rec.STOCK_NO", tmmsmqc_fp_rec["STOCK_NO"].ToString());
			//cmd_tymsm04_inq.Parameters.Set("tmmsmqc_fp_rec.HALL_NO", tmmsmqc_fp_rec["HALL_NO"].ToString());
			//v_count_04 = cmd_tymsm04_inq.ExecuteScalar().ToInt32();

			//if (v_count_04 == 0)
			//{
			//	l_ok_flag = 2;
			//	l_reason = "炉号" + tmmsmqc_fp_rec["HEAT_NO"].ToString() + "库号或跨号错误";
			//	l_scrap_remark = l_scrap_remark + "-" + l_reason;
			//}
			if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString().GetLength() < 7)
			{
				if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString().Trim() == "BDQ601" || tmmsmqc_fp_rec["OLD_ST_NO"].ToString().Trim() == "GB600G")
				{

				}
				else 
				{
					l_ok_flag = 2;
					l_reason = "按牌号" + tmmsmqc_fp_rec["OLD_ST_NO"].ToString() + "未找到出钢记号";
					l_scrap_remark = l_scrap_remark + "-" + l_reason;
				}
			}
			
			/***************************数据校验  end ***************************/
			if (l_ok_flag == 1)
			{
				tmmsm01_rec["MAT_STATUS"] = "29";

				if (tmmsmqc_fp_rec["STOCK_NO"].ToString().Trim().Substring(0, 1) != "A"&&tmmsmqc_fp_rec["STOCK_NO"].ToString().Trim().Substring(0, 1) != "B")
				{
				}
				else
				{
					v_process_desc = " ";
					v_count_04 = 0;
					Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec.STOCK_PLACE_NO =[{0}]", tmmsmqc_fp_rec["STOCK_PLACE_NO"].ToString());
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS SQL Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库
					default: // 所有数据库适用，通用SQL语句
						sqlstr = "		 SELECT  COUNT(1)   "
							"		   FROM twm04 A     "
							"		  WHERE A.STOCK_PLACE_NO =  @tmmsm01_rec.STOCK_PLACE_NO    ";
						break;
					}

					cmd_twm04_inq.SetCommandText(sqlstr);
					cmd_twm04_inq.Parameters.Set("tmmsm01_rec.STOCK_PLACE_NO", tmmsm01_rec["STOCK_PLACE_NO"].ToString().Trim());
					v_count_04 = cmd_twm04_inq.ExecuteScalar().ToInt32();
					Log::Trace("", __FUNCTION__, "v_count_04 =[{0}]", v_count_04);
					Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec.COLUMN_NO =[{0}]", tmmsmqc_fp_rec["COLUMN_NO"].ToString());
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.v_count_04 =[{0}]", v_count_04);

					if (v_count_04 == 0)
					{
						v_process_desc = "库位号";
						tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_fp_rec["STOCK_NO"].ToString().Trim() + tmmsmqc_fp_rec["HALL_NO"].ToString().Trim() + "9999";
					}
				}
				

				//产线类型 mat_line_type
				tmmsm01_rec["MAT_LINE_TYPE"] = "SM";
				tmmsm01_rec["OLD_HEAT_NO"] = tmmsmqc_fp_rec["OLD_ORDER_NO"];
				
				if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString().GetLength() < 7)
				{
					if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString().Trim() == "BDQ601")
					{
						tmmsm01_rec["ST_NO"] = "AL4TU300";
					}
					else if (tmmsmqc_fp_rec["OLD_ST_NO"].ToString().Trim() == "GB600G")
					{
						tmmsm01_rec["ST_NO"] = "AX4TU101";
					}
				}

				if (tmmsmqc_fp_rec["SURFACE_DECIDE_CODE"].ToString() == "0")
				{
					tmmsm01_rec["MAT_STATUS"] = "20";
				}

				if (tmmsmqc_fp_rec["HOLD_FLAG"].ToString() == "2")
				{
					tmmsm01_rec["HOLD_FLAG"] = "2";
					tmmsm01_rec["HOLD_REMARK"] = "期初导入,质量封锁原因:" + tmmsmqc_fp_rec["HOLD_REMARK"].ToString();
					tmmsm01_rec["MAT_STATUS"] = "22";
					tmmsm01_rec["HOLD_TIME"] = "20211001000000";
					tmmsm01_rec["HOLD_MAKER"] = "QC";
				}
				else
				{
					tmmsm01_rec["HOLD_FLAG"] = "0";
					tmmsm01_rec["HOLD_REMARK"] = " ";
					tmmsm01_rec["HOLD_TIME"] = " ";
					tmmsm01_rec["HOLD_MAKER"] = " ";
				}

				//读取目标表中同炉材料号的最大SEQ流水号
				v_count = 0;
				v_max_seq = 0;
				
				Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec.OLD_ORDER_NO =[{0}]", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString());
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default: // 所有数据库适用，通用SQL语句
					sqlstr = "		 SELECT  COUNT(1)   "
						"		   FROM TMMSM01QC A     "
						"		  WHERE A.OLD_HEAT_NO	=  @tmmsmqc_fp_rec.OLD_ORDER_NO    ";
					break;
				}

				cmd_tmmsm01qc_inq.SetCommandText(sqlstr);
				cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.OLD_ORDER_NO", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString().Trim());
				v_count = cmd_tmmsm01qc_inq.ExecuteScalar().ToInt32();
				Log::Trace("", __FUNCTION__, "tmmsm01_rec.v_count =[{0}]", v_count);

				if (v_count > 0)
				{
					//switch (conn->DatabaseKind)
					//{
					//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					//case DB_KIND_MSSQL:	        // MS SQL Server数据库
					//case DB_KIND_ORACLE:	    // Oracle 数据库
					//default: // 所有数据库适用，通用SQL语句
					//	sqlstr = "		 SELECT  TO_NUMBER(MAX(SUBSTR(MAT_NO,11,2)))   "
					//		"		   FROM TMMSM01QC A     "
					//		"		  WHERE A.OLD_HEAT_NO	=  @tmmsmqc_fp_rec.OLD_ORDER_NO    ";
					//	break;
					//}

					//cmd_tmmsm01qc_inq.SetCommandText(sqlstr);
					//Log::Trace("", __FUNCTION__, "tmmsm01_rec.sqlstr =[{0}]", sqlstr);
					//cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.OLD_ORDER_NO", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString());
					//cmd_tmmsm01qc_inq.ExecuteReader();

					//if (cmd_tmmsm01qc_inq.Read())
					//{
					//	v_max_seq = cmd_tmmsm01qc_inq.GetInt32(1);
					//}
					//cmd_tmmsm01qc_inq.Close();

					//Log::Trace("", __FUNCTION__, "tmmsm01_rec.v_max_seq =[{0}]", v_max_seq);
				}

				

			
				Log::Trace("", __FUNCTION__, "tmmsm01_rec.v_process_desc =[{0}]", v_process_desc);

				if (tmmsmqc_fp_rec["STOCK_NO"].ToString().Trim().Substring(0, 1) != "A"&&tmmsmqc_fp_rec["STOCK_NO"].ToString().Trim().Substring(0, 1) != "B")
				{
					tmmsm01_rec["STOCK_NO"] = tmmsmqc_fp_rec["STOCK_NO"];
				}
				else
				{
					if (v_process_desc == "库位号")
					{
						tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_fp_rec["STOCK_NO"].ToString() +  "Z9999";  //材料库位号
						tmmsm01_rec["STOCK_NO"] = tmmsmqc_fp_rec["STOCK_NO"];                      //库号
						tmmsm01_rec["HALL_NO"] = "Z";                       //跨号
						tmmsm01_rec["ROWNO"] = "99";                             //行号
						tmmsm01_rec["COLUMN_NO"] = "99";                             //列号
						tmmsm01_rec["LAYERNO"] = 0;                              //层号
					}
					else
					{
						Log::Trace("", __FUNCTION__, "tmmsm01_rec.STOCK_PLACE_NO =[{0}]", tmmsm01_rec["STOCK_PLACE_NO"].ToString());
						tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_fp_rec["STOCK_NO"].ToString() + tmmsmqc_fp_rec["HALL_NO"].ToString() + tmmsmqc_fp_rec["ROWNO"].ToString() + tmmsmqc_fp_rec["COLUMN_NO"].ToString(); //材料库位号
						tmmsm01_rec["STOCK_NO"] = tmmsmqc_fp_rec["STOCK_NO"];                      //库号
						tmmsm01_rec["HALL_NO"] = tmmsmqc_fp_rec["HALL_NO"];                       //跨号
						tmmsm01_rec["ROWNO"] = tmmsmqc_fp_rec["ROWNO"];                         //行号
						if (tmmsmqc_fp_rec["COLUMN_NO"].ToString().GetLength() > 1)
						{
							tmmsm01_rec["COLUMN_NO"] = tmmsmqc_fp_rec["COLUMN_NO"];                     //列号
						}
						else
						{
							tmmsm01_rec["COLUMN_NO"] = "0"+ tmmsmqc_fp_rec["COLUMN_NO"].ToString();                     //列号
						}
						
						tmmsm01_rec["LAYERNO"] = 0;                       //层号
					}
				}

				v_mat_theory_wt = 0;
		
				Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec.MAT_NUM. =[{0}]", tmmsmqc_fp_rec["MAT_NUM"].ToDecimal());

				sum_num = 0;
			
				for (i = 1; i <= tmmsmqc_fp_rec["MAT_NUM"].ToDecimal(); i++)
				{
					if (i < tmmsmqc_fp_rec["MAT_NUM"].ToDecimal())
					{
						tmmsm01_rec["MAT_THEORY_WT"] = tmmsmqc_fp_rec["MAT_THEORY_WT"].ToDecimal() / tmmsmqc_fp_rec["MAT_NUM"];
						tmmsm01_rec["MAT_THEORY_WT"] = tmmsm01_rec["MAT_THEORY_WT"].ToDecimal().Round(3);
						v_mat_theory_wt = tmmsm01_rec["MAT_THEORY_WT"].ToDecimal() + v_mat_theory_wt;
					}
					else
					{
						//最后一笔坯料重量 = 炉次总重量 - 其它坯料总重量
						//tmmsm01_rec["MAT_THEORY_WT"] = tmmsmqc_fp_rec["MAT_THEORY_WT"].ToDecimal() - v_mat_theory_wt;

						tmmsm01_rec["MAT_THEORY_WT"] = tmmsmqc_fp_rec["MAT_THEORY_WT"];
					}

					

					/**************************数据赋值tmmsm01 begin**************************/
					tmmsm01_rec["REC_CREATOR"] = "QC";                         //记录创建责任者                         
					tmmsm01_rec["REC_CREATE_TIME"] = datetime;                         //记录创建时刻                           
					tmmsm01_rec["REC_REVISOR"] = " ";                         //记录修改责任者                         
					tmmsm01_rec["REC_REVISE_TIME"] = " ";                         //记录修改时刻                           
					tmmsm01_rec["REC_ERASOR"] = " ";                         //记录删除责任者                         
					tmmsm01_rec["REC_ERASE_TIME"] = " ";                         //记录删除时间                           
					tmmsm01_rec["ARCHIVE_FLAG"] = " ";                         //归档标记                               
					tmmsm01_rec["ARCHIVE_STAMP_NO"] = " ";                         //归档邮戳号                             
					tmmsm01_rec["COMPANY_CODE"] = "7208";                         //公司代码                               
					tmmsm01_rec["COMPANY_NAME"] = "重庆钢铁有限责任公司";          //公司(帐套)中文名称                     
				/*	tmmsm01_rec["MAT_NO"] = tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString() + CString::Format("%.2d", (i + v_max_seq));   */               //炉号

					tmmsm01_rec["MAT_NO"] = tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString() ;

					Log::Trace("", __FUNCTION__, "tmmsm01_rec.MAT_NO =[{0}]", tmmsm01_rec["MAT_NO"].ToString());
				                           
					//tmmsm01_rec["MAT_LINE_TYPE"]                                      = "";                         //物料产线类型                           
					tmmsm01_rec["MAT_KIND"] = "SM";                         //物料种类                               
					if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "A212")
					{
						tmmsm01_rec["CAST_NO"] = "15****";
					}
					else if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "A213")
					{
						tmmsm01_rec["CAST_NO"] = "16****";
					}
					else if (tmmsmqc_fp_rec["UNIT_CODE"].ToString() == "A214")
					{
						tmmsm01_rec["CAST_NO"] = "17****";
					}

					tmmsm01_rec["UNIT_CODE"] = tmmsmqc_fp_rec["UNIT_CODE"];                //机组代码???

					//tmmsm01_rec["UNIT_CODE"]                                          = tmmsmqc_fp_rec["UNIT_CODE"];                //机组代码                               
					tmmsm01_rec["NEXT_UNIT_CODE"] = " ";                         //下道机组代码                           
					tmmsm01_rec["FACTORY_DIV"] = tmmsmqc_fp_rec["FACTORY_DIV"];              //厂别区分 
					if (tmmsm01_rec["FACTORY_DIV"].ToString() == "A1"
						|| tmmsm01_rec["FACTORY_DIV"].ToString() == "A2"
						)
					{
						tmmsm01_rec["MAT_LINE_TYPE"] = "SM";
					}
					else
					{
						tmmsm01_rec["MAT_LINE_TYPE"] = "BW";
					}

					tmmsm01_rec["FACTORY_PROD"] = tmmsm01_rec["FACTORY_DIV"];
					tmmsm01_rec["MAT_SHAPE_FLAG"] = "A";                        // "1" :板坯 ; "a" : 方坯                           
					tmmsm01_rec["ORIGIN_MAT_NO"] = " ";                         //外购炉号                             
					tmmsm01_rec["RAW_ORIGIN"] = tmmsmqc_fp_rec["RAW_ORIGIN"];                         //原料来源                               
					tmmsm01_rec["MAT_ORIGIN"] = "X";                         //材料来源大类                           
					tmmsm01_rec["MAT_ORIGIN_DETAIL"] = " ";                         //材料来源细分       

					tmmsm01_rec["PRODUCT_FLAG"] = tmmsmqc_fp_rec["PRODUCT_FLAG"];                         //成品标记          
					if (tmmsmqc_fp_rec["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						tmmsm01_rec["SPARE_ITEM_4"] = tmmsmqc_fp_rec["VEHICLE_NO"];
					}

					tmmsm01_rec["MAT_STATUS"] = tmmsm01_rec["MAT_STATUS"];                         //材料状态码                             
					tmmsm01_rec["MAT_THICK"] = tmmsmqc_fp_rec["MAT_THICK"];                           //材料厚度                               
					tmmsm01_rec["MAT_WIDTH"] = tmmsmqc_fp_rec["MAT_WIDTH"];                           //材料宽度                               
					tmmsm01_rec["MAT_LEN"] = tmmsmqc_fp_rec["MAT_LEN"];                           //材料长度                               
					tmmsm01_rec["MAT_ACT_THICK"] = tmmsmqc_fp_rec["MAT_THICK"];                           //材料实际厚度                           
					tmmsm01_rec["MAT_ACT_WIDTH"] = tmmsmqc_fp_rec["MAT_WIDTH"];                           //材料实际宽度                           
					tmmsm01_rec["MAT_ACT_LEN"] = tmmsmqc_fp_rec["MAT_LEN"];                           //材料实际长度                           
					tmmsm01_rec["MAT_TARG_THICK"] = tmmsmqc_fp_rec["MAT_THICK"];                           //材料目标厚度                           
					tmmsm01_rec["MAT_TARG_WIDTH"] = tmmsmqc_fp_rec["MAT_WIDTH"];                           //材料目标宽度                           
					tmmsm01_rec["MAT_TARG_LEN"] = tmmsmqc_fp_rec["MAT_LEN"];                           //材料目标长度                           
					//tmmsm01_rec.QTY = 1;                           //数量                                   
					tmmsm01_rec["MAT_NUM"] = 1;                           //材料件数(根数)

					if (tmmsm01_rec["PRODUCT_FLAG"].ToString().Trim() == "0" || tmmsm01_rec["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						tmmsm01_rec["MAT_ACT_WT"] = tmmsm01_rec["MAT_THEORY_WT"];                           //材料实际重量   
					}
					//else if(tmmsm01_rec["PRODUCT_FLAG"].ToString().Trim() == "1")
					//{
					//	
					//	sqlstr = "		 SELECT  (SUM(MAT_ACT_WT)/COUNT(1))/SUM(MAT_NUM),count(1)   "
					//		"		   FROM tmmsmqc_fp      "
					//		"		  WHERE VEHICLE_NO	=  @tmmsmqc_fp_rec.VEHICLE_NO    ";
					//	cmd_tmmsm01qc_inq.SetCommandText(sqlstr);					
					//	cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.VEHICLE_NO", tmmsmqc_fp_rec["VEHICLE_NO"].ToString());
					//	cmd_tmmsm01qc_inq.ExecuteReader();

					//	if (cmd_tmmsm01qc_inq.Read())
					//	{
					//		tmmsm01_rec["MAT_ACT_WT"] = cmd_tmmsm01qc_inq.GetDecimal(1);
					//		sum_num = cmd_tmmsm01qc_inq.GetDecimal(2);
					//	}
					//	cmd_tmmsm01qc_inq.Close();

					//	if (i < tmmsmqc_fp_rec["MAT_NUM"].ToDecimal())
					//	{
					//		
					//		tmmsm01_rec["MAT_ACT_WT"] = tmmsm01_rec["MAT_ACT_WT"].ToDecimal().Round(3);
					//		

					//	}
					//	else
					//	{
					//		tmmsmqc_fp_rec["FLAG"] = "1";
					//		Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec["OLD_ORDER_NO"] =[{0}]", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString());
					//		Log::Trace("", __FUNCTION__, "tmmsmqc_fp_rec["VEHICLE_NO"] =[{0}]", tmmsmqc_fp_rec["VEHICLE_NO"].ToString());

					//		sqlstr = "		 UPDATE tmmsmqc_fp SET FLAG ='1'   "								
					//			"		  WHERE OLD_ORDER_NO	=  @tmmsmqc_fp_rec.OLD_ORDER_NO   ";
					//		cmd_tmmsm01qc_inq.SetCommandText(sqlstr);
					//		cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.OLD_ORDER_NO", tmmsmqc_fp_rec["OLD_ORDER_NO"].ToString());
					//		cmd_tmmsm01qc_inq.ExecuteNonQuery();

					//		sqlstr = "		 SELECT  COUNT(1)   "
					//			"		   FROM tmmsmqc_fp      "
					//			"		  WHERE VEHICLE_NO	=  @tmmsmqc_fp_rec.VEHICLE_NO AND FLAG ='1'  ";
					//		cmd_tmmsm01qc_inq.SetCommandText(sqlstr);
					//		cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.VEHICLE_NO", tmmsmqc_fp_rec["VEHICLE_NO"].ToString());
					//		cmd_tmmsm01qc_inq.ExecuteReader();

					//		if (cmd_tmmsm01qc_inq.Read())
					//		{
					//			flag_num = cmd_tmmsm01qc_inq.GetDecimal(1);
					//		}
					//		cmd_tmmsm01qc_inq.Close();

					//	
					//		Log::Trace("", __FUNCTION__, "flag_num =[{0}]", flag_num);
					//		Log::Trace("", __FUNCTION__, "sum_num =[{0}]", sum_num);
					//		//最后一笔坯料重量 = 炉次总重量 - 其它坯料总重量
					//		if (sum_num == flag_num)
					//		{
					//			sqlstr = "		 SELECT  sum(mat_act_wt)   "
					//				"		   FROM tmmsm01qc      "
					//				"		  WHERE spare_item_4	=  @tmmsmqc_fp_rec.VEHICLE_NO   ";
					//			cmd_tmmsm01qc_inq.SetCommandText(sqlstr);
					//			cmd_tmmsm01qc_inq.Parameters.Set("tmmsmqc_fp_rec.VEHICLE_NO", tmmsmqc_fp_rec["VEHICLE_NO"].ToString());
					//			cmd_tmmsm01qc_inq.ExecuteReader();

					//			if (cmd_tmmsm01qc_inq.Read())
					//			{
					//				sum_wt = cmd_tmmsm01qc_inq.GetDecimal(1);
					//			}
					//			cmd_tmmsm01qc_inq.Close();
					//			Log::Trace("", __FUNCTION__, "sum_wt =[{0}]", sum_wt);

					//			tmmsm01_rec["MAT_ACT_WT"] = tmmsmqc_fp_rec["MAT_ACT_WT"].ToDecimal() - sum_wt ;
					//		}
					//		else
					//		{
					//			tmmsm01_rec["MAT_ACT_WT"] = tmmsm01_rec["MAT_ACT_WT"].ToDecimal().Round(3);
					//			
					//		}
					//		
					//	}

					//}

					tmmsm01_rec["MAT_THEORY_WT"] = tmmsm01_rec["MAT_THEORY_WT"];                           //材料理论重量                           
					tmmsm01_rec["MEASURE_WT_FLAG"] = "0";                         //称重标记                               
					tmmsm01_rec["PRODUCT_PACK_FLAG"] = " ";                         //成品包装标志                           
					tmmsm01_rec["PACK_TYPE_CODE"] = " ";                         //包装类型代码                           
					tmmsm01_rec["PONO"] = tmmsmqc_fp_rec["HEAT_NO"];                         //制造命令号                             
					tmmsm01_rec["HEAT_NO"] = tmmsmqc_fp_rec["HEAT_NO"];                         //熔炼号 
					tmmsm01_rec["ST_NO"] = tmmsm01_rec["ST_NO"];
					//tmmsm01_rec["ST_NO"]                                              = " ";                         //出钢记号                               
					tmmsm01_rec["PROD_MAKER"] = "QC";                         //生产责任者                             
					if (tmmsmqc_fp_rec["PROD_TIME"].ToString().GetLength() == 8)
					{
						tmmsm01_rec["PROD_TIME"] = tmmsmqc_fp_rec["PROD_TIME"].ToString() + "000000";
					}
					else
					{
						tmmsm01_rec["PROD_TIME"] = tmmsmqc_fp_rec["PROD_TIME"];
					}

					Log::Trace("", __FUNCTION__, "tmmsm01_rec.PROD_TIME =[{0}]", tmmsm01_rec["PROD_TIME"].ToString());
					//tmmsm01_rec["PROD_TIME"]                                          = tmmsmqc_fp_rec["PROD_TIME"];                //生产时刻                               
					tmmsm01_rec["PROD_SHIFT_NO"] = " ";                         //生产班次                               
					tmmsm01_rec["PROD_SHIFT_GROUP"] = " ";                         //生产班组                               
					tmmsm01_rec["PROD_CLASS_CODE"] = " ";                         //产品大类代码                           
					tmmsm01_rec["PROD_CODE"] = " ";                         //品名代码                               
					tmmsm01_rec["SG_CODE"] = " ";                         //代表钢种代码                           
					tmmsm01_rec["STD_SG_CODE"] = " ";                         //标准牌号(钢级)代码                     
					tmmsm01_rec["SG_SIGN"] = tmmsmqc_fp_rec["OLD_ST_NO"];                         //牌号（钢级）                           
					tmmsm01_rec["SG_STD"] = " ";                         //标准                                   
					tmmsm01_rec["OLD_STD_SG_CODE"] = " ";                         //原标准牌号(钢级)代码                   
					tmmsm01_rec["OLD_SG_SIGN"] = " ";                         //原牌号（钢级）                         
					tmmsm01_rec["OLD_SG_STD"] = " ";                         //原标准                                 
					tmmsm01_rec["MAT_TRACK_NO"] = CDateTime::Now().ToString("yyyymmddhhmissff");                         //材料跟踪号                             
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.MAT_TRACK_NO =[{0}]", tmmsm01_rec["MAT_TRACK_NO"].ToString());
					tmmsm01_rec["PASS_BACKLOG_SEQ_NO"] = 0;                           //实际通过工序序列号                     
					tmmsm01_rec["WHOLE_BACKLOG_ACT"] = " ";                         //实际全程工序途径码                     
					tmmsm01_rec["MAT_DESTION"] = tmmsm01_rec["MAT_DESTION"];				//材料去向                              
					tmmsm01_rec["MAT_MATCH_FLAG"] = " ";                         //材料配比标记                           
					tmmsm01_rec["ORDER_NO"] = " ";                         //合同号                                 
					                            
					tmmsm01_rec["FIN_CUST_CODE"] = " ";                         //最终用户代码                           
					tmmsm01_rec["WHOLE_BACKLOG_NO"] = 0;                           //全程工序途径码顺序号                   
					tmmsm01_rec["WHOLE_BACKLOG"] = " ";                         //全程工序途径码                         
					tmmsm01_rec["WHOLE_BACKLOG_SEQ"] = 1;                           //全程工序顺序号                       
					tmmsm01_rec["WHOLE_BACKLOG_CODE"] = tmmsm01_rec["WHOLE_BACKLOG_CODE"];	//全程工序代码                           
					tmmsm01_rec["NEXT_WHOLE_BACKLOG_SEQ"] = 0;                           //后全程工序顺序号                       
					tmmsm01_rec["NEXT_WHOLE_BACKLOG_CODE"] = " ";                         //后全程工序代码                         
					tmmsm01_rec["MSC"] = " ";                         //冶金规范码                             
					tmmsm01_rec["OLD_MSC"] = " ";                         //原冶金规范码                           
					tmmsm01_rec["MSC_LINE_NO"] = " ";                         //产线号                                 
					tmmsm01_rec["OLD_MSC_LINE"] = " ";                         //原产线号                               
					tmmsm01_rec["PSC"] = " ";                         //产品规范码                             
					tmmsm01_rec["OLD_PSC"] = " ";                         //原产品规范码_001                       
					tmmsm01_rec["APN"] = " ";                         //产品最终用途码                         
					tmmsm01_rec["OLD_APN"] = " ";                         //原产品用途码                           
					tmmsm01_rec["CONFM_FLAG"] = "0";                         //准发确认标记                           
					tmmsm01_rec["CONFM_PLAN_NO"] = " ";                         //准发计划号                             
					tmmsm01_rec["CONFM_REJECT_CAUSE"] = " ";                         //准发计划拒绝原因                       
					tmmsm01_rec["APP_DECIDE_FLAG"] = "0";                         //现货申报标记                           
					tmmsm01_rec["TRANSFER_FLAG"] = "0";                         //转库计划标记                           
					tmmsm01_rec["TRANSFER_PLAN_NO"] = " ";                         //转库计划号                             
					tmmsm01_rec["TRANSFER_REJECT_CAUSE"] = " ";                         //转库计划拒绝原因                       
					tmmsm01_rec["PROD_READY_DATE"] = " ";                         //生产备妥日期                           
					tmmsm01_rec["BILL_OF_LADING_NO"] = " ";                         //提货单号                               
					tmmsm01_rec["COME_PROC_AGREE_NO"] = " ";                         //来料加工协议号                         
					tmmsm01_rec["COMBINE_FLAG"] = " ";                         //组批标记                               
					tmmsm01_rec["MERG_MAT_NO"] = " ";                         //组卷炉号                             
					tmmsm01_rec["HOLD_FLAG"] = tmmsm01_rec["HOLD_FLAG"];                         //封锁标记 
					tmmsm01_rec["MNG_HOLD_CAUSE_CODE"] = " ";                         //管理封锁原因代码                       
					tmmsm01_rec["MNG_HOLD_TIME"] = " ";                         //管理封锁时刻                           
					tmmsm01_rec["MNG_HOLD_MAKER"] = " ";                         //管理封锁责任者                         
					tmmsm01_rec["MNG_HOLD_REMARK"] = " ";                         //管理封锁注释                           
					tmmsm01_rec["HOLD_CAUSE_CODE"] = " ";                         //封锁原因代码                           
					tmmsm01_rec["HOLD_TIME"] = tmmsm01_rec["HOLD_TIME"];                         //封锁时刻                               
					tmmsm01_rec["HOLD_MAKER"] = tmmsm01_rec["HOLD_MAKER"];                         //封锁责任者                             
					tmmsm01_rec["HOLD_REMARK"] = tmmsmqc_fp_rec["HOLD_REMARK"];              //封锁注释                               
					tmmsm01_rec["REL_TIME"] = " ";                         //释放时刻                               
					tmmsm01_rec["REL_MAKER"] = " ";                         //释放责任者                             
					tmmsm01_rec["REL_REMARK"] = " ";                         //释放注释                               
					tmmsm01_rec["REMAIN_CAUSE_CODE"] = " ";                         //余材原因代码                           
					tmmsm01_rec["REMAIN_DECIDE_TIME"] = " ";                         //余材判定时刻                           
					tmmsm01_rec["REMAIN_DECIDE_MAKER"] = " ";                         //余材判定责任者                         
					tmmsm01_rec["REMAIN_DECIDE_CODE"] = " ";                         //余材判定代码                           
					tmmsm01_rec["REMAIN_REMARK"] = " ";                         //余材注释                               
					tmmsm01_rec["DROP_LEVEL_TYPE"] = " ";                         //降级种类                               
					tmmsm01_rec["DROP_LEVEL_CAUSE_CODE"] = " ";                         //降级理由代码                           
					tmmsm01_rec["DROP_LEVEL_REMARK"] = " ";                         //降级注释                               
					tmmsm01_rec["SCRAP_CAUSE_CODE"] = " ";                         //报废原因代码                           
					tmmsm01_rec["SCRAP_MAKER"] = " ";                         //报废责任者                             
					tmmsm01_rec["SCRAP_TIME"] = " ";                         //报废时刻                               
					tmmsm01_rec["SCRAP_REMARK"] = " ";                         //报废注释                               
					tmmsm01_rec["SURFACE_DECIDE_CODE"] = tmmsmqc_fp_rec["SURFACE_DECIDE_CODE"];      //表面判定代码               
					tmmsm01_rec["SURFACE_DECIDE_CODE"] = "0";
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.SURFACE_DECIDE_CODE =[{0}]", tmmsm01_rec["SURFACE_DECIDE_CODE"].ToString());
					tmmsm01_rec["SURFACE_DECIDE_TIME"] = "20181231000000";                         //表面判定时间                           
					tmmsm01_rec["SURFACE_DECIDE_MAKER"] = "QC";                         //表面判定责任者                         
					tmmsm01_rec["PCH_JUDGE_CODE"] = "0";                         //性能判定代码                           
					tmmsm01_rec["PCH_JUDGE_RESULT"] = " ";				                //性能判定结果                           
					tmmsm01_rec["PCH_JUDGE_ABN"] = " ";                         //性能判定异常原因                       
					tmmsm01_rec["PCH_JUDGE_TIME"] = " ";                         //性能判定时间                           
					tmmsm01_rec["PCH_JUDGE_MAKER"] = " ";                         //性能判定责任者                         
					tmmsm01_rec["COMPLEX_DECIDE_CODE"] = "0";                         //综合判定代码                           
					tmmsm01_rec["COMPLEX_DECIDE_TIME"] = " ";                         //综合判定时间                           
					tmmsm01_rec["COMPLEX_DECIDE_MAKER"] = " ";                         //综合判定责任者                         
					tmmsm01_rec["OLD_COMPLEX_DECIDE_CODE"] = " ";                         //原综合判定代码                         
					tmmsm01_rec["OLD_COMPLEX_DECIDE_TIME"] = " ";                         //原综合判定时刻                         
					tmmsm01_rec["OLD_COMPLEX_DECIDE_MAKER"] = " ";                         //原综合判定责任人                       
					tmmsm01_rec["REPAIR_FLAG"] = "0";                         //返修标记                               
					tmmsm01_rec["REPAIR_REMARK"] = " ";                         //返修注释                               
					tmmsm01_rec["SAMPLE_LOT_NO"] = " ";                         //试批号                                 
					tmmsm01_rec["NEW_TEST_NO"] = " ";                         //新试号                                 
					tmmsm01_rec["DEFECT_CODE"] = " ";                         //缺陷代码                               
					tmmsm01_rec["DEFECT_CLASS"] = " ";                         //缺陷等级                               
					tmmsm01_rec["PLAN_NO"] = " ";                         //计划号 

					tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"];           //材料库位号                             
					tmmsm01_rec["STOCK_NO"] = tmmsm01_rec["STOCK_NO"];                         //库号                                   
					tmmsm01_rec["HALL_NO"] = tmmsm01_rec["HALL_NO"];                       //跨号???                                   
					tmmsm01_rec["ROWNO"] = tmmsm01_rec["ROWNO"];                         //行号                                   
					tmmsm01_rec["COLUMN_NO"] = tmmsm01_rec["COLUMN_NO"];                         //列号                                   
					tmmsm01_rec["LAYERNO"] = tmmsm01_rec["LAYERNO"];                         //层号                                   
					tmmsm01_rec["STORE_AREA"] = " ";                         //存储区域                               
					tmmsm01_rec["IN_FLAG"] = "1";                         //入库标记                               
					tmmsm01_rec["IN_STOCK_TIME"] = tmmsm01_rec["PROD_TIME"];                         //入库时刻                               
					tmmsm01_rec["OUT_STOCK_TIME"] = " ";                         //出库时刻                               
					tmmsm01_rec["CMD_FLAG"] = " ";                         //吊车命令标志   

					

					if (tmmsmqc_fp_rec["PRODUCT_CODE"].ToString().Trim().GetLength() == 5)
					{
						tmmsm01_rec["PRODUCT_CODE"] = tmmsmqc_fp_rec["PRODUCT_CODE"];                         //产副品代码           
					}
					else
					{
						tmmsm01_rec["PRODUCT_CODE"] = "B1210";                         //产副品代码           
					}

				                  
					tmmsm01_rec["PRODUCT_CODE_1"] = " ";                         //产副品代码一                           
					tmmsm01_rec["AI_TIME_1"] = " ";                         //成本抛帐时间一                         
					tmmsm01_rec["AI_WT_1"] = 0;                           //成本抛帐重量一                         
					tmmsm01_rec["AI_ST_NO_1"] = " ";                         //成本抛账出钢记号一  
					if (tmmsm01_rec["PRODUCT_FLAG"].ToString().Trim() == "0")
					{
						tmmsm01_rec["AI_THICK_CODE_1"] = "0";                         //成本抛账厚度组距码一       
					}
					else if (tmmsm01_rec["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						tmmsm01_rec["AI_THICK_CODE_1"] = "1";                         //成本抛账厚度组距码一                   
					}
				
					tmmsm01_rec["AI_WIDTH_CODE_1"] = " ";                         //成本抛账宽度组距码一                   
					tmmsm01_rec["AI_GRADE_1"] = " ";                         //成本抛账钢级/硬度组/材质一             
					tmmsm01_rec["PRODUCT_CODE_2"] = " ";                         //产副品代码二                           
					tmmsm01_rec["AI_TIME_2"] = " ";                         //成本抛帐时间二                         
					tmmsm01_rec["AI_WT_2"] = 0;                           //成本抛帐重量二                         
					tmmsm01_rec["AI_ST_NO_2"] = " ";                         //成本抛账出钢记号二                     
					tmmsm01_rec["AI_THICK_CODE_2"] = " ";                         //成本抛账厚度组距码二                   
					tmmsm01_rec["AI_WIDTH_CODE_2"] = " ";                         //成本抛账宽度组距码二                   
					tmmsm01_rec["AI_GRADE_2"] = " ";                         //成本抛账钢级/硬度组/材质二             
					tmmsm01_rec["PRODUCT_CODE_3"] = " ";                         //产副品代码三                           
					tmmsm01_rec["AI_TIME_3"] = " ";                         //成本抛帐时间三                         
					tmmsm01_rec["AI_WT_3"] = 0;                           //成本抛帐重量三                         
					tmmsm01_rec["AI_ST_NO_3"] = " ";                         //成本抛账出钢记号三                     
					tmmsm01_rec["AI_THICK_CODE_3"] = " ";                         //成本抛账厚度组距码三                   
					tmmsm01_rec["AI_WIDTH_CODE_3"] = " ";                         //成本抛账宽度组距码三                   
					tmmsm01_rec["AI_GRADE_3"] = " ";                         //成本抛账钢级/硬度组/材质三             
				                     
					tmmsm01_rec["PREC_SLAB_NO"] = " ";                         //预定板坯号                             
					tmmsm01_rec["PREC_ST_NO"] = tmmsm01_rec["ST_NO"];                         //预定出钢记号                           
					tmmsm01_rec["DECI_ST_NO"] = tmmsm01_rec["ST_NO"];                         //决定出钢记号                           
					tmmsm01_rec["JUDGE_ST_NO"] = tmmsm01_rec["ST_NO"];                         //判定出钢记号                           
					tmmsm01_rec["CUT_ST_NO"] = tmmsm01_rec["ST_NO"];                         //切断出钢记号                           
					tmmsm01_rec["FIN_ST_NO"] = tmmsm01_rec["ST_NO"];                         //最终出钢记号                           
					tmmsm01_rec["JUDGE_TIME"] = " ";                         //判定时间                               
					tmmsm01_rec["JUDGE_MAKER"] = " ";                         //判定者                                 
					tmmsm01_rec["SLAB_CUT_TIME"] = " ";                         //板坯切断时刻                           
					tmmsm01_rec["HSF_END_TIME"] = " ";                         //精整结束时刻                               
					//tmmsm01_rec["CAST_NO"]                                            = " ";                         //连连浇号(cast号)                       
					tmmsm01_rec["CAST_DIV_NO"] = 0;                           //cast分割号                             
					tmmsm01_rec["REFINE_ROUTE_CODE"] = " ";                         //精炼路径代码                           
					tmmsm01_rec["HOT_CHARGE_FLAG"] = "0";                         //热装标记                               
					tmmsm01_rec["HOT_SEND_FLAG"] = "0";                         //热送标记                               
					tmmsm01_rec["ISE_RESULT"] = " ";                         //ise合否                                
					tmmsm01_rec["OUT_SM_STOCK_TIME"] = " ";                         //出炼钢库时刻                           
					tmmsm01_rec["SLABTOP_FLAG"] = " ";                         //板坯top点确认标志                      
					tmmsm01_rec["SLABTOP_TIME"] = " ";                         //板坯top点时刻                          
					tmmsm01_rec["SLAB_COLD_HOT_FLAG"] = " ";                         //板坯冷热标志                           
					tmmsm01_rec["ABNR_OCC_PLACE_1"] = 0;                           //品质异常发生位置1                      
					tmmsm01_rec["ABNR_OCC_PLACE_2"] = 0;                           //品质异常发生位置2                      
					tmmsm01_rec["ABNR_OCC_PLACE_3"] = 0;                           //品质异常发生位置3                      
					tmmsm01_rec["ABNR_OCC_PLACE_4"] = 0;                           //品质异常发生位置4                      
					tmmsm01_rec["ABNR_END_PLACE_1"] = 0;                           //品质异常结束位置1                      
					tmmsm01_rec["ABNR_END_PLACE_2"] = 0;                           //品质异常结束位置2                      
					tmmsm01_rec["ABNR_END_PLACE_3"] = 0;                           //品质异常结束位置3                      
					tmmsm01_rec["ABNR_END_PLACE_4"] = 0;                           //品质异常结束位置4                      
					tmmsm01_rec["ABNR_CODE_1"] = " ";                         //品质异常代码1                          
					tmmsm01_rec["ABNR_CODE_2"] = " ";                         //品质异常代码2                          
					tmmsm01_rec["ABNR_CODE_3"] = " ";                         //品质异常代码3                          
					tmmsm01_rec["ABNR_CODE_4"] = " ";                         //品质异常代码4                          
					tmmsm01_rec["ABN_DEAL_CODE1"] = " ";                         //异常处置代码1                          
					tmmsm01_rec["ABN_DEAL_CODE2"] = " ";                         //异常处置代码2                          
					tmmsm01_rec["ABN_DEAL_CODE3"] = " ";                         //异常处置代码3                          
					tmmsm01_rec["ABN_DEAL_CODE4"] = " ";                         //异常处置代码4                          
					tmmsm01_rec["ADJUST_WIDTH_MARK"] = " ";                         //调宽标记                               
					tmmsm01_rec["SLAB_HEAD_WIDTH"] = 0;                           //板坯头部宽度                           
					tmmsm01_rec["SLAB_TAIL_WIDTH"] = 0;                           //板坯尾部宽度                           
					tmmsm01_rec["SLAB_TAPPER_WIDTH_START"] = 0;                           //板坯宽度变化开始点                     
					tmmsm01_rec["SLAB_TAPPER_WIDTH_LEN"] = 0;                           //板坯宽度变化长度                       
					tmmsm01_rec["SLAB_PLACE_STD"] = " ";                         //板坯位置充当标准                       
					tmmsm01_rec["SLAB_PLACE_CODE"] = " ";                         //板坯位置代码                           
					tmmsm01_rec["MACH_CLEAR_FLAG"] = " ";                         //机清标志                               
					tmmsm01_rec["HAND_CLEAR_GRADE"] = " ";                         //手清等级                               
					tmmsm01_rec["HEAT_GRADE"] = " ";                         //炉次等级                               
					tmmsm01_rec["QUALITY_GRADE"] = " ";                         //质量等级                               
					tmmsm01_rec["SLAT_UNLADE_CAUSE"] = " ";                         //板坯下线理由                           
					tmmsm01_rec["RETURN_FLAG"] = 0;                           //返回标记                               
					tmmsm01_rec["SLAB_RETURN_CAUSE_CODE"] = " ";                         //板坯返送理由代码                       
					tmmsm01_rec["SLAB_RETURN_TIME"] = " ";                         //板坯返送时刻                           
					tmmsm01_rec["RHEAT_SLAB_FLAG"] = " ";                         //回炉板坯标志                           
					tmmsm01_rec["SLAB_HEAT_CAUSE_CODE"] = " ";                         //板坯回炉理由代码                       
					tmmsm01_rec["SLAB_HEAT_TIME"] = " ";                         //板坯回炉时刻                           
					tmmsm01_rec["FINISH_FLAG"] = " ";                         //精整标记                               
					tmmsm01_rec["IC_CC_FLAG"] = " ";                         //模连铸标志                             
					tmmsm01_rec["LSLAB_NO"] = " ";                         //长坯号                                 
					tmmsm01_rec["PONO_SLAB"] = " ";                         //命令板坯号                             
					tmmsm01_rec["FIX_SLAB_NUM"] = 0;                           //定尺板坯块数                           
					tmmsm01_rec["PONO_SLAB_1"] = " ";                         //命令板坯号1                            
					tmmsm01_rec["PONO_SLAB_2"] = " ";                         //命令板坯号2                            
					tmmsm01_rec["PONO_SLAB_3"] = " ";                         //命令板坯号3                            
					tmmsm01_rec["PONO_SLAB_4"] = " ";                         //命令板坯号4                            
					tmmsm01_rec["PONO_SLAB_5"] = " ";                         //命令板坯号5                            
					tmmsm01_rec["PONO_SLAB_6"] = " ";                         //命令板坯号6                            
					tmmsm01_rec["PONO_SLAB_7"] = " ";                         //命令板坯号7                            
					tmmsm01_rec["PONO_SLAB_8"] = " ";                         //命令板坯号8                            
					tmmsm01_rec["PLATE_DT_CODE"] = " ";                         //厚板流向代码                           
					tmmsm01_rec["PROD_CODE_HP"] = " ";                         //厚板品种代码                           
					tmmsm01_rec["SLAB_FLAG"] = " ";
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.SLAB_FLAG =[{0}]", tmmsm01_rec["SLAB_FLAG"].ToString());

					//tmmsm01_rec["SLAB_FLAG"]                                          = " ";                         //长板坯标记                             
					tmmsm01_rec["SUB_BACKLOG_CODE"] = " ";                         //小工序代码                             
					tmmsm01_rec["SUB_BACKLOG_SEQ"] = 0;                           //小工序顺序号                           
					tmmsm01_rec["NEXT_SUB_BACKLOG_CODE"] = " ";                         //后小工序代码                           
					tmmsm01_rec["NEXT_SUB_BACKLOG_SEQ"] = 0;                           //后小工序顺序号                         
					tmmsm01_rec["IF_IN_SECUT_FLAG"] = " ";                         //是否在二切中标志                       
					tmmsm01_rec["PILE_INDEX"] = " ";                         //堆垛指标                               
					tmmsm01_rec["BD_FLAG"] = "0";                         //bd材标记                               
					tmmsm01_rec["OLD_PLATE_NO"] = " ";                         //原钢板号                               
					tmmsm01_rec["COMP_FLAG"] = " ";                         //复合标记                               
					tmmsm01_rec["COMP_TYPE"] = " ";                         //复合类型                               
					tmmsm01_rec["IN_MAT_NO"] = " ";                         //入口炉号                             
					tmmsm01_rec["MAT_NO_OLD"] = " ";               //原炉号     

					tmmsm01_rec["MAT_ID"] = tmmsm01_rec["MAT_NO"];//材料标识号  

					tmmsm01_rec["PRINT_NO"] = " ";                         //喷印号  

					if (tmmsm01_rec["MAT_ACT_THICK"].ToDecimal() == 150)
					{
						tmmsm01_rec["INGOT_CODE"] = "F150";                         //锭型代码???   
					}
					else if (tmmsm01_rec["MAT_ACT_THICK"].ToDecimal() == 170)
					{
						tmmsm01_rec["INGOT_CODE"] = "F170";
					}
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.INGOT_CODE =[{0}]", tmmsm01_rec["INGOT_CODE"].ToString());
					tmmsm01_rec["CROSS_CODE"] = "A";                         //截面代码                               
					tmmsm01_rec["LEN_FROM"] = 0;                           //长度起                                 
					tmmsm01_rec["LEN_TO"] = 0;                           //长度止                                 
					tmmsm01_rec["FIX_FLAG"] = "1";                         //定尺标记???                               
					//tmmsm01_rec.MAT_TUBE = 1;                           //材料根数                               
			                    
					tmmsm01_rec["STOCK_OPER_ORDER"] = " ";                         //库操作指示                             
					tmmsm01_rec["SAMPLE_LOT_NO_1"] = " ";                         //试批号（特殊试验1）                    
					tmmsm01_rec["SAMPLE_LOT_NO_2"] = " ";                         //试批号（特殊试验2）                    
					tmmsm01_rec["SAMPLE_LOT_NO_3"] = " ";                         //试批号（特殊试验3）                    
					tmmsm01_rec["SAMPLE_LOT_NO_4"] = " ";                         //试批号（特殊试验4）                    
					tmmsm01_rec["DEVO_INGOT_WT"] = 0;                           //投料锭量                               
					tmmsm01_rec["DEVO_FLAN_WT"] = 0;                           //投料坯量                               
					tmmsm01_rec["ROLL_PLAN_NO"] = " ";                         //轧制计划号                             
				                           
					//tmmsm01_rec["OLD_HEAT_NO"] = " ";                         //原熔炼号                               
					tmmsm01_rec["OLD_ST_NO"] = " ";                         //原出钢记号                             
					tmmsm01_rec["CLD_TIME"] = " ";                         //组坯产生时刻                           
					tmmsm01_rec["OLD_PONO"] = " ";                         //原制造命令号                           
					tmmsm01_rec["STEEL_GROUP"] = " ";                         //钢种组                                 
					tmmsm01_rec["SPARE_ITEM_0"] = " ";                         //备用字段_0                             
					tmmsm01_rec["SPARE_ITEM_1"] = "1";                         //备用字段_1                             
					tmmsm01_rec["SPARE_ITEM_2"] = " ";                         //备用字段_2                             
					tmmsm01_rec["SPARE_ITEM_3"] = " ";                         //备用字段_3                             
					                          
					tmmsm01_rec["SPARE_ITEM_5"] = " ";                         //备用字段_5                             
					tmmsm01_rec["SPARE_ITEM_6"] = " ";                         //备用字段_6                             
					tmmsm01_rec["SPARE_ITEM_7"] = " ";                         //备用字段_7                             
					tmmsm01_rec["SPARE_ITEM_8"] = tmmsmqc_fp_rec["STOCK_PLACE_NO"];       //备用字段_8                             
					tmmsm01_rec["SPARE_ITEM_9"] = " ";                        //备用字段_9                             
					tmmsm01_rec["SPARE_ITEM_N1"] = 0;                           //备用字段_n1                            
					tmmsm01_rec["SPARE_ITEM_N2"] = 0;                           //备用字段_n2  
					Log::Trace("", __FUNCTION__, "tmmsm01_rec.SPARE_ITEM_8 =[{0}]", tmmsm01_rec["SPARE_ITEM_8"].ToString());

					//tmmsm01_rec.Print();
					//tmmsm01_rec.Insert();

					tmmsm01qc.CopyFrom(tmmsm01_rec);
					tmmsm01qc.Insert();

					tmmsm01_rec.MergeTo(inBlk_pes.Tables[0], false);
					/**************************数据赋值tmmsm01 end**************************/
				}
			}

			if (l_scrap_remark.GetLength() > 300)
			{
				l_scrap_remark = l_scrap_remark.Substring(0, 299);
			}
			tmmsmqc_fp_rec["PROCESS_DESC"] = l_scrap_remark;

			tmmsmqc_fp_rec["PROCESS_FLAG"] = CString::Format("%d", l_ok_flag);

			tmmsmqc_fp_rec.Update("PROCESS_DESC, PROCESS_FLAG", "HEAT_NO, OLD_ST_NO, STOCK_PLACE_NO, MAT_NUM, MAT_LEN");
		}
		cmd_tmmsmbp_inq.Close();

		//int inblk_pes_count = inBlk_pes.Tables[0].Rows.get_Count();
		//Log::Trace("", __FUNCTION__, "====inBlk_pes.Tables[0].Rows.get_Count [{0}]====", inblk_pes_count);
		//if (inBlk_pes.Tables[0].Rows.get_Count() > 0)
		//{
		//	//直接调用这个即可，EGGGP为系统名 100不知道啥意思..完整调用如下:
		//	//serivce 调用serivce
		//	f_epex_call_cgi_svc(conn, "EGGEP", "mmsmqc_ins_f", &inBlk_pes, bcls_ret, 100);

		//	struct  ei_sys s_tmp;
		//	bcls_ret->GetSYS(&s_tmp);
		//	if (s_tmp.flag < 0)
		//	{
		//		Log::Trace("", __FUNCTION__, "mmsmqc_ins_f调用失败！s.flag = [{0}] s.msg = [{1}] s.sysmsg = [{2}]", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//}

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
