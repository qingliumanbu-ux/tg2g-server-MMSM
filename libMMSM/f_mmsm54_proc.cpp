/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料计划增删改（转库计划、回收计划)
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 
 


//外部函数声明
//int f_ymsm52_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm54_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm54_proc";                //定义函数英文名称  
CString FunctionCname = "原辅料计划增删改";          //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
  int   doFlag = 0;
  int   fetchRowCount = 0;
  int   i = 0;
  int   blkNum;

  CString sqlstr="";
  CString v_proc_div= "";
  CString v_factory_div = "";
  CString v_pract_rcv_flag= "";
  CString v_mat_type = "";

  CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

     
  try
  {
		CPageInfo pageInfo;	

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm54("TMMSM54");
	CModel tmmsm56("TMMSM56");
	
       //初始化实体类
	
		tmmsm54.Reset();
		tmmsm56.Reset();
		
		/*判断是否存在指定块*/
		
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();

		if (bcls_rec->Tables["PARA"].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();

		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE"].ToString().Trim();
	
		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "tmmsm54["HANDLE_DIV"] =[{0}]", tmmsm54["HANDLE_DIV"].ToString()); //计划类型
	

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm54.Reset();
			tmmsm54.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm54["FACTORY_DIV"] = v_factory_div;
			tmmsm54.TrimOrBlank();

			if (tmmsm54["MAT_CODE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "物料代码不能为空!");
			}
		
			if (tmmsm54["PLAN_MAKE_TIME"].ToString().Trim() == "")
			{
				tmmsm54["PLAN_MAKE_TIME"] = dateNow;
			}

			//如果材料类型为空，勾连物料主数据中的材料类型。

			if (v_mat_type.Trim() == "" || tmmsm54["MAT_TYPE"].ToString().Trim().GetLength() > 1)
			{
				sqlstr = "SELECT  MAT_TYPE "
					" FROM    TMMSM50 "
					" WHERE   MAT_CODE = @mat_code";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("mat_code", tmmsm54["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm54["MAT_TYPE"] = cmd_sql.GetString(1);
				}
				cmd_sql.Close();

			}

			//Log::Trace("", __FUNCTION__, "tmmsm54["MAT_TYPE"] =[{0}]", tmmsm54["MAT_TYPE"].ToString());

			tmmsm54["PLAN_STATUS"] = '1';   //计划状态
			
		

			//加工出库队列数据
			tmmsm56.CopyFrom(tmmsm54);
			tmmsm56["OUT_STOCK_WT"] = tmmsm54["PLAN_WT"];
			tmmsm56["OUT_STOCK_NO"] = "C" + dateNow;
			tmmsm56["AFFIRM_FLAG"] = "0";

			if (tmmsm54["HANDLE_DIV"].ToString() == "C")//转库计划
			{
				tmmsm56["HANDLE_DIV"] = "6"; //转库出库
			}
			else if (tmmsm54["HANDLE_DIV"].ToString() == "B")//回收计划
			{
				tmmsm56["HANDLE_DIV"] = "4"; //回收出库
			}
							
			if (v_proc_div == "I")
			{
				tmmsm54["REC_CREATE_TIME"] = dateNow;
				tmmsm54["REC_CREATOR"] = s.userid;

				tmmsm56["REC_CREATE_TIME"] = dateNow;
				tmmsm56["REC_CREATOR"] = s.userid;

				//计划号生成
				if (tmmsm54["HANDLE_DIV"].ToString().Trim() == 'B')//回收计划
				{
					tmmsm54["PLAN_NO_Y"] = "H" + dateNow;
				}
				
				else if (tmmsm54["HANDLE_DIV"].ToString().Trim() == 'C')//转库计划
				{
					tmmsm54["PLAN_NO_Y"] = "Z" + dateNow;
				}
				tmmsm56["PLAN_NO_Y"] = tmmsm54["PLAN_NO_Y"]; 

				//Log::Trace("", __FUNCTION__, "tmmsm54.PLAN_NO1111=[{0}]", tmmsm54["PLAN_NO_Y"].ToString());
				tmmsm54.Print();
				tmmsm54.Insert();
				//Log::Trace("", __FUNCTION__, "tmmsm54.PLAN_NO222=[{0}]", tmmsm54["PLAN_NO_Y"].ToString());
				//tmmsm56.Insert();

				//Log::Trace("", __FUNCTION__, "tmmsm54.PLAN_NO333=[{0}]", tmmsm54["PLAN_NO_Y"].ToString());

         /*       #pragma region 判断是否请车，组织叫车委托信息 modify by ShiYong @20160519
				if (tmmsm54["IF_VEHICLE"].ToString().Trim() == "1")
				{
					if (!bcls_rec->Tables.Contains("YMSM52"))
					{
						bcls_rec->Tables.Add("YMSM52");
						bcls_rec->Tables["YMSM52"].Columns.Add(DT_STRING, "PROC_DIV");
						bcls_rec->Tables["YMSM52"].Columns.Add(DT_STRING, "MAT_KIND");
						bcls_rec->Tables["YMSM52"].Columns.Add(DT_STRING, "PLAN_NO_Y");
						bcls_rec->Tables["YMSM52"].Columns.Add(DT_STRING, "MAT_CODE");
					}
					CDataRow & dr = bcls_rec->Tables["YMSM52"].Rows.Add();
					dr["PROC_DIV"] = "SND";
					if (tmmsm54["MAT_TYPE"].ToString() == "1" || tmmsm54["MAT_TYPE"].ToString() == "3")
					{
						dr["MAT_KIND"] = "YF";
					}
					else
					{
						dr["MAT_KIND"] = "FG";
					}
					dr["PLAN_NO_Y"] = tmmsm54["PLAN_NO_Y"];
					dr["MAT_CODE"] = tmmsm54["MAT_CODE"];
				}
                #pragma endregion*/
		
			} 
			else
			{
				//Log::Trace("", __FUNCTION__, "tmmsm54["PLAN_STATUS"] =[{0}]", tmmsm54["PLAN_STATUS"].ToString());

				//根据计划号返回出库信息是否出库确认
				sqlstr = " SELECT AFFIRM_FLAG,OUT_STOCK_NO"
					"	   FROM	TMMSM56 "
					"      WHERE  PLAN_NO_Y = @plan_no_y";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("plan_no_y", tmmsm54["PLAN_NO_Y"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm56["AFFIRM_FLAG"] = cmd_sql.GetString(1);
					tmmsm56["OUT_STOCK_NO"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				//已经出库确认的不允许修改或者删除
				if (tmmsm56["AFFIRM_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, "该出库单号已经出库确认，不允许修改或者删除转库计划信息!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U")//修改
				{
					
					//取原记录的创建时间和创建人
					sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR"
						"	   FROM	TMMSM54 "
						"      WHERE  PLAN_NO_Y = @plan_no_y";

					//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Clear();
					cmd_sql.Parameters.Set("plan_no_y", tmmsm54["PLAN_NO_Y"].ToString());
					cmd_sql.ExecuteReader();

					if (cmd_sql.Read())
					{
						tmmsm54["REC_CREATE_TIME"] = cmd_sql.GetString(1);
						tmmsm54["REC_CREATOR"] = cmd_sql.GetString(2);
					}
					cmd_sql.Close();

					tmmsm54["REC_REVISE_TIME"] = dateNow;
					tmmsm54["REC_REVISOR"] = s.userid;
					tmmsm56["REC_REVISE_TIME"] = dateNow;
					tmmsm56["REC_REVISOR"] = s.userid;

					tmmsm54.Delete("PLAN_NO_Y");
					tmmsm54.Insert();

					tmmsm56.Delete("PLAN_NO_Y");
					tmmsm56.Insert();


				}
				else if (v_proc_div == "D")//删除
				{
					tmmsm54.Delete("PLAN_NO_Y");
					tmmsm56.Delete("PLAN_NO_Y");
				}

			}

		}

	/*	#pragma region 判断是否有叫车委托信息发送叫车委托  by ShiYong @20160519
		if (bcls_rec->Tables.Contains("YMSM52") && bcls_rec->Tables["YMSM52"].Rows.get_Count() > 0)
		{
			doFlag = f_ymsm52_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion*/


	
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

