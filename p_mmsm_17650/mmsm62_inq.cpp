///*========================================================================*/
///*== [service名  ]:  mmsm62_inq       ||  [对应VC#画面 ]:  ALL           ==*/
///*== [程序编制人 ]:  向萍             ||  [程序定稿日期]:2016-2-4 14:00:05==*/
///*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
///*========================================================================*/
///*== [数据库表   ]： tmmsm10等                                          ==*/
///*== [调用函数   ]： 无				                                    ==*/
///*== [service功能]： 生产实绩班报查询                                  ==*/
///*========================================================================*/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




/*<remark>=========================================================
///<summary>
///炼钢生产班报表查询
///<para>炼钢生产班报表查询</para>
///<para>数据库表TMMSM62(炼钢生产计划与实绩班报)</para>
///<para>主调函数:前台mmsm62画面F2(查询)按钮</para>
///</summary>
///<returns></returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm62_inq)

int f_mmsm62_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int  doFlag = 0;							/*返回值*/
	int blkNum = 0;
	int i = 0;

	CString v_factory_div = "";
	CString v_prod_date = ""; //生产日期
	CString v_prod_time_from = ""; //查询生产时刻条件
	CString v_prod_time_to = "";
	CString v_prod_shift_group = ""; //班组
	CString v_dev_code_sum_e = "";  //各工序炉数英文名
	int     v_dev_code_num = 0;   //工序总数
	int     fetchRowCount=0;
	int		TotalRecordCount = 0;

	CString v_dev_code[100] = {""}; //设备代码数组  100行
	int     v_dev_code_sum[100] = { 0 }; //设备代码对应的总炉数
	
	// 定义表的实体对象
	CModel tmmsm62("TMMSM62");
	CModel tmmsm10("TMMSM10");

	CString sqlstr = "";
	CString sqlstr_temp = "";
	CDbCommand cmd_inq(conn);


	try
	{
		//-------------------------------------------------------
		//将设备代码放入数组，以提高效率
		sqlstr = CString("SELECT T.DEV_CODE "
			"  FROM  TPSSMD1 T "
			"  WHERE T.FACTORY_DIV = '1' "
			"  ORDER BY T.AREA_ID ,T.STATION_NO ");

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			v_dev_code[i] = cmd_inq.GetString(1);
			
			v_dev_code_num ++;    //该厂别的设备个数
			i++;
			
		}
		cmd_inq.Close();

	
		//定义返回参数
		//1.炉数以及生产综述信息
		
		blkNum = bcls_ret->Tables.IndexOf("TMMSM62");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("TMMSM62");

			for (i = 0; i < v_dev_code_num; i++)
			{
				v_dev_code_sum_e = v_dev_code[i] + "_SUM"; //每个设备的炉次总数(即为前台ED54功能号对应的英文名)

				bcls_ret->Tables["TMMSM62"].Columns.Add(DT_STRING, v_dev_code_sum_e);
			}
		}
	

		//2.炉次信息
		blkNum = bcls_ret->Tables.IndexOf("TMMSM10");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("TMMSM10");
		}


		////--------------------------------------------------------------
		//获得输入参数
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_DATE"))
			v_prod_date = bcls_rec->Tables[0].Rows[0]["PROD_DATE"].ToString().Trim();  //生产日期
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			v_prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();  //班组

		if (v_prod_date.GetLength() != 8) //一定要年月日
		{
			sprintf(s.msg, "生产日期不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//Log::Trace(" ", __FUNCTION__, "prod_date =[{0}]",v_prod_date);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * FROM TMMSM62 "
				"  WHERE PROD_DATE = @prod_date ";

			if (v_factory_div.Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_DIV = @factory_div";
			}

			if (v_prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @prod_shift_group";
			}

			sqlstr_temp += " ORDER BY prod_shift_group ASC";

			sqlstr = sqlstr + sqlstr_temp;

			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", v_factory_div);
		cmd_inq.Parameters.Set("prod_date", v_prod_date);
		cmd_inq.Parameters.Set("prod_shift_group", v_prod_shift_group);
		cmd_inq.ExecuteReader();

		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm62);
			tmmsm62["PROD_OVERVIEW"] = tmmsm62["PROD_OVERVIEW"].ToString() + "\n\r";  //生产综述
		}
		cmd_inq.Close();


		//Log::Trace(" ", __FUNCTION__, "tmmsm62["PROD_OVERVIEW"] =[{0}]", tmmsm62["PROD_OVERVIEW"].ToString());


		v_prod_time_from = v_prod_date + "000000";
		v_prod_time_to = v_prod_date + "235959";


		//Log::Trace("", __FUNCTION__, "from=[{0}], to=[{1}]", v_prod_time_from, v_prod_time_to);

		sqlstr = "";
		sqlstr_temp = "";


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * FROM TMMSM10 "
				" WHERE PROD_TIME >= @prod_time_from"
				" AND   PROD_TIME <= @prod_time_to";

			if (v_factory_div.Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_DIV = @factory_div";
			}

			if (v_prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @prod_shift_group";
			}


			sqlstr_temp += " ORDER BY CAST_NO ASC, CAST_DIV_NO ASC";

			sqlstr = sqlstr + sqlstr_temp;

			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", v_factory_div);
		cmd_inq.Parameters.Set("prod_time_from", v_prod_time_from);
		cmd_inq.Parameters.Set("prod_time_to", v_prod_time_to);
		cmd_inq.Parameters.Set("prod_shift_group", v_prod_shift_group);
		cmd_inq.ExecuteReader();

		
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm10);
			tmmsm10.MergeTo(bcls_ret->Tables["TMMSM10"], false);

			//统计各工序炉次数量

			for (i = 0; i < v_dev_code_num; i++)
			{
				/*//Log::Trace(" ", __FUNCTION__, "v_dev_code[i] =[{0}]", v_dev_code[i]);*/

				if (tmmsm10["SMELT_DEV_CODE"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				if (tmmsm10["SR1_DEV_CODE"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				if (tmmsm10["SR2_DEV_CODE"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				if (tmmsm10["SR3_DEV_CODE"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				if (tmmsm10["SR4_DEV_CODE"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				if (tmmsm10["CC_MACH_NO"].ToString() == v_dev_code[i])
				{
					v_dev_code_sum[i] ++;
				}
				
			/*	//Log::Trace(" ", __FUNCTION__, "i =[{0}]", i);
				//Log::Trace(" ", __FUNCTION__, "v_dev_code_sum[i] =[{0}]", v_dev_code_sum[i]);*/
			}

		}
		cmd_inq.Close();

				

		//班报汇总返回
		tmmsm62.MergeTo(bcls_ret->Tables["TMMSM62"], false);

		for (i = 0; i < v_dev_code_num; i++)
		{
			v_dev_code_sum_e = v_dev_code[i] + "_SUM"; //每个设备的炉次总数

			if (bcls_ret->Tables["TMMSM62"].Columns.Contains(v_dev_code_sum_e))
			{
				bcls_ret->Tables["TMMSM62"].Rows[0][v_dev_code_sum_e] = v_dev_code_sum[i];
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

