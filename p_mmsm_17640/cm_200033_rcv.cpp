/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-7-13
Version:1.0
Description: 接收PES发送的铸坯切断实绩信息
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include "epex.h"

/***** C++ 的业务头文件部分 *****/ 


//#include "tmmsm01.h"
 
/* ***** 静态函数申明 ***** */

int f_mmsm3301n_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_mmsm3301d_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_mmsm3301u_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 接收PES发送的铸坯切断实绩信息
/// <para>
/// 接收PES发送的铸坯切断实绩信息并处理
/// </para>
/// </summary>
/// <param name="tmmsm33">处理的实绩信息</param>
/// <param name="MAT_TUBE">支数</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE_TELE(cm_200033_rcv)

int f_cm_200033_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	
	CTracer log(__FUNCTION__);
	
	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	CString sqlstr = "";
 CString   v_proc_div = "";    //处理区分：N-新增; U-修改; D-删除; I-累计新增（相同材料分批新增）

	CModel tmmsm33("TMMSM33");

 CDbCommand cmd_inq(conn);

	try
	{

	
		//调用材料主档信息处理。

		/* 设置块名 */
		/* 设置块名 */
		
		tmmsm33.Reset();
		tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm33.TrimOrBlank();
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim(); //处理区分：N-新增(包括相同材料分批新增); U-修改; D-删除;
		tmmsm33["PRACT_COLL_MODE"] = "1";

		////Log::Info("", __FUNCTION__, "传入参数,v_proc_div=[{0}]", v_proc_div);
		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["MAT_NO"] =[{0}]", tmmsm33["MAT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["SLAB_CUT_SEQ"] =[{0}]", tmmsm33["SLAB_CUT_SEQ"].ToDecimal());

		if (v_proc_div == "D")  //D-删除
		{
	
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr = "SELECT * "
						"  FROM TMMSM33 "
						" WHERE MAT_NO	= @tmmsm33.MAT_NO"
						" AND   SLAB_CUT_SEQ = @tmmsm33.SLAB_CUT_SEQ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmmsm33.MAT_NO", tmmsm33["MAT_NO"].ToString());
				cmd_inq.Parameters.Set("tmmsm33.SLAB_CUT_SEQ", tmmsm33["SLAB_CUT_SEQ"].ToDecimal());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm33);
				}
				cmd_inq.Close();

				tmmsm33.Delete("MAT_NO");

		}
		else if (v_proc_div == "N")
		{
			tmmsm33.Insert();
		}

		tmmsm33.Print();

		if(!bcls_rec->Tables.Contains("TMMSM33"))
		{
			bcls_rec->Tables.Add("TMMSM33");
		}
		if(!bcls_rec->Tables["TMMSM33"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["TMMSM33"].Columns.Add(DT_STRING,"PROC_DIV");
		}

		
		tmmsm33.MergeTo(bcls_rec->Tables["TMMSM33"],false);
		bcls_rec->Tables["TMMSM33"].Rows[0]["PROC_DIV"] = v_proc_div;
	
		if (v_proc_div == "N")
		{
			doFlag = f_mmsm3301n_proc(bcls_rec, bcls_ret, conn);
		}
		else if (v_proc_div == "D")
		{
			doFlag = f_mmsm3301d_proc(bcls_rec, bcls_ret, conn);
		}
		else if (v_proc_div == "U")
		{
			doFlag = f_mmsm3301u_proc(bcls_rec, bcls_ret, conn);
		}
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		//doFlag = f_mmsm3301n_proc(bcls_rec, bcls_ret, conn);
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}


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

