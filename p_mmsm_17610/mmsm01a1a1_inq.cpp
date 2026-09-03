/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Description: 炼钢钢坯材料详细信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料详细信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  



//外部函数声明

BM2F_ENTERACE(mmsm01a1a1_inq)   

int f_mmsm01a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmmsm01("TMMSM01");
	CModel tep0002("TEP0002");
	CModel tqmts29("TQMTS29");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		/* 设置返回块列 */
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_WT");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_INNER_DIA");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_LEN");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_WIDTH");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_THICK");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"PROD_CNAME");

		/* 获取输入参数 */
		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
	
		Log::Trace("",__FUNCTION__,"tmmsm01.MAT_NO		= [{0}]",(const char*)tmmsm01["MAT_NO"].ToString());
		
		/* 检查输入参数合法性 */
		if(tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  

		/* 查询热卷材料当前档&历史档 */
		tmmsm01.Query("MAT_NO");
		if(tmmsm01["MAT_ID"].ToString().Trim() == "")
		{
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr	= "SELECT * "
							  "  FROM HMMSM01 "
							  " WHERE MAT_NO = @tmmsm01.MAT_NO "
							  "   AND MAT_ID = (SELECT NVL(MAX(MAT_ID), ' ') "
							  "					  FROM HMMSM01 "
							  "					 WHERE MAT_NO = @tmmsm01.MAT_NO) ";
					break;
			}     
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm01.MAT_NO",tmmsm01["MAT_NO"].ToString()); 
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);
			}
			else
			{
				strcpy(s.msg,"材料号不存在!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
		}
		/* 设置返回块的值 */
		tmmsm01.MergeTo(bcls_ret->Tables[0],false);

		#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		/* 查询合同信息 */
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= "SELECT ORDER_WT, "
						  "		  ORDER_INNER_DIA," 
						  "		  ORDER_LEN," 
						  "		  ORDER_WIDTH," 
						  "		  ORDER_THICK," 
						  "		  PROD_CNAME " 
						  "  FROM TOM01 "
						  " WHERE ORDER_NO = @tmmsm01.ORDER_NO ";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm01.ORDER_NO",tmmsm01["ORDER_NO"].ToString());
		cmd_inq.ExecuteReader(); 
		if(cmd_inq.Read())
		{ 
			bcls_ret->Tables[0].Rows[0]["ORDER_WT"]			= cmd_inq.GetDecimal(1);
			bcls_ret->Tables[0].Rows[0]["ORDER_INNER_DIA"]	= cmd_inq.GetDecimal(2);
			bcls_ret->Tables[0].Rows[0]["ORDER_LEN"]		= cmd_inq.GetDecimal(3);
			bcls_ret->Tables[0].Rows[0]["ORDER_WIDTH"]		= cmd_inq.GetDecimal(4);
			bcls_ret->Tables[0].Rows[0]["ORDER_THICK"]		= cmd_inq.GetDecimal(5);
			bcls_ret->Tables[0].Rows[0]["PROD_CNAME"]		= cmd_inq.GetString(6);
		}		
		cmd_inq.Close();
		#endif

		/* 根据代码配置表压入化学成分列名 */
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= "SELECT * "
						  "  FROM TEP0002 "
						  " WHERE CODE_CLASS = 'QMYS2N' "
						  "	  AND TRIM(CODE_DESC_3_CONTENT) IS NOT NULL "
						  " ORDER BY CODE_DESC_2_CONTENT ASC  ";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.ExecuteReader(); 
		while(cmd_inq.Read())
		{ 
			cmd_inq.Fetch(tep0002);
			
			tep0002["CODE_DESC_1_CONTENT"]		= tep0002["CODE_DESC_1_CONTENT"].ToString().ToLower();

			//增加返回块的列名(化学成分)。增加校验是为了避免列重名-化学成分代码定义重复(化学成分可定义大写，也可定义小写，而列名不区分大小写)
			if(bcls_ret->Tables[0].Columns.Contains(tep0002["CODE_DESC_1_CONTENT"].ToString()) == false)
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,tep0002["CODE_DESC_1_CONTENT"].ToString()); 
			}	
		}		
		cmd_inq.Close();


		/* 根据PONO,取得各个化学元素的值 */
		if (tmmsm01["PONO"].ToString().Trim() != "")
		{
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT * "
						"  FROM TQMTS29 "
						" WHERE PONO	= @tmmsm01.PONO "
						" ORDER BY	ELM_CODE ASC";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm01.PONO", tmmsm01["PONO"].ToString());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{

				
				cmd_inq.Fetch(tqmts29);
				tqmts29.TrimOrBlank();
				tqmts29["ELM_NAME"] = tqmts29["ELM_NAME"].ToString().ToLower();
				//Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_NAME	= [{0}]", (const char*)tqmtqq0["ELM_NAME"].ToString());
				//Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_ACT		= [{0}]", tqmtqq0["ELM_ACT"].ToDecimal().ToDouble());
				//增加返回块的(化学成分)值
				bcls_ret->Tables[0].Rows[0][tqmts29["ELM_NAME"].ToString()] = tqmts29["ELM_ACT"];
			}
			cmd_inq.Close();
		}

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



