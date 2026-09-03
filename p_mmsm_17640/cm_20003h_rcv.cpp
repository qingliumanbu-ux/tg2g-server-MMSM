/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2013-05-24
Description: 炼钢材料并批电文接收
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 

/*<remark>=========================================================
/// <summary>
/// 棒线钢坯并批电文接收
/// <para>
/// <para>
/// </summary>
/// <param name="MAT_NO">材料号 </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) ;

// service入口
BM2F_ENTERACE_TELE(cm_20003h_rcv)

int f_cm_20003h_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_main("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

    try
    { 
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 新增块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO_OLD");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_WT");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WT");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_THEORY_WT");
		}

		/* 获取输入参数 */
		tmmsm01_main["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAIN_MAT_NO"].ToString().Trim();	//主材料号

		//Log::Trace("", __FUNCTION__, "主材料 tmmsm01_main.MAT_NO		= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());

		/* 查询主材料信息 */
		tmmsm01_main.Query("MAT_NO");
		tmmsm01_main.TrimOrBlank();

		/* 获取被并批材料号 */
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			/* 获取被并批材料 */
			tmmsm01["MAT_NO"] = bcls_rec->Tables[1].Rows[i]["MAT_NO"].ToString().Trim();	//被并批材料号

			//Log::Trace("", __FUNCTION__, "被并批材料号 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

			/* 被并批材料号为空,则跳出循环 */
			if (tmmsm01["MAT_NO"].ToString().Trim() == "")
			{
				break;
			}

			/* 查询被并批材料信息 */
			tmmsm01.Query("MAT_NO");
			tmmsm01.TrimOrBlank();

			/* 设置母材料的支数和重量 */
			tmmsm01_main["MAT_NUM"] = tmmsm01_main["MAT_NUM"].ToDecimal() + tmmsm01["MAT_NUM"].ToDecimal();
			tmmsm01_main["MAT_WT"] = tmmsm01_main["MAT_WT"].ToDecimal() + tmmsm01["MAT_WT"].ToDecimal();
			tmmsm01_main["MAT_ACT_WT"] = tmmsm01_main["MAT_ACT_WT"].ToDecimal() + tmmsm01["MAT_ACT_WT"].ToDecimal();
			tmmsm01_main["MAT_THEORY_WT"] = tmmsm01_main["MAT_THEORY_WT"].ToDecimal() + tmmsm01["MAT_THEORY_WT"].ToDecimal();

			/* 修改被并材料号的MAT_NO_OLD为主材料号 */
			tmmsm01["MAT_NO_OLD"] = tmmsm01_main["MAT_NO"];
			tmmsm01["REC_REVISOR"] = s.userid;
			tmmsm01["REC_REVISE_TIME"] = datetime;
			tmmsm01.Update("MAT_NO_OLD,"
						   "REC_REVISOR,"
					   	   "REC_REVISE_TIME",
						   "MAT_NO");

			/* 设置物料跟踪参数-被并材料删除 */
			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"] = "MM32";	//材料被并批删除
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "00";
			bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"] = "cm_20003h_rcv";
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO_OLD"] = tmmsm01_main["MAT_NO"];
		}

		/* 调用物料跟踪-被并材料删除 */
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* 修改母材料支数等值 */
		//tmmsm01_main["REC_REVISOR"]		= s.userid;							
		//tmmsm01_main["REC_REVISE_TIME"]	= datetime;							
		//sqlstr = "tmmsm01_main.Update()";
		//tmmsm01_main.Update( "MAT_NUM,"
		//					 "MAT_WT,"
		//					 "MAT_ACT_WT,"
		//					 "MAT_THEORY_WT,"
		//					 "REC_REVISOR,"
		//					 "REC_REVISE_TIME",
		//					 "MAT_NO");


		//Log::Trace("", __FUNCTION__, "主材料抛物料跟踪履历 tmmsm01_main.MAT_NO		= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());
		/* 主材料抛物料跟踪履历 */
		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM31";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_20003h_rcv";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01_main["MAT_NO"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = tmmsm01_main["MAT_NUM"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = tmmsm01_main["MAT_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tmmsm01_main["MAT_ACT_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = tmmsm01_main["MAT_THEORY_WT"];

		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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
	return doFlag;
}
