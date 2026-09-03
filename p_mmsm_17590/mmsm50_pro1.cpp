/*========================================================================*/
/*== [service名  ]:  mmsm50_pro1      ||  [对应VC#画面 ]:  ALL           ==*/
/*== [程序编制人 ]:  向萍             ||  [程序定稿日期]:2016-2-4 14:00:05==*/
/*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
/*========================================================================*/
/*== [数据库表   ]： tmmsm50                                            ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 原辅料按炉消耗信息查询画面需要显示的原辅料字段     ==*/
/*========================================================================*/


/******框架头******/
#include "stdafx.h"


/******业务头******/ 





//从字符串中根据指定分隔符拆分数据
// 入口字符，分隔字符，函数是返回字符信息。
int f_mmsm_get_multi_value(CString v_in_str, CString v_spilit_flag, CString *v_out_str99, int *v_out_str_cnt, CDbConnection* conn);

/******service入口******/
BM2F_ENTERACE(mmsm50_pro1)

int f_mmsm50_pro1(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm50_pro1";                //定义函数英文名称  
	CString FunctionCname = "原辅料按炉消耗信息";   //定义函数中文名称


	/*定义程序用变量*/
	int fetchRowCount = 0;
	int doFlag = 0;
	int i = 0;
	int j = 0;
	int m = 0;
	CString  function_id = "MMSM55_INQ" ;/*自定义显示项目号*/
	CString v_userid = s.userid;

	/*定义业务用变量*/
	CString v_prod_dif = "";
	 
 	CString v_mat_code = ""; //原辅料代码
	CString v_mat_name = ""; //原辅料名称
	CString v_factory_div = "";

	CString v_column_name = "";
	CString v_column_cname = "";

	CString sqlstr = "";
	CString sqlwhere = "";
	int     v_total_count  = 0;
	int     v_station_count = 0;
	CString v_mat_station_d = "";
	CString v_station_id = "";
	CString v_station_name = "";

	CString v_station_id_s[100] = { "" }; //设备类型数组  100行
	


	/****** 业务处理开始 ******/
	try
	{	
		 
		/*实体类定义*/

	CModel tmmsm50("TMMSM50");
	CModel tmmsm50a("TMMSM50A");

		/* 数据库操作类定义 */
		CDbCommand cmd_inq(conn); //与DB 建立连接。

		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition = " SELECT  t.* FROM tmmsm50 t  where t.factory_div = @factory_div";
		CString  c_sql_orderBY = "   ";

	
		/* 数据库操作类定义2 */
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CString  c_sql_where2 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition2 = "  SELECT  t.* FROM tmmsm50 t  where t.factory_div = @factory_div";



		/* 数据库操作类定义3 */
		CDbCommand cmd_sql3(conn); //与DB 建立连接。
		CString  c_sql_where3 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition3 = " SELECT  t.* FROM tmmsm50 t  where t.factory_div = @factory_div";

		/* 数据库操作类定义3 */
		CDbCommand cmd_sql4(conn); //与DB 建立连接。
		CString  c_sql_where4 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition4 = " SELECT  t.* FROM tmmsm50 t  where t.factory_div = @factory_div";


		//测试阶段，控制可操作人员。
		/*if (v_userid.Trim() != "admin")
		{
			sprintf(s.msg,"您的帐号[%s]，暂不支持当前操作。",(const char*)v_userid);
			throw CApplicationException(-1, s.msg, FunctionEname);
		}*/
 
		//将TPSSMD1表中的设备代码，追加到功能号 = MMSM62_INQ中去。
		//==================================================
		CDecimal v_cnt = 0;
		CString v_func_id = "MMSM55_INQ"; 
		
		CString v_class_code = "";
		CDecimal v_seq_no = 0;
		CString v_item_ename = "";
		CString v_item_cname = "";
		CString v_item_must_flag = "";
		CString v_item_type = "";
		CString v_item_len = "";
		CString v_culture = "";


		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		//Log::Trace(" ", __FUNCTION__, "bcls_rec->Tables[0].Rows.get_Count() =[{0}]", bcls_rec->Tables[0].Rows.get_Count());

		//如果前台没有传入
		if (bcls_rec->Tables[0].Rows.get_Count()==0)
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TMMSM50A ";
				
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				//Log::Trace(" ", __FUNCTION__, "tmmsm50a["MAT_CODE"] =[{0}]", tmmsm50a["MAT_CODE"].ToString());

				sprintf(s.msg, "请先删除所有的显示工序信息!"); //系统错误信息
				throw CApplicationException(-1, s.msg, log.Location);

			}
			cmd_inq.Close();

			
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TMMSM50 "
					" WHERE  MAT_STATION_D <>' '";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm50);
				//Log::Trace(" ", __FUNCTION__, "tmmsm50["MAT_CODE"] =[{0}]", tmmsm50["MAT_CODE"].ToString());

				tmmsm50.MergeTo(bcls_rec->Tables[0], false);

			}
			cmd_inq.Close();

		}
		
		
		 for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		 {
		
			 v_mat_code = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().Trim();
			 v_mat_name = bcls_rec->Tables[0].Rows[i]["MAT_NAME"].ToString().Trim();
			 v_mat_station_d = bcls_rec->Tables[0].Rows[i]["MAT_STATION_D"].ToString().Trim();

			 //Log::Trace(" ", __FUNCTION__, "v_mat_code =[{0}]", v_mat_code);
			 //Log::Trace(" ", __FUNCTION__, "v_mat_name =[{0}]", v_mat_name);
			 //Log::Trace(" ", __FUNCTION__, "v_mat_station_d =[{0}]", v_mat_station_d);


			 c_sql_condition = "update tmmsm50 "
				 " set mat_station_d = @mat_station_d"
				 " where mat_code = @mat_code"
				 ;

			 //Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);
			 sqlstr = c_sql_condition;
			 cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句  
			 cmd_sql.Parameters.Set("mat_code", v_mat_code); //功能号。
			 cmd_sql.Parameters.Set("mat_station_d", v_mat_station_d);

			 cmd_sql.ExecuteNonQuery();
			 cmd_sql.Close();

			 //先将TMMSM50A中该物料代码的
			 c_sql_condition = "delete tmmsm50a "
				 " where mat_code = @mat_code"
				 ;

			 //Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);
			 sqlstr = c_sql_condition;
			 cmd_sql3.SetCommandText(c_sql_condition);// 设置执行的SQL语句  
			 cmd_sql3.Parameters.Set("mat_code", v_mat_code); //功能号。
			 cmd_sql3.ExecuteNonQuery();
			 cmd_sql3.Close();



			 f_mmsm_get_multi_value(v_mat_station_d, ",", v_station_id_s, &v_station_count, conn);

			 //Log::Trace(" ", __FUNCTION__, "v_station_count =[{0}]", v_station_count);

			 for (m = 0; m <= v_station_count ; m++)
			 {
				 //Log::Trace(" ", __FUNCTION__, "m =[{0}]", m);
				 v_station_id = v_station_id_s[m].Trim();

				 //Log::Trace(" ", __FUNCTION__, "v_station_id =[{0}]", v_station_id);

				 switch (conn->DatabaseKind)
				 {
				 case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				 case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				 case DB_KIND_MSSQL:	        // MS SQL Server数据库
				 case DB_KIND_ORACLE:	    // Oracle 数据库
				 default:
					 sqlstr = "SELECT CODE_DESC_1_CONTENT "
						 "  FROM TEP0002 "
						 " WHERE code_class = 'PSD1'"
						 " AND   code = @station_id";
					 break;
				 }
				 cmd_sql4.SetCommandText(sqlstr);
				 cmd_sql4.Parameters.Clear();
				 cmd_sql4.Parameters.Set("station_id", v_station_id);
				 cmd_sql4.ExecuteReader();
				 if (cmd_sql4.Read())
				 {
					 v_station_name = cmd_sql4.GetString(1);
				 }
				 cmd_sql4.Close();

				 //Log::Trace(" ", __FUNCTION__, "v_station_name =[{0}]", v_station_name);


				
				 //新增表TMMSM50
				 //=============
				 c_sql_condition2 = "insert into tmmsm50a "
					 "(mat_code, mat_name "
					 ", station_id, station_name,seq_no,view_flag) "
					 " values(@mat_code, @mat_name "
					 ", @station_id, @station_name "
					 ", @seq_no,'0') "

					 ;

				 //Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);
				 sqlstr = c_sql_condition2;
				 cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				 cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。
				 cmd_sql2.Parameters.Set("class_code", v_class_code);
				 cmd_sql2.Parameters.Set("seq_no", v_seq_no);

				 cmd_sql2.Parameters.Set("mat_code", v_mat_code);
				 cmd_sql2.Parameters.Set("mat_name", v_mat_name);
				 cmd_sql2.Parameters.Set("station_id", v_station_id);

				 cmd_sql2.Parameters.Set("station_name", v_station_name);
				 cmd_sql2.Parameters.Set("seq_no", v_seq_no);
				 cmd_sql2.ExecuteNonQuery();
				 cmd_sql2.Close();



			 }

		 }


		/*cmd_sql.Close();*/
	

		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  




	}
	/*捕获数据库操作异常*/
	catch(CDbException& ex)  //捕获数据库操作异常
	{  
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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



	/// <summary>
	/// 返回总记录数
	/// </summary>     
	bcls_ret->Tables.Add("PageInfo");
	bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
	bcls_ret->Tables["PageInfo"].Rows.Add();
	bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = v_total_count;
	 
	 
	////EDLog(1, 1, " **************%s end*****************", (const char*)FunctionEname);

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}
