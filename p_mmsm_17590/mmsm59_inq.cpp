/*========================================================================*/
/*== [service名  ]:  mmsm59_inq       ||  [对应VC#画面 ]:  ALL           ==*/
/*== [程序编制人 ]:  向萍             ||  [程序定稿日期]:2016-2-4 14:00:05==*/
/*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
/*========================================================================*/
/*== [数据库表   ]： tmmsm2a                                            ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 原辅料按炉消耗信息查询                             ==*/
/*========================================================================*/
//框架头文件
#include "stdafx.h"




//业务头文件


//外部函数声明
//int f_mmsm_get_multi_value(CString v_in_str, CString v_spilit_flag, CString *v_out_str99, int *v_out_str_cnt, CDbConnection* conn);


BM2F_ENTERACE(mmsm59_inq)

int f_mmsm59_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int	TotalRecordCount = 0;
	int i = 0;
	int j = 0;
	int m = 0;
	int RetRecordCount = 0;
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";


	CString v_mat_code = ""; //原辅料代码
	CString v_mat_name = ""; //原辅料名称
	CString v_factory_div = "";


	int     fetchRowCount = 0;
	int     v_station_count = 0;
	int     v_mat_station_num = 0;
	CString v_mat_station_d = "";
	CString v_station_id = "";
	CString v_station_name = "";
	CString v_item_ename = "";
	CString v_item_enames = "";
	CString v_heat_no = "";

	CString v_mat_station[500] = { "" }; //物料代码+ 设备类型数组  100行
	CString v_station_id_s[500] = { "" }; //设备类型数组  100行

	CPageInfo pageInfo;

	/* 业务变量 */

	CModel tmmsm2a("TMMSM2A");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))	ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))	ch_start_time_t = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();


		//Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm2a["HEAT_NO"].ToString());
		//Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		//Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);

		ch_start_time_f = (ch_start_time_f + "000000").Substring(0, 14);
		ch_start_time_t = (ch_start_time_t + "235959").Substring(0, 14);
		//Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		//Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);

		//动态配置返回块的数据项，根据ED54配置字段
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select t.item_ename "
				" from  ted54 t "
				" where func_id = 'MMSM59_INQ' AND ITEM_ENAME <> 'HEAT_NO'"
				" order by t.seq_no  ";

			break;
		}


		cmd_inq.SetCommandText(sqlstr);// 设置执行的SQL语句  
		//cmd_inq.Parameters.Set("factory_div", v_factory_div);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			v_item_ename = cmd_inq.GetString(1).Trim();

			v_item_enames = v_item_enames + "'" + v_item_ename + "' " + v_item_ename + ",";
		}

		cmd_inq.Close();

		v_item_enames = "(" + v_item_enames.Substring(0, v_item_enames.GetLength() - 1) + ")";

		//Log::Trace(" ", __FUNCTION__, "v_item_enames =[{0}]", v_item_enames);

		sqlstr_temp = "SELECT HEAT_NO,MAT_CODE||'_'||STATION_ID MAT_CODE,DEVO_WT FROM TMMSM2A WHERE HEAT_NO <> ' '";

		if (tmmsm2a["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND HEAT_NO = @heat_no";
		}
		if (ch_start_time_f.Trim() != "")
		{
			sqlstr_temp += " AND DEVO_TIME	>= @ch_start_time_f";
		}
		if (ch_start_time_t.Trim() != "")
		{
			sqlstr_temp += " AND DEVO_TIME	<= @ch_start_time_t";
		}

		sqlstr = "SELECT * FROM (" + sqlstr_temp + ") PIVOT(sum(devo_wt) FOR MAT_CODE IN " + v_item_enames + ") ORDER BY HEAT_NO";
		//Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

		sqlstr_count = "SELECT COUNT(1) FROM (" + sqlstr_temp + ") PIVOT(sum(devo_wt) FOR MAT_CODE IN " + v_item_enames + ")";
		//Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);

		cmd_inq.Parameters.Set("heat_no", tmmsm2a["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
		cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();


		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


