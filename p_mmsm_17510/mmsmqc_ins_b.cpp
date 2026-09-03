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

BM2F_ENTERACE(mmsmqc_ins_b)

int f_mmsmqc_ins_b(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	int v_count_st = 0;

	CString	datetime = "";
	CString	datetime_18 = "";
	CString l_reason = " ";
	CString l_scrap_remark = " ";
	CString l_heat_no = " ";

	/* 实体类定义 */
	CModel tmmsm01_rec("TMMSM01");
	CModel tmmsm03_rec("TMMSM03");
	CModel tmmsm04_rec("TMMSM04");
	CModel tmmsm01qc("TMMSM01QC");
	CModel tmmsm03qc("TMMSM03QC");
	CModel tmmsm04qc("TMMSM04QC");
	CModel tmmsmqc_bp_rec("TMMSMQC_BP");
	CModel tmmsmqc_bp_ret("TMMSMQC_BP");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_tmmsmbp_inq(conn); 
	CDbCommand cmd_tqmts0z_inq(conn); 
	CDbCommand cmd_twm04_inq(conn);

	try
	{
		EIClass inBlk_pes;
		inBlk_pes.Tables[0].Rows.Clear();

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		datetime_18 = CDateTime::Now().ToString("yyyymmddhhmissff");

		trace_line = trace_line + 1;
		l_ok_flag = 1;
		tmmsmqc_bp_rec["PROCESS_DESC"] = ' ';
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
				" FROM TMMSMQC_BP "
				" WHERE PROCESS_FLAG <> '1' "
				" ORDER BY MAT_NO ";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}];", sqlstr);
		cmd_tmmsmbp_inq.SetCommandText(sqlstr);
		cmd_tmmsmbp_inq.ExecuteReader();

		while (cmd_tmmsmbp_inq.Read())
		{
		
			cmd_tmmsmbp_inq.Fetch(tmmsmqc_bp_rec);

			l_scrap_remark = "";
			l_ok_flag = 1;

			Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.MAT_NO =[{0}]", tmmsmqc_bp_rec["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.MAT_NO =[{0}]", tmmsmqc_bp_rec["MAT_NO"].ToString().Substring(0, 7));

			//tmmsmqc_bp_rec.Print();
			/***************************数据校验  begin ***************************/
			if (tmmsmqc_bp_rec["MAT_NO"].ToString().Substring(0, tmmsmqc_bp_rec["HEAT_NO"].ToString().GetLength()) != tmmsmqc_bp_rec["HEAT_NO"].ToString())
			{
				l_ok_flag = 2;
				l_reason = "材料号:" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "与炉号:" + tmmsmqc_bp_rec["HEAT_NO"].ToString() + "不匹配";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["MAT_THEORY_WT"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "理论重量小于等于0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["MAT_THICK"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "厚度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["MAT_WIDTH"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "宽度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["MAT_LEN"].ToDecimal() <= 0)
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "长度为0";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["FACTORY_DIV"].ToString() != "A1" && tmmsmqc_bp_rec["FACTORY_DIV"].ToString() != "A2" && tmmsmqc_bp_rec["FACTORY_DIV"].ToString() != "J1" && tmmsmqc_bp_rec["FACTORY_DIV"].ToString() != "J2" && tmmsmqc_bp_rec["FACTORY_DIV"].ToString() != "H1")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "厂别区分不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "产副品码为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "机组代码为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}
			else
			{

				if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "1")
				{
					tmmsmqc_bp_rec["UNIT_CODE"] = "A113";
				}
				else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "2")
				{
					tmmsmqc_bp_rec["UNIT_CODE"] = "A114";
				}
				else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "3")
				{
					tmmsmqc_bp_rec["UNIT_CODE"] = "A115";
				}
				else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "4")
				{
					tmmsmqc_bp_rec["UNIT_CODE"] = "A116";
				}
				else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "X")
				{
					tmmsmqc_bp_rec["UNIT_CODE"] = " ";
				}
				else
				{
					l_ok_flag = 2;
					l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "机组代码有误";
					l_scrap_remark = l_scrap_remark + "-" + l_reason;
				}
			}

			if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A113")
			{
				tmmsm01_rec["CAST_NO"] = "11****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A114")
			{
				tmmsm01_rec["CAST_NO"] = "12****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A115")
			{
				tmmsm01_rec["CAST_NO"] = "13****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A116")
			{
				tmmsm01_rec["CAST_NO"] = "14****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A212")
			{
				tmmsm01_rec["CAST_NO"] = "15****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A213")
			{
				tmmsm01_rec["CAST_NO"] = "16****";
			}
			else if (tmmsmqc_bp_rec["UNIT_CODE"].ToString() == "A214")
			{
				tmmsm01_rec["CAST_NO"] = "17****";
			}

		
			tmmsmqc_bp_rec["WHOLE_BACKLOG_CODE"] = "A1";
			

			if (tmmsmqc_bp_rec["OLD_ST_NO"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "出钢记号为空";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			v_count_st = 0;

			
			if (tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "A1210" || tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "AK210" || tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "A1410")
			{
				tmmsm01_rec["MAT_DESTION"] = "14";
			}
			else if (tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "A1220" || tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "AK220" || tmmsmqc_bp_rec["PRODUCT_CODE"].ToString() == "A1420")
			{
				tmmsm01_rec["MAT_DESTION"] = "15";
			}

			//根据炼钢牌号读取出钢记号
			if (tmmsmqc_bp_rec["OLD_ST_NO"].ToString().Trim() != ""&&tmmsmqc_bp_rec["OLD_ST_NO"].ToString().GetLength()<8)
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:    // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:     // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:    // MS SQL Server数据库
				case DB_KIND_ORACLE:         // Oracle 数据库
				default:
					sqlstr = " select CODE_DESC_1_CONTENT from tep0002 where code_class = 'MHCC' and CODE_DESC_2_CONTENT = @tmmsmqc_bp_rec.OLD_ST_NO ";
					break;
				}
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}];", sqlstr);
				cmd_tqmts0z_inq.SetCommandText(sqlstr);
				cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_bp_rec.OLD_ST_NO", tmmsmqc_bp_rec["OLD_ST_NO"].ToString().Trim());
				cmd_tqmts0z_inq.ExecuteReader();

				if (cmd_tqmts0z_inq.Read())
				{
					tmmsm01_rec["ST_NO"] = cmd_tqmts0z_inq.GetString(1);
					v_count_st = 1;
				}
			
				cmd_tqmts0z_inq.Close();
			}
			else
			{
				v_count_st = 1;
				tmmsm01_rec["ST_NO"] = tmmsmqc_bp_rec["OLD_ST_NO"].ToString().Trim();
			}


			if (tmmsmqc_bp_rec["HEAT_NO"].ToString() == " ")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "熔炼号为空";
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
			//		"  WHERE HEAT_NO = @tmmsmqc_bp_rec.HEAT_NO ";
			//	break;
			//}
			//cmd_tqmts0z_inq.SetCommandText(sqlstr);
			//cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_bp_rec.HEAT_NO", tmmsmqc_bp_rec["HEAT_NO"].ToString());
			//v_count_29 = cmd_tqmts0z_inq.ExecuteScalar().ToInt32();

			//Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.v_count_29 =[{0}]", v_count_29);
			//if (v_count_29 == 0)
			//{
			//	l_ok_flag = 2;
			//	l_reason = "炉号[" + tmmsmqc_bp_rec["HEAT_NO"].ToString() + "]的成分不存在";
			//	l_scrap_remark = l_scrap_remark + "-" + l_reason;
			//}

			if (tmmsmqc_bp_rec["HOLD_FLAG"].ToString() != "0" && tmmsmqc_bp_rec["HOLD_FLAG"].ToString() != "2" && tmmsmqc_bp_rec["HOLD_FLAG"].ToString() != "1")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "封锁标记不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			if (tmmsmqc_bp_rec["SURFACE_DECIDE_CODE"].ToString() != "0" && tmmsmqc_bp_rec["SURFACE_DECIDE_CODE"].ToString() != "1")
			{
				l_ok_flag = 2;
				l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "表判代码不合要求";
				l_scrap_remark = l_scrap_remark + "-" + l_reason;
			}

			tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"].ToString() + tmmsmqc_bp_rec["HALL_NO"].ToString();

			v_count_04 = 0;
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库
			//default: // 所有数据库适用，通用SQL语句
			//	sqlstr = "		 SELECT  COUNT(1)   "
			//		"		   FROM twm04 A     "
			//		"		  WHERE A.STOCK_NO =  @tmmsmqc_bp_rec.STOCK_NO    "
			//		"		    AND A.HALL_NO =  @tmmsmqc_bp_rec.HALL_NO    ";
			//	break;
			//}

			//cmd_twm04_inq.SetCommandText(sqlstr);
			//cmd_twm04_inq.Parameters.Set("tmmsmqc_bp_rec.STOCK_NO", tmmsmqc_bp_rec["STOCK_NO"].ToString());
			//cmd_twm04_inq.Parameters.Set("tmmsmqc_bp_rec.HALL_NO", tmmsmqc_bp_rec["HALL_NO"].ToString());
			//v_count_04 = cmd_twm04_inq.ExecuteScalar().ToInt32();

			//Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.v_count_04 =[{0}]", v_count_04);
			//if (v_count_04 == 0)
			//{
			//	l_ok_flag = 2;
			//	l_reason = "材料号" + tmmsmqc_bp_rec["MAT_NO"].ToString() + "库号或跨号错误";
			//	l_scrap_remark = l_scrap_remark + "-" + l_reason;
			//}
			if (tmmsmqc_bp_rec["OLD_ST_NO"].ToString() != "协议品"&&tmmsmqc_bp_rec["OLD_ST_NO"].ToString() != "处理品"&&v_count_st!=1)
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库

				default: // 所有数据库适用，通用SQL语句
					sqlstr = " SELECT COUNT(1) "
						"  FROM TQMTS0X "
						"  WHERE substr(st_no,4,3) = substr(@tmmsmqc_bp_rec.OLD_ST_NO,4)  "
						//"    AND VALID_FLAG = '1' "
						/*	"    AND (ST_NO LIKE 'A%' OR ST_NO LIKE 'C%' OR ST_NO LIKE 'G%' OR ST_NO LIKE 'H%' OR ST_NO LIKE 'J%' OR ST_NO LIKE 'K%') "*/;
					break;
				}
				cmd_tqmts0z_inq.SetCommandText(sqlstr);
				cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_bp_rec.OLD_ST_NO", tmmsmqc_bp_rec["OLD_ST_NO"].ToString());
				v_count_0x = cmd_tqmts0z_inq.ExecuteScalar().ToInt32();

				Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.v_count_0x =[{0}]", v_count_0x);
				if (v_count_0x == 0)
				{
					l_ok_flag = 2;
					l_reason = "按牌号" + tmmsmqc_bp_rec["OLD_ST_NO"].ToString() + "未找到出钢记号";
					l_scrap_remark = l_scrap_remark + "-" + l_reason;
				}
			}
			

			Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.l_ok_flag =[{0}]", l_ok_flag);
			Log::Trace("", __FUNCTION__, "tmmsmqc_bp_rec.l_scrap_remark =[{0}]", l_scrap_remark);
			/***************************数据校验  end ***************************/

			if (l_ok_flag == 1)
			{
				Log::Trace("", __FUNCTION__, "**************************数据赋值tmmsm01 begin**************************");
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
				tmmsm01_rec["MAT_NO"] = tmmsmqc_bp_rec["MAT_NO"];                  //材料号                                 

			                           
				//tmmsm01_rec["MAT_LINE_TYPE"]                                      = "";                         //物料产线类型                           
				tmmsm01_rec["MAT_KIND"] = "SM";                         //物料种类                               

				tmmsm01_rec["UNIT_CODE"] = tmmsmqc_bp_rec["UNIT_CODE"];                    //机组代码???

				//tmmsm01_rec["UNIT_CODE"]                                          = tmmsmqc_bp_rec["UNIT_CODE"];                //机组代码                               
				tmmsm01_rec["NEXT_UNIT_CODE"] = " ";                         //下道机组代码                           
				tmmsm01_rec["FACTORY_DIV"] = tmmsmqc_bp_rec["FACTORY_DIV"];              //厂别区分 
				if (tmmsm01_rec["FACTORY_DIV"].ToString() == "A1"
					|| tmmsm01_rec["FACTORY_DIV"].ToString() == "A2"
					)
				{
					tmmsm01_rec["MAT_LINE_TYPE"] = "SM";
				}
				else if (tmmsm01_rec["FACTORY_DIV"].ToString() == "J1" || tmmsm01_rec["FACTORY_DIV"].ToString() == "J2")
				{
					tmmsm01_rec["MAT_LINE_TYPE"] = "HP";
				}
				else if (tmmsm01_rec["FACTORY_DIV"].ToString() == "H1" )
				{
					tmmsm01_rec["MAT_LINE_TYPE"] = "HR";
				}

				tmmsm01_rec["FACTORY_PROD"] = tmmsm01_rec["FACTORY_DIV"];
				tmmsm01_rec["MAT_SHAPE_FLAG"] = "1";                        // "1" :板坯 ; "a" : 方坯                           
				tmmsm01_rec["ORIGIN_MAT_NO"] = tmmsmqc_bp_rec["MAT_NO"];                         //外购材料号                             
				tmmsm01_rec["RAW_ORIGIN"] = tmmsmqc_bp_rec["RAW_ORIGIN"];                         //原料来源                               
				tmmsm01_rec["MAT_ORIGIN"] = "X";                         //材料来源大类                           
				tmmsm01_rec["MAT_ORIGIN_DETAIL"] = " ";                         //材料来源细分                           
				tmmsm01_rec["PRODUCT_FLAG"] = "0";                         //成品标记                               
				tmmsm01_rec["MAT_STATUS"] = "29";                         //材料状态码                             
				tmmsm01_rec["MAT_THICK"] = tmmsmqc_bp_rec["MAT_THICK"];                           //材料厚度                               
				tmmsm01_rec["MAT_WIDTH"] = tmmsmqc_bp_rec["MAT_WIDTH"];                           //材料宽度                               
				tmmsm01_rec["MAT_LEN"] = tmmsmqc_bp_rec["MAT_LEN"];                           //材料长度                               
				tmmsm01_rec["MAT_ACT_THICK"] = tmmsmqc_bp_rec["MAT_THICK"];                           //材料实际厚度                           
				tmmsm01_rec["MAT_ACT_WIDTH"] = tmmsmqc_bp_rec["MAT_WIDTH"];                           //材料实际宽度                           
				tmmsm01_rec["MAT_ACT_LEN"] = tmmsmqc_bp_rec["MAT_LEN"];                           //材料实际长度                           
				tmmsm01_rec["MAT_TARG_THICK"] = tmmsmqc_bp_rec["MAT_THICK"];                           //材料目标厚度                           
				tmmsm01_rec["MAT_TARG_WIDTH"] = tmmsmqc_bp_rec["MAT_WIDTH"];                           //材料目标宽度                           
				tmmsm01_rec["MAT_TARG_LEN"] = tmmsmqc_bp_rec["MAT_LEN"];                           //材料目标长度                           
				//tmmsm01_rec.QTY = 1;                           //数量                                   
				tmmsm01_rec["MAT_NUM"] = 1;                           //材料件数(根数)                         
				tmmsm01_rec["MAT_ACT_WT"] = tmmsmqc_bp_rec["MAT_THEORY_WT"];                           //材料实际重量                           
				tmmsm01_rec["MAT_THEORY_WT"] = tmmsmqc_bp_rec["MAT_THEORY_WT"];                           //材料理论重量                           
				tmmsm01_rec["MEASURE_WT_FLAG"] = "0";                         //称重标记                               
				tmmsm01_rec["PRODUCT_PACK_FLAG"] = " ";                         //成品包装标志                           
				tmmsm01_rec["PACK_TYPE_CODE"] = " ";                         //包装类型代码                           
				tmmsm01_rec["PONO"] = tmmsmqc_bp_rec["HEAT_NO"];                         //制造命令号                             
				tmmsm01_rec["HEAT_NO"] = tmmsmqc_bp_rec["HEAT_NO"];                         //熔炼号 

				if (tmmsmqc_bp_rec["OLD_ST_NO"].ToString() != "协议品"&&tmmsmqc_bp_rec["OLD_ST_NO"].ToString() != "处理品"&&v_count_st != 1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS SQL Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库

					default: // 所有数据库适用，通用SQL语句
						sqlstr = " SELECT ST_NO "
							"  FROM TQMTS0X "
							"  WHERE substr(st_no,4,3) = substr(@tmmsmqc_bp_rec.OLD_ST_NO,4) order by st_no "
							//"    AND VALID_FLAG = '1' "
							/*"    AND (ST_NO LIKE 'A%' OR ST_NO LIKE 'C%' OR ST_NO LIKE 'G%' OR ST_NO LIKE 'H%' OR ST_NO LIKE 'I%' OR ST_NO LIKE 'J%' OR ST_NO LIKE 'K%') "*/;
						break;
					}
					cmd_tqmts0z_inq.SetCommandText(sqlstr);
					cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_bp_rec.OLD_ST_NO", tmmsmqc_bp_rec["OLD_ST_NO"].ToString());
					cmd_tqmts0z_inq.ExecuteReader();
					if (cmd_tqmts0z_inq.Read())
					{
						tmmsm01_rec["ST_NO"] = cmd_tqmts0z_inq.GetString(1);
					}
					cmd_tqmts0z_inq.Close();
				}
				else if (tmmsmqc_bp_rec["OLD_ST_NO"].ToString() == "协议品" || tmmsmqc_bp_rec["OLD_ST_NO"].ToString() == "处理品")
				{
					tmmsm01_rec["ST_NO"] = "XYP00000";
				}

				//tmmsm01_rec["ST_NO"]                                              = " ";                         //出钢记号                               
				tmmsm01_rec["PROD_MAKER"] = "QC";                         //生产责任者                             
				if (tmmsmqc_bp_rec["PROD_TIME"].ToString().GetLength() == 8)
				{
					tmmsm01_rec["PROD_TIME"] = tmmsmqc_bp_rec["PROD_TIME"].ToString() + "000000";
				}
				else
				{
					tmmsm01_rec["PROD_TIME"] = tmmsmqc_bp_rec["PROD_TIME"];
				}

				//tmmsm01_rec["PROD_TIME"]                                          = tmmsmqc_bp_rec["PROD_TIME"];                //生产时刻                               
				tmmsm01_rec["PROD_SHIFT_NO"] = " ";                         //生产班次                               
				tmmsm01_rec["PROD_SHIFT_GROUP"] = " ";                         //生产班组                               
				tmmsm01_rec["PROD_CLASS_CODE"] = " ";                         //产品大类代码                           
				tmmsm01_rec["PROD_CODE"] = " ";                         //品名代码                               
				tmmsm01_rec["SG_CODE"] = " ";                         //代表钢种代码                           
				tmmsm01_rec["STD_SG_CODE"] = " ";                         //标准牌号(钢级)代码                     
				tmmsm01_rec["SG_SIGN"] = tmmsmqc_bp_rec["OLD_ST_NO"];                         //牌号（钢级）                           
				tmmsm01_rec["SG_STD"] = " ";                         //标准                                   
				tmmsm01_rec["OLD_STD_SG_CODE"] = " ";                         //原标准牌号(钢级)代码                   
				tmmsm01_rec["OLD_SG_SIGN"] = " ";                         //原牌号（钢级）                         
				tmmsm01_rec["OLD_SG_STD"] = " ";                         //原标准          

				tmmsm01_rec["MAT_TRACK_NO"] = CDateTime::Now().ToString("yyyymmddhhmissff");                         //材料跟踪号                             

				tmmsm01_rec["PASS_BACKLOG_SEQ_NO"] = 0;                           //实际通过工序序列号                     
				tmmsm01_rec["WHOLE_BACKLOG_ACT"] = " ";                         //实际全程工序途径码   
			
				tmmsm01_rec["PRODUCT_CODE"] = tmmsmqc_bp_rec["PRODUCT_CODE"];

				tmmsm01_rec["MAT_MATCH_FLAG"] = " ";                         //材料配比标记                           
				tmmsm01_rec["ORDER_NO"] = " ";                         //合同号                                 
				tmmsm01_rec["OLD_ORDER_NO"] = " ";                         //原合同号                               
				tmmsm01_rec["FIN_CUST_CODE"] = " ";                         //最终用户代码                           
				tmmsm01_rec["WHOLE_BACKLOG_NO"] = 0;                           //全程工序途径码顺序号                   
				tmmsm01_rec["WHOLE_BACKLOG"] = " ";                         //全程工序途径码                         
				tmmsm01_rec["WHOLE_BACKLOG_SEQ"] = 1;                           //全程工序顺序号                       
				tmmsm01_rec["WHOLE_BACKLOG_CODE"] = tmmsmqc_bp_rec["WHOLE_BACKLOG_CODE"]; //全程工序代码                           
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
				tmmsm01_rec["MERG_MAT_NO"] = " ";                         //组卷材料号                             
				tmmsm01_rec["HOLD_FLAG"] = tmmsmqc_bp_rec["HOLD_FLAG"];                         //封锁标记 
				if (tmmsmqc_bp_rec["HOLD_FLAG"].ToString() == "2")
				{
					tmmsm01_rec["HOLD_REMARK"] = "期初导入，质量封锁";
					tmmsm01_rec["MAT_STATUS"] = "22";
				}
				else if(tmmsmqc_bp_rec["HOLD_FLAG"].ToString() == "1")
				{
					tmmsm01_rec["HOLD_REMARK"] = "期初导入，管理封锁";
					tmmsm01_rec["MAT_STATUS"] = "21";
				}
				else
				{
					tmmsm01_rec["HOLD_REMARK"] = " ";
				}
				tmmsm01_rec["MNG_HOLD_CAUSE_CODE"] = " ";                         //管理封锁原因代码                       
				tmmsm01_rec["MNG_HOLD_TIME"] = " ";                         //管理封锁时刻                           
				tmmsm01_rec["MNG_HOLD_MAKER"] = " ";                         //管理封锁责任者                         
				tmmsm01_rec["MNG_HOLD_REMARK"] = " ";                         //管理封锁注释                           
				tmmsm01_rec["HOLD_CAUSE_CODE"] = " ";                         //封锁原因代码                           
				tmmsm01_rec["HOLD_TIME"] = " ";                         //封锁时刻                               
				tmmsm01_rec["HOLD_MAKER"] = " ";                         //封锁责任者                             
				tmmsm01_rec["HOLD_REMARK"] = tmmsmqc_bp_rec["HOLD_REMARK"];              //封锁注释                               
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
				tmmsm01_rec["SURFACE_DECIDE_CODE"] = tmmsmqc_bp_rec["SURFACE_DECIDE_CODE"];      //表面判定代码  
				tmmsm01_rec["SURFACE_DECIDE_CODE"] = "0";
				tmmsm01_rec["SURFACE_DECIDE_TIME"] = " ";                         //表面判定时间                           
				tmmsm01_rec["SURFACE_DECIDE_MAKER"] = " ";                         //表面判定责任者                         
				tmmsm01_rec["PCH_JUDGE_CODE"] = "0";                         //性能判定代码                           
				tmmsm01_rec["PCH_JUDGE_RESULT"] = " ";                         //性能判定结果                           
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
				if (tmmsmqc_bp_rec["HALL_NO"].ToString() == "0X")
				{
					tmmsmqc_bp_rec["HALL_NO"] = "Z";
				}

				if (tmmsmqc_bp_rec["FACTORY_DIV"].ToString() == "A1")
				{
					tmmsmqc_bp_rec["STOCK_NO"] = tmmsmqc_bp_rec["STOCK_NO"];
					
					tmmsmqc_bp_rec["HALL_NO"] = tmmsmqc_bp_rec["HALL_NO"];
					tmmsmqc_bp_rec["ROWNO"] = tmmsmqc_bp_rec["ROWNO"];;
					tmmsmqc_bp_rec["COLUMN_NO"] = tmmsmqc_bp_rec["COLUMN_NO"];;
					tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"].ToString() + tmmsmqc_bp_rec["HALL_NO"].ToString() + tmmsmqc_bp_rec["ROWNO"].ToString() + tmmsmqc_bp_rec["COLUMN_NO"].ToString();

				}
				else
				{
					//如果是轧钢原料库，则直接转换即可
					tmmsmqc_bp_rec["STOCK_NO"] = tmmsmqc_bp_rec["STOCK_NO"];
					
					if (tmmsmqc_bp_rec["HALL_NO"].ToString() > "3")
					{
						tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"].ToString() + "99999";
					}
					else
					{
						tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"];
						//tmmsmqc_bp_rec["ROWNO"] + tmmsmqc_bp_rec["COLUMN_NO"].ToString();

						if (tmmsmqc_bp_rec["HALL_NO"].ToString().GetLength() > 1)
						{
							tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["HALL_NO"].ToString().Substring(1, 1);
						}
						else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["HALL_NO"].ToString();


						if (tmmsmqc_bp_rec["FACTORY_DIV"].ToString() == "H1")
						{
							//热轧按列行
							if (tmmsmqc_bp_rec["COLUMN_NO"].ToString().GetLength() > 1)
							{
								tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["COLUMN_NO"].ToString();
							}
							else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_bp_rec["COLUMN_NO"].ToString();

							if (tmmsmqc_bp_rec["ROWNO"].ToString().GetLength() > 1)
							{
								tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["ROWNO"].ToString();
							}
							else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_bp_rec["ROWNO"].ToString();
						}
						else
						{
							if (tmmsmqc_bp_rec["HALL_NO"].ToString() == "1")
							{
								//中厚板1跨都是虚拟库位，直接不拼接
								tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"].ToString() + "99999";
							}
							else
							{
								//厚板按行列
								if (tmmsmqc_bp_rec["ROWNO"].ToString().GetLength() > 1)
								{
									tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["ROWNO"].ToString();
								}
								else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_bp_rec["ROWNO"].ToString();

								if (tmmsmqc_bp_rec["COLUMN_NO"].ToString().GetLength() > 1)
								{
									tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + tmmsmqc_bp_rec["COLUMN_NO"].ToString();
								}
								else tmmsm01_rec["STOCK_PLACE_NO"] = tmmsm01_rec["STOCK_PLACE_NO"].ToString() + "0" + tmmsmqc_bp_rec["COLUMN_NO"].ToString();

							}
						}
			
					}
					
				}
				
				v_count_04 = 0;
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

				tmmsm01_rec["HALL_NO"] = tmmsmqc_bp_rec["HALL_NO"];;                         //跨号       

				
					if (v_count_04 == 0)
					{
						if (tmmsmqc_bp_rec["FACTORY_DIV"].ToString() == "A1")
						{
						  tmmsm01_rec["STOCK_PLACE_NO"] = tmmsmqc_bp_rec["STOCK_NO"].ToString().Trim()  + "Z9999";
						  tmmsm01_rec["HALL_NO"] = "Z";
						  tmmsm01_rec["ROWNO"] = "99";											//行号     
						  tmmsm01_rec["COLUMN_NO"] = "99";     
						}//列号   
						else
						{
							
							tmmsm01_rec["ROWNO"] = "99";											//行号     
							tmmsm01_rec["COLUMN_NO"] = "99";
						}
					}
					else
					{
						
						tmmsm01_rec["ROWNO"] = tmmsmqc_bp_rec["ROWNO"];                                         //行号     
						tmmsm01_rec["COLUMN_NO"] = tmmsmqc_bp_rec["COLUMN_NO"];                                 //列号
					}
				

				//tmmsm01_rec["STOCK_PLACE_NO"]                                     = tmmsmqc_bp_rec["STOCK_PLACE_NO"];           //材料库位号                             
				tmmsm01_rec["STOCK_NO"] = tmmsmqc_bp_rec["STOCK_NO"];                         //库号    

				                            
				//tmmsm01_rec["ROWNO"]                                              = tmmsmqc_bp_rec["ROWNO"];                         //行号                                   
				//tmmsm01_rec["COLUMN_NO"]                                          = tmmsmqc_bp_rec["COLUMN_NO"];                         //列号                                   
				tmmsm01_rec["LAYERNO"] = tmmsmqc_bp_rec["LAYERNO"];                         //层号     

				int find_id = 0;
				CString seq = "";
				find_id = tmmsmqc_bp_rec["MAT_NO"].ToString().Find("-", 0); //查找分隔符是[,]
				Log::Trace("", __FUNCTION__, "find_id =[{0}]", find_id);
				if (find_id < 0)
				{//从头找也没找到指定的分隔符,则说明只有一个值，直接处理后返回。
					seq = "0";
				}
				else
				{//存在分隔符。
					seq = tmmsmqc_bp_rec["MAT_NO"].ToString().Substring(find_id+1,1);
				}

				Log::Trace("", __FUNCTION__, "seq =[{0}]", seq);
								
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库

				default: // 所有数据库适用，通用SQL语句
					sqlstr = "SELECT RPAD(SUBSTR(MAT_NO,1,6),8,'0')||SUBSTR(MAT_NO,8,1)|| "
						" DECODE(substr(mat_no, 9, 1), 'A', '01', 'B', '02', 'C', '03', 'D', '04', 'E', '05', 'F', '06', 'G','07', 'H', '08','I', '09', 'J', '10', 'K', '11', 'L', '12', 'M', '13', 'N', '14', 'O', '15', 'P', '16','T','0T',rpad(substr(mat_no, 9, 1),2,'0')) || @cut_seq || '0'"
						" FROM tmmsmqc_bp WHERE MAT_NO = @tmmsmqc_bp_rec.MAT_NO "
						;
					break;
				}
				cmd_tqmts0z_inq.SetCommandText(sqlstr);
				cmd_tqmts0z_inq.Parameters.Set("tmmsmqc_bp_rec.MAT_NO", tmmsmqc_bp_rec["MAT_NO"].ToString());
				cmd_tqmts0z_inq.Parameters.Set("cut_seq", seq);
				cmd_tqmts0z_inq.ExecuteReader();
				if (cmd_tqmts0z_inq.Read())
				{
					tmmsm01_rec["MAT_NO"] = cmd_tqmts0z_inq.GetString(1);
				}
				cmd_tqmts0z_inq.Close();

				Log::Trace("", __FUNCTION__, "tmmsm01_rec.MAT_NO. =[{0}]", tmmsm01_rec["MAT_NO"].ToString());
				
				tmmsm01_rec["STORE_AREA"] = " ";                         //存储区域                               
				tmmsm01_rec["IN_FLAG"] = "1";                         //入库标记                               
				tmmsm01_rec["IN_STOCK_TIME"] = tmmsm01_rec["PROD_TIME"];                         //入库时刻  

				tmmsm01_rec["OUT_STOCK_TIME"] = " ";                         //出库时刻                               
				tmmsm01_rec["CMD_FLAG"] = " ";                         //吊车命令标志                           
				//tmmsm01_rec["PRODUCT_CODE"] = " ";                         //产副品代码                             
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

				if (tmmsmqc_bp_rec["MAT_LEN"].ToDecimal() > 4050)
				{
					tmmsm01_rec["SLAB_FLAG"] = "L";
				}
				else
				{
					tmmsm01_rec["SLAB_FLAG"] = "S";
				}

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
				tmmsm01_rec["IN_MAT_NO"] = " ";                         //入口材料号                             
				tmmsm01_rec["MAT_NO_OLD"] = tmmsmqc_bp_rec["MAT_NO_OLD"];               //原材料号  

				tmmsm01_rec["PRINT_NO"] = " ";                         //喷印号                                 
				tmmsm01_rec["INGOT_CODE"] = " ";                         //锭型代码                               
				tmmsm01_rec["CROSS_CODE"] = " ";                         //截面代码                               
				tmmsm01_rec["LEN_FROM"] = 0;                           //长度起                                 
				tmmsm01_rec["LEN_TO"] = 0;                           //长度止                                 
				tmmsm01_rec["FIX_FLAG"] = " ";                         //定尺标记                               
				//tmmsm01_rec.MAT_TUBE = 0;                           //材料根数                               
			                 
				tmmsm01_rec["STOCK_OPER_ORDER"] = " ";                         //库操作指示                             
				tmmsm01_rec["SAMPLE_LOT_NO_1"] = " ";                         //试批号（特殊试验1）                    
				tmmsm01_rec["SAMPLE_LOT_NO_2"] = " ";                         //试批号（特殊试验2）                    
				tmmsm01_rec["SAMPLE_LOT_NO_3"] = " ";                         //试批号（特殊试验3）                    
				tmmsm01_rec["SAMPLE_LOT_NO_4"] = " ";                         //试批号（特殊试验4）                    
				tmmsm01_rec["DEVO_INGOT_WT"] = 0;                           //投料锭量                               
				tmmsm01_rec["DEVO_FLAN_WT"] = 0;                           //投料坯量                               
				tmmsm01_rec["ROLL_PLAN_NO"] = " ";                         //轧制计划号                             
			                    
				tmmsm01_rec["OLD_HEAT_NO"] = " ";                         //原熔炼号                               
				tmmsm01_rec["OLD_ST_NO"] = " ";                         //原出钢记号                             
				tmmsm01_rec["CLD_TIME"] = " ";                         //组坯产生时刻                           
				tmmsm01_rec["OLD_PONO"] = " ";                         //原制造命令号                           
				tmmsm01_rec["STEEL_GROUP"] = " ";                         //钢种组                                 
				tmmsm01_rec["SPARE_ITEM_0"] = " ";                         //备用字段_0                             
				tmmsm01_rec["SPARE_ITEM_1"] = "1";                         //备用字段_1                             
				tmmsm01_rec["SPARE_ITEM_2"] = " ";                         //备用字段_2                             
				tmmsm01_rec["SPARE_ITEM_3"] = " ";                         //备用字段_3                             
				tmmsm01_rec["SPARE_ITEM_4"] = " ";                         //备用字段_4                             
				tmmsm01_rec["SPARE_ITEM_5"] = " ";                         //备用字段_5                             
				tmmsm01_rec["SPARE_ITEM_6"] = " ";                         //备用字段_6                             
				tmmsm01_rec["SPARE_ITEM_7"] = " ";                         //备用字段_7                             
				tmmsm01_rec["SPARE_ITEM_8"] = tmmsmqc_bp_rec["STOCK_PLACE_NO"];       //备用字段_8                             
				tmmsm01_rec["SPARE_ITEM_9"] = " ";                        //备用字段_9                             
				tmmsm01_rec["SPARE_ITEM_N1"] = 0;                           //备用字段_n1                            
				tmmsm01_rec["SPARE_ITEM_N2"] = 0;                           //备用字段_n2  

				tmmsm01_rec["MAT_ID"] = tmmsm01_rec["MAT_NO"];//材料标识号  
				//tmmsm01_rec.Insert();

				tmmsm01qc.CopyFrom(tmmsm01_rec);

				tmmsm01qc.Insert();
				/**************************数据赋值tmmsm01 end**************************/

				/**************************数据赋值tmmsm03 begin**************************/
				tmmsm03_rec["REC_CREATOR"] = "QC";                        //记录创建责任者     
				tmmsm03_rec["REC_CREATE_TIME"] = tmmsm01_rec["REC_CREATE_TIME"];                      // 记录创建时刻     
				tmmsm03_rec["REC_REVISOR"] = " ";                        //记录修改责任者     
				tmmsm03_rec["REC_REVISE_TIME"] = " ";                      // 记录修改时刻     
				tmmsm03_rec["REC_ERASOR"] = " ";                         //     记录删除责任者     
				tmmsm03_rec["REC_ERASE_TIME"] = " ";                       //记录删除时间     
				tmmsm03_rec["ARCHIVE_FLAG"] = " ";                             //归档标记     
				tmmsm03_rec["ARCHIVE_STAMP_NO"] = " ";                      //归档邮戳号     
				tmmsm03_rec["COMPANY_CODE"] = "7208";                             //公司代码     
				tmmsm03_rec["COMPANY_NAME"] = "重庆钢铁有限责任公司";                   //公司(帐套)中文名称     
				tmmsm03_rec["AIM_MAT_NO"] = tmmsm01_rec["MAT_NO"];                             //     目的材料号     
				tmmsm03_rec["MAT_NO"] = tmmsm01_rec["MAT_NO"];                                     // 材料号

				tmmsm03_rec["MAT_ID"] = tmmsm01_rec["MAT_ID"];                                 // 材料标识号     
				tmmsm03_rec["PONO_SLAB"] = " ";                              //    命令板坯号     
				tmmsm03_rec["ORDER_NO"] = " ";                                   //   合同号     
				tmmsm03_rec["ORDER_REMAIN_DIV"] = " ";               //合同材/预备材区分     
				tmmsm03_rec["PILE_INDEX"] = " ";                               //     堆垛指标     
				tmmsm03_rec["HOT_CHARGE_FLAG"] = "0";                          // 热装标记     
				tmmsm03_rec["INFUR_SLAB_THICK"] = 0;                //进加热炉板坯厚度     
				tmmsm03_rec["INFUR_SLAB_WID"] = 0;                   //进加热炉板坯宽度     
				tmmsm03_rec["INFUR_SLAB_LEN"] = 0;                   //进加热炉板坯长度     
				tmmsm03_rec["INFUR_SLAB_MAX_LEN"] = 0;          //进加热炉板坯最大长度     
				tmmsm03_rec["INFUR_SLAB_MIN_LEN"] = 0;          //进加热炉板坯最小长度     
				tmmsm03_rec["INFUR_SLAB_WT"] = 0;                    //进加热炉板坯重量     
				tmmsm03_rec["INFUR_SLAB_MAX_WT"] = 0;           //进加热炉板坯最大重量     
				tmmsm03_rec["INFUR_SLAB_MIN_WT"] = 0;           //进加热炉板坯最小重量  
				//tmmsm03_rec.Insert();

				tmmsm03qc.CopyFrom(tmmsm03_rec);

				tmmsm03qc.Insert();
				/**************************数据赋值tmmsm03 end**************************/

				/**************************数据赋值tmmsm04 begin**************************/
				tmmsm04_rec["REC_CREATOR"] = "QC";                         //记录创建责任者      
				tmmsm04_rec["REC_CREATE_TIME"] = tmmsm01_rec["REC_CREATE_TIME"];                       //    记录创建时刻    
				tmmsm04_rec["REC_REVISOR"] = " ";                         //记录修改责任者      
				tmmsm04_rec["REC_REVISE_TIME"] = " ";                       //    记录修改时刻    
				tmmsm04_rec["REC_ERASOR"] = " ";                           //    记录删除责任者       
				tmmsm04_rec["REC_ERASE_TIME"] = " ";                        //   记录删除时间     
				tmmsm04_rec["ARCHIVE_FLAG"] = " ";                              // 归档标记           
				tmmsm04_rec["ARCHIVE_STAMP_NO"] = " ";                       //归档邮戳号     
				tmmsm04_rec["COMPANY_CODE"] = "7208";                              // 公司代码           
				tmmsm04_rec["COMPANY_NAME"] = "重庆钢铁有限责任公司";                    // 公司(帐套)中文名称 
				tmmsm04_rec["AIM_MAT_NO"] = tmmsm01_rec["MAT_NO"];                               //    目的材料号           
				tmmsm04_rec["SUB_BACKLOG_SEQ"] = 0;                         //    小工序顺序号    
				tmmsm04_rec["PONO_SLAB"] = " ";                                //   命令板坯号            
				tmmsm04_rec["SUB_BACKLOG_CODE"] = " ";                                //   小工序代码            
				tmmsm04_rec["BACKLOG_ADD_DIV"] = " ";                              //   工序追加区分          
				tmmsm04_rec["BACKLOG_ADD_CAUSE_CODE"] = " ";                          //   工序追加原因代码      
				tmmsm04_rec["ACT_SUB_BACKLOG_CODE"] = " ";                            //   实际小工序代码        
				tmmsm04_rec["BACKLOG_PASS_TIME"] = " ";                             //    工序通过时刻         
				tmmsm04_rec["BACKLOG_DECIDE_CODE"] = " ";                      // 工序通过判定代码   
				tmmsm04_rec["MAT_NO"] = tmmsm01_rec["MAT_NO"];                                // 材料号             
				tmmsm04_rec["MAT_ID"] = tmmsm01_rec["MAT_ID"];                            // 材料标识号

				//tmmsm04_rec.Insert();

				tmmsm04qc.CopyFrom(tmmsm04_rec);

				tmmsm04qc.Insert();

				Log::Trace("", __FUNCTION__, "**************************数据赋值tmmsm01 end**************************");
				//tmmsm01_rec.MergeTo(inBlk_pes.Tables[0], false);
				/**************************数据赋值tmmsm04 end**************************/
			}

			if (l_scrap_remark.GetLength() > 300)
			{
				l_scrap_remark = l_scrap_remark.Substring(0, 299);
			}
			tmmsmqc_bp_rec["PROCESS_DESC"] = l_scrap_remark;

			tmmsmqc_bp_rec["PROCESS_FLAG"] = CString::Format("%d", l_ok_flag);
			tmmsmqc_bp_rec["MAT_NO"] = tmmsmqc_bp_rec["MAT_NO"];

			tmmsmqc_bp_rec.Update("PROCESS_DESC, PROCESS_FLAG", "MAT_NO");
		}
		cmd_tmmsmbp_inq.Close();

		//int inblk_pes_count = inBlk_pes.Tables[0].Rows.get_Count();
		//Log::Trace("", __FUNCTION__, "====inBlk_pes.Tables[0].Rows.get_Count [{0}]====", inblk_pes_count);
		//if (inBlk_pes.Tables[0].Rows.get_Count() > 0)
		//{
		//	//直接调用这个即可，EGGGP为系统名 100不知道啥意思..完整调用如下:
		//	//serivce 调用serivce
		//	f_epex_call_cgi_svc(conn, "CG7ZZ", "mmsmqc_ins_b", &inBlk_pes, bcls_ret, 100);

		//	struct  ei_sys s_tmp;
		//	bcls_ret->GetSYS(&s_tmp);
		//	if (s_tmp.flag < 0)
		//	{
		//		Log::Trace("", __FUNCTION__, "mmsmqc_ins_b调用失败！s.flag = [{0}] s.msg = [{1}] s.sysmsg = [{2}]", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);
		//		throw CApplicationException(-1, s.msg, log.Location);
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
