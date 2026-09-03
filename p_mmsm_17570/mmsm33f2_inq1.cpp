/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-06-03
Version:1.0
Description: 铸坯切断实绩材料信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"




/*<remark>=========================================================
/// <summary>
/// 铸坯切断实绩材料信息查询
/// <para>
/// 查询铸坯切断材料实绩信息
/// </para>
/// </summary>
/// <param name="SM_UNIT_NO">主工序代码</param>
/// <param name="PONO">制造命令号</param>
/// <returns>实绩信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33f2_inq1)


int f_mmsm33f2_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sm_unit_no = "";
	CString factory_div = "";
	CString pono = "";
	CString heat_no = "";
	CString mat_destion = "";
	CString cast_lot_no = "";
	CString billet_type = "";
	CString slab_plan_dest = "";
	CString code = "";             //存放EP01查询结果CODE
	CString code_desc = "";			//存放EP01查询结果code_desc_1_content
	CDecimal  sum_slab_wt = 0;     //已有铸坯总重量
	CDecimal  sum_mat_tube = 0;     //已有铸坯总支数

	CString sqlstr = "";


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq33(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
		{
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_LOT_NO"))
		{
			cast_lot_no = bcls_rec->Tables[0].Rows[0]["CAST_LOT_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("BILLET_TYPE"))
		{
			billet_type = bcls_rec->Tables[0].Rows[0]["BILLET_TYPE"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
		{
			slab_plan_dest = bcls_rec->Tables[0].Rows[0]["SLAB_PLAN_DEST"].ToString().Trim();
		}


		Log::Info("", __FUNCTION__, "factory_div      =[{0}]", factory_div);
		Log::Info("", __FUNCTION__, "pono      =[{0}]", pono);
		Log::Info("", __FUNCTION__, "heat_no      =[{0}]", heat_no);
		Log::Info("", __FUNCTION__, "cast_lot_no      =[{0}]", cast_lot_no);
		Log::Info("", __FUNCTION__, "billet_type      =[{0}]", billet_type);
		Log::Info("", __FUNCTION__, "slab_plan_dest      =[{0}]", slab_plan_dest);

		bcls_ret->Tables[0].set_TableName("SLAB_PLAN");
		bcls_ret->Tables.Add("SLAB_PROD");

		//wzn_20170919 根据系统配置，查询管理方式，根据管理方式选择查询逻辑
		sqlstr = " select a.code,nvl(b.ingot_code,' ')ingot_code,nvl(b.code_desc_1_content,' ') code_desc_1_content from("
			" select CODE, code_class from tep0002 where code_class = 'MS41' AND CODE_DESC_2_CONTENT = '1')a "
			" left JOIN "
			" (select code as ingot_code, code_desc_1_content, code_class from tep0002 where code_class = 'PM2D'"
			" and code in (select distinct ingot_code from tpssm03 where pono=@pono))b on a.code_class<>b.code_class"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			code = cmd_inq.GetString(1);
			code_desc = cmd_inq.GetString(3);
		}
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "code =[{0}] code_desc=[{1}]", code, code_desc);

		//if (billet_type.Trim() == "3" || billet_type.Trim() == "4")  //EPEP01代码PSA6里 方圆坯类型，按量显示 ，可客制化
		if (code.Trim() == "1" || (code.Trim() == "0" && code_desc.Trim() == "01"))  //EPEP01代码PSA6里 方圆坯类型，按量显示 ，可客制化
		{
			//小方坯、圆坯（按量显示）
			//sqlstr = " SELECT * "
			//	"   FROM (SELECT MIN(SLAB_NO) AS SLAB_NO"//,COUNT(SLAB_NUM) AS SLAB_NUM "
			//	" FROM TPSSM03 WHERE 1=1 GROUP BY SLAB_THICK,SLAB_WIDTH,SLAB_LEN,SLAB_WT,ORDER_NO,PONO) TEMP  LEFT JOIN TPSSM03 T3 ON T3.SLAB_NO = TEMP.SLAB_NO   "
			//	"	WHERE PONO = @pono"
			//	" AND SLAB_PROD_FLAG <> '1' "
			sqlstr = "SELECT * FROM "
				" (SELECT LSLAB_NO, SUM(SLAB_LEN) SLAB_LEN_L, SUM(SLAB_WT) SLAB_WT_L,"
				" SUM(SLAB_NUM) SLAB_NUM_L, MIN(SLAB_NUM) SLAB_NUM_N,"
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_NUM) SLAB_NUM_REMAIN, "
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_LEN) SLAB_LEN_REMAIN, "
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_WT) SLAB_WT_REMAIN "
				" FROM  TPSSM03 WHERE  PONO = @pono"
				//" AND SLAB_PROD_FLAG <> '1' "
				" GROUP BY LSLAB_NO"
				" )A"
				" LEFT JOIN"
				" (SELECT * FROM TPSSM03 WHERE  PONO = @pono )B"
				" ON A.LSLAB_NO = B.LSLAB_NO order BY b.LSLAB_NO,b.slab_no"

				;
		}
		else
		{
			//按命令坯显示，无需显示代预标记
			sqlstr = "SELECT * FROM "
				" (SELECT LSLAB_NO, SUM(SLAB_LEN) SLAB_LEN_L, SUM(SLAB_WT) SLAB_WT_L,"
				" SUM(SLAB_NUM) SLAB_NUM_L, MIN(SLAB_NUM) SLAB_NUM_N,"
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_NUM) SLAB_NUM_REMAIN, "
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_LEN) SLAB_LEN_REMAIN, "
				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_WT) SLAB_WT_REMAIN "
				" FROM  TPSSM03 WHERE  PONO = @pono"
				//" AND SLAB_PROD_FLAG <> '1' "
				" GROUP BY LSLAB_NO"
				" )A"
				" LEFT JOIN"
				" (SELECT * FROM TPSSM03 WHERE  PONO = @pono )B"
				" ON A.LSLAB_NO = B.LSLAB_NO order BY b.LSLAB_NO,b.slab_no";


//#if defined _LINE_HP       //定义有厚板产线 且要查询代预标记
//			sqlstr = "SELECT A.*,B.*,C.PICK_PATTERN_CODE FROM "
//				" (SELECT LSLAB_NO, SUM(SLAB_LEN) SLAB_LEN_L, SUM(SLAB_WT) SLAB_WT_L,"
//				" SUM(SLAB_NUM) SLAB_NUM_L, MIN(SLAB_NUM) SLAB_NUM_N,"
//				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_NUM) SLAB_NUM_REMAIN, "
//				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_LEN) SLAB_LEN_REMAIN, "
//				" SUM(decode(slab_prod_flag,'1',0,1)*SLAB_WT) SLAB_WT_REMAIN "
//				" FROM  TPSSM03 WHERE  PONO = @pono"
//				" GROUP BY LSLAB_NO"
//				" )A"
//				" LEFT JOIN"
//				" (SELECT * FROM TPSSM03 WHERE  PONO = @pono )B"
//				" ON A.LSLAB_NO = B.LSLAB_NO "
//				" LEFT JOIN"
//				" (SELECT max(PICK_PATTERN_CODE) PICK_PATTERN_CODE,PONO_SLAB FROM tqmtqj3a WHERE  PONO = @pono GROUP BY PONO_SLAB )C"
//				" ON B.SLAB_NO = C.PONO_SLAB "
//				"order BY b.LSLAB_NO,b.slab_no";
//#endif
		}



		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["SLAB_PLAN"]);
		cmd_inq.Close();



		//取生成的材料信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT  (SELECT PRINT_NO FROM TMMSM01 T WHERE T1.MAT_NO = T.MAT_NO) PRINT_NO, (SELECT BATCH FROM TMMSM01 T WHERE T1.MAT_NO = T.MAT_NO)  BATCH ,(SELECT STOCK_L2 FROM TMMSM01 T WHERE T1.MAT_NO = T.MAT_NO) STOCK_L2, (SELECT GUIDE_DEST FROM TMMSM01 T WHERE T1.MAT_NO = T.MAT_NO) GUIDE_DEST,T1.* "
				"   FROM TMMSM33  T1 "
				"	WHERE PONO				= @pono and ARCHIVE_FLAG<> '3' "//ARCHIVE_FLAG 为3时是盘库新增在33表占位。该数据不显示出来
				"	ORDER BY MAT_NO, SLAB_CUT_SEQ ASC "
				;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("sm_unit_no", sm_unit_no);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["SLAB_PROD"]);
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护3人员。*/, arguments, 1);
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
