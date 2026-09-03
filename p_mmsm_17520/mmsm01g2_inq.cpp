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

BM2F_ENTERACE(mmsm01g2_inq)

int f_mmsm01g2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		CDbCommand cm_sqlcount(conn);
		CDecimal para = 0;
		

		CString v_query_flag = bcls_rec->Tables[0].Rows[0]["query_flag"];

		CString v_pono = bcls_rec->Tables[0].Rows[0]["pono"];//该列有多行值
		CString v_order_no = bcls_rec->Tables[0].Rows[0]["order_no"];
		CString v_whole_backlog_code1 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code1"];
		CString v_whole_backlog_code2 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code2"];
		CString v_whole_backlog_code3 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code3"];

		CString m_whole_backlog_code1 = "";
		CString m_whole_backlog_code2 = "";
		CString m_whole_backlog_code3 = "";

		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];
		CString v_dest_fin = bcls_rec->Tables[0].Rows[0]["direct"];
		CString v_quality_grade = bcls_rec->Tables[0].Rows[0]["quality_grade"];

		CString v_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"];
		CString v_product_flag = bcls_rec->Tables[0].Rows[0]["product_flag"];//成品标志
		CString v_order_status = bcls_rec->Tables[0].Rows[0]["order_status"];

		CString v_st_no = bcls_rec->Tables[0].Rows[0]["st_no"];//出钢记号

		CString v_ai_time_1_f = bcls_rec->Tables[0].Rows[0]["ai_time_1_f"];
		CString v_ai_time_1_t = bcls_rec->Tables[0].Rows[0]["ai_time_1_t"];
		CString v_orderby = bcls_rec->Tables[0].Rows[0]["orderby"];//排序方式
		CString v_query_is1f = bcls_rec->Tables[0].Rows[0]["query_is1f"];//无炉次
		CString v_clean_if_flag = bcls_rec->Tables[0].Rows[0]["clean_if_flag"];//未清理
		CString v_ibb_flag = bcls_rec->Tables[0].Rows[0]["ibb_flag"];  //IBB 
		CString v_hot_flag = bcls_rec->Tables[0].Rows[0]["hot_flag"]; //比热送
		CString v_store_area = bcls_rec->Tables[0].Rows[0]["store_area"];

		if (v_orderby == "1"){
			sql_orderby = " ORDER BY ST_NO ASC, MAT_ACT_WIDTH ASC ,MAT_NO ASC ";
		}
		else if (v_orderby == "2"){
			sql_orderby = " ORDER BY STOCK_NO ASC, MAT_NO ASC ";
		}
		else if (v_orderby = "3"){
			sql_orderby = " ORDER BY MAT_NO ASC ";
		}

		if (bcls_rec->Tables.Contains("STOCK_NO_IN")){
			bcls_rec->Tables.Remove("STOCK_NO_IN");
		}
		if (bcls_rec->Tables.Contains("DEST")){
			bcls_rec->Tables.Remove("DEST");
		}
		bcls_rec->Tables.Add("STOCK_NO_IN");
		bcls_rec->Tables["STOCK_NO_IN"].Columns.Add(DT_STRING, "STOCK_NO");
		bcls_rec->Tables.Add("DEST");
		bcls_rec->Tables["DEST"].Columns.Add(DT_STRING, "DEST_FIN");

		int PageSize = (int)bcls_rec->Tables[1].Rows[0]["PageSize"];
		int PageStart = (int)bcls_rec->Tables[1].Rows[0]["PageStart"];
		Log::Info("", __FUNCTION__, "PageSize=【{0}】；PageStart=【{1}】", PageSize, PageStart);
		bcls_ret->Tables[0].set_TableName("MMSM01G2_INQ");
		
		if (v_pono.Trim().GetLength() > 0){
			sql_where += " AND ( 1<>1 ";
			int rowcnt = bcls_rec->Tables[0].Rows.get_Count();
			for (int i = 0; i<rowcnt; i++){
				if (bcls_rec->Tables[0].Rows[i]["PONO"].ToString().Trim().GetLength() != 0){
					sql_where += " OR A.PONO LIKE '" + bcls_rec->Tables[0].Rows[i]["PONO"].ToString() + "' ";
				}
			}
			sql_where += ")";
		}

		if (v_order_no.Trim().GetLength() > 0){
			if (v_order_no.Trim() == "Y"){
				sql_where += " AND trim(A.ORDER_NO)>''";
			}
			else if (v_order_no.Trim() == "N"){
				sql_where += " AND  trim(A.ORDER_NO)<=''";
			}
			else{
				sql_where += " AND A.ORDER_NO LIKE @v_order_no";
			}
		}
		if (v_whole_backlog_code1.Trim().GetLength() > 0 || v_whole_backlog_code2.Trim().GetLength() > 0 || v_whole_backlog_code3.Trim().GetLength() > 0)
		{
			sql_where += " AND ( 1 <> 1";
			if (v_whole_backlog_code1.Trim().GetLength() > 0){
				m_whole_backlog_code1 = "%" + v_whole_backlog_code1 + "%";
				sql_where += " OR ( A.WHOLE_BACKLOG LIKE @m_whole_backlog_code1 AND  "
					" (POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code1)-POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code1)/2 *2)>0)";
			}
			if (v_whole_backlog_code1.Trim().GetLength() > 0){
				m_whole_backlog_code2 = "%" + v_whole_backlog_code2 + "%";
				sql_where += " OR ( A.WHOLE_BACKLOG LIKE @m_whole_backlog_code2 AND  "
					" (POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code2)-POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code2)/2 *2)>0)";

			}
			if (v_whole_backlog_code1.Trim().GetLength() > 0){
				m_whole_backlog_code3 = "%" + v_whole_backlog_code3 + "%";
				sql_where += " OR ( A.WHOLE_BACKLOG LIKE @m_whole_backlog_code3 AND  "
					" (POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code3)-POSSTR(A.WHOLE_BACKLOG,@v_whole_backlog_code3)/2 *2)>0)";
			}
			sql_where += ")";
		}
		Log::Info("", __FUNCTION__, " 开始进入");
#pragma region 获取向库区输入的多个值,设置sql语句和将输入值放入表STOCK_NO_IN
		if (v_stock_no.Trim().GetLength() > 0)
		{
			Log::Info("", __FUNCTION__, " 位置0");
			if (v_stock_no.Trim().GetLength() <= 3)
			{
				v_stock_no = "%" + v_stock_no + "%";
				sql_where += " AND A.STOCK_NO LIKE @v_stock_no";
				Log::Info("", __FUNCTION__, " 位置01");
			}
			else
			{
				sql_where += " AND ( 1 <>1 ";
				Log::Info("", __FUNCTION__, " 位置1");
				v_stock_no = v_stock_no.Trim() + " ";
				int Loop = v_stock_no.GetLength();
				int rowc = 0;//将要填充的行数
				Log::Info("", __FUNCTION__, "位置0，v_stock_no=【{0}】,Loop=【{1}】", v_stock_no,Loop);
				for (int index = 0, count = 0, top = -1; index < Loop; index++){
					if (v_stock_no.GetAt(index) != ' ')
					{
						count++;
						if (count == 1){
							top  = index;
						}
					}
					else{    
						if (top != -1){
							if (count<= 3)
							{
								bcls_rec->Tables["STOCK_NO_IN"].Rows.Add();
								bcls_rec->Tables["STOCK_NO_IN"].Rows[rowc]["STOCK_NO"] = "%" + v_stock_no.Substring(top, (count)) + "%";
								para = rowc;	rowc++;
								sql_where += "  OR  A.STOCK_NO LIKE @v_stock_no" + para.ToString();
							}
							else{
								int max = (count + 2) / 3;
								int lastct = count - 3 * (max - 1);
								for (int i = 0; i < max; i++){
									bcls_rec->Tables["STOCK_NO_IN"].Rows.Add();
									if (i + 1 == max){
										bcls_rec->Tables["STOCK_NO_IN"].Rows[rowc]["STOCK_NO"] = "%" + v_stock_no.Substring(top + i * 3, lastct) + "%";
									}
									else
									{
										bcls_rec->Tables["STOCK_NO_IN"].Rows[rowc]["STOCK_NO"] =   v_stock_no.Substring(top + i * 3, 3);
									}
									para = rowc;	rowc++;
									sql_where += "  OR  A.STOCK_NO LIKE @v_stock_no" + para.ToString();
								}
							}

						}

						///重置使用过的指针
						top = -1;	count = 0;
					}
				}
				sql_where += ") ";
			}
		}
#pragma endregion

#pragma region 获取向去向输入的多个值,设置sql语句和将输入值放入表DEST
		if (v_dest_fin.Trim().GetLength() > 0)
		{
			sql_where += " AND ( 1 <>1 ";
			v_dest_fin = v_dest_fin.Trim() + " ";
			int Loop = v_dest_fin.GetLength();
			int rowc = 0;//将要填充的行数
			Log::Info("", __FUNCTION__, "位置0，v_dest_fin=【{0}】,Loop=【{1}】", v_dest_fin, Loop);
			for (int index = 0, count = 0, top = -1; index < Loop; index++)
			{
				if (v_dest_fin.GetAt(index) != ' ')
				{
					count++;
					if (count == 1){
						top = index;
					}	
				}
				else{
					if (top != -1){
						if (count <= 2)
						{
							bcls_rec->Tables["DEST"].Rows.Add();
							bcls_rec->Tables["DEST"].Rows[rowc]["DEST_FIN"] = v_dest_fin.Substring(top, count);
							para = rowc; rowc++;
							sql_where += "  OR  A.DEST_FIN LIKE @v_dest_fin" + para.ToString();
						}
						else{
							int max = (count + 1) / 2; int lastct = count - 2 * (max - 1);
							for (int i = 0; i < max; i++){
								bcls_rec->Tables["DEST"].Rows.Add();
								if (i + 1 == max){
									bcls_rec->Tables["DEST"].Rows[rowc]["DEST_FIN"] = v_dest_fin.Substring(top + i * 2, lastct);
								}
								else
								{
									bcls_rec->Tables["DEST"].Rows[rowc]["DEST_FIN"] = v_dest_fin.Substring(top + i * 2, 2);
								}
								para = rowc; rowc++;
								sql_where += "  OR  A.DEST_FIN LIKE @v_dest_fin" + para.ToString();
							}
						}

					}
					///重置使用过的指针,和小段字符长度的
					top = -1; count = 0;
				}
			}
			sql_where += ")";
		}
	
#pragma endregion

		if (v_quality_grade.Trim().GetLength() > 0){
			sql_where += " AND A.QUALITY_GRADE LIKE @v_quality_grade";
		}
		//此处默认材料状态码都为两位(两状态码之间由“，”和“ ”分隔）
		if (v_mat_status.Trim().GetLength() > 0){
			sql_where += " AND A.MAT_STATUS IN(";
			int max = (v_mat_status.Trim().GetLength() + 2) / 4;
			for (int i = 0; i<max; i++){
				sql_where += " '" + v_mat_status.Trim().Substring(4 * i, 2) + "',";
			}
			Log::Info("", __FUNCTION__, "sql_where=[{0}]", sql_where);
			sql_where=sql_where.Substring(0,sql_where.GetLength() - 1);
			Log::Info("", __FUNCTION__, "sql_where=[{0}]", sql_where);
			sql_where += ")";
		}
		if (v_product_flag.Trim().GetLength() > 0){
			sql_where += " AND A.PRODUCT_FLAG LIKE @v_product_flag";
		}
		//此处默认合同状态码都为两位(两状态码之间由“，”和“ ”分隔）
		if (v_order_status.Trim().GetLength() > 0){
			sql_where += " AND B.ORDER_STATUS IN(";
			int max = (v_order_status.Trim().GetLength() + 2) / 4;
			for (int i = 0; i<max; i++){
				sql_where += " '" + v_order_status.Trim().Substring(4 * i, 2) + "',";
			}
			Log::Info("", __FUNCTION__, "sql_where=[{0}]", sql_where);
			sql_where = sql_where.Substring(0, sql_where.GetLength() - 1);
			Log::Info("", __FUNCTION__, "sql_where=[{0}]", sql_where);
			sql_where += ")";
		}
		if (v_st_no.Trim().GetLength() > 0){
			sql_where += " AND A.ST_NO LIKE @v_st_no";
		}
		if (v_ai_time_1_f.Trim().GetLength() > 0){
			sql_where += " AND A.SLAB_CUT_TIME >= @v_ai_time_1_f";
		}
		if (v_ai_time_1_t.Trim().GetLength() > 0){
			sql_where += " AND A.SLAB_CUT_TIME <= @v_ai_time_1_t";
		}
		if (v_query_is1f.Trim().GetLength() > 0){

		}
		if (v_clean_if_flag.Trim().GetLength() > 0){

		}
		if (v_ibb_flag.Trim().GetLength() > 0){

		}
		if (v_hot_flag.Trim().GetLength() > 0){

		}
		if (v_store_area.Trim().GetLength() > 0){
			sql_where += " AND A.STORE_AREA LIKE @v_store_area||%";
		}

		if (v_query_flag == "0"){
			sqlcount = "SELECT COUNT(A.MAT_NO) FROM TMMSM01 A LEFT JOIN TOM01 B ON A.ORDER_NO=B.ORDER_NO WHERE 1=1 ";

			sql = " SELECT A.MAT_NO,A.ST_NO,A.MAT_ACT_THICK,A.MAT_ACT_WIDTH,A.MAT_ACT_LEN,A.MAT_ACT_WT,"
				" A.DEST_FIN,A.STOCK_NO,A.MAT_STATUS,A.MNG_HOLD_CAUSE_CODE,A.SLAB_CUT_TIME,A.ORDER_NO,"
				" A.HSF_END_TIME,A.PLAN_NO,A.CONFM_PLAN_NO,A.TRANSFER_PLAN_NO,A.WHOLE_BACKLOG,A.QUALITY_GRADE,"
				" A.SLAB_HEAD_WIDTH,A.SLAB_TAIL_WIDTH,A.IN_STOCK_TIME,A.OUT_STOCK_TIME,A.SLAB_PLACE_CODE,A.STORE_AREA,A.SLAT_UNLADE_CAUSE,"
				" B.DELIVY_DATE,B.ORDER_TYPE_CODE,B.ORDER_STATUS"
				
				/*" A.WHOLE_BACKLOG_CODE,A.MAT_GROUP,"
				" A.MAT_MATCH_ERR_CODE,B.ORDER_STATUS,B.DELIVY_WEEK_FLAG,B.ORDER_DELIVERY_DATE,"
				" B.ORDER_TYPE_CODE,A.SLAT_UNLADE_CAUSE,A.BATCH_PROD_CODE,"
				" A.BACKLOG, ' ' As flag, B.BAND_ORD_SORT,A.SLAB_REMARK,A.WHOLE_BACKLOG,"
				" A.PONO,A.ST_CHE_CAUSE_CODE,"
				" A.MAT_CAUSE_CODE1,A.HOT_TEST_CODE,A.STORE_AREA,A.FORM_PLATE_FLAG,"
				" A.SECUT_PLAN_NO,A.STORE_AREA "*/
				" FROM TMMSM01 A LEFT JOIN TOM01 B  on A.ORDER_NO = B.ORDER_NO where 1 = 1 ";
		}
		else if (v_query_flag = "1"){
			sqlcount= "SELECT COUNT(A.MAT_NO) FROM HMMSM01 A LEFT JOIN TOM01 B ON A.ORDER_NO=B.ORDER_NO WHERE 1=1 ";

			sql=" SELECT A.MAT_NO,A.ST_NO,A.MAT_ACT_THICK,A.MAT_ACT_WIDTH,A.MAT_ACT_LEN,A.MAT_ACT_WT,"
				" A.DEST_FIN,A.STOCK_NO,A.MAT_STATUS,A.MNG_HOLD_CAUSE_CODE,A.SLAB_CUT_TIME,A.ORDER_NO,"
				" A.HSF_END_TIME,A.PLAN_NO,A.CONFM_PLAN_NO,A.TRANSFER_PLAN_NO,A.WHOLE_BACKLOG,A.QUALITY_GRADE,"
				" A.SLAB_HEAD_WIDTH,A.SLAB_TAIL_WIDTH,A.IN_STOCK_TIME,A.OUT_STOCK_TIME,A.SLAB_PLACE_CODE,A.STORE_AREA,A.SLAT_UNLADE_CAUSE,"
				" B.DELIVY_DATE,B.ORDER_TYPE_CODE,B.ORDER_STATUS"

				" FROM HMMSM01 A LEFT JOIN TOM01 B  on A.ORDER_NO = B.ORDER_NO where 1 = 1 ";
		}
		sqlcount += sql_where; sql += sql_where + sql_orderby;
		cm_sqlcount.SetCommandText(sqlcount);
		cm_sql.SetCommandText(sql);

#pragma region 设置cm_sqlcount和cm_sql的参数
		if (v_order_no.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_order_no", v_order_no.Trim());

			cm_sql.Parameters.Set("v_order_no", v_order_no.Trim());
		}
		if (v_whole_backlog_code1.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("m_whole_backlog_code1",
				m_whole_backlog_code1.Trim());
			cm_sqlcount.Parameters.Set("v_whole_backlog_code1",
				v_whole_backlog_code1.Trim());

			cm_sql.Parameters.Set("m_whole_backlog_code1",
				m_whole_backlog_code1.Trim());
			cm_sql.Parameters.Set("v_whole_backlog_code1",
				v_whole_backlog_code1.Trim());

		}
		if (v_whole_backlog_code2.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("m_whole_backlog_code2",
				m_whole_backlog_code2.Trim());
			cm_sqlcount.Parameters.Set("v_whole_backlog_code2",
				v_whole_backlog_code2.Trim());

			cm_sql.Parameters.Set("m_whole_backlog_code2",
				m_whole_backlog_code2.Trim());
			cm_sql.Parameters.Set("v_whole_backlog_code2",
				v_whole_backlog_code2.Trim());
		}
		if (v_whole_backlog_code3.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("m_whole_backlog_code3",
				m_whole_backlog_code3.Trim());
			cm_sqlcount.Parameters.Set("v_whole_backlog_code3",
				v_whole_backlog_code3.Trim());

			cm_sql.Parameters.Set("m_whole_backlog_code3",
				m_whole_backlog_code3.Trim());
			cm_sql.Parameters.Set("v_whole_backlog_code3",
				v_whole_backlog_code3.Trim());
		}
		Log::Info("", __FUNCTION__, "进入cm_sqlcount参数v_stock_no设置,v_stock_no=【{0}】",v_stock_no);
		if (v_stock_no.Trim().GetLength() > 0)
		{
			Log::Info("", __FUNCTION__, "进入参数v_stock_no设置0");
			if (v_stock_no.Trim().Find("%")>=0){
				cm_sqlcount.Parameters.Set("v_stock_no", v_stock_no);
				cm_sql.Parameters.Set("v_stock_no", v_stock_no);
				Log::Info("", __FUNCTION__, "v_stock_no=【{0}】", v_stock_no);
			}
			else{
				int rowcnt = bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count();
				for (int i = 0; i < rowcnt; i++)
				{
					para = i; 
					cm_sqlcount.Parameters.Set("v_stock_no" + para.ToString(), 
						bcls_rec->Tables["STOCK_NO_IN"].Rows[i]["STOCK_NO"].ToString());

					cm_sql.Parameters.Set("v_stock_no" + para.ToString(),
						bcls_rec->Tables["STOCK_NO_IN"].Rows[i]["STOCK_NO"].ToString());
					Log::Info("", __FUNCTION__, "v_stock_no" + para.ToString()+"=【{0}】", v_stock_no);
				}
			}
		}
		Log::Info("", __FUNCTION__, "进入cm_sqlcount参数v_dest_fin设置,v_dest_fin=【{0}】", v_dest_fin);
		if (v_dest_fin.Trim().GetLength() > 0){
			int rowcnt = bcls_rec->Tables["DEST"].Rows.get_Count();
			for (int i = 0; i < rowcnt; i++)
			{
				para = i;
				cm_sqlcount.Parameters.Set("v_dest_fin" + para.ToString(),
					bcls_rec->Tables["DEST"].Rows[i]["DEST_FIN"].ToString());

				cm_sql.Parameters.Set("v_dest_fin" + para.ToString(),
					bcls_rec->Tables["DEST"].Rows[i]["DEST_FIN"].ToString());
				Log::Info("", __FUNCTION__, "v_dest_fin" + para.ToString() + "=【{0}】",
					bcls_rec->Tables["DEST"].Rows[i]["DEST_FIN"].ToString());
			}
		}
		if (v_quality_grade.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_quality_grade", v_quality_grade);

			cm_sql.Parameters.Set("v_quality_grade", v_quality_grade);
		}
		/*if (v_mat_status.Trim().GetLength() > 0){

		}*/
		if (v_product_flag.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_product_flag", v_product_flag);

			cm_sql.Parameters.Set("v_product_flag", v_product_flag);
		}
		/*if (v_order_status.Trim().GetLength() > 0){

		}*/
		if (v_st_no.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_st_no", v_st_no);

			cm_sql.Parameters.Set("v_st_no", v_st_no);
		}
		if (v_ai_time_1_f.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_ai_time_1_f", v_ai_time_1_f);

			cm_sql.Parameters.Set("v_ai_time_1_f", v_ai_time_1_f);
		}
		if (v_ai_time_1_t.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_ai_time_1_t", v_ai_time_1_t);

			cm_sql.Parameters.Set("v_ai_time_1_t", v_ai_time_1_t);
		}
		if (v_store_area.Trim().GetLength() > 0){
			cm_sqlcount.Parameters.Set("v_store_area", v_store_area);

			cm_sql.Parameters.Set("v_store_area", v_store_area);
		}
#pragma endregion
		
		Log::Info("", __FUNCTION__, " 位置3,sqlcount=【{0}】",sqlcount);
	    CDecimal RecCount = cm_sqlcount.ExecuteScalar();
		cm_sqlcount.Close();	Log::Info("", __FUNCTION__, " 位置4");
		bcls_ret->Tables["MMSM01G2_INQ"].ExtendedProperties.Add("RECCOUNT", RecCount.ToString());
		Log::Info("", __FUNCTION__, "查询出的总记录数为【{0}】", RecCount.ToString());
		Log::Info("", __FUNCTION__, " 位置5 ,sql=【{0}】",sql);
		cm_sql.ExecuteQuery(bcls_ret->Tables[0]);
		cm_sql.Close();

		Log::Info("", __FUNCTION__, "sql_where=【{0}】", sql_where);
		para = 0;
		for (int row; row < bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count(); row++){
			CString logstr = "v_stock_no" + para.ToString(); para = para + 1;
			logstr += "=【{0}】";
			Log::Info("", __FUNCTION__, logstr, bcls_rec->Tables["STOCK_NO_IN"].Rows[row]["STOCK_NO"].ToString());
		}
		para = 0;
		for (int row; row < bcls_rec->Tables["DEST"].Rows.get_Count(); row++){
			CString logstr = "v_dest_fin" + para.ToString(); para = para + 1;
			logstr +="=【{0}】";
			Log::Info("", __FUNCTION__, logstr, bcls_rec->Tables["DEST"].Rows[row]["DEST_FIN"].ToString());
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
