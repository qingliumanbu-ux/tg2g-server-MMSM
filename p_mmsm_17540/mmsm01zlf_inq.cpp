/*============================================================================*/
/*== [service名  ]:  mmsm01zlf_inq       ||  [对应VC#画面 ]:GCPMSI00          ==*/
/*== [程序编制人 ]:  张颖               ||  [程序定稿日期]:2022/7/28 20:30:28==*/
/*== [程序修改人 ]：                    ||  [程序修改日期]:                  ==*/
/*========================================================================*/
/*== [数据库表   ]： TGCPMSI00                                          ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 表TGCPMSI00_信息查询                               ==*/
/*========================================================================*/
/******框架头******/
#include "stdafx.h" 



/*<remark>=========================================================
/// <summary>
/// 表TGCPMSI00_信息查询
/// <para>
///    功能叙述段落
/// </para>
/// </summary>
/// <param name="bcls_rec">传入块  </param>
/// <param name="bcls_ret">返回块  </param>
/// <returns>返回：是否成功</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm01zlf_inq)

int f_mmsm01zlf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm01zlf_inq";                //定义函数英文名称  
	CString FunctionCname = "表TMMSM01_信息查询";              //定义函数中文名称 


	//程序用变量
	int   i = 0;
	int   fun_i = 0; //功能个数。
	int   fetchRowCount = 0;
	int		TotalRecordCount = 0;
	int   doFlag = 0;

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString  userid = s.userid;  
	CString  sqlstr = "";  //SQL 信息。 
	CString  sqlstr_count = "";  //SQL 信息。 
	CString v_mat_no = "";
	CString v_mat_kind = "";
	CString v_unit_code = "";

	CDecimal v_cnt = 0; 


	try
	{ 
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CPageInfo pageInfo;//分页

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		CString    c_sql_where      = "  WHERE   1 = 1 "; //修改条件。
		CString    c_sql_condition  =  " SELECT  t.* FROM TMMSM01 t  " ;
		CString    c_sql_count = " SELECT  count(*) FROM TMMSM01  ";
		CString    c_order_by       =  " order by t.MAT_NO ";

		//材料号
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		//物料种类
		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString();
		//机组代码
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
			v_unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString();

		Log::Trace("", __FUNCTION__, "in ==v_mat_no[{0}]  ", v_mat_no);
		Log::Trace("", __FUNCTION__, "in ==v_mat_kind[{0}]  ", v_mat_kind);
		Log::Trace("", __FUNCTION__, "in ==v_unit_code[{0}]  ", v_unit_code);


		if (v_mat_no.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND t.mat_no LIKE @mat_no  || '%' ";
		}
		if (v_mat_kind.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND t.mat_kind LIKE @mat_kind  || '%' ";
		}
		if (v_unit_code.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND t.unit_code LIKE @unit_code  || '%' ";
		}
		 
		//信息初始化语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_order_by;
		c_sql_count = c_sql_count + c_sql_where;
		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);  
		Log::Trace("", __FUNCTION__, "c_sql_count[{0}]  ", c_sql_count);

		sqlstr = c_sql_condition;
		sqlstr_count = c_sql_count;
		cmd_sql.Parameters.Set("mat_no", v_mat_kind);
		cmd_sql.Parameters.Set("mat_kind", v_mat_kind);
		cmd_sql.Parameters.Set("unit_code", v_unit_code);
		cmd_sql.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_sql.ExecuteScalar().ToInt32();
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句  

		cmd_sql.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_sql.Close();


		//返回的记录数。 
		fetchRowCount = bcls_ret->Tables[0].Rows.get_Count();

		/*设置系统返回参数*/ 
		sprintf(s.msg, "查询到[%d]条记录。", fetchRowCount);

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
		  
	}

	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CString str = "err:[" + sqlstr + "]";
		sprintf(s.msg, "sqlCode[%d]-[%s],%s"
			, ex.GetCode(), (const char*)ex.GetMsg()
			, (const char*)str);
		doFlag = -1;        //数据库异常时返回-1，事务将被回滚

		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]  ", sqlstr);
		Log::Trace("", __FUNCTION__, "sqlerr =[{0}]-[{1}]  "
			, ex.GetCode(), ex.GetMsg());
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex) //其他错误。
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//将来可能要拆service处理，SO ，此处记录一下当前service名称,用于显示前台。
	//=============
	strcpy(s.sysmsg, s.svc_name);
	s.flag = doFlag;
	return doFlag;

}