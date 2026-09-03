/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 板坯切断实绩删除
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件


/***** C++ 的业务头文件部分 *****/ 




//连铸铸坯产出处理函数
int f_mmsm3301d_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


//外部函数声明

BM2_FUNCTION_EXPORT
 int f_mmsm33_delete(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_delete";                //定义函数英文名称  
	CString FunctionCname = "板坯切断_信息删除";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	  

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	
	CString c_heat_confm_flag = "";
	CString c_factory_div = ""; //厂别区分
	CString c_pono_status = "";

	CString sqlstr = "";
	

	CModel tmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);

	try
	{
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM33");
			bcls_rec->Tables["TMMSM33"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		
	

		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			//取得一行数据， MergeFrom方法将获得bcls_rec指定表的指定行的数据
			tmmsm33.Reset();
			tmmsm01.Reset();

			tmmsm33["MAT_NO"]  =  bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "MAT_NO	= [{0}]", tmmsm33.MAT_NO );

			if (tmmsm33["MAT_NO"].ToString() == "")
			{
				strcpy(s.sysmsg,_RES("GCRSS0000035")/*"材料号不能为空!"*/);
				throw CApplicationException(-1, s.msg, log.Location); 
			}

			//查询对应的产出实绩
			tmmsm33.Query("MAT_NO");
			tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
			if (!tmmsm01.Query())
			{
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString()};// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "材料号{0}仓库信息不存在", arguments, 1);//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//增加校验炉次确定不允许删除
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				sqlstr = CString(" SELECT PONO_STATUS FROM TPSSM41 "
					" WHERE PONO = @pono ");
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", tmmsm33["PONO"]);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				c_pono_status = cmd_inq.GetString(1).Trim();
			}
			
			cmd_inq.Close();

			if (c_pono_status == "91")
			{
				sprintf(s.msg, "炉次[%s]已做炉次确定，不再接收铸坯删除（切断）信息。", (const char*)tmmsm33["PONO"]);
							
				throw CApplicationException(-1, s.msg, log.Location); 
			}


			/* 材料是否在当前档 */
			if (tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料[%s]不在当前档!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			///* 校验逻辑合法性 */
			//if (tmmsm01["PLAN_NO"].ToString().Trim() != "")
			//{
			//	sprintf(s.msg, "材料号[%s]在作业计划[%s]中,不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["PLAN_NO"].ToString());
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			if (tmmsm01["CONFM_FLAG"].ToString().Trim() != "0")
			{
				sprintf(s.msg, "材料号[%s]材料状态[%s]是准发,不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["MAT_STATUS"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//Log::Trace("", __FUNCTION__, "CONFM_FLAG				= [%s]", (const char*)tmmsm01["CONFM_FLAG"].ToString());
			if (tmmsm01["TRANSFER_FLAG"].ToString().Trim() != "0")
			{
				sprintf(s.msg, "材料号[%s]在转库计划中,转库状态是[%s],不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["TRANSFER_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["APP_DECIDE_FLAG"].ToString().Trim() != "0")
			{
				sprintf(s.msg, "材料号[%s]在现货申报计划中,现货申报标记是[%s],不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["APP_DECIDE_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "0" && tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "") 
			{
				sprintf(s.msg, "材料号[%s]在板坯TOP点确认标记是[%s],不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["SLABTOP_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["MAT_KIND"].ToString().Trim() != "SM")//物料类型SM
			{
				sprintf(s.msg, "材料号[%s]物料类型是[%s],不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["MAT_KIND"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["TRANSFER_PLAN_NO"].ToString().Trim() != "")
			{
				strcpy(s.msg, "该材料已经编入转库计划，请先撤销计划");
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != "SM")
			{
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["MAT_LINE_TYPE"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "材料号{0}信息产线已变为{1}，已不在炼钢厂，可能已在轧钢厂确认，请联系轧钢原料人员进行退库", arguments, 2);//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "3")
			{

				if (tmmsm33["SLAB_THICK"].ToDecimal() != tmmsm01["MAT_THICK"].ToDecimal() || tmmsm33["SLAB_WIDTH"].ToDecimal() != tmmsm01["MAT_WIDTH"].ToDecimal() || tmmsm33["SLAB_LEN"].ToDecimal() != tmmsm01["MAT_LEN"].ToDecimal() || tmmsm33["SLAB_WT"].ToDecimal() != tmmsm01["MAT_ACT_WT"].ToDecimal() || tmmsm33["MAT_TUBE"].ToDecimal() != tmmsm01["MAT_NUM_CUT"].ToDecimal() || tmmsm01["MAT_NUM"].ToDecimal() != tmmsm01["MAT_NUM_CUT"].ToDecimal())
				{
					strcpy(s.msg, "材料主档规格信息已发生变化，不能删除");//格式化字符串
					strcpy(s.sysmsg, s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			
			}

			if (tmmsm01["SLABTOP_FLAG"].ToString() != "0" && tmmsm01["SLABTOP_FLAG"].ToString().Trim() != "")
			{
				strcpy(s.msg, "材料已经在直供出坯画面操作，确认状态已经发生变化，不能删除");
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm33.MergeTo(bcls_rec->Tables["TMMSM33"], false);
			bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"] = "D";   //处理区分：N-新增(包括相同材料分批新增); U-修改; D-删除;

			//----------------------------------------------
			tmmsm33.Delete("MAT_NO");
		}
		if (bcls_rec->Tables["TMMSM33"].Rows.get_Count()>0){
			//调用材料主档信息处理。
			ret = f_mmsm3301d_proc(bcls_rec, bcls_ret, conn);
			if (ret < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
