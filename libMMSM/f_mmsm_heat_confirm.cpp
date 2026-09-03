/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2013-05-24
Description: 炉次确定时调用
**************************************************/
//框架头文件
#include "stdafx.h" 



//业务头文件
  /*炼钢物料主表*/


#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数

#endif


//外部函数声明 

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif


int f_mmsm_heat_confirm(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int i = 0;

	/* 业务变量 */
	
	/* 实体类定义 */
	
	CModel tmmsm01("TMMSM01");

	#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
	CModel tpmof03("TPMOF03");
	#endif


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_sql(conn);

	try
	{
		
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}

		bcls_rec->Tables["PMOF99"].Rows.Clear();

	
		int v_total_row = bcls_rec->Tables["MMSMCONFM"].Rows.get_Count(); //总记录数。 

		if (v_total_row <= 0)
		{
			sprintf(s.msg, "没有可操作的记录信息[MMSMCONFM]。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (i = 0; i < v_total_row; i++)
		{
			tmmsm01["PONO"] = bcls_rec->Tables["MMSMCONFM"].Rows[i]["PONO"].ToString().Trim();	//炉次确定PONO号


			#if defined _SYS_MMS || defined _SYS_MES

			sqlstr = " SELECT ORDER_NO, PONO, WHOLE_BACKLOG_NO, WHOLE_BACKLOG, WHOLE_BACKLOG_SEQ, WHOLE_BACKLOG_CODE"
				" FROM TMMSM01 WHERE PONO = @pono"
				" GROUP BY ORDER_NO,PONO,WHOLE_BACKLOG_NO,WHOLE_BACKLOG,WHOLE_BACKLOG_SEQ,WHOLE_BACKLOG_CODE ";

			//Log::Trace("", "", "sqlstr = {0}", sqlstr);
			//Log::Trace("", "", "tmmsm01[PONO] = {0}", tmmsm01["PONO"].ToString());

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("pono", tmmsm01["PONO"].ToString());
			cmd_sql.ExecuteReader();
			while (cmd_sql.Read())
			{


				tpmof03["EVENT_ID"] = "52ED";
				tpmof03["SYSTEM_ID"] = "MMSM";
				tpmof03["FUNC_ID"] = CString(s.svc_name);

				tpmof03["ORDER_NO"] = cmd_sql.GetString(1);
				tpmof03["WHOLE_BACKLOG_NO"] = cmd_sql.GetDecimal(3);
				tpmof03["WHOLE_BACKLOG"] = cmd_sql.GetString(4);
				tpmof03["WHOLE_BACKLOG_SEQ"] = cmd_sql.GetDecimal(5);
				tpmof03["WHOLE_BACKLOG_CODE"] = cmd_sql.GetString(6);
				tpmof03["CUST_MAT_NO"] = tmmsm01["PONO"];

				//Log::Trace("", "", "ORDER_NO"] = {0}", tpmof03["ORDER_NO"].ToString());
				//Log::Trace("", "", "WHOLE_BACKLOG_NO"] = {0}", tpmof03["WHOLE_BACKLOG_NO"].ToDecimal());
				//Log::Trace("", "", "WHOLE_BACKLOG"] = {0}", tpmof03["WHOLE_BACKLOG"].ToString());
				//Log::Trace("", "", "WHOLE_BACKLOG_SEQ"] = {0}", tpmof03["WHOLE_BACKLOG_SEQ"].ToDecimal());
				//Log::Trace("", "", "WHOLE_BACKLOG_CODE"] = {0}", tpmof03["WHOLE_BACKLOG_CODE"].ToString());
				//Log::Trace("", "", "CUST_MAT_NO"] = {0}", tpmof03["CUST_MAT_NO"].ToString());

				tpmof03.TrimOrBlank();

				if (tpmof03["ORDER_NO"].ToString().Trim() != "")
				{
					tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);
				}

			}
			cmd_sql.Close();

			if (bcls_rec->Tables["PMOF99"].Rows.get_Count()>0)
			{
				doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}


			#endif  

		}

		//处理成功。 
		strcpy(s.msg, "恭喜，处理成功。");
    


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
	//cmd_sql.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
