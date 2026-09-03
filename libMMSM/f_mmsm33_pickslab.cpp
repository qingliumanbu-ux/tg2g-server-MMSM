/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 板坯挂命令板坯
remark:本函数目前暂不考虑按批管理
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/


//int f_pssm03_prodflag_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm33_pickslab(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_pickslab";                //定义函数英文名称  
	CString FunctionCname = "铸坯挂命令板坯";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	int blkNum_ps = 0;
	CDecimal n_count = 0;
	CString sqlstr = "";
	CString v_pono_slab_falg = "";
	CString v_slab_str = "";
	CString ic_cc_div_sql = " and INGOT_CODE = @ingot_code ";   //模连铸区分 特殊处理SQL
	CString if_get_flag = "";
	CDecimal fix_slab_num = 0;
	CModel tmmsm33("TMMSM33");
	CModel tpssm03("TPSSM03");
	CDbCommand cmd_inq(conn);

	try
	{
		tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[0]);

		blkNum = bcls_rec->Tables.IndexOf("PICKED_SLAB");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PICKED_SLAB");
		}
		//系统获取的未产出命令信息 wzn_20170921
		if (!bcls_rec->Tables.Contains("PICKSLAB"))
		{
			bcls_rec->Tables.Add("PICKSLAB");
		}
		bcls_rec->Tables["PICKSLAB"].Rows.Clear();

		//参数传入的未产出命令信息 wzn_20170921
		blkNum = bcls_rec->Tables.IndexOf("INPUT_PONO_SLAB");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("INPUT_PONO_SLAB");
		}
		blkNum_ps = bcls_rec->Tables.IndexOf("TPSSM03");

		sqlstr = "SELECT CODE_DESC_5_CONTENT"
			"  FROM TEP0002 "
			"  WHERE CODE_CLASS 	=  'PSA62N'"
			"  AND CODE = @code ";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("code", tmmsm33["SLAB_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_pono_slab_falg = cmd_inq.GetString(1); //命令板坯消耗方式  P按炉  C 按LOT
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "v_pono_slab_falg[{0}]  ", v_pono_slab_falg);
		if (tmmsm33["STATION_ID"].ToString().Trim() == "I")
		{
			ic_cc_div_sql = " and INGOT_CODE = @ingot_code ";
		}
		else ic_cc_div_sql = "";
		//if (tmmsm33["SLAB_TYPE"].ToString() == "3") //方坯
		if (v_pono_slab_falg == "P")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				//sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST FROM TPSSM03 WHERE LSLAB_NO = "
				//	"(SELECT LSLAB_NO FROM "
				//	"(SELECT LSLAB_NO FROM tpssm03 WHERE PONO = @pono "
				//	" and INGOT_CODE = @ingot_code"
				//	" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1)) "
				//	"FETCH FIRST 1 ROWS ONLY) AND SLAB_PROD_FLAG <> '1' ORDER BY SLAB_NO";
				sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST,SLAB_PROD_FLAG FROM TPSSM03 WHERE PONO = @pono " + ic_cc_div_sql +
					" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1) "
					;
				break;
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				//sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST FROM TPSSM03 WHERE LSLAB_NO = "
				//	"(SELECT LSLAB_NO FROM "
				//	"(SELECT LSLAB_NO FROM tpssm03 WHERE PONO = @pono "
				//	" and INGOT_CODE = @ingot_code "
				//	"AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1)) "
				//	"WHERE ROWNUM = 1) AND SLAB_PROD_FLAG <> '1' ORDER BY SLAB_NO";
				sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST,SLAB_PROD_FLAG FROM TPSSM03 WHERE PONO = @pono " + ic_cc_div_sql +
					" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1) "
					;
				break;
			}
		}
		else
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				//sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST FROM TPSSM03 WHERE LSLAB_NO = "
				//	"(SELECT LSLAB_NO FROM "
				//	"(SELECT LSLAB_NO FROM tpssm03 WHERE CAST_LOT_NO = (SELECT CAST_LOT_NO FROM TPSSM01 WHERE PONO = @pono) "
				//	" and INGOT_CODE = @ingot_code "
				//	"AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1)) "
				//	"FETCH FIRST 1 ROWS ONLY) ORDER BY SLAB_NO";
				sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST,SLAB_PROD_FLAG FROM TPSSM03 "
					" WHERE CAST_LOT_NO = (SELECT CAST_LOT_NO FROM TPSSM01 WHERE PONO=@pono )" + ic_cc_div_sql +
					" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1) "
					;
				break;
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				//sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST FROM TPSSM03 WHERE LSLAB_NO = "
				//	"(SELECT LSLAB_NO FROM "
				//	"(SELECT LSLAB_NO FROM tpssm03 WHERE CAST_LOT_NO = (SELECT CAST_LOT_NO FROM TPSSM01 WHERE PONO = @pono)"
				//	" and INGOT_CODE = @ingot_code "
				//	" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1)) "
				//	" WHERE ROWNUM = 1) ORDER BY SLAB_NO";
				sqlstr = "SELECT LSLAB_NO,SLAB_NO,SLAB_DEST,SLAB_PROD_FLAG FROM TPSSM03 "
					" WHERE CAST_LOT_NO = (SELECT CAST_LOT_NO FROM TPSSM01 WHERE PONO=@pono )" + ic_cc_div_sql +
					" AND SLAB_PROD_FLAG <> '1' ORDER BY SUBSTR(SLAB_NO, 1, 8) || SUBSTR(SLAB_NO, 10, 4) || SUBSTR(SLAB_NO, 9, 1) "
					;
				break;
			}
		}

		Log::Trace("", "", "sqlstr_PICKSLAB={0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tmmsm33["PONO"].ToString());
		cmd_inq.Parameters.Set("ingot_code", tmmsm33["INGOT_CODE"].ToString());
		cmd_inq.ExecuteQuery(bcls_rec->Tables["PICKSLAB"]);

		//前台指定slab_no
		bcls_rec->Tables["INPUT_PONO_SLAB"].Rows.Clear();
		CString slab_num = "";
		CString slab_value = "";
		Log::Trace("", __FUNCTION__, "blkNum_ps[{0}]  ", blkNum_ps);
		//将命令信息取出到  
		if (blkNum_ps >= 0)
		{
			for (int seq_ps = 1; seq_ps <= bcls_rec->Tables["TPSSM03"].Rows.get_Count(); seq_ps = seq_ps + 1)
			{
				slab_value = bcls_rec->Tables["TPSSM03"].Rows[seq_ps-1]["SLAB_NO"];
				if (slab_value.Trim() > " ")
				{
					tpssm03["SLAB_NO"] = slab_value;
					if (tpssm03.Query("SLAB_NO") == false)
					{
						sprintf(s.msg, "命令板坯[{0}]不存在。", (const char*)tpssm03["SLAB_NO"].ToString());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (tpssm03["SLAB_PROD_FLAG"].ToString() == "1")
					{
						continue;
					}
					tpssm03.MergeTo(bcls_rec->Tables["INPUT_PONO_SLAB"], false);
					Log::Trace("", __FUNCTION__, "slab_no[{0}]  ", tpssm03["SLAB_NO"].ToString());
				}
			}
		}
		else
		{
			for (CDecimal seq_n = 1; seq_n <= 12; seq_n = seq_n + 1)
			{
				slab_num = "PONO_SLAB_" + seq_n.ToString();
				if (bcls_rec->Tables["TMMSM33"].Columns.Contains(slab_num))
				{
					slab_value = bcls_rec->Tables["TMMSM33"].Rows[0][slab_num];
					if (slab_value.Trim() > " ")
					{
						tpssm03["SLAB_NO"] = slab_value;
						if (tpssm03.Query("SLAB_NO") == false)
						{
							sprintf(s.msg, "命令板坯[{0}]不存在。", (const char*)tpssm03["SLAB_NO"].ToString());
							throw CApplicationException(-1, s.msg, log.Location);
						}
						if (tpssm03["SLAB_PROD_FLAG"].ToString() == "1")
						{
							continue;
						}
						tpssm03.MergeTo(bcls_rec->Tables["INPUT_PONO_SLAB"], false);
						//bcls_rec->Tables[0].Rows[0][slab_num] = " ";
						//v_slab_str = v_slab_str + "'" + tpssm03["SLAB_NO"].ToString() + "',";

					}
				}
			}
		}
		
		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
		{
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[i]);
			//Log::Trace("", __FUNCTION__, "[{0}]FIX_SLAB_NUM [{1}]  ", i, tmmsm33["FIX_SLAB_NUM"].ToDecimal());
			Log::Trace("", __FUNCTION__, "[{0}]PONO_SLAB_1 [{1}]  ", i, tmmsm33["PONO_SLAB_1"].ToString());
			Log::Trace("", __FUNCTION__, "[{0}]PONO_SLAB_2 [{1}]  ", i, tmmsm33["PONO_SLAB_2"].ToString());

			fix_slab_num = 0;


			v_slab_str = "";

			if (bcls_rec->Tables["INPUT_PONO_SLAB"].Rows.get_Count() > 0)
			{
				if_get_flag = "N";
				//v_slab_str = v_slab_str.Trim();
				//v_slab_str = v_slab_str.SubstringNE(0, v_slab_str.GetLength() - 1);
				Log::Trace("", __FUNCTION__, "111  "); 
			}
			else
			{
				if_get_flag = "Y";
				Log::Trace("", __FUNCTION__, "222  ");
			}


			tmmsm33["LSLAB_NO"] = "";
			tmmsm33["PONO_SLAB_1"] = "";
			tmmsm33["PONO_SLAB_2"] = "";
			tmmsm33["PONO_SLAB_3"] = "";
			tmmsm33["PONO_SLAB_4"] = "";
			tmmsm33["PONO_SLAB_5"] = "";
			tmmsm33["PONO_SLAB_6"] = "";
			tmmsm33["PONO_SLAB_7"] = "";
			tmmsm33["PONO_SLAB_8"] = "";
			tmmsm33["PONO_SLAB_9"] = "";
			tmmsm33["PONO_SLAB_10"] = "";
			tmmsm33["PONO_SLAB_11"] = "";
			tmmsm33["PONO_SLAB_12"] = "";







			if (if_get_flag.Trim() == "Y")
			{
				n_count = bcls_rec->Tables["PICKSLAB"].Rows.get_Count();
			}
			else
			{
				n_count = bcls_rec->Tables["INPUT_PONO_SLAB"].Rows.get_Count();
				//wzn_201709111454  增加对设定倍尺和实际倍尺数的校验
				//if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() != n_count)
				//{
				//	sprintf(s.sysmsg, "设定倍尺数[%d]与实绩倍尺数[%d]不一致!",tmmsm33["FIX_SLAB_NUM"].ToDecimal(), n_count);
				//	strcpy(s.msg, s.sysmsg);
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}
				//tmmsm33["FIX_SLAB_NUM"] = n_count;
			}

			Log::Trace("", "", "if_get_flag={0},n_count={1}", if_get_flag, n_count);

			for (int k = 0; k < n_count; k++)
			{
				tpssm03.Reset();

				if (if_get_flag.Trim() == "Y")
				{
					tpssm03.MergeFrom(bcls_rec->Tables["PICKSLAB"].Rows[k]);
				}
				else
				{
					tpssm03.MergeFrom(bcls_rec->Tables["INPUT_PONO_SLAB"].Rows[k]);
				}

				Log::Trace("", "", "tpssm03.LSLAB_NO ={0},tpssm03.SLAB_NO ={1}", tpssm03["LSLAB_NO"].ToString(), tpssm03["SLAB_NO"].ToString());
				if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() <= 0)
				{
					//Log::Trace("", "", "break tmmsm33["FIX_SLAB_NUM"] ={0}", tmmsm33["FIX_SLAB_NUM"].ToDecimal());
					break;
				}
				if (fix_slab_num >= tmmsm33["FIX_SLAB_NUM"].ToDecimal())
				{
					//Log::Trace("", "", "break tmmsm33["FIX_SLAB_NUM"] ={0}<= fix_slab_num[{1}]", tmmsm33["FIX_SLAB_NUM"].ToDecimal(), fix_slab_num);
					break;
				}
				if (tpssm03["SLAB_PROD_FLAG"].ToString().Trim() == "1")continue;
				//Log::Trace("", "", "k={0},FIX_SLAB_NUM={1}", k, tmmsm33["FIX_SLAB_NUM"].ToDecimal());
				if (if_get_flag.Trim() == "Y")
				{
					bcls_rec->Tables["PICKSLAB"].Rows[k]["SLAB_PROD_FLAG"] = "1";
				}
				if (if_get_flag.Trim() != "Y")
				{
					bcls_rec->Tables["INPUT_PONO_SLAB"].Rows[k]["SLAB_PROD_FLAG"] = "1";
				}
				v_slab_str = v_slab_str + "'" + tpssm03["SLAB_NO"].ToString() + "',";
				fix_slab_num = fix_slab_num + 1;

				if (fix_slab_num == 1)
				{
					bcls_rec->Tables["TMMSM33"].Rows[i]["LSLAB_NO"] = tpssm03["LSLAB_NO"];
					bcls_rec->Tables["TMMSM33"].Rows[i]["SLAB_PLAN_DEST"] = tpssm03["SLAB_DEST"];
				}
				if (fix_slab_num > 12)
				{
					//Log::Trace("", "", "fix_slab_num={0} [{1}]", fix_slab_num, k);
					//sprintf(s.sysmsg, "设定倍尺数[%d]与实绩倍尺数[%d]不一致!",tmmsm33["FIX_SLAB_NUM"].ToDecimal(), n_count);
					sprintf(s.sysmsg, "匹配命令数超过12，请检查输入信息是否正确!");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + fix_slab_num.ToString()] = tpssm03["SLAB_NO"];

				
			}
			//Log::Trace("", "", "fix_slab_num={0}", fix_slab_num);
			tmmsm33["FIX_SLAB_NUM"] = fix_slab_num;
			bcls_rec->Tables["TMMSM33"].Rows[i]["FIX_SLAB_NUM"] = fix_slab_num;
			//wzn_201709111454  增加对设定倍尺和实际倍尺数的校验  项目定制
			//if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() != n_count)
			//{
			//	//Log::Trace("", "", "FIX_SLAB_NUM={0} [{1}]", tmmsm33["FIX_SLAB_NUM"].ToDecimal(), n_count);
			//	//sprintf(s.sysmsg, "设定倍尺数[%d]与实绩倍尺数[%d]不一致!",tmmsm33["FIX_SLAB_NUM"].ToDecimal(), n_count);
			//	sprintf(s.sysmsg, "剩余命令坯/锭不足，无法满足产出设定倍尺数!");
			//	strcpy(s.msg, s.sysmsg);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}


			CDecimal c_flag = 0;//是否跨长坯
			CDecimal c_len = 0;//命令长度
			if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() > 0)
			{
				//将匹配好的命令信息进行合理性校验，如不合理，，改为余材
				if (v_slab_str.Trim().GetLength()>1)
				{
					//Log::Trace("", "", "v_slab_str={0}", v_slab_str);
					v_slab_str = v_slab_str.Trim();
					v_slab_str = v_slab_str.SubstringNE(0, v_slab_str.GetLength() - 1);
					//Log::Trace("", __FUNCTION__, "y v_slab_str[{0}]  ", v_slab_str); //将SLAB_NO拼入字符串备用
				}
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					sqlstr = "SELECT count(distinct LSLAB_NO),sum(slab_len) FROM TPSSM03 WHERE SLAB_NO in ( " + v_slab_str + ")"
						;
					break;
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT count(distinct LSLAB_NO),sum(slab_len) FROM TPSSM03 WHERE SLAB_NO in ( " + v_slab_str + ")"
						;
					break;
				}
				//Log::Trace("", "", "sqlstr_checkSLAB={0}", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					c_flag = cmd_inq.GetDecimal(1);
					c_len = cmd_inq.GetDecimal(2);
				}
				cmd_inq.Close();

				if (c_flag > 1)  //跨长坯，清空命令
				{
					sprintf(s.sysmsg, "跨长坯号切割不允许!");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm33["SLAB_LEN"].ToDecimal() - c_len <0 && tmmsm33["STATION_ID"].ToString().Trim() != "I")//过短，清空命令  客制化 是报错或清空命令产出余材
				{
					//tmmsm33["FIX_SLAB_NUM"] = 0;   //清空命令产出余材
					//Log::Trace("", "", "过短，清空命令[{0}][{1}][{2}]", tmmsm33["SLAB_LEN"].ToDecimal(), c_len, tmmsm33["SLAB_LEN"].ToDecimal() - c_len);
				}
				//if (tmmsm33["SLAB_LEN"].ToDecimal() - c_len <0 || tmmsm33["SLAB_LEN"].ToDecimal() - c_len>80)//超长，清空命令  客制化 设定
				//{
				//	tmmsm33["FIX_SLAB_NUM"] = 0;
				//	//Log::Trace("", "", "超长，清空命令[{0}][{1}][{2}]", tmmsm33["SLAB_LEN"].ToDecimal(), c_len, tmmsm33["SLAB_LEN"].ToDecimal() - c_len);
				//}
				if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() == 0)
				{
					bcls_rec->Tables["TMMSM33"].Rows[i]["LSLAB_NO"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_1"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_2"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_3"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_4"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_5"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_6"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_7"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_8"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_9"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_10"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_11"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_12"] = " ";
				}
			}

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
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm33.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
