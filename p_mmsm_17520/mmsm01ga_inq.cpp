/*<remark>=========================================================
/// <summary>
/// 板坯综合信息查询
/// 
/// <para>数据库表：
///1. 炼钢板坯物料主表 TMMSM01
///2. 炼钢板坯物料历史表HMMSM01
///3. 合同主档表TOM01
///</para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>板坯综合信息</returns>
===========================================================</remark>*/
#include "stdafx.h"

BM2F_ENTERACE(mmsm01ga_inq)

int f_mmsm01ga_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;

	CString sqlstr = " ";
	int	blkNum1 = 0;
	int	blkNum = 0;
	/* 实体类定义 */
	try
	{
		CString sql_orderby = "";
		CString sql_where = "";
		CString sql = "";
		CDbCommand cm_sql(conn);
		CString sqlcount = "";
		CDecimal RecT = 0;
		int	PageSize = 0; /* 每页记录数 */
		int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
		int	start_row = 0; /* 将要压入outBlock的起始行 */

		bool cont_flag = false;//判断是否需要连接TOM01表
		CString cont_sql = "";


		CString v_query_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"];

		CString v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		CString v_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"];
		CString v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"];
		CString v_pono = bcls_rec->Tables[0].Rows[0]["pono"];
		CString v_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"];
		CString v_origin_mat_no = bcls_rec->Tables[0].Rows[0]["ORIGIN_MAT_NO"];
		CString v_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"];
		CString v_order_no = bcls_rec->Tables[0].Rows[0]["order_no"];
		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];
		CString v_in_flag = bcls_rec->Tables[0].Rows[0]["in_flag"];
		CString v_st_no = bcls_rec->Tables[0].Rows[0]["st_no"];
		CString v_product_flag = bcls_rec->Tables[0].Rows[0]["product_flag"];
		CString v_slat_unlade_cause = bcls_rec->Tables[0].Rows[0]["slat_unlade_cause"];
		CString v_mat_destion = bcls_rec->Tables[0].Rows[0]["mat_destion"];
		CDecimal v_mat_len_s = bcls_rec->Tables[0].Rows[0]["mat_len_s"];
		CDecimal v_mat_len_e = bcls_rec->Tables[0].Rows[0]["mat_len_e"];
		CDecimal v_mat_width_s = bcls_rec->Tables[0].Rows[0]["mat_width_s"];
		CDecimal v_mat_width_e = bcls_rec->Tables[0].Rows[0]["mat_width_e"];
		CDecimal v_mat_thick_s = bcls_rec->Tables[0].Rows[0]["mat_thick_s"];
		CDecimal v_mat_thick_e = bcls_rec->Tables[0].Rows[0]["mat_thick_e"];
		CString v_mat_type = bcls_rec->Tables[0].Rows[0]["mat_type"];
		CString v_sg_sign = bcls_rec->Tables[0].Rows[0]["sg_sign"];
		CString v_order_type_code = bcls_rec->Tables[0].Rows[0]["order_type_code"];
		CString v_delivy_date = bcls_rec->Tables[0].Rows[0]["delivy_date"];
		CString v_next_sub_backlog_code = bcls_rec->Tables[0].Rows[0]["next_sub_backlog_code"];
		CString v_company_name = bcls_rec->Tables[0].Rows[0]["company_name"];


		PageSize = bcls_rec->Tables[0].Rows[0]["PageSize"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		
		CDecimal v_orderby = bcls_rec->Tables[0].Rows[0]["orderby"];//排序方式
		if (v_orderby > 3 || v_orderby <= 0){
			v_orderby = 1;
		}
		Log::Info("", __FUNCTION__, "v_orderby=[{0}]", v_orderby);
		if (v_orderby == 1){
			sql_orderby = " ORDER BY A.ST_NO ASC, A.MAT_ACT_WIDTH ASC ,A.MAT_NO ASC ";
		}
		else if (v_orderby == 2){
			sql_orderby = " ORDER BY A.STOCK_NO ASC, A.MAT_NO ASC ";
		}
		else if (v_orderby == 3){
			sql_orderby = " ORDER BY A.MAT_NO ASC ";
		}

		/* 检查输入参数合法性 */
		if (v_query_flag.Trim() == "")
		{
			sprintf(s.msg, "记录类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 设置开始时刻和结束时刻 */
		if (v_prod_time_from.Trim() != "")
		{
			v_prod_time_from += "000000";
		}
		if (v_prod_time_to.Trim() != "")
		{
			v_prod_time_to += "235959";
		}
		if (v_mat_status.Find(",")){
			v_mat_status = v_mat_status.Replace(",", "','");
		}
		if (v_mat_len_s < 0 || v_mat_len_s>9999999999){ v_mat_len_s = 0; }
		if (v_mat_len_e < 0 || v_mat_len_e>9999999999){ v_mat_len_e = 0; }
		if (v_mat_width_s < 0 || v_mat_width_s>9999999999){ v_mat_width_s = 0; }
		if (v_mat_width_e < 0 || v_mat_width_e>9999999999){ v_mat_width_e = 0; }
		if (v_mat_thick_s < 0 || v_mat_thick_s>9999999999){ v_mat_thick_s = 0; }
		if (v_mat_thick_e < 0 || v_mat_thick_e>9999999999){ v_mat_thick_e = 0; }
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			if (v_query_flag.Trim() == "T")
			{
				sqlcount = " SELECT COUNT(1) "
					" FROM (SELECT A.*   FROM TMMSM01 A ";
				
				sql = " SELECT A.*   FROM TMMSM01 A ";
				
			}
			else //cs_archive_flag = "H"
			{
				sqlcount = " SELECT COUNT(1) "
					" FROM (SELECT A.*   FROM HMMSM01 A ";

				sql = " SELECT A.*   FROM HMMSM01 A ";
			}
			//材料类型
			if (v_mat_type.Trim()!=""){
				if (v_mat_type.Trim() == "1"){
					cont_sql = " WHERE A.MAT_LINE_TYPE <> 'SM'";
				}
				else if (v_mat_type.Trim() = "0"){
					cont_sql = " WHERE A.MAT_LINE_TYPE ='SM'";
				}
			}
			else
			{
				cont_sql = " WHERE 1=1 ";
			}

			//材料号范围
			if (v_mat_no.Trim() != "")
			{
				if (v_mat_no.Find(",", 0) <= 0)	 //单个材料号时，支持模糊查询
				{
					sql_where += " AND A.MAT_NO LIKE @v_mat_no || '%' ";
					//mat_no_flag = 1;
					Log::Trace("", __FUNCTION__, "v_mat_no= [{0}]", (const char*)v_mat_no);
				}
				else
				{
					
					sql_where += " AND A.MAT_NO IN ('" + v_mat_no.Trim() + "') ";
					//mat_no_flag = 0;
					Log::Trace("", __FUNCTION__, "else= [{0}]", (const char*)v_mat_no);
				}
			}
			if (v_prod_time_from.Trim() != "")
			{
				sql_where += " AND A.PROD_TIME >= @v_prod_time_from ";
				Log::Info("", __FUNCTION__, "v_prod_time_from=[{0}]", v_prod_time_from);
			}
			if (v_pono.Trim() != "")
			{
				sql_where += " AND A.PONO LIKE	@v_pono ||'%' ";
				Log::Info("", __FUNCTION__, "v_pono=[{0}]", v_pono);
			}
			if (v_prod_time_to.Trim() != "")
			{
				sql_where+= " AND A.PROD_TIME <=  @v_prod_time_to ";
				Log::Info("", __FUNCTION__, "v_prod_time_to=[{0}]", v_prod_time_to);
			}
			if (v_mat_status.Trim() != "")
			{
				sql_where += " AND A.MAT_STATUS IN ( '"+ v_mat_status.Trim()+"')" ;
				Log::Info("", __FUNCTION__, "v_mat_status=[{0}]", v_mat_status);
			}
			if (v_origin_mat_no.Trim() != "")
			{
				sql_where += " AND A.ORIGIN_MAT_NO LIKE @v_origin_mat_no ||'%' ";
				Log::Info("", __FUNCTION__, "v_origin_mat_no=[{0}]", v_origin_mat_no);
			}
			if (v_heat_no.Trim() != "")
			{
				sql_where += " AND A.HEAT_NO LIKE '%'||@v_heat_no ||'%' ";
				Log::Info("", __FUNCTION__, "v_heat_no=[{0}]", v_heat_no);
			}
			if (v_order_no.Trim() != "")
			{
				sql_where += " AND A.ORDER_NO LIKE @v_order_no ||'%' ";
				Log::Info("", __FUNCTION__, "v_order_no=[{0}]", v_order_no);
			}
			//start
			if (v_stock_no.Trim() != ""){
				sql_where += " AND A.STOCK_NO LIKE @v_stock_no ||'%' ";
				Log::Info("", __FUNCTION__, "v_stock_no=[{0}]", v_stock_no);
			}
			if (v_in_flag.Trim() != "")
			{
				sql_where+= " AND A.IN_FLAG LIKE @v_in_flag  ";
				Log::Info("", __FUNCTION__, "v_in_flag=[{0}]", v_in_flag);
			}
			if (v_st_no.Trim() != ""){
				sql_where += " AND A.ST_NO LIKE '%'||@v_st_no ||'%' ";
				Log::Info("", __FUNCTION__, "v_st_no=[{0}]", v_st_no);
			}
			if (v_product_flag.Trim() != ""){
				sql_where += " AND A.PRODUCT_FLAG = @v_product_flag ";
				Log::Info("", __FUNCTION__, "v_product_flag=[{0}]", v_product_flag);
			}
			if (v_slat_unlade_cause.Trim() != ""){
				sql_where += " AND A.SLAT_UNLADE_CAUSE LIKE @v_slat_unlade_cause ||'%' ";
				Log::Info("", __FUNCTION__, "v_slat_unlade_cause=[{0}]", v_slat_unlade_cause);
			}
			if (v_mat_destion.Trim() != ""){
				sql_where += " AND A.MAT_DESTION = @v_mat_destion AND A.ORDER_NO !=' ' ";
				Log::Info("", __FUNCTION__, "v_mat_destion=[{0}]", v_mat_destion);
			}
			if (v_mat_len_s != 0){
				sql_where += " AND A.MAT_LEN >=  @v_mat_len_s ";
				Log::Info("", __FUNCTION__, "v_mat_len_s=[{0}]", v_mat_len_s);
			}
			if (v_mat_len_e != 0){
				sql_where += " AND A.MAT_LEN <=  @v_mat_len_e ";
			}
			if (v_mat_width_s != 0){
				sql_where += " AND A.MAT_WIDTH >= @v_mat_width_s ";
			}
			if (v_mat_width_e != 0){
				sql_where += " AND A.MAT_WIDTH <= @v_mat_width_e ";
			}
			if (v_mat_thick_s != 0){
				sql_where += " AND A.MAT_THICK >= @v_mat_thick_s ";
			}
			if (v_mat_thick_e != 0){
				sql_where += " AND A.MAT_THICK <= @v_mat_thick_e ";
			}
			if (v_sg_sign.Trim() != ""){
				sql_where += " AND A.SG_SIGN LIKE  '%'||@v_sg_sign ||'%' ";
				Log::Info("", __FUNCTION__, "v_sg_sign=[{0}]", v_sg_sign);
			}
			if (v_order_type_code.Trim() != ""){
				cont_flag = true;
				sql_where += " AND B.ORDER_TYPE_CODE LIKE @v_order_type_code ||'%' ";
				Log::Info("", __FUNCTION__, "v_order_type_code=[{0}]", v_order_type_code);
			}
			if (v_delivy_date.Trim() != ""){
				cont_flag = true;
				sql_where += " AND B.DELIVY_DATE = @v_delivy_date ";
				Log::Info("", __FUNCTION__, "v_delivy_date=[{0}]", v_delivy_date);
			}
			if (v_next_sub_backlog_code.Trim() != ""){
				sql_where += " AND A.NEXT_SUB_BACKLOG_CODE LIKE @v_next_sub_backlog_code ||'%' ";
				Log::Info("", __FUNCTION__, "v_next_sub_backlog_code=[{0}]", v_next_sub_backlog_code);
			}
			if(v_company_name.Trim() != ""){
				sql_where += " AND A.ORDER_NO IN (SELECT DISTINCT ORDER_NO FROM TOM01 WHERE COMPANY_NAME LIKE '%'||@v_company_name||'%') ";
				Log::Info("", __FUNCTION__, "v_company_name=[{0}]", v_company_name);
			}
			break;
		}

		if (cont_flag){
			cont_sql = "JOIN TOM01 B ON A.ORDER_NO=B.ORDER_NO " + cont_sql;
		}
		sqlcount += cont_sql+sql_where+")";
		sql += cont_sql+sql_where +sql_orderby;
	

#pragma region 设置cm_sql的参数
		Log::Trace("", __FUNCTION__, "sql_where			= [{0}]", (const char*)sql_where);
		Log::Trace("", __FUNCTION__, "sqlcount		= [{0}]", (const char*)sqlcount);
		Log::Trace("", __FUNCTION__, "sql				= [{0}]", (const char*)sql);
		cm_sql.Parameters.Clear();
		if (v_mat_no.Trim() != "")
		{
			cm_sql.Parameters.Set("v_mat_no", v_mat_no.Trim());
		}
		if (v_prod_time_from.Trim() != "")
		{
			cm_sql.Parameters.Set("v_prod_time_from", v_prod_time_from);
		}
		if (v_pono.Trim() != "")
		{
			cm_sql.Parameters.Set("v_pono", v_pono.Trim());
		}
		if (v_prod_time_to.Trim() != "")
		{
			cm_sql.Parameters.Set("v_prod_time_to", v_prod_time_to);
		}
	/*	if (v_mat_status.Trim() != "")
		{
			cm_sql.Parameters.Set("v_mat_status", v_mat_status);
		}*/
		if (v_origin_mat_no.Trim() != "")
		{
			cm_sql.Parameters.Set("v_origin_mat_no", v_origin_mat_no.Trim());
		}
		if (v_heat_no.Trim() != "")
		{
			cm_sql.Parameters.Set("v_heat_no", v_heat_no.Trim());
		}
		if (v_order_no.Trim() != "")
		{
			cm_sql.Parameters.Set("v_order_no", v_order_no.Trim());
		}
		if (v_stock_no.Trim() != ""){
			cm_sql.Parameters.Set("v_stock_no", v_stock_no.Trim());
		}
		if (v_in_flag.Trim() != "")
		{
			cm_sql.Parameters.Set("v_in_flag", v_in_flag);
		}
		if (v_st_no.Trim() != ""){
			cm_sql.Parameters.Set("v_st_no", v_st_no.Trim());
		}
		if (v_product_flag.Trim() != ""){
			cm_sql.Parameters.Set("v_product_flag", v_product_flag);
		}
		if (v_slat_unlade_cause.Trim() != ""){
			cm_sql.Parameters.Set("v_slat_unlade_cause", v_slat_unlade_cause.Trim());
		}
		if (v_mat_destion.Trim() != ""){
			cm_sql.Parameters.Set("v_mat_destion", v_mat_destion.Trim());
		}
		if (v_mat_len_s > 0){
			cm_sql.Parameters.Set("v_mat_len_s", v_mat_len_s);
		}
		if (v_mat_len_e > 0){
			cm_sql.Parameters.Set("v_mat_len_e", v_mat_len_e);
		}
		if (v_mat_width_s > 0){
			cm_sql.Parameters.Set("v_mat_width_s", v_mat_width_s);
		}
		if (v_mat_width_e > 0){
			cm_sql.Parameters.Set("v_mat_width_e", v_mat_width_e);
		}
		if (v_mat_thick_s > 0){
			cm_sql.Parameters.Set("v_mat_thick_s", v_mat_thick_s);
		}
		if (v_mat_thick_e > 0){
			cm_sql.Parameters.Set("v_mat_thick_e", v_mat_thick_e);
		}
		if (v_sg_sign.Trim() != ""){
			cm_sql.Parameters.Set("v_sg_sign", v_sg_sign.Trim());
		}
		if (v_order_type_code.Trim() != ""){
			cm_sql.Parameters.Set("v_order_type_code", v_order_type_code.Trim());
		}
		if (v_delivy_date.Trim() != ""){
			cm_sql.Parameters.Set("v_delivy_date", v_delivy_date);
		}
		if (v_next_sub_backlog_code.Trim() != ""){
			cm_sql.Parameters.Set("v_next_sub_backlog_code", v_next_sub_backlog_code.Trim());
		}
		if (v_company_name.Trim() != ""){
			cm_sql.Parameters.Set("v_company_name", v_company_name.Trim());
		}	
#pragma endregion

		Log::Trace("", __FUNCTION__, " sqlcount				= [{0}]", (const char*)sqlcount);
		cm_sql.SetCommandText(sqlcount);
		RecT = cm_sql.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "cd_count			= [{0}]", RecT);
		start_row = PageSize * (current_page_no - 1);
		if (start_row > RecT.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "current_page_no		= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "PageSize= [{0}]", PageSize);
		Log::Trace("", __FUNCTION__, "start_row			= [{0}]", start_row);
		cm_sql.SetCommandText(sql);
		cm_sql.ExecuteQuery(bcls_ret->Tables[0], start_row, PageSize);
		cm_sql.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = RecT.ToInt32();

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
		strcpy(s.sysmsg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return(doFlag);
}
