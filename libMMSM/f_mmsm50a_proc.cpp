/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料工序表新增、删除
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  
  



//外部函数声明
int f_mmsm_get_multi_value(CString v_in_str, CString v_spilit_flag, CString *v_out_str99, int *v_out_str_cnt, CDbConnection* conn);


BM2_FUNCTION_EXPORT
int f_mmsm50a_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm50a_pro";                //定义函数英文名称  
CString FunctionCname = "TMMSM50A新增";              //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   m = 0;
	int   n_count = 0;
	int   blkNum;

	CString sqlstr="";
	CString v_station_id_s[100] = { "" }; //设备类型数组  100行
	int     v_station_count = 0;

	CString v_station_id = "";
	CString v_station_name = "";
	CString v_class_code = "";
	CString v_proc_div = "";
	
	int   v_seq_no = 0;

	   
  
	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CDbCommand cmd_sql4(conn); //与DB 建立连接。

		CString  c_sql_condition2 = "";

		/* 实体类定义 */
	CModel tmmsm50("TMMSM50");
	CModel tmmsm50a("TMMSM50A");

		//初始化实体类
		tmmsm50.Reset();
		tmmsm50a.Reset();

		/* 获取输入参数*/

		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		//Log::Trace("", __FUNCTION__, "v_proc_div[{0}] ", v_proc_div);

		for (int i = 0; i < bcls_rec->Tables["MMSM50A"].Rows.get_Count(); i++)
		{
			tmmsm50.Reset();
			tmmsm50.MergeFrom(bcls_rec->Tables["MMSM50A"].Rows[i]);
			tmmsm50.TrimOrBlank();
		
			//Log::Trace("", __FUNCTION__, "MAT_CODE[{0}] ", tmmsm50["MAT_CODE"].ToString());
			//Log::Trace("", __FUNCTION__, "MAT_STATION_D[{0}] ", tmmsm50["MAT_STATION_D"].ToString());

			if (tmmsm50["MAT_CODE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "物料代码不能为空"); //系统错误信息
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (v_proc_div == "I")
			{

				f_mmsm_get_multi_value(tmmsm50["MAT_STATION_D"].ToString(), ",", v_station_id_s, &v_station_count, conn);

				//Log::Trace(" ", __FUNCTION__, "v_station_count =[{0}]", v_station_count);

				for (m = 0; m <= v_station_count; m++)
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
				
					cmd_sql2.Parameters.Set("mat_code", tmmsm50["MAT_CODE"].ToString());
					cmd_sql2.Parameters.Set("mat_name", tmmsm50["MAT_NAME"].ToString());
					cmd_sql2.Parameters.Set("station_id", v_station_id);
					cmd_sql2.Parameters.Set("station_name", v_station_name);
					cmd_sql2.Parameters.Set("seq_no", v_seq_no);

					cmd_sql2.ExecuteNonQuery();
					cmd_sql2.Close();
				}


			}

			else if (v_proc_div == "D")
			{
				/*tmmsm50a.Delete(tmmsm50["MAT_CODE"].ToString());*/
				//先将TMMSM50A中该物料代码的
				c_sql_condition2 = "delete tmmsm50a "
					" where mat_code = @mat_code"
					;

				//Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition2);
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("mat_code", tmmsm50["MAT_CODE"].ToString()); //功能号。
				cmd_sql2.ExecuteNonQuery();
				cmd_sql2.Close();

			}

		}
		
	
		
		/*设置系统返回参数*/
		strcpy(s.msg,  _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

} 

