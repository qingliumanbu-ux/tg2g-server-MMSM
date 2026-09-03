/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王建征
Version:    1.0
Date:       2020-07-14
Description: 炼钢期初数据验证
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢期初数据验证
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
//#include "tmmsmbg.h"  
 



BM2F_ENTERACE(mmsmbg_vali)

int f_mmsmbg_vali(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");
	CString	erro_flag("0");//数据校验标记
	CString	erro_info("");//数据校验信息

	/* 实体类定义 */ 	
	//CTMMSMBG tmmsmbg(conn);
	CModel tmmsm01qc("TMMSM01QC");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel hmmsm01("HMMSM01");
	CModel hmmsm96("HMMSM96");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_temp(conn);

	//期初数据DBLink名HBC1.DBLINK_HBC1

	/* 添加并设置块名 */
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
	}

	/* 添加并设置输出参数 */
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ERRO_FLAG");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ERRO_INFO");

	try
	{
		//获取系统当前时刻
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			erro_flag = "0";
			erro_info = "";
			//重置结构体待用
			tmmsm01qc.Reset();

			//获取输入参数
			tmmsm01qc.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			#pragma region 材料号验证
			//材料号验证:不能重复
			if (tmmsm01qc.QueryCount("MAT_NO")>0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[卷号]材料号([S0001]" + tmmsm01qc["MAT_NO"].ToString()+ "[E0001])重复";
			}
			if (tmmsm01.QueryCount("MAT_NO") > 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[炉号]材料号([S0001]" + tmmsm01["MAT_NO"].ToString() + "[E0001])重复";
			}
			if (hmmsm01.QueryCount("MAT_NO") > 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[炉号]材料号([S0001]" + hmmsm01["MAT_NO"].ToString() + "[E0001])重复";
			}
			#pragma endregion

			#pragma region 炉号验证
			if (tmmsm01qc["HEAT_NO"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[炉号]材料号([S0002]" + tmmsm01qc["MAT_NO"].ToString() + "[E0002])炉号为空";
			}
			if (tmmsm01qc["SLAB_NO"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[炉号]材料号([S0002]" + tmmsm01qc["SLAB_NO"].ToString() + "[E0002])板坯号为空";
			}
			#pragma endregion

			#pragma region 钢种验证
			if (tmmsm01qc["ST_NO"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[炉号]材料号([S0002]" + tmmsm01qc["ST_NO"].ToString() + "[E0002])钢种为空";
			}
			#pragma endregion

		
			#pragma region 规格验证
			//规格验证:是否缺失 锭型代码是否存在
			if (tmmsm01qc["MAT_THICK"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]材料号([S0005]" + tmmsm01qc["MAT_NO"].ToString() + "[E0005])厚度为0";
			}
			if (tmmsm01qc["MAT_WIDTH"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]材料号([S0006]" + tmmsm01qc["MAT_NO"].ToString() + "[E0006])宽度为0";
			}
			if (tmmsm01qc["MAT_LEN"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]材料号([S0007]" + tmmsm01qc["MAT_NO"].ToString() + "[E0007])长度为0";
			}
			if (tmmsm01qc["MAT_ACT_THICK"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]材料号([S0008]" + tmmsm01qc["MAT_NO"].ToString() + "[E0008])实际厚度为0";
			}
			if (tmmsm01qc["MAT_ACT_WIDTH"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]材料号([S0009]" + tmmsm01qc["MAT_NO"].ToString() + "[E0009])实际宽度为0";
			}
			if (tmmsm01qc["MAT_ACT_LEN"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[规格]坯材料号([S0010]" + tmmsm01qc["MAT_NO"].ToString() + "[E0010])实际长度为0";
			}
			
			#pragma endregion

			#pragma region 件数验证
			//件数验证:条张数是否正确:不为0
			if (tmmsm01qc["MAT_NUM"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[件数]材料号([S0013]" + tmmsm01qc["MAT_NO"].ToString() + "[E0013])支数为0";
			}
			
			#pragma endregion

			

			#pragma region 重量验证
			//重量验证:称重标记与理重、实重是否一致，有无缺漏
			/*if (tmmsm01qc["MAT_THEORY_WT"].ToDecimal() == 0)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[重量]材料号([S0016]" + tmmsm01qc["MAT_NO"].ToString() + "[E0016])无理重";
			}*/
			//if (tmmsm01qc["MEASURE_WT_FLAG"].ToString() == "1")//已称重，必须有实重
			//{
			//	if (tmmsm01qc["MAT_ACT_WT"].ToDecimal() == 0)
			//	{
			//		erro_flag = "-1";
			//		erro_info = erro_info + "@[重量]材料号([S0017]" + tmmsm01qc["MAT_NO"].ToString() + "[E0017])已称重但没实重";
			//	}
			//}			
			#pragma endregion

		

			#pragma region 内部钢种验证
			//if (tmmsm01qc.FACTORY_DIV=="CX")//邯宝
			//{
			//	//内部钢种转换
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = "SELECT NEW_ST_NO FROM TMM00QCGZ "
			//			" WHERE OLD_ST_NO=@tmmsm01qc.ST_NO ";
			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("tmmsm01qc.ST_NO", tmmsm01qc.ST_NO);
			//	cmd_inq.ExecuteReader();
			//	if (cmd_inq.Read())
			//	{
			//		tmmsm01qc.ST_NO = cmd_inq.GetString(1).Trim();
			//	}
			//	cmd_inq.Close();

			//	//内部钢种验证:是否存在
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = "SELECT * FROM TQMTS0X "
			//			" WHERE ST_NO=@tmmsm01qc.ST_NO ";
			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("tmmsm01qc.ST_NO", tmmsm01qc.ST_NO);
			//	cmd_inq.ExecuteReader();
			//	if (!cmd_inq.Read())
			//	{
			//		erro_flag = "-1";
			//		erro_info = erro_info + "@[内部钢种]内部钢种([S0019]" + tmmsm01qc.ST_NO + "[E0019])未维护";
			//	}
			//	cmd_inq.Close();
			//} 
			//else//CSP厂
			//{
			//	//内部钢种验证:是否存在
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = "SELECT * FROM TQMTS0X "
			//			" WHERE UPPER(LABEL1)=UPPER(@tmmsm01qc.ST_NO) "
			//			" AND FACTORY_DIV='S3' "
			//			" AND (ST_NO like 'A%' or ST_NO like 'B%' or ST_NO like 'C%' "
			//			" or ST_NO like 'S%'or ST_NO like 'R%') ";
			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("tmmsm01qc.ST_NO", tmmsm01qc.ST_NO);
			//	cmd_inq.ExecuteReader();
			//	if (!cmd_inq.Read())
			//	{
			//		erro_flag = "-1";
			//		erro_info = erro_info + "@[内部钢种]内部钢种([S0019]" + tmmsm01qc.ST_NO + "[E0019])未维护";
			//	}
			//	cmd_inq.Close();
			//}

			////内部钢种验证:是否存在
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:
			//	sqlstr = "SELECT * FROM TQMTS0X "
			//		" WHERE ST_NO=@tmmsm01qc.ST_NO ";
			//	break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("tmmsm01qc.ST_NO", tmmsm01qc.ST_NO);
			//cmd_inq.ExecuteReader();
			//if (!cmd_inq.Read())
			//{
			//	erro_flag = "-1";
			//	erro_info = erro_info + "@[内部钢种]内部钢种([S0019]" + tmmsm01qc.ST_NO + "[E0019])未维护";
			//}
			//cmd_inq.Close();
			#pragma endregion

			#pragma region 生产时刻验证
			//生产时刻验证:校验是否是日期格式
			if (tmmsm01qc["PROD_TIME"].ToString().GetLength()!=14)
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[生产时刻]材料号([S0020]" + tmmsm01qc["MAT_NO"].ToString() + "[E0020])生产时刻格式错误";
			}
			#pragma endregion

			#pragma region 牌号验证
			//牌号验证:牌号校验，是否存在
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:
			//	sqlstr = "SELECT * FROM TQMTPA4 "
			//		" WHERE SG_SIGN=@tmmsm01qc.SG_SIGN ";
			//	break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("tmmsm01qc.SG_SIGN", tmmsm01qc["SG_SIGN"]);
			//cmd_inq.ExecuteReader();
			//if (!cmd_inq.Read())
			//{
			//	erro_flag = "-1";
			//	erro_info = erro_info + "@[牌号]牌号([S0021]" + tmmsm01qc["SG_SIGN"].ToString() + "[E0021])未维护";
			//}
			//cmd_inq.Close();
			#pragma endregion

			#pragma region PSC验证 石钢无
			//PSC验证:不为空
			/*if (tmmsm01qc.PSC.Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[PSC]材料号([S0022]" + tmmsm01qc.MAT_NO + "[E0022])PSC为空";
			}*/
			#pragma endregion

			#pragma region 库位验证
			//库位验证:库位、跺位、入库时刻不能为空
			if (tmmsm01qc["STOCK_NO"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[库位]材料号([S0023]" + tmmsm01qc["MAT_NO"].ToString() + "[E0023])库号为空";
			}
			else
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT * FROM TSI0021 "
						" WHERE STOCK_NO=@tmmsm01qc.STOCK_NO ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tmmsm01qc.STOCK_NO", tmmsm01qc["STOCK_NO"]);
				cmd_inq.ExecuteReader();
				if (!cmd_inq.Read())
				{
					erro_flag = "-1";
					erro_info = erro_info + "@[库位]材料号([S0023]" + tmmsm01qc["MAT_NO"].ToString() + "[E0023])库号未维护";
				}
				cmd_inq.Close();
			}

			if (tmmsm01qc["STOCK_PLACE_NO"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[库位]材料号([S0024]" + tmmsm01qc["MAT_NO"].ToString() + "[E0024])跺位为空";
			}
			else
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT * FROM TWM04 "
						" WHERE STOCK_PLACE_NO=@tmmsm01qc.STOCK_PLACE_NO ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tmmsm01qc.STOCK_PLACE_NO", tmmsm01qc["STOCK_PLACE_NO"]);
				cmd_inq.ExecuteReader();
				if (!cmd_inq.Read())
				{
					erro_flag = "-1";
					erro_info = erro_info + "@[库位]材料号([S0024]" + tmmsm01qc["MAT_NO"].ToString() + "[E0024])跺位未维护";
				}
				cmd_inq.Close();
			}
			if (tmmsm01qc["IN_STOCK_TIME"].ToString().Trim() == "")
			{
				erro_flag = "-1";
				erro_info = erro_info + "@[库位]材料号([S0025]" + tmmsm01qc["MAT_NO"].ToString() + "[E0025])入库时刻为空";
			}
			#pragma endregion

			#pragma region 标准验证
			//标准验证:是否存在
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:
			//	sqlstr = "SELECT * FROM TQMTPA5 "
			//		" WHERE SG_STD=@tmmsm01qc.SG_STD ";
			//		//" WHERE UPPER(REPLACE(SG_STD,' ',''))=UPPER(REPLACE(@tmmsm01qc.SG_STD,' ','')) ";
			//	break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("tmmsm01qc.SG_STD", tmmsm01qc["SG_STD"]);
			//cmd_inq.ExecuteReader();
			//if (!cmd_inq.Read())
			//{
			//	erro_flag = "-1";
			//	erro_info = erro_info + "@[标准]标准([S0026]" + tmmsm01qc["SG_STD"].ToString() + "[E0026])未维护";
			//}
			//cmd_inq.Close();
			#pragma endregion
			
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[i]["ERRO_FLAG"] = erro_flag;
			bcls_ret->Tables[0].Rows[i]["ERRO_INFO"] = erro_info;
			//正、负反馈则继续循环，不写落地表
			if (erro_flag!="0")
			{				
				continue;
			} 
			else
			{
				////期初数据落地
				//tmmsm01qc.REC_CREATOR = "YZ";
				//tmmsm01qc.REC_CREATE_TIME = datetime;
				//tmmsm01qc.Insert();
			}
		}

		////清除此次验证数据
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:
		//	sqlstr = "DELETE FROM TMMHRBG "
		//		" WHERE REC_CREATOR='YZ' ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteNonQuery();
		//cmd_inq.Close();
	   
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		
