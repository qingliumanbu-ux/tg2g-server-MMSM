/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Deshription: 炼钢钢坯材料成品转在制品
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料成品转在制品
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 


//外部函数声明
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	

BM2F_ENTERACE(mmsm01a1f9_pro)                                               

int f_mmsm01a1f9_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
		}

		
		/* 获取输入参数 */
		for(int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm96["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			//Log::Trace("", __FUNCTION__, "tmmsm96.MAT_NO			= [{0}]", (const char*)tmmsm96["MAT_NO"].ToString());

			/* 查询材料信息 */
			tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
			if (tmmsm01.Query("MAT_NO") == false)
			{
				sprintf(s.msg, "[%s]的材料信息不存在。", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//太钢定制  无综判，无封锁，无合同才可以进行转换
			if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "0")
			{
				strcpy(s.msg, "该材料综合判定已判，无法进行该操作！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["HOLD_FLAG"].ToString().Trim() != "0")
			{
				strcpy(s.msg, "该材料不是未封锁状态，无法进行该操作！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["PONO_SLAB"].ToString().Trim() == "")
			{
				strcpy(s.msg, "该材料无虚拟板坯，无法进行该操作！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 校验材料信息 */
			if (tmmsm01["MAT_STATUS"].ToString().Trim().Substring(0,1) == "2")
			{
				sprintf(s.msg, "物料[%s]是在制品，不能执行成品转在制品功能。", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/*if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg, "物料[%s]不是余材，不能执行成品转在制品功能。", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/

			/* 设置抛帐参数 */
			tmmsm96["EVENT_ID"] = "MM22";	//成品转在制品
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm01a1f9_pro";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		}

		/* 调用物料跟踪函数 */
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);	
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GHRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
