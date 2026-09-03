/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Description: 炼钢钢坯材料管理封锁
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料管理封锁
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


#if defined _SYS_MMS || defined _SYS_MES 

#endif


//外部函数声明
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);	
BM2_FUNCTION_IMPORT

#if defined _SYS_MMS || defined _SYS_MES 
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);	
#endif


BM2F_ENTERACE(mmsm01a1f10_pro)

int f_mmsm01a1f10_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义
	 
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    
	
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

    #if defined _SYS_MMS || defined _SYS_MES 
	CModel tpmof03("TPMOF03");
    #endif


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
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOF99"); 
		}

		/* 获取输入参数 */
		// 获取 table0 参数
		tmmsm96["MNG_HOLD_CAUSE_CODE"] = bcls_rec->Tables[0].Rows[0]["MNG_HOLD_CAUSE_CODE"].ToString().Trim();
		tmmsm96["MNG_HOLD_REMARK"] = bcls_rec->Tables[0].Rows[0]["MNG_HOLD_REMARK"].ToString().Trim();

		// 获取 table1 材料号
		for(int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			tmmsm96["MAT_NO"] = bcls_rec->Tables[1].Rows[i]["MAT_NO"].ToString().Trim();

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
			//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_ID			= [{0}]",(const char*)tmmsm01["MAT_ID"].ToString());	
			
			/* 材料是否在当前档 */			 
			if(tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg,"材料[%s]不在当前档!",(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			/* 校验逻辑合法性 */
			if(tmmsm01["PLAN_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg,"材料号[%s]在作业计划[%s]中,不能管理封锁!",(const char*)tmmsm01["MAT_NO"].ToString(),(const char*)tmmsm01["PLAN_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tmmsm01["CONFM_FLAG"].ToString().Trim() != "0")  
			{
				sprintf(s.msg,"材料号[%s]材料状态[%s]是准发,不能管理封锁!",(const char*)tmmsm01["MAT_NO"].ToString(),(const char*)tmmsm01["MAT_STATUS"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//Log::Trace("",__FUNCTION__,"CONFM_FLAG				= [%s]",(const char*)tmmsm01["CONFM_FLAG"].ToString());
			if(tmmsm01["TRANSFER_FLAG"].ToString().Trim() != "0")  
			{
				sprintf(s.msg,"材料号[%s]在转库计划中,转库状态是[%s],不能管理封锁!",(const char*)tmmsm01["MAT_NO"].ToString(),(const char*)tmmsm01["TRANSFER_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tmmsm01["APP_DECIDE_FLAG"].ToString().Trim() != "0")  
			{
				sprintf(s.msg,"材料号[%s]在现货申报计划中,现货申报标记是[%s],不能管理封锁!",(const char*)tmmsm01["MAT_NO"].ToString(),(const char*)tmmsm01["APP_DECIDE_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 设置物料跟踪参数 */
			tmmsm96["EVENT_ID"] = "MM07";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm01a1f10_pro";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
			
			
            #if defined _SYS_MMS || defined _SYS_MES 
			/* 合同材,设置合同跟踪接口参数 */
			if(tmmsm01["ORDER_NO"].ToString().Trim()	!=	"" && tmmsm01["HOLD_FLAG"].ToString().Trim() == "0")
			{
				tpmof03["EVENT_ID"]			= "58";
				tpmof03["SYSTEM_ID"]			= "MMSM";
				tpmof03["FUNC_ID"]				= "mmsm01a1f10_pro";
				tpmof03["ORDER_NO"]			= tmmsm01["ORDER_NO"];
				tpmof03["WHOLE_BACKLOG_NO"]	= tmmsm01["WHOLE_BACKLOG_NO"];
				tpmof03["WHOLE_BACKLOG"]		= tmmsm01["WHOLE_BACKLOG"];
				tpmof03["WHOLE_BACKLOG_CODE"]	= tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
				tpmof03["WHOLE_BACKLOG_SEQ"]	= tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
				tpmof03["MAT_NO"]				= tmmsm01["MAT_NO"];
				tpmof03["WT"]					= tmmsm01["MAT_ACT_WT"];
				tpmof03["PREV_MAT_STATUS"]		= "23";	//未封锁
				tpmof03["MAT_STATUS"]			= "21"; //已封锁
				tpmof03.MergeTo(bcls_rec->Tables["PMOF99"],false);
			}
           #endif

		}

		/* 调用物料跟踪 */
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);	
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

        #if defined _SYS_MMS || defined _SYS_MES 
		/* 调用合同跟踪 */
		if(tpmof03["EVENT_ID"].ToString().Trim() == "58")
		{
			doFlag = f_pmof99_v3(bcls_rec, bcls_ret,conn);	
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		} 
        #endif
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
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
