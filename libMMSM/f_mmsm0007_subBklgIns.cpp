/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      顾云峰
Version:     1.0
Date:        2013-08-26
Description: 板坯小工序追加
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 板坯小工序追加
/// <para>
/// </para>
/// <para>数据库表：TMMSM01(板坯物料主表)/TMMSM04(板坯小工序表)                </para>
/// <para>                                                                     </para>
/// </summary>
/// <param name="">                                                           </param>
/// <returns>                                                               </returns>
============================================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient; 

#include "EI_TUXClass.h"

/* ***** 程序表结构引用 ***** */
//#include "tmmsm01.h" 
//#include "tmmsm03.h"
//#include "tmmsm04.h" 

//外部函数声明

int f_mmsm0007_subBklgIns(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	int i_count = 0;
	CString  datetime = "";

	//使用的表结构变量
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	/*CTMMSM01 tmmsm01(conn);
	CTMMSM03 tmmsm03(conn);
    CTMMSM04 tmmsm04(conn);*/

	CString  sqlstr("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq2(conn);
	
	try
	{
		if(bcls_rec->Tables.Contains("MMSM0007") == false)
		{
			strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		
		//获取传入参数
		tmmsm04.MergeFrom(bcls_rec->Tables["MMSM0007"].Rows[0]);
		Log::Trace("",__FUNCTION__,"mat_no = [{0}]",tmmsm04["MAT_NO"].ToString());

		//将已有未完成的工序重新排序
		sqlstr = " UPDATE TMMSM04 "
			     "    SET SUB_BACKLOG_SEQ = SUB_BACKLOG + 1, "
				 "  WHERE MAT_NO = @tmmsm04.mat_no "
				 "    AND SUB_BACKLOG_SEQ >= @tmmsm04.sub_backlog_seq ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm04.mat_no",tmmsm04["MAT_NO"]);
		cmd_inq.Parameters.Set("tmmsm04.sub_backlog_seq",tmmsm04["SUB_BACKLOG_SEQ"]);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//增加板坯工序
		cmd_inq.SetCommandText(" SELECT AIM_MAT_NO FROM TMMSM03 WHERE MAT_NO = @tmmsm04.mat_no ");
		cmd_inq.Parameters.Set("tmmsm04.mat_no",tmmsm04["MAT_NO"]);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())
		{
			tmmsm04["AIM_MAT_NO"] = cmd_inq.GetString(1);

			tmmsm04.Insert();
		}
		cmd_inq.Close();

		//根据最新的板坯工序，更新板坯主档
		cmd_inq.SetCommandText(" SELECT NEXT_SUB_BACKLOG_SEQ FROM TMMSM01 WHERE MAT_NO = @tmmsm04.mat_no ");
		cmd_inq.Parameters.Set("tmmsm04.mat_no",tmmsm04["MAT_NO"]);
		tmmsm01["NEXT_SUB_BACKLOG_SEQ"] = cmd_inq.ExecuteScalar().ToUInt32();
		cmd_inq.Close();

		if(tmmsm01["NEXT_SUB_BACKLOG_SEQ"].ToString() == 0)
		{
			tmmsm01["NEXT_SUB_BACKLOG_SEQ"] = tmmsm04["SUB_BACKLOG_SEQ"];
			tmmsm01["NEXT_SUB_BACKLOG_CODE"] = tmmsm04["SUB_BACKLOG_CODE"];
		}
		else
		{
			cmd_inq.SetCommandText(" SELECT SUB_BACKLOG_CODE FROM TMMSM04 WHERE MAT_NO = @tmmsm04.mat_no AND SUB_BACKLOG_SEQ = @tmmsm01.next_sub_backlog_seq fetch first 1 rows ONLY ");
			cmd_inq.Parameters.Set("tmmsm04.mat_no",tmmsm04["MAT_NO"]);
			cmd_inq.Parameters.Set("tmmsm01.next_sub_backlog_seq",tmmsm01["NEXT_SUB_BACKLOG_SEQ"]);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tmmsm01["NEXT_SUB_BACKLOG_CODE"] = cmd_inq.GetString(1);
			}
			else tmmsm01["NEXT_SUB_BACKLOG_CODE"] = " ";
			cmd_inq.Close();
		}

		tmmsm01["MAT_NO"] = tmmsm04["MAT_NO"];
		tmmsm01.Update("NEXT_SUB_BACKLOG_SEQ,NEXT_SUB_BACKLOG_CODE","MAT_NO");		
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