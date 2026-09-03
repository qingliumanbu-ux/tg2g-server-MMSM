/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2015-04-01
Description: 炼钢缺陷修改
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢缺陷修改
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件 
 
 
 

//外部函数声明
#if defined _SYS_PES	//PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif


BM2_FUNCTION_EXPORT
int f_mmsm0604_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	
	/* 业务变量 */
	CString	datetime("");
	CString	cs_mmsm_defect_log_seq("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm06("TMMSM06");
	CModel tmmsm061("TMMSM061");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM0604");
		if(blkNum < 0)
		{
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			strcpy(s.sysmsg,"传入数据块 MMSM0604 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
			/* 添加并设置块名 */
		#if defined _SYS_PES	//PES
		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if(blkNum < 0)
		{
				bcls_rec->Tables.Add("MMSMSND"); 
		}

		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TABLE_TYPE");
		}

		#endif
		

		for(int i = 0; i < bcls_rec->Tables["MMSM0604"].Rows.get_Count(); i++)
		{
			tmmsm06.Reset();
			tmmsm061.Reset();
			tmmsm061.MergeFrom(bcls_rec->Tables["MMSM0604"].Rows[i]);
			tmmsm061.TrimOrBlank();

			/* 打印输入参数 */
			//Log::Trace("", __FUNCTION__, "修改缺陷 传入参数,tmmsm061.DEFECT_ID			= [{0}]", tmmsm061["DEFECT_ID"].ToString());
			//Log::Trace("", __FUNCTION__, "修改缺陷 传入参数,tmmsm061.MAT_NO				= [{0}]",tmmsm061["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "修改缺陷 传入参数,tmmsm061.SURFACE_DIV		= [{0}]",tmmsm061["SURFACE_DIV"].ToString());
			//Log::Trace("", __FUNCTION__, "修改缺陷 传入参数,tmmsm061.DEFECT_CODE		= [{0}]",tmmsm061["DEFECT_CODE"].ToString());

			/* 检查输入参数合法性 */
			if(tmmsm061["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"材料号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location); 
			}

			/* 修改缺陷信息 */
			tmmsm06.CopyFrom(tmmsm061);
			tmmsm06["REC_REVISOR"]			=	s.userid;
			tmmsm06["REC_REVISE_TIME"]		=	datetime;
			tmmsm06.TrimOrBlank();
			tmmsm06.Update( "DEFECT_NO," 
							"DEFECT_SURF,"
							"DEFECT_CODE,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"DEFECT_ID");

			/* 获取缺陷履历流水号 */
			#if defined _SYS_PES || _SYS_MES
				cs_mmsm_defect_log_seq = EPGetNextSeq("MMSM_DEFECT_LOG_SEQ", conn);
				if (cs_mmsm_defect_log_seq.Trim() == "")
				{
					strcpy(s.msg, "获取 缺陷履历流水号 失败!");/*系统出现异常，请联系系统维护人员。*/
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				tmmsm061["DEFECT_LOG_SEQ"] = datetime + cs_mmsm_defect_log_seq;			//缺陷履历流水号
			#endif

			/* 新增缺陷信息履历表 */
			tmmsm061["CLEAR_DIV"]			= "U";			//U-修改
			tmmsm061["REC_CREATOR"]		= s.userid;
			tmmsm061["REC_CREATE_TIME"]	= datetime;
			tmmsm061.TrimOrBlank();
			tmmsm061.Insert();   

			#if defined _SYS_PES	//PES
			/* 设置缺陷电文参数 */
			tmmsm061.MergeTo(bcls_rec->Tables["MMSMSND"],false);
			#endif

		}

		#if defined _SYS_PES	//PES
  	
		bcls_rec->Tables["MMSMSND"].Rows[0]["TABLE_TYPE"] = "MMSM3Y";
				
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		#endif

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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
