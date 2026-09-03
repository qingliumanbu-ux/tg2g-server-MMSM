/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     mfj
Version:    1.0
Date:       2024-03-09
Description: 
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件


/**********
弹窗前校验，一些后台校验提前加在这里。在弹窗前进行提示
比如分切，修磨，切废，碳钢在线检验记录等部分画面弹窗
********/


BM2F_ENTERACE(mmsmwljy_pro)

int f_mmsmwljy_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_table = "";//表名  不可为空
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		v_table = bcls_rec->Tables[0].Rows[0]["TABLE"].ToString().Trim();
		if (v_table == "")
		{
			sprintf(s.sysmsg, "表名不能为空!");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (v_table == "MMSM34")//修磨
		{
			tmmsm01.Query();
			//2024-03-08  添加一个前置条件
			if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场,违反LOGISTICS_STATUS约束，不允许修磨", arguments, 0);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0" && tmmsm01["C_STATESIGN"].ToString().Trim() != ""
				&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场，违反C_STATESIGN约束，不允许修磨", arguments, 0);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != "SM")
			{
				strcpy(s.msg, "材料未在炼钢产线，不能修磨");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["IN_FLAG"].ToString().Trim() != "1")
			{
				strcpy(s.msg, "材料未入库，不能修磨");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//

		}
		else if (v_table == "MMSM35")//分切
		{
			tmmsm01.Query();
			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}未综判,不允许分段处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许分段处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0"&& tmmsm01["C_STATESIGN"].ToString().Trim() != ""
				&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许分段处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}


		}
		else if (v_table == "MMSM39")//切废
		{
			tmmsm01.Query();
			//2024-03-08  添加一个前置条件
			if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场,违反LOGISTICS_STATUS约束，不允许修磨", arguments, 0);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0" && tmmsm01["C_STATESIGN"].ToString().Trim() != ""
				&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已不在现场，违反C_STATESIGN约束，不允许修磨", arguments, 0);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != "SM")
			{
				strcpy(s.msg, "材料未在炼钢产线，不能修磨");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["IN_FLAG"].ToString().Trim() != "1")
			{
				strcpy(s.msg, "材料未入库，不能修磨");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
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


