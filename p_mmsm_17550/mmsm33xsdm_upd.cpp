/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:	ShiYong
Date:	2024-02-29
Version:	1.0
Description: 板坯称重系数断面维护
**************************************************/

//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm33xsdm_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm33xsdm_upd)

int f_mmsm33xsdm_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_operate = "";
	CDecimal v_width = 0;
	CDecimal v_maxwidth = 0;
	CDecimal v_minwidth = 0;
	CString v_strand_no = "";
	CString v_st_no = "";
	CString v_c_div = "";


	/* 业务变量 */
	CModel tmmsm33xsdm("TMMSM33XSDM");
	CModel tmmsm33czxs("TMMSM33CZXS");
	CModel tqmts0x("TQMTS0X");
	//CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		EIClass bcls_rec_st;
		bcls_rec_st.Tables[0].Clear();
		bcls_rec_st.Tables[0].Columns.Add(tqmts0x);
		bcls_rec_st.Tables[0].Rows.Add();

			v_operate = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tmmsm33xsdm.Reset();
				tmmsm33xsdm.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm33czxs.Reset();
				tmmsm33czxs.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				if (v_operate == "I")
				{
					tmmsm33xsdm.Print();
					tmmsm33xsdm.Insert();

					v_width = bcls_rec->Tables[0].Rows[0]["WIDTH"];
					v_maxwidth = bcls_rec->Tables[0].Rows[0]["MAXWIDTH"];
					v_minwidth = bcls_rec->Tables[0].Rows[0]["MINWIDTH"];
					//v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString().Trim();
					v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();

					Log::Info("", "", "111");

					//switch (conn->DatabaseKind)
					//{
					//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					//case DB_KIND_MSSQL:	        // MS SQL Server数据库
					//case DB_KIND_ORACLE:	        // Oracle 数据库
					//default: // 所有数据库适用，通用SQL语句

					//	sqlstr = " SELECT t.ST_NO from TQMTS0X t WHERE 1 = 1 ";

					//	break;
					//}

					//cmd_inq.SetCommandText(sqlstr);
					//cmd_inq.ExecuteReader();
					//if (cmd_inq.Read()){
					//	bcls_rec_st.Tables[0].Rows[0]["ST_NO"] = cmd_inq.GetString(1);
					//}

					//for (int j = 1; j <= tqmts0x.QueryCount("ST_NO"); j++)
					//{

					if (v_c_div == "2")//碳钢
					{
						v_strand_no = "XABCDEFZ";
						for (int i = 0; i < 8; i++)
						{
							tmmsm33czxs.Reset();
							if (i == 0)
							{
								tmmsm33czxs["STRAND_NO"] = "CX";
							}
							else
							{
								tmmsm33czxs["STRAND_NO"] = v_strand_no.Substring(i, 1);
								Log::Info("", __FUNCTION__, "STRAND_NO=[{0}]", tmmsm33czxs["STRAND_NO"].ToString());
							}

							tmmsm33czxs["ST_NO"] = v_st_no;
							tmmsm33czxs["C_DIV"] = v_c_div;
							tmmsm33czxs["COE_A"] = 1;
							tmmsm33czxs["COE_B"] = 1;
							tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05;
							tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95;
							tmmsm33czxs["UPDATE_TIME_LIMIT"] = 30;//30分钟
							tmmsm33czxs["WIDTH"] = v_width;
							tmmsm33czxs["MAXWIDTH"] = v_maxwidth;
							tmmsm33czxs["MINWIDTH"] = v_minwidth;
							if (!tmmsm33czxs.QueryCount("ST_NO,STRAND_NO,WIDTH"))//根据主键去查，没有就新增，有就不处理
							{
								tmmsm33czxs.Insert();
							}


						}
					}
					else if (v_c_div == "1") //不锈钢
					{
						v_strand_no = "XABZ";
						for (int i = 0; i < 4; i++)
						{
							tmmsm33czxs.Reset();
							if (i == 0)
							{
								tmmsm33czxs["STRAND_NO"] = "CX";
							}
							else
							{
								tmmsm33czxs["STRAND_NO"] = v_strand_no.Substring(i, 1);
								Log::Info("", __FUNCTION__, "STRAND_NO=[{0}]", tmmsm33czxs["STRAND_NO"].ToString());
							}

							//tmmsm33czxs["ST_NO"] = v_st_no;
							tmmsm33czxs["C_DIV"] = v_c_div;
							tmmsm33czxs["COE_A"] = 1;
							tmmsm33czxs["COE_B"] = 1;
							tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05;
							tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95;
							tmmsm33czxs["UPDATE_TIME_LIMIT"] = 30;//30分钟
							tmmsm33czxs["WIDTH"] = v_width;
							tmmsm33czxs["MAXWIDTH"] = v_maxwidth;
							tmmsm33czxs["MINWIDTH"] = v_minwidth;
							if (!tmmsm33czxs.QueryCount("ST_NO,STRAND_NO,WIDTH"))//根据主键去查，没有就新增，有就不处理
							{
								tmmsm33czxs.Insert();
							}
						}
					}
				//}
				}
				else if (v_operate == "D")
				{
					v_width = bcls_rec->Tables[0].Rows[0]["WIDTH"];
					v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString().Trim();
					v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();

					tmmsm33xsdm.Delete();
					Log::Info("", __FUNCTION__, "v_width=[{0}]", v_width);
					Log::Info("", __FUNCTION__, "v_strand_no=[{0}]", v_strand_no);
					Log::Info("", __FUNCTION__, "v_c_div=[{0}]", v_c_div);

					tmmsm33czxs.Delete("WIDTH,C_DIV");
				}
				else if (v_operate == "U")
				{
					tmmsm33xsdm.Update("MINWIDTH,MAXWIDTH", "WIDTH,STRAND_NO,C_DIV");
					tmmsm33czxs.Update("MINWIDTH,MAXWIDTH", "WIDTH,C_DIV");
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


	return doFlag;

}
