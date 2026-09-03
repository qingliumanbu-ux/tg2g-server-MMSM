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
//#include "tmmsm01.h"
//#include "tpssm03.h"

//#include "tmmsm96.h"



//外部函数声明
BM2_FUNCTION_EXPORT
 int f_mmsm33_check(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
		/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_check";                //定义函数英文名称  
	CString FunctionCname = "板坯切断_信息新增";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
 
	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString check_flag = "";
	
	int mat_seq  = 0;
	int fetchRowCount = 0;
	CString sqlstr = "";
	EIClass inBlock;          //材料主档信息处理用
	//EIClass outBlock;

	//CTMMSM01 tmmsm01(conn);
	//CTPSSM03 tpssm03(conn);
	CDbCommand cmd_inq(conn);
	CModel tmmsm33("TMMSM33");
	CModel tmmsm33_b("TMMSM33");

	//CTMMSM96 tmmsm96(conn);

	try
	{
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM33 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		for (int n = 0; n < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); n++)
		{
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[n]);
			Log::Trace("", "", "n ={0}", n);
			Log::Trace("", "", "tmmsm33.SLAB_CUT_TIME ={0}", tmmsm33["SLAB_CUT_TIME"].ToString());   
			//check
			if (tmmsm33["STATION_NO"].ToString().Trim() == "")
			{
				//Log::Trace("", "", "tmmsm33["STATION_NO"] ={0}", tmmsm33["STATION_NO"].ToString());   
				strcpy(s.sysmsg, "连铸机号不能为空");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			//太钢定制 从连铸坯信号电文获取钢种，存在连铸坯产出时，连铸信号还没来的情况，故解开限制
			//if (tmmsm33["ST_NO"].ToString().Trim() == "")
			//{
			//	//Log::Trace("", "", "tmmsm33["ST_NO"] ={0}", tmmsm33["ST_NO"].ToString());
			//	strcpy(s.sysmsg, "出钢记号不能为空");
			//	strcpy(s.msg, s.sysmsg);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			
			//if (tmmsm33["MAT_THEORY_WT"].ToDecimal() == 0)
			//{
			//	//Log::Trace("", "", "tmmsm33["MAT_THEORY_WT"] ={0}", tmmsm33["MAT_THEORY_WT"].ToDecimal());
			//	strcpy(s.sysmsg, "铸坯理论重量不能为0 ");
			//	strcpy(s.msg, s.sysmsg);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			//tmmsm33["SLAB_TYPE"] = bcls_rec->Tables["TMMSM33"].Rows[n]["BILLET_TYPE"];
			Log::Trace("", "", "tmmsm33.SLAB_TYPE ={0}", tmmsm33["SLAB_TYPE"].ToString());
			
			if (tmmsm33["SLAB_TYPE"].ToString().Trim() == "")
			{
				//Log::Trace("", "", "tmmsm33["SLAB_TYPE"] ={0}", tmmsm33["SLAB_TYPE"].ToString());
				strcpy(s.sysmsg, "坯型代码不能为空");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			if (tmmsm33["PONO"].ToString().Trim() == "")
			{
				//Log::Trace("", "", "tmmsm33["PONO"] ={0}", tmmsm33["PONO"].ToString());
				strcpy(s.sysmsg, "制造命令号不能为空");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm33["HEAT_NO"].ToString().Trim() == "")
			{
				//Log::Trace("", "", "tmmsm33["HEAT_NO"] ={0}", tmmsm33["HEAT_NO"].ToString());
				strcpy(s.sysmsg, "炉次号不能为空");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm33["SLAB_CUT_TIME"].ToString().Trim().GetLength() != 14)
			{
				//Log::Trace("", "", "tmmsm33["SLAB_CUT_TIME"] ={0}", tmmsm33["SLAB_CUT_TIME"].ToString());
				strcpy(s.sysmsg, "切断时刻格式必须14位");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//Log::Trace("", "", "SLAB_TYPE {0}", tmmsm33["SLAB_TYPE"].ToString());
			if (tmmsm33["SLAB_TYPE"].ToString() == "5")//模铸
			{
				if (tmmsm33["INGOT_CODE"].ToString().Trim() == "")
				{
					//Log::Trace("", "", "tmmsm33["INGOT_CODE"] ={0}", tmmsm33["INGOT_CODE"].ToString());
					strcpy(s.sysmsg, "锭型代码不能为空");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("", "", "模铸 {0}", tmmsm33["INGOT_CODE"].ToString());
				//查询锭型代码重量参数配置有无
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:        // Oracle 数据库
				default:
					sqlstr = CString(
						" select count(1) from tmmsmwt where ingot_code =@tmmsm33.INGOT_CODE "
						);
					break;
				}
				//Log::Trace("", "", "sqlstr1  {0}", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tmmsm33.INGOT_CODE", tmmsm33["INGOT_CODE"].ToString());
				CDecimal get_num = cmd_inq.ExecuteScalar();
				//Log::Trace("", "", "get_num  {0}", get_num);
				//wzn_201709081439 校验质量是否有锭型配置 有则取质量的数据
				sqlstr = "select count(1)"//ingot_code,ingot_name,slab_thick,slab_width,slab_len,ingot_unit_wt 
					" from tqmtmd9 "
					" where ingot_code =@ingot_code ";
				cmd_inq.SetCommandText(sqlstr);
				//Log::Trace("", "", "sqlstr 2 {0}", sqlstr);
				cmd_inq.Parameters.Set("ingot_code", tmmsm33["INGOT_CODE"].ToString());
				CDecimal get_num_q = cmd_inq.ExecuteScalar();
				//Log::Trace("", "", "get_num_q{0}", get_num_q);
				if (get_num_q > 0)
				{
					sqlstr = "select ingot_code,ingot_name,slab_thick,slab_width,slab_len,ingot_unit_wt "
						" from tqmtmd9 "
						" where ingot_code =@ingot_code ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						//Log::Trace("", "", "get_num_q1{0}", get_num_q);
						tmmsm33["SLAB_THICK"] = cmd_inq.GetDecimal(3);
						tmmsm33["SLAB_WIDTH"] = cmd_inq.GetDecimal(4);
						tmmsm33["SLAB_LEN"] = cmd_inq.GetDecimal(5);
						if (get_num <= 0)tmmsm33["SLAB_WT"] = cmd_inq.GetDecimal(6);
					}
					cmd_inq.Close();
					bcls_rec->Tables["TMMSM33"].Rows[n].Merge(tmmsm33);//wzn_201709081439 将获取的信息返回块
				}
				//wzn_201709081439 若质量和物料都没有配置 报错
				if (get_num <= 0 && get_num_q<=0)
				{
					//bcls_rec->Tables["TMMSM33"].Rows[0].Merge(tmmsm33);
					//Log::Trace("", "", "锭型代码={0}未配置锭重参数表TMMSMWT", tmmsm33["INGOT_CODE"].ToString());
					sprintf(s.sysmsg, "锭型代码[%s]未配置锭重参数表TMMSMWT", (const char*)tmmsm33["INGOT_CODE"].ToString());
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//wzn_201709081439 增加模铸命令信息锭型的校验
				if (tmmsm33["PONO_SLAB_1"].ToString().Trim() != "")
				{

					//Log::Trace("", "", "wzn_201709081439", tmmsm33["PONO_SLAB_1"].ToString());
					tmmsm33_b["INGOT_CODE"] = "";
					sqlstr = "select ingot_code from tpssm03 where slab_no =@tmmsm33.PONO_SLAB_1 ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("tmmsm33.PONO_SLAB_1", tmmsm33["PONO_SLAB_1"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						tmmsm33_b["INGOT_CODE"] = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
					if (tmmsm33_b["INGOT_CODE"].ToString() != tmmsm33["INGOT_CODE"].ToString())
					{
						strcpy(s.sysmsg, "实绩中锭型与命令不一致");
						strcpy(s.msg, s.sysmsg);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (tmmsm33["SLAB_TYPE"].ToString() == "4")//圆坯
			{
				//Log::Trace("", "", "tmmsm33["SLAB_TYPE"] ={0}", tmmsm33["SLAB_TYPE"].ToString());
				//Log::Trace("", "", "SLAB_THICK={0}，SLAB_LEN[{1}],MAT_TUBE[{2}]", tmmsm33["SLAB_THICK"].ToDecimal(), tmmsm33["SLAB_LEN"].ToDecimal(), tmmsm33["MAT_TUBE"].ToDecimal());
				if (tmmsm33["SLAB_THICK"].ToDecimal() <= 0 || tmmsm33["SLAB_LEN"].ToDecimal() <= 0 || tmmsm33["MAT_TUBE"].ToDecimal() <= 0)
				{
					strcpy(s.sysmsg, "规格、支数均不能为0");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else 
			{
				if (tmmsm33["SLAB_THICK"].ToDecimal() <= 0 || tmmsm33["SLAB_WIDTH"].ToDecimal() <= 0 || tmmsm33["SLAB_LEN"].ToDecimal() <= 0 || tmmsm33["MAT_TUBE"].ToDecimal() <= 0)
				{
					//Log::Trace("", "", "SLAB_THICK={0}，SLAB_WIDTH[{1}]SLAB_LEN[{2}],MAT_TUBE[{3}]", tmmsm33["SLAB_THICK"].ToDecimal(), tmmsm33["SLAB_WIDTH"].ToDecimal(), tmmsm33["SLAB_LEN"].ToDecimal(), tmmsm33["MAT_TUBE"].ToDecimal());
					strcpy(s.sysmsg, "规格、支数均不能为0");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}

			if (tmmsm33["SLAB_WT"].ToDecimal() == 0)
			{
				//Log::Trace("", "", "tmmsm33["SLAB_WT"] ={0}", tmmsm33["SLAB_WT"].ToDecimal());
				strcpy(s.sysmsg, "铸坯重量不能为0 ");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm33["MAT_NO"].ToString().Trim() == "" )
			{
				//Log::Trace("", "", "tmmsm33["MAT_NO"] ={0}", tmmsm33["MAT_NO"].ToString());
				strcpy(s.sysmsg, "铸坯号不能为空！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//检验PONO和炉号的对应关系  
			int if_ok = 0;
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				sqlstr = CString(
					" select sum(cnt) from ("
					" SELECT count(1) cnt  FROM tpssm11 "
					"  WHERE HEAT_NO = @heat_no "
					"    AND PONO = @pono "
					" union all "
					" SELECT count(1)  cnt FROM tpssm41 "
					"  WHERE HEAT_NO = @heat_no "
					"    AND PONO = @pono )"
					);
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm33["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("pono", tmmsm33["PONO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if_ok = cmd_inq.GetInt32(1);
			}
			cmd_inq.Close();
			//Log::Trace("", "", "if_ok={0}", if_ok);
			if (if_ok <= 0)
			{
				/*sprintf(s.sysmsg, "系统中不存在PONO[%s]与炉号[%s]的对应关系!",
					(const char*)tmmsm33["PONO"].ToString(), (const char*)tmmsm33["HEAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}
			//check   wzn_201709111045 命令信息是否已被使用
			CDecimal num = 1;
			CString pono_slab_str = "";
			for (num = 1; num <= 12; num = num+1)
			{
				if (bcls_rec->Tables["TMMSM33"].Columns.Contains("PONO_SLAB_" + num.ToString()) )
				{
					if (bcls_rec->Tables["TMMSM33"].Rows[n]["PONO_SLAB_" + num.ToString()].ToString().Trim() != "")
					{
						pono_slab_str = pono_slab_str + "'" + bcls_rec->Tables["TMMSM33"].Rows[n]["PONO_SLAB_" + num.ToString()].ToString().Trim() + "',";
					}
				}
			}
			//Log::Trace("", "", "pono_slab_str={0}", pono_slab_str);
			if (pono_slab_str.Trim() > "")
			{
				pono_slab_str = pono_slab_str.SubstringNE(0, pono_slab_str.GetLength() - 1);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:        // Oracle 数据库
				default:
					sqlstr = "select slab_no,pono from tpssm03 "
						" where slab_prod_flag in ('1','9') "
						" and slab_no in(";
					sqlstr += pono_slab_str;
					sqlstr += ")";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					CString slab = cmd_inq.GetString(1);
					/*sprintf(s.sysmsg, "命令号号[%s]已产出!",
						(const char*)slab);
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);*/
				}
				cmd_inq.Close();
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
		//tmmsm01.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
