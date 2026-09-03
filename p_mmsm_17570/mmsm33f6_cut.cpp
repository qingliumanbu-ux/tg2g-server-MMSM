/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-05-24
Description: 板坯切断完毕
**************************************************/
//框架头文件
#include "stdafx.h" 
 

//业务头文件


//外部函数声明

#if  defined _SYS_MES   || defined _SYS_PES
int f_pssm11_cut_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#if defined(_WMS_DEPENDENT_SM) 
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用仓库接口，生成板坯入库队列
#endif


BM2F_ENTERACE(mmsm33f6_cut)

int f_mmsm33f6_cut(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString v_factory_div = "";
	CString v_heat_no = "";
	CString v_cut_fin_flag = "";
	CString v_manage_flag = "";
	CString v_mat_no = "";
	CString v_unit_code = "";
	CDecimal v_mat_num = 0;
	int  blkNum = 0;

	/* 实体类定义 */
	CModel tmmsm33("TMMSM33");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{
		/* 判断是否存在指定块*/

#if defined(_WMS_DEPENDENT_SM) 
		/*调用仓库接口（PES）  f_ym00sm_queue函数用*/
		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
		}
#endif

		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("CUT_FIN_FLAG"))
			v_cut_fin_flag = bcls_rec->Tables[0].Rows[0]["CUT_FIN_FLAG"].ToString().TrimOrBlank().ToUpper();

		tmmsm33["HEAT_NO"] = v_heat_no;

		Log::Trace("", "", "tmmsm33[\"HEAT_NO\"] ={0} v_cut_fin_flag=[{1}] v_factory_div =[{2}]", tmmsm33["HEAT_NO"].ToString(), v_cut_fin_flag, v_factory_div);

		/*if (tmmsm33.QueryCount("HEAT_NO")<=0)
		{
			sprintf(s.msg, "该炉号[%s]没有切断实绩!", (const char*)tmmsm33["HEAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

#if  defined _SYS_MES   || defined _SYS_PES

		if (!bcls_rec->Tables.Contains("PSSM11"))
		{
			bcls_rec->Tables.Add("PSSM11");
		}
		if (!bcls_rec->Tables["PSSM11"].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING, "FACTORY_DIV");
		}
		if (!bcls_rec->Tables["PSSM11"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["PSSM11"].Columns.Contains("CUT_FIN_FLAG"))
		{
			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING, "CUT_FIN_FLAG");
		}
 
		bcls_rec->Tables["PSSM11"].Rows.Add();
		bcls_rec->Tables["PSSM11"].Rows[0]["FACTORY_DIV"] = v_factory_div;
		bcls_rec->Tables["PSSM11"].Rows[0]["HEAT_NO"] = v_heat_no;
		bcls_rec->Tables["PSSM11"].Rows[0]["CUT_FIN_FLAG"] = v_cut_fin_flag;

		//Log::Trace("", "", "v_factory_div={0},v_heat_no={1},v_cut_fin_flag={2}", v_factory_div, v_heat_no, v_cut_fin_flag);

		doFlag = f_pssm11_cut_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif 



		sqlstr = "SELECT MANAGE_FLAG"
			"  FROM TMMSM33 "
			"  WHERE heat_no = @heat_no";

		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("heat_no", v_heat_no);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			v_manage_flag = cmd_sql.GetString(1);

		}
		cmd_sql.Close();

		Log::Trace("", "", "v_manage_flag={0}", v_manage_flag);


		if (v_manage_flag == "1") //按批管理
		{
			sqlstr = "SELECT  MAT_NO,MAT_NUM,UNIT_CODE"
				"  FROM TMMSM01 "
				"  WHERE heat_no = @heat_no";


			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("heat_no", v_heat_no);
			cmd_sql.ExecuteReader();
			while (cmd_sql.Read())
			{
				v_mat_no = cmd_sql.GetString(1);
				v_mat_num = cmd_sql.GetDecimal(2);
				v_unit_code = cmd_sql.GetString(3);

#pragma region 调用仓库接口，生成入库队列
#if defined(_WMS_DEPENDENT_SM) 

				if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() <= 0)
				{
					bcls_rec->Tables["WM00QUE"].Rows.Add();
				}
				bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1B";
				bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = v_mat_no;
				bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = v_mat_num;
				bcls_rec->Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = v_unit_code;
				if (v_cut_fin_flag == "1")
				{
					bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
				}
				else
				{
					bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "D";
				}

				doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);//2022-08-12 去头文件时编译报错 暂时注销
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

#endif   //PES函数的调用
#pragma endregion

			}
			cmd_sql.Close();


		}


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


