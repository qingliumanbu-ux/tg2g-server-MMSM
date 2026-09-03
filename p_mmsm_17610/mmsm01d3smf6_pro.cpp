/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2016-09-24
Description: 物料放冷和放冷取消
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料放冷和放冷取消
/// <para>
/// <para>
/// </summary>
/// <param name="MAT_NO">材料号 </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
 

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm01d3smf6_pro)

int f_mmsm01d3smf6_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加与设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}
		
		/* 获取输入参数 */
		// 获取 table0 参数
		tmmsm96["SLAB_COLD_HOT_FLAG"] = bcls_rec->Tables[0].Rows[0]["SLAB_COLD_HOT_FLAG"].ToString().Trim();

		//Log::Trace("", __FUNCTION__, "tmmsm96.SLAB_COLD_HOT_FLAG		= [{0}]", (const char*)tmmsm96["SLAB_COLD_HOT_FLAG"].ToString());
		

		// 获取 table1 材料号
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm96["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "tmmsm96.MAT_NO			= [{0}]", (const char*)tmmsm96["MAT_NO"].ToString());


			/* 检查输入参数合法性 */
			if (tmmsm96["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 查询材料主档表 */
			tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
			tmmsm01.Query();
			//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_ID			= [{0}]", (const char*)tmmsm01["MAT_ID"].ToString());

			/* 材料是否在当前档 */
			if (tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料[%s]不在当前档!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			

			/* 设置物料跟踪参数 */
			tmmsm96["EVENT_ID"] = "MM54";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm01d3smf6_pro";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

		}

		/* 调用物料跟踪 */
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
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
